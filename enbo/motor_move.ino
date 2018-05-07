#define TURN_TIME_LEFT_LAB 115 // 
#define TURN_TIME_RIGHT_LAB 100 //
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
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * 88 / 10);
    }
    else if(degree >= 30 && degree < 45)
    {
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * TURN_TIME_LEFT_LAB/ 10);
    }
    else if(degree >= 20 && degree < 30)
    {
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * 142/ 10);
    }
    else if(degree >= 10 && degree < 20)
    {
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * 145/ 10);
    }
    else if(degree >= 0 && degree < 10)
    {
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * 275/ 10);
    }
    else {
      servoLeft.write(115);
      servoRight.write(115);
      delay(degree * 80 / 10);     
    }
    stopcar();
  }
  
  else {
    if (degree >= 45 && degree <= 90)
    {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * 70 / 10);//57
    }
    else if(degree >= 30 && degree < 45)
    {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * TURN_TIME_RIGHT_LAB/ 10);
    }
    else if(degree >= 20 && degree < 30)
    {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * 115/ 10);
    }
    else if(degree >= 10 && degree < 20)
    {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * 140/ 10);
    }
    else if(degree >= 0 && degree < 10)
    {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * 270/ 10);
    }
    else {
      servoLeft.write(65);
      servoRight.write(65);
      delay(degree * 66 / 10);    
    }
    stopcar();
  }
}
