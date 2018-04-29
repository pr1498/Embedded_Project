unsigned long time, duration;
unsigned long count = 0;
uint16_t degree = 0;

void tone_read() {
  float max_freq_amp[NUM_FREQ];
  uint16_t max_degree[NUM_FREQ];

  while (degree < 360) {
    //sampling for one sec
    time = millis();
    count = 0;
    duration = 0;
    for (int i = 0; i < NUM_FREQ; i++) freq_amp_new[i] = 0;

    while (duration < time + ONE_SEC) {
      float tone_read_val[NUM_FREQ];
      for (int i = 0; i < NUM_FREQ ; i++) {
        tone_read_val[i] = tone0[i].read() * 10000;
        freq_amp_new[i] = tone_read_val[i] + freq_amp_new[i];
      }
      count++;
      duration = millis();
    }

    for (int i = 0; i < NUM_FREQ; i++) {
      freq_amp_new[i] = freq_amp_new[i] / count;
      pattern[degree / 10][i] = freq_amp_new[i];

      //setting the value of max amplitude, and the angle at which max amplitude is seen
      if (freq_amp_new[i] > max_freq_amp[i]) {
        max_freq_amp[i] = freq_amp_new[i];
        max_degree[i] = degree;
        Serial.print("For frequency number ");
        Serial.print(i);
        Serial.print(" New max ampl ");
        Serial.println(freq_amp_new[i]);
        Serial.print("New max degree ");
        Serial.println(max_degree[i]);
      }
    }

    prox_data[degree / 10] = detect_distance();
    turn(1, 10); //turn left by 10 degrees, for 360 degrees
    degree += 10;

  }

  uint32_t pattern_avg = 0, max_freq_avg = 0, second_max_avg = 0, max_num = 0, second_max_num = 0;
  //now we can see which frequencies are in audible range
  for (int j = 0; j < NUM_FREQ; j++) {
    if (tone_done[j] != 1) {
      for (int i = 0; i < 37; i++) {
        pattern_avg += pattern[i][j];
      }
      if (pattern_avg > max_freq_avg) {
        max_freq_avg = pattern_avg;
        max_num = j;
      }
      else if (pattern_avg > second_max_avg) {
        second_max_avg = pattern_avg;
        second_max_num = j;
      }
      Serial.print("AVerage data for freq number ");
      Serial.print(j);
      Serial.print(" = ");
      Serial.println(pattern_avg / 37); //have to run this multiple times to find out threshold
    }
  }
  Serial.print("Max amplitude frequency = ");
  Serial.print(max_num);
  Serial.print(" Amplitude avg = ");
  Serial.println(max_freq_avg / 37);
  Serial.print("Second max frequency = ");
  Serial.print(second_max_num);
  Serial.print(" Amplitude avg = ");
  Serial.println(second_max_avg / 37);
  //if 1st freq is greater and is above threshold
  if (max_num > second_max_num && max_freq_avg > THRESHOLD_VAL) freq_to_detect = max_num;
  //if 2nd freq is greater and is above theshold
  else if (second_max_num > THRESHOLD_VAL) freq_to_detect = second_max_num;
  else freq_to_detect = INVALID_VAL; //invalid val, that means none are above threshold
}
