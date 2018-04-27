#include <ADC.h>
#include <Audio.h>
#include <Servo.h>

#define POT_PIN        A14
#define SERVO_LEFT_PIN 10
#define SERVO_RIGHT_PIN 9
#define LED_PIN 13
#define MIC_PIN A0
#define PROX_TRIG_PIN 23
#define PROX_ECHO_PIN 22
#define AVG_NUM   0xA //number of ffts to be averaged
#define ONE_SEC 500// 1000 //in milliseconds, since the beep is 1 sec long
#define THRESHOLD_VAL 600 //can be changed depending upon the kind of data that we see

//------Frequencies to detect-----------//
#define FREQ_1 1000                     //
#define FREQ_2 4000                     //
#define FREQ_3 5000                     //
#define FREQ_5 6000                     //
#define FREQ_6 7000                     //
//--------------------------------------//

unsigned long microseconds, microseconds_start;

AudioInputAnalog         adc1(A0);           //xy=225,128
AudioAnalyzePeak         peak1;          //xy=414.0000228881836,265.00000762939453
AudioAnalyzeToneDetect   tone1, tone2;          //xy=545,175
AudioConnection          patchCord1(adc1, peak1);
AudioConnection          patchCord2(adc1, tone1);
AudioConnection          patchCord3(adc1, tone2);

//servo object
Servo servoLeft;
Servo servoRight;

//adc object
ADC *adc = new ADC();

void setup() {
  Serial.begin(9600);
  AudioMemory(12);
  pinMode(PROX_TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(PROX_ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(LED_PIN, OUTPUT);

  //set frequencies to detect and number of cycles
  tone1.frequency(FREQ_1, 10);
  tone2.frequency(FREQ_2, 10);

  //set servo pins
  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);

  //POT settings
  pinMode(POT_PIN, INPUT);
  adc->setAveraging(16); // set number of averages
  adc->setResolution(16); // set bits of resolution

  // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
  // see the documentation for more information
  // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
  // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
  // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
}

//variable for detecting data pattern
uint32_t pattern[37], pattern_scaled[37], prox_data[37];
bool direction = 1;
float freq_amp_old, freq_amp_new;
uint16_t degree = 0;
uint32_t mismatch_cnt = 0;

void loop() {
  if (tone1.available()) {
    float max_freq_amp = 0, min_dist = 6000;
    uint16_t max_degree = 0, min_prox_degree = 0;
    int dist;
    unsigned long time, duration;
    unsigned long count = 0;
    degree = 0;
    while (degree < 360) {
      //sampling for one sec
      time = millis();
      count = 0;
      duration = 0;
      freq_amp_new = 0;
      while (duration < time + ONE_SEC) {
        float tone1_read_val = tone1.read() * 10000;
        freq_amp_new = tone1_read_val + freq_amp_new;
        count++;

        duration = millis();
      }

      freq_amp_new = freq_amp_new / count;

      pattern[degree / 10] = freq_amp_new;
      prox_data[degree / 10] = detect_distance();

      //setting the value of max amplitude, and the angle at which max amplitude is seen
      if (freq_amp_new > max_freq_amp) {
        max_freq_amp = freq_amp_new;
        max_degree = degree;
        Serial.print("New max ampl ");
        Serial.println(freq_amp_new);
        Serial.print("New max degree ");
        Serial.println(max_degree);
      }
      turn(1, 10); //turn left by 10 degrees, for 360 degrees
      degree += 10;
      freq_amp_old = freq_amp_new;
    }
    Serial.print("Max amplitude ");
    Serial.println(max_freq_amp);
    Serial.print("Max degree ");
    Serial.println(max_degree);
    Serial.print("Min distance value ");
    Serial.println(min_dist);
    Serial.print("Min distance degree ");
    Serial.println(min_prox_degree);
    for (int i = 0; i <= 36 ; i++) {
      if (pattern[i] >= max_freq_amp * 3 / 4) pattern_scaled[i] = 4;
      else if (pattern[i] >= max_freq_amp * 1 / 2) pattern_scaled[i] = 3;
      else if (pattern[i] >= max_freq_amp * 1 / 4) pattern_scaled[i] = 2;
      else pattern_scaled[i] = 1;
    }
    int avg_pattern = 0;
    int max_avg = 0;
    int max_new_deg = 0;
    int weighted_avg, max_weight_avg;
    for (int j = 0; j < 37; j++) {
      avg_pattern = 0;
      weighted_avg = 0;
      if (pattern_scaled[j] == 4) {
        for (int i = (j - 5); i < (j + 5); i++) {
          int k = i;
          if (i < 0) k = 37 + i;
          else if (i > 36) k = i - 37;
          avg_pattern += pattern[k];
          weighted_avg += (pattern[k] * k);
        }
        if (avg_pattern > max_avg) {
          max_avg = avg_pattern;
          max_new_deg = j * 10;
          max_weight_avg = (weighted_avg * 10) / max_avg;
        }
      }
    }

    //try to find the spot using proximity detector
    int max_deg_idx = max_new_deg / 10;
    //    for(int i= (max_deg_idx - 5) ; i < (max_deg_idx + 5); i++){
    //
    //    }
    if (max_new_deg != max_degree) {
      Serial.print("Calculated max is not same as observed max. Observed =  ");
      Serial.print(max_degree);
      Serial.print(" Calculated = ");
      Serial.println(max_new_deg);
      Serial.print(" Weighted = ");
      Serial.println(max_weight_avg);
    }

    //since max avg is actually the summation over 10 values, dividing it by 10 here
    //if max avg is less than the threshold value there is no need to turn , just go straight and do another round of sweep
    //ideally this should never happen
    if ((max_avg / 10) > THRESHOLD_VAL) {
      max_degree = max_weight_avg;
      if (max_degree > 180) {
        turn(0, 360 - max_degree);
      }
      else {
        turn(1, max_degree);
      }
    }

    digitalWrite(LED_PIN, 1);
    forward(20, 25);
    digitalWrite(LED_PIN, 0);

  }
}
