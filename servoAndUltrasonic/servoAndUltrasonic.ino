#include <Servo.h>
#include <Ultrasonic.h>
Servo servo1;
Ultrasonic ultrasonic(8,9);
int val;

void setup() {
  Serial.begin(9600);
  servo1.attach(6);
  servo1.write(90);
}

void loop() {
  int servoPosition;
  int duration;
  if (Serial.available() > 0) {
    val = Serial.read();
    if (val == '1'){
      Serial.println(97);
      servo1.write(97);
      delay(20);
    }
    else if (val == '2'){
      Serial.println(32);
      servo1.write(32);
      delay(20);
    }
    else if (val == '3'){
      Serial.println(165);
      servo1.write(165);
      delay(20);
    }
    else if (val == '4'){
      Serial.println("Scan");
      servoslow(servo1, 165, 50, 32);
    }
 
  }
duration = ultrasonic.Ranging(CM);
Serial.println(duration);  
delay(100);
}

void servoslow( Servo num, int pos, int time, int start)
{ 
  num.write(start);
  int duration;
  delay(20);  
  for ( int i=start; i<pos; i++) 
      {  num.write(i);
          duration = ultrasonic.Ranging(CM);
          Serial.println(duration); 
         delay(time);
      }
}
