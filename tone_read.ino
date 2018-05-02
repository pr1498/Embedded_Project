unsigned long count = 0;
  
void tone_read() {

  //initialization to 0s
  for (int i = 0; i < NUM_FREQ; i++) {
    max_freq_amp[i] = 0;
    max_degree[i] = 0;
    freq_amp_new[i] = 0;
  }

  for (int turns = 0; turns < NUM_TURNS; turns++) {
    for (int i = 0; i < NUM_FREQ; i++) {
      freq_amp_new[i] = sample_data(i);
      digitalWrite(LED_PIN, 0); //is turned on when a maxima is seen
      pattern[turns][i] = freq_amp_new[i];
      //        Serial.println(freq_amp_new[i]);
      //        Serial.print("  ");
      //setting the value of max amplitude, and the angle at which max amplitude is seen
      if (freq_amp_new[i] > max_freq_amp[i]) {
        digitalWrite(LED_PIN, 1);
        max_freq_amp[i] = freq_amp_new[i];
        max_degree[i] = turns * 360/NUM_TURNS;
        Serial.print("For frequency number ");
        Serial.print(i);
        Serial.print(" New max ampl ");
        Serial.println(freq_amp_new[i]);
        Serial.print("New max degree ");
        Serial.println(max_degree[i]);
      }
    }

    prox_data[turns] = detect_distance();
    Serial.print("Distance ");
    Serial.println(prox_data[turns]);
    
    turn(1, (360 / NUM_TURNS)); //turn left by 10 degrees, for 360 degrees

  }

  uint32_t max_freq_avg = 0, second_max_avg = 0, max_num = 0, second_max_num = 0;
  //now we can see which frequencies are in audible range
  for (int j = 0; j < NUM_FREQ; j++) {
    if (tone_done[j] != 1) {
      if ( max_freq_amp[j] > max_freq_avg) {
        max_freq_avg = max_freq_amp[j];
        max_num = j;
      }
      else if (max_freq_amp[j] > second_max_avg) {
        second_max_avg = max_freq_amp[j];
        second_max_num = j;
      }
    }
  }
  
  Serial.print("Max amplitude frequency = ");
  Serial.print(max_num);
  Serial.print(" Amplitude avg = ");
  Serial.println(max_freq_avg / NUM_TURNS);
  Serial.print("Second max frequency = ");
  Serial.print(second_max_num);
  Serial.print(" Amplitude avg = ");
  Serial.println(second_max_avg / NUM_TURNS);

  
  //if 1st freq is greater and is above threshold
  if (max_num >= second_max_num && max_freq_avg > THRESHOLD_VAL) freq_to_detect = max_num;
  //if 2nd freq is greater and is above theshold
  else if (second_max_avg > THRESHOLD_VAL) freq_to_detect = second_max_num;
  else freq_to_detect = INVALID_VAL; //invalid val, that means none are above threshold
  Serial.print("Frequency to detect = ");
  Serial.println(freq_to_detect);
  
}
