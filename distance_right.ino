// defines variables for ultrasonic detector
int distance_right;

int detect_distance_right(){
  long duration;
  // Clears the trigPin
  digitalWrite(PROX_TRIG_PIN_right, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(PROX_TRIG_PIN_right, HIGH);
  delayMicroseconds(10);
  digitalWrite(PROX_TRIG_PIN_right, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(PROX_ECHO_PIN_right, HIGH);
  // Calculating the distance
  distance_right= duration*0.034/2;
  // Prints the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.println(distance);
  return distance_right;
}
