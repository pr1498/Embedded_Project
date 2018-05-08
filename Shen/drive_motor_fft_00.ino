#include <Servo.h>
#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

//------PIN numbers-------------//
#define POT_PIN        A1
#define SERVO_LEFT_PIN 10
#define SERVO_RIGHT_PIN 9
#define SERVO2_LEFT_PIN  5
#define SERVO2_RIGHT_PIN 6
#define LED_PIN 13
#define MIC_PIN A0
#define PROX_TRIG_PIN 23
#define PROX_ECHO_PIN 22

//--------------------------------//

#define AVG_NUM   0xA //number of ffts to be averaged
#define INVALID_VAL   250 //outside the valid count of frequencies, we are only going to get 10 frequencies at max
#define NUM_TURNS     12

//------Frequencies to detect-----------//
#define LAST_FREQUENCY 9500
#define SAMPLING_FREQUENCY 20000
#define NUM_FREQ   10
//--------------------------------------//


//servo object
Servo servoLeft;
Servo servoRight;
Servo servoLeftforward;
Servo servoRightforward;


uint16_t frequency[10] = {5000, 5500, 6000, 6500, 7000, 7500, 8000, 8500, 9000, 9500};
unsigned int sampling_period_us;
const uint16_t samples = 256; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = SAMPLING_FREQUENCY;
uint16_t freq_idx[10];
double total_avg[NUM_FREQ]; //to store fft output

