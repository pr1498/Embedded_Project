//int right_speed;
//int rotate_speed_left;
//int rotate_speed_right;
//
//void calibration() {
//  int value;
////  value = adc->analogRead(POT_PIN); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
//  Serial.print("Pin: ");
//  Serial.print(POT_PIN);
//  Serial.print(", value: ");
//  right_speed = value * 70 / adc->getMaxValue(ADC_0);
//  Serial.println(right_speed, DEC);
//
//  forward(20, 25);
//  delay(5000);
//
//}
//
//void rotate_left() {
//  int value;
//  value = adc->analogRead(POT_PIN); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
//  Serial.print("Pin: ");
//  Serial.print(POT_PIN);
//  Serial.print(", value: ");
//  rotate_speed_left = value * 90 / adc->getMaxValue(ADC_0);
//  Serial.println(rotate_speed_left, DEC);
//
//  for (int i = 0; i < 12; i++) {
//    turn(1, 30);
//    delay(50);
//  }
//
//  delay(5000);
//}
//
//void rotate_right() {
//  int value;
//  value = analogRead(POT_PIN); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
//  Serial.print("Pin: ");
//  Serial.print(POT_PIN);
//  Serial.print(", value: ");
//  rotate_speed_right = value * 90 / 1024;
//  Serial.println(rotate_speed_left, DEC);
//
//  for (int i = 0; i < 12; i++) {
//    turn(0, 30);
//    delay(50);
//  }
//
//  delay(5000);
//}
