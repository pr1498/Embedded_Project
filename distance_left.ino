// defines variables for ultrasonic detector
int distance_left;

int detect_distance_left(){
  long duration;
  // Clears the trigPin
  digitalWrite(PROX_TRIG_PIN_left, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(PROX_TRIG_PIN_left, HIGH);
  delayMicroseconds(10);
  digitalWrite(PROX_TRIG_PIN_left, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(PROX_ECHO_PIN_left, HIGH);
  // Calculating the distance
  distance_left= duration*0.034/2;
  // Prints the distance on the Serial Monitor
//  Serial.print("Distance: ");
//  Serial.println(distance);
  return distance_left;
}
