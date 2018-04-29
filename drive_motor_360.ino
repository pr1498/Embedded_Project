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
#define INVALID_VAL   250 //outside the valid count of frequencies, we are only going to get 10 frequencies at max

//------Frequencies to detect-----------//
#define NUM_FREQ   2 //set for detecting 2 right now
#define FREQ_1 5000                     //
#define FREQ_2 6000                     //
#define FREQ_3 7000                     //
#define FREQ_4 8000                     //
#define FREQ_5 9000                     //
#define FREQ_6 10000                    //
#define FREQ_7 11000                    //
#define FREQ_8 12000                    //
#define FREQ_9 13000                    //
#define FREQ_10 14000                   //
//--------------------------------------//

AudioInputAnalog         adc1(A0);           //xy=225,128
AudioAnalyzePeak         peak1;          //xy=414.0000228881836,265.00000762939453
AudioAnalyzeToneDetect   tone0[NUM_FREQ];          //xy=545,175
AudioConnection          patchCord1(adc1, peak1);
AudioConnection          patchCord2(adc1, tone0[0]);
AudioConnection          patchCord3(adc1, tone0[1]);
//AudioConnection          patchCord4(adc1, tone0[2]);
//AudioConnection          patchCord5(adc1, tone0[3]);
//AudioConnection          patchCord6(adc1, tone0[4]);
//AudioConnection          patchCord7(adc1, tone0[5]);
//AudioConnection          patchCord8(adc1, tone0[6]);
//AudioConnection          patchCord9(adc1, tone0[7]);
//AudioConnection          patchCord10(adc1, tone0[8]);
//AudioConnection          patchCord11(adc1, tone0[9]);

//servo object
Servo servoLeft;
Servo servoRight;

//adc object for reading POT
ADC *adc = new ADC();
uint8_t right_wheel_offset; //to be updated based on POT reading

void setup() {
  Serial.begin(9600);
  AudioMemory(12);
  pinMode(PROX_TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(PROX_ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(LED_PIN, OUTPUT);

  //set frequencies to detect and number of cycles
  tone0[0].frequency(FREQ_1, 10);
  tone0[1].frequency(FREQ_2, 10);
  //  tone0[2].frequency(FREQ_3, 10);
  //  tone0[3].frequency(FREQ_4, 10);
  //  tone0[4].frequency(FREQ_5, 10);
  //  tone0[5].frequency(FREQ_6, 10);
  //  tone0[6].frequency(FREQ_7, 10);
  //  tone0[7].frequency(FREQ_8, 10);
  //  tone0[8].frequency(FREQ_9, 10);
  //  tone0[9].frequency(FREQ_10, 10);

  //set servo pins
  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);

  //POT settings
  pinMode(POT_PIN, INPUT);
  adc->setAveraging(16); // set number of averages
  adc->setResolution(16); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

  calibration();
}

//variable for detecting data pattern
uint32_t pattern[37][NUM_FREQ], pattern_scaled[37][NUM_FREQ];
uint32_t prox_data[37]; //distance is not dependent on frequency
float freq_amp_new[NUM_FREQ];
bool tone_done[NUM_FREQ]; //1 for each tone
uint8_t freq_to_detect = 0;

void loop() {
  if (tone0[0].available()) {
    float max_freq_amp = 0, min_dist = 6000;
    uint16_t max_degree = 0, min_prox_degree = 0;

    //initial tone read, looks at all frequencies
    tone_read();

    if (freq_to_detect != INVALID_VAL) {
      for (int i = 0; i <= 36 ; i++) {
        if (pattern[i][freq_to_detect] >= max_freq_amp * 3 / 4) pattern_scaled[i][freq_to_detect] = 4;
        else if (pattern[i][freq_to_detect] >= max_freq_amp * 1 / 2) pattern_scaled[i][freq_to_detect] = 3;
        else if (pattern[i][freq_to_detect] >= max_freq_amp * 1 / 4) pattern_scaled[i][freq_to_detect] = 2;
        else pattern_scaled[i][freq_to_detect] = 1;
      }

      int avg_pattern = 0;
      int max_avg = 0;
      int max_new_deg = 0;
      int weighted_avg, max_weight_avg;
      for (int j = 0; j < 37; j++) {
        avg_pattern = 0;
        weighted_avg = 0;
        if (pattern_scaled[j][freq_to_detect] == 4) {
          for (int i = (j - 5); i < (j + 5); i++) {
            int k = i;
            if (i < 0) k = 37 + i;
            else if (i > 36) k = i - 37;
            avg_pattern += pattern[k][freq_to_detect];
            weighted_avg += (pattern[k][freq_to_detect] * k);
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
      min_dist = 6000; //very large value, hopefully !!!
      for (int i = (max_deg_idx - 2) ; i < (max_deg_idx + 2); i++) {
        int k = i;
        if (i < 0) k = 37 + i;
        else if (i > 36) k = i - 37;
        if ( min_dist < prox_data[k]) {
          min_dist = prox_data[k];
          min_prox_degree = k * 10;
        }
      }

      Serial.print("Min distance value ");
      Serial.println(min_dist);
      Serial.print("Min distance degree ");
      Serial.println(min_prox_degree);

      if (max_new_deg != max_degree) {
        Serial.print("Calculated max is not same as observed max. Observed =  ");
        Serial.print(max_degree);
        Serial.print(" Calculated = ");
        Serial.println(max_new_deg);
        Serial.print(" Weighted = ");
        Serial.println(max_weight_avg);
      }

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
    tone_done[freq_to_detect] = 1;
  }
}
