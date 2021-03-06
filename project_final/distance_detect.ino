// defines variables for ultrasonic detector
int distance;

int detect_distance() {
  long duration;
  // Clears the trigPin
  digitalWrite(PROX_TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(PROX_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(PROX_TRIG_PIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(PROX_ECHO_PIN, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
  // Prints the distance on the Serial Monitor
  //  Serial.print("Distance: ");
  //  Serial.println(distance);
  return distance;
}

