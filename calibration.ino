int value;
void calibration() {
  value = adc->analogRead(POT_PIN); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
  Serial.print("Pin: ");
  Serial.print(POT_PIN);
  Serial.print(", value: ");
  value = map(value, 0, 1023, 0, 90);
  Serial.println(value,DEC);
}

