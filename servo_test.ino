void servo_test() { 
  
    forward(20, 30); //range should be between 10 and 90
    delay(500);
turn(1, 30);
    delay(1000);
    turn(1, 60);
    delay(1000);
 
 
    turn(1, 90);
    delay(1000);
 
    turn(1, 180);

    turn(0, 30);
    delay(1000);
    turn(0, 60);
    delay(1000);
 
 
    turn(0, 90);
    delay(1000);
 
    turn(0, 180);
}
