//#include <ADC.h>
#include <Servo.h>
#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define POT_PIN        A14
#define SERVO_LEFT_PIN 10
#define SERVO_RIGHT_PIN 9
#define LED_PIN 13
#define MIC_PIN A0
#define PROX_TRIG_PIN 23
#define PROX_ECHO_PIN 22
#define AVG_NUM   0xA //number of ffts to be averaged
#define ONE_SEC 250 //in milliseconds, since the beep is 1 sec long
#define THRESHOLD_VAL 250 //236 when noisy?? 
#define INVALID_VAL   250 //outside the valid count of frequencies, we are only going to get 10 frequencies at max
#define NUM_TURNS     15

//------Frequencies to detect-----------//
#define LAST_FREQUENCY 9500
#define SAMPLING_FREQUENCY 20000
#define NUM_FREQ   10 //set for detecting 2 right now
//--------------------------------------//


//servo object
Servo servoLeft;
Servo servoRight;
uint16_t frequency[10] = {5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500};
unsigned int sampling_period_us;
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = SAMPLING_FREQUENCY;
uint16_t freq_idx[10];
double total_avg[NUM_FREQ]; //to store fft output

void setup() {
  Serial.begin(115200);
  pinMode(PROX_TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(PROX_ECHO_PIN, INPUT); // Sets the echoPin as an Input
  pinMode(LED_PIN, OUTPUT);
  pinMode(MIC_PIN, INPUT);

  sampling_period_us = round(1000000 * (1.0 / samplingFrequency));
  for (int i = 0; i < NUM_FREQ; i++) {
    freq_idx[i] = frequency[i] * samples / samplingFrequency;
    Serial.print(" Freq Index ");
    Serial.println(freq_idx[i]);
  }

  //set servo pins
  servoLeft.attach(SERVO_LEFT_PIN);
  servoRight.attach(SERVO_RIGHT_PIN);

  //  //POT settings
  //  pinMode(POT_PIN, INPUT);
  //  adc->setAveraging(16); // set number of averages
  //  adc->setResolution(16); // set bits of resolution
  //  adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
  //  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

  //  calibration();

  Serial.println("START");
}

//variable for detecting data pattern
uint32_t pattern[NUM_TURNS][NUM_FREQ], pattern_scaled[NUM_TURNS];
uint32_t prox_data[NUM_TURNS]; //distance is not dependent on frequency
uint32_t freq_amp_new[NUM_FREQ];
uint32_t max_freq_amp[NUM_FREQ];
bool tone_done[NUM_FREQ]; //1 for each tone
uint8_t freq_to_detect = 0;
uint16_t dist;
bool new_frequency = 0;
bool finish = 0;
bool redo = 0;
uint16_t max_degree[NUM_FREQ];
uint32_t threshold_val[10] = {250, 250, 250, 250, 250, 250, 250, 250, 160, 160};

void loop() {
  float min_dist = 6000;
  uint16_t min_prox_degree = 0;
  redo = 0;
  new_frequency = 0;
  tone_read();

  if (freq_to_detect != INVALID_VAL) {

    //try to find the spot using proximity detector
    int max_deg_idx = max_degree[freq_to_detect]; //(max_new_deg * NUM_TURNS) / 360;
    min_dist = 6000; //very large value, hopefully !!!
    for (int i = (max_deg_idx - 2) ; i < (max_deg_idx + 2); i++) {
      int k = i;
      if (i < 0) k = NUM_TURNS + i;
      else if (i > NUM_TURNS - 1 ) k = i - NUM_TURNS;
      if ( min_dist > prox_data[k]) {
        min_dist = prox_data[k];
        min_prox_degree = k * (360 / NUM_TURNS);
      }
    }

    //    Serial.print("Min distance value ");
    //    Serial.println(min_dist);
    //    Serial.print("Min distance degree ");
    //    Serial.println(min_prox_degree);

    if (max_degree[freq_to_detect] > 180) {
      turn(0, 360 - max_degree[freq_to_detect]);
    }
    else {
      turn(1, max_degree[freq_to_detect]);
    }

    dist = detect_distance();

    while ( new_frequency != 1 && dist > 15 && finish != 1 && redo != 1) {
      for (int i = 0; i < 10; i++) {
        sample_data();
        for (int freq = 0; freq < NUM_FREQ; freq++) {
          freq_amp_new[freq] += total_avg[freq];
        }
      }

      for (unsigned int i = 0; i < NUM_FREQ; i++) {
        freq_amp_new[i] = freq_amp_new[i] / 10;
        //we don't need to look at nay more frequencies when we are at the end
        if (frequency[freq_to_detect] != LAST_FREQUENCY && i != freq_to_detect && freq_amp_new[i] > threshold_val[i] && tone_done[i] != 1) {
          new_frequency = 1;
          Serial.print("New Frequency detected");
          Serial.println((i * 500) + 5000);
        }
      }
      if ( freq_amp_new[freq_to_detect] < 0.5 * max_freq_amp[freq_to_detect]) redo = 1; //most likely we turned wrong
      if (redo != 1) {
        if (dist > 30) {
          forward(20, 30);
        }
        else {
          if (frequency[freq_to_detect] != LAST_FREQUENCY) {
            forward(20, dist - 15);
          }
          else {
            forward(20, dist);
            finish = 1;
            Serial.println("Moving towards last frequency ");
            digitalWrite(LED_PIN, 1);
          }
        }
        dist = detect_distance();
        Serial.print("Distance ");
        Serial.println(dist);
      }
      else {
        Serial.println("Must check again. New amplitude value ");
        Serial.println(freq_amp_new[freq_to_detect]);
      }
    }

    if (dist < 15 && frequency[freq_to_detect] == LAST_FREQUENCY) {
      forward(20, dist - 5);
      finish = 1;
      Serial.println("Moving towards last frequency ");
      digitalWrite(LED_PIN, 1);
    }

    if (redo != 1) {
      tone_done[freq_to_detect] = 1;
      Serial.print("Finished searching for frequency ");
      Serial.println(freq_to_detect * 500 + 5000);
    }
    if (finish == 1) {
      while (1);
    }

  } //freq INVALID
  else {
    dist = detect_distance();

    if (dist > 30) {
      forward(20, 30);
    }
    else {
      forward(20, dist - 15);
    }
  }
}

