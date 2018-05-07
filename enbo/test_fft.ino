/**
unsigned long cnt = 0;
unsigned long avg[NUM_FREQ];

void loop() {

  cnt++;
//  for (int i = 0 ; i < 100; i++) {
    sample_data();
  //  for (int j = 0; j < NUM_FREQ; j++) {
  //    avg[0] += total_avg[0];
      Serial.print(8 * 500 + 5000);
      Serial.print("  Average data ");
      Serial.println(total_avg[8]);
   // }
//  }
//  for (int j = 0; j < NUM_FREQ; j++) {
 //   avg[j] = avg[j] / 100;
//    Serial.print(j * 500 + 5000);
//    Serial.print("  Average data ");
//    Serial.println(avg[j]);
 // }
 // while (1);
}
*/
