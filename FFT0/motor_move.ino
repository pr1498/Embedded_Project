#define TURN_TIME_LEFT 115 // 51for maker space 120 for left less rthan 45
#define TURN_TIME_RIGHT 125 //
#define RIGHT_WHEEL_FORWARD_SPEED -1.7//or 1.6


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
      servoLeft.write(134);
      servoRight.write(134);
      delay(degree * 60 / 10);
    }
    else if(degree < 45)
    {
      servoLeft.write(120);
      servoRight.write(120);
      delay(degree * TURN_TIME_LEFT / 10);
    }
    else {
      servoLeft.write(116);
      servoRight.write(116);
      delay(degree * 100 / 10);     
    }
    stopcar();
  }
  else {
    if (degree >= 45 && degree <= 90)
    {
      servoLeft.write(61);
      servoRight.write(61);
      delay(degree * 80 / 10);//57
    }
    else if(degree < 45)
    {
      servoLeft.write(63.8);
      servoRight.write(63.8);
      delay(degree * TURN_TIME_RIGHT / 10);
    }
    else {
      servoLeft.write(66);
      servoRight.write(66);
      delay(degree * 90 / 10);    
    }
    stopcar();
  }
}