void setup() {
  Serial.begin(115200);
  pinMode(PROX_TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(PROX_ECHO_PIN, INPUT); // Sets the echoPin as an Inputs
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
  servoLeftforward.attach(SERVO2_LEFT_PIN);
  servoRightforward.attach(SERVO2_RIGHT_PIN);

  servo_test();

  Serial.println("START");
}


//variable for detecting data pattern
uint32_t pattern[NUM_TURNS][NUM_FREQ], pattern_scaled[NUM_TURNS];
uint32_t prox_data[NUM_TURNS]; //distance is not dependent on frequency
uint32_t freq_amp_new[NUM_FREQ];
uint32_t freq_amp_1[NUM_FREQ], freq_amp_2[NUM_FREQ], freq_amp_3[NUM_FREQ];
uint32_t max_freq_amp[NUM_FREQ];
bool tone_done[NUM_FREQ]; //1 for each tone
uint8_t freq_to_detect = 0;
uint16_t dist, dist_left, dist_right;
bool new_frequency = 0;
bool finish = 0;
bool redo = 0;
uint16_t max_degree[NUM_FREQ];
uint32_t threshold_val[10] = {500, 200, 300, 150, 250, 250, 250, 250, 120, 120};
uint16_t invalid_freq_cnt = 0;
bool direction = 0;
int prev_freq_done = -1;

void loop() {
  /*float min_dist = 6000;
  uint16_t min_prox_degree = 0;
  redo = 0;
  new_frequency = 0;

  tone_read();

  if (freq_to_detect != INVALID_VAL) {

    if (max_degree[freq_to_detect] > 180) {
      Serial.print("Turn degree right");
      Serial.println(360 - max_degree[freq_to_detect]);
      turn(0, 360 - max_degree[freq_to_detect]);
      direction = 0;
    }
    else {
      Serial.print("Turn degree left");
      Serial.println(max_degree[freq_to_detect]);
      turn(1, max_degree[freq_to_detect]);
      direction = 1;
    }

    //since we only sample for half a second , we might turn an extra amount, depending upon where we sampled the max.
    //but this doesn't matter if it's not the last beacon
    if (frequency[freq_to_detect] == LAST_FREQUENCY) {

    }

    //if finish is 1 or redo is 1 or tone_done is 1 break
    while (finish != 1 && redo != 1 && tone_done[freq_to_detect] != 1 ) {

      move_forward();

      //reset to 0
      for (int freq = 0; freq < NUM_FREQ; freq++) {
        freq_amp_1[freq] = 0;
        freq_amp_2[freq] = 0;
        freq_amp_3[freq] = 0;
      }

      for (int i = 0; i < 2; i++) {
        sample_data();
        for (int freq = 0; freq < NUM_FREQ; freq++) {
          freq_amp_1[freq] += total_avg[freq];
        }
      }
      turn(0, (360 / NUM_TURNS));
      for (int i = 0; i < 2; i++) {
        sample_data();
        for (int freq = 0; freq < NUM_FREQ; freq++) {
          freq_amp_2[freq] += total_avg[freq];
        }
      }
      turn(1, (2 * 360 / NUM_TURNS));
      for (int i = 0; i < 2; i++) {
        sample_data();
        for (int freq = 0; freq < NUM_FREQ; freq++) {
          freq_amp_3[freq] += total_avg[freq];
        }
      }
      if (freq_amp_2[freq_to_detect] > freq_amp_1[freq_to_detect] && freq_amp_2[freq_to_detect] > freq_amp_3[freq_to_detect]) {
        freq_amp_new[freq_to_detect] = freq_amp_2[freq_to_detect] / 2;
        turn(0, (2 * 360 / NUM_TURNS));
      }
      else if (freq_amp_1[freq_to_detect] > freq_amp_2[freq_to_detect] && freq_amp_1[freq_to_detect] > freq_amp_3[freq_to_detect]) {
        freq_amp_new[freq_to_detect] = freq_amp_1[freq_to_detect] / 2;
        turn(0, (360 / NUM_TURNS));
      }
      else {
        freq_amp_new[freq_to_detect] = freq_amp_3[freq_to_detect] / 2;
      }

      Serial.print("Frequency amplitude = ");
      Serial.println(freq_amp_new[freq_to_detect]);
      //amplitude should be increasing if we are moving in the right direction
      if ( freq_amp_new[freq_to_detect] < 0.7 * max_freq_amp[freq_to_detect]) redo = 1;

      if (redo != 1 && dist < 10) {
        tone_done[freq_to_detect] = 1;
        prev_freq_done = freq_to_detect;
        Serial.print("Finished searching for frequency ");
        Serial.println(freq_to_detect * 500 + 5000);
        if (frequency[freq_to_detect] == LAST_FREQUENCY) {
          finish = 1;
          Serial.println("This is the last frequency. Mission completed !!");
          digitalWrite(LED_PIN, 1);
        }
      }
    }

    if (finish == 1) {
      while (1);
    }
    invalid_freq_cnt = 0;
  } //freq INVALID
  else {
    uint16_t angle = 0;
    Serial.println("Could not find any frequency. Moving towards closest beacon. ");
    //assuming this happens try to look at the proximity data and then move forward
    for (int i = 0; i < NUM_TURNS; i++) {
      float data = prox_data[i];
      if (min_dist > data)angle = i * 360 / NUM_TURNS;
    }
    if (angle > 180) {
      turn(0, 360 - angle);
      direction = 0;
    }
    else {
      turn(1, angle);
      direction = 1;
    }

  }   

}

void move_forward() {
  dist      = detect_distance();

  if (dist > 400) dist = 0;

  Serial.print("Distance ");
  Serial.println(dist);
  if (dist > 30) {
    forward(20, 30);
  }
  else {
    if (frequency[freq_to_detect] != LAST_FREQUENCY) {
      if (dist > 15) {
        forward(20, dist - 15);
        Serial.println("Can't move too close");
      }
    }
    else {
      forward(20, dist);
      Serial.println("Moving towards last frequency ");
    }
  }

  //update distance again, since this is used by the main loop to
  //determine if we have reached the desired beacon or not
  dist       = detect_distance();
//  dist_left  = detect_distance_left();
//  dist_right = detect_distance_right();
  if (dist > 400) dist = 0;
  if (dist_left > 400) dist_left = 0;
  if (dist_right > 400) dist_right = 0;*/
}
