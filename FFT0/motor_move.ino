#define TURN_TIME_45 130 //151 for maker space 
#define RIGHT_WHEEL_FORWARD_SPEED 5

void forward(int speed, int distance)
{
  servoLeft.write(90 - speed);
  servoRight.write(90 + RIGHT_WHEEL_FORWARD_SPEED + speed);
  if (speed == 20) delay(100 * distance);
  else if (speed == 90) delay(27 * distance);
  stopcar();
}

void reverse(int speed, int distance)
{
  servoLeft.write(90 + speed);
  servoRight.write(90 - 5 - speed);
  if (speed == 20) delay(100 * distance);
  else if (speed == 90) delay(27 * distance);
  stopcar();
}

void turnLeft()
{
  servoLeft.write(90);
  servoRight.write(120);
}
void turnRight()
{
  servoLeft.write(0);
  servoRight.write(90);
}

void stopcar()
{
  servoLeft.write(90);
  servoRight.write(90);
}

void turnLeftdegree(int degree)
{
  if (degree >= 45)
  {
    servoLeft.write(118);
    servoRight.write(118);
    delay(degree * 100 / 10);
  }
  else
  {
    servoLeft.write(110);
    servoRight.write(110);
    delay(degree * 200 / 10);
  }
  stopcar();
}

void turnRightdegree(int degree)
{
  if (degree >= 45)
  {
    servoLeft.write(62);
    servoRight.write(62);
    delay(degree * 100 / 10);
  }
  else
  {
    servoLeft.write(70);
    servoRight.write(70);
    delay(degree * 200 / 10);
  }
  stopcar();
}

void turn(bool direction, int degree) {
  if (direction == 1) {
    if (degree >= 45 && degree <= 90)
    {
      servoLeft.write(118);
      servoRight.write(118);
      delay(degree * 66 / 10);
    }
    else if(degree < 45)
    {
      servoLeft.write(112);
      servoRight.write(112);
      delay(degree * TURN_TIME_45 / 10);
    }
    else {
      servoLeft.write(116);
      servoRight.write(116);
      delay(degree * 63 / 10);     
    }
    stopcar();
  }
  else {
    if (degree >= 45 && degree <= 90)
    {
      servoLeft.write(62);
      servoRight.write(62);
      delay(degree * 57 / 10);
    }
    else if(degree < 45)
    {
      servoLeft.write(70);
      servoRight.write(70);
      delay(degree * TURN_TIME_45 / 10);
    }
    else {
      servoLeft.write(66);
      servoRight.write(66);
      delay(degree * 60 / 10);    
    }
    stopcar();
  }
}
