//--- Analyze FFT data, make decisions about which frequency to detect---//
unsigned long count = 0;

void tone_read() {

  //initialization to 0s
  for (int i = 0; i < NUM_FREQ; i++) {
    max_freq_amp[i] = 0;
    max_degree[i] = 0;
    freq_amp_new[i] = 0;
    for (int j = 0; j < NUM_TURNS; j++) {
      pattern[j][i] = 0;
      pattern_scaled[j][i] = 0;
    }
  }

  for (int turns = 0; turns < NUM_TURNS; turns++) {
    sample_data(); //fill the data buffers with fft

    //analyze data
    for (int i = 0; i < NUM_FREQ; i++) {
      freq_amp_new[i] = total_avg[i];
      pattern[turns][i] = freq_amp_new[i];
      //        Serial.println(freq_amp_new[i]);
      //        Serial.print("  ");
      //setting the value of max amplitude, and the angle at which max amplitude is seen
      if (freq_amp_new[i] >= max_freq_amp[i]) {
        max_freq_amp[i] = freq_amp_new[i];
        max_degree[i] = turns * 360 / NUM_TURNS;
        Serial.print("For frequency number ");
        Serial.print(i * 500 + 5000 );
        Serial.print(" New max ampl ");
        Serial.println(freq_amp_new[i]);
        Serial.print("New max degree ");
        Serial.println(max_degree[i]);
      }
    }

    prox_data[turns] = detect_distance();

    turn(1, (360 / NUM_TURNS)); //turn left by 10 degrees, for 360 degrees

  }

  uint32_t max_freq_avg = 0, second_max_avg = 0, max_num = 0, second_max_num = 0;

  //now we can see which frequencies are in audible range
  for (int j = 0; j < NUM_FREQ; j++) {
    if (tone_done[j] != 1 && j > prev_freq_done) {
      if ( max_freq_amp[j] > max_freq_avg && max_freq_amp[j] > threshold_val[j]) {
        max_freq_avg = max_freq_amp[j];
        max_num = j;
      }
      else if (max_freq_amp[j] > second_max_avg && max_freq_amp[j] > threshold_val[j]) {
        second_max_avg = max_freq_amp[j];
        second_max_num = j;
      }
    }
  }

  Serial.print("Max amplitude frequency = ");
  Serial.print(max_num * 500 + 5000);
  Serial.print(" Amplitude avg = ");
  Serial.println(max_freq_avg);
  Serial.print("Second max frequency = ");
  Serial.print(second_max_num * 500 + 5000);
  Serial.print(" Amplitude avg = ");
  Serial.println(second_max_avg);

  //if can't find any frequency just search for next
  if (max_freq_avg == 0 ) {
    freq_to_detect = INVALID_VAL;
  }
  else {
    freq_to_detect = max_num;
  }

  Serial.print("Frequency to detect = ");
  Serial.println( 500 * freq_to_detect + 5000);

}
