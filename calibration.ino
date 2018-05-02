int value;
void calibration() {
  value = adc->analogRead(POT_PIN); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
  Serial.print("Pin: ");
  Serial.print(POT_PIN);
  Serial.print(", value ADC1: ");
  Serial.println(value * 3.3 / adc->getMaxValue(ADC_1), DEC);
  
  int speedva;
  int sensorValue = analogRead(calibratepin);
  float voltage = sensorValue  * (5.0 / 1023.0);
  speedva = 90 * (voltage / 3.3);

  return speedva;
}

