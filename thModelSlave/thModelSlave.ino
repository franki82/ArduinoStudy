#include <Servo.h>
Servo servoBase, servo1, servo2, servo3, servo4, servoClaws;
int servoBasePin = 2, servoBaseStart = 90, servoBasePosition = 90, servoBaseRightBorder = 15, servoBaseLeftBorder = 180;
int servo1Pin = 3, servo1Start = 170, servo1Position = 170, servo1RightBorder = 30, servo1LeftBorder = 180;
int servo2Pin = 4, servo2Start = 140, servo2Position = 140, servo2RightBorder = 20, servo2LeftBorder = 155;
int servo3Pin = 5, servo3Start = 95, servo3Position = 95, servo3RightBorder = 0, servo3LeftBorder = 180;
int servo4Pin = 6, servo4Start = 30, servo4Position = 30, servo4RightBorder = 10, servo4LeftBorder = 180;
int servoClawsPin = 7, servoClawsStart = 120, servoClawsPosition = 120, servoClawsRightBorder = 80, servoClawsLeftBorder = 160;

String primaryString = "";
boolean bool2 = false;
int servoRightBorder = 10, servoLeftBorder = 180, turnTimeout = 30;
void setup() {
  Serial.begin(9600);
  delay(200);
  servoBase.attach(servoBasePin);
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo3.attach(servo3Pin);
  servo4.attach(servo4Pin);
  servoClaws.attach(servoClawsPin);
  delay(500);
  servoBase.write(servoBaseStart);
  delay(2000);
  servo1.write(servo1Start);
  delay(2000);
  servo2.write(servo2Start);
  delay(2000);
  servo3.write(servo3Start);
  delay(2000);
  servo4.write(servo4Start);
  delay(2000);
  servoClaws.write(servoClawsStart);
  delay(500);
}

void loop() {
  while (Serial.available() > 0){
    char c = Serial.read();
    if(c!='\n')  primaryString += c;
    else bool2=true;   
  }
  if (bool2){ 
    bool2=false;
    int myCommand = primaryString.toInt();

    switch (myCommand){
     case 10: 
        servoBasePosition = turnToUp(servoBase, servoBasePosition, servoBaseLeftBorder);
     break;
     case 11:
        servoBasePosition = turnToDown(servoBase, servoBasePosition, servoBaseRightBorder);
     break; 
     case 20:
        servo1Position = turnToUp(servo1, servo1Position, servo1LeftBorder);
     break;
     case 21:
        servo1Position = turnToDown(servo1, servo1Position, servo1RightBorder);
     break;
     case 30:
        servo2Position = turnToUp(servo2, servo2Position, servo2LeftBorder);
     break;
     case 31:
        servo2Position = turnToDown(servo2, servo2Position, servo2RightBorder);
     break;
     case 40:
        servo3Position = turnToUp(servo3, servo3Position, servo3LeftBorder);
     break;
     case 41:
        servo3Position = turnToDown(servo3, servo3Position, servo3RightBorder);
     break;
     case 50:
        servo4Position = turnToUp(servo4, servo4Position, servo4LeftBorder);
     break;
     case 51:
        servo4Position = turnToDown(servo4, servo4Position, servo4RightBorder);
     break;
     case 60:
        servoClawsPosition = turnToUp(servoClaws, servoClawsPosition, servoClawsLeftBorder);
     break;
     case 61:
        servoClawsPosition = turnToDown(servoClaws, servoClawsPosition, servoClawsRightBorder);
     break;
     case 77:
      servo1Position = turnToUp(servo1, servo1Position, servo1Start);
      servo2Position = turnToUp(servo2, servo2Position, servo2Start);    
      servo4Position = turnToDown(servo4, servo4Position, servo4Start);
      if (servoBasePosition > 90){
        servoBasePosition = turnToDown(servoBase, servoBasePosition, servoBaseStart);
      } else{
        servoBasePosition = turnToUp(servoBase, servoBasePosition, servoBaseStart);
      }
      if(servo3Position > 95){
        servo3Position = turnToDown(servo3, servo3Position, servo3Start);
      } else {
        servo3Position = turnToUp(servo3, servo3Position, servo3Start);
      }
      if (servoClawsPosition > 120){
        servoClawsPosition = turnToDown(servoClaws, servoClawsPosition, servoClawsStart);
      } else {
        servoClawsPosition = turnToUp(servoClaws, servoClawsPosition, servoClawsStart);
      }
      //for parking
     break;
    }

    primaryString = "";
  }

}

int turnToUp(Servo servo, int currentServoPosition, int servoLeftBorder){
  if (currentServoPosition < servoLeftBorder){
     servo.write(currentServoPosition);
     delay(turnTimeout);
     currentServoPosition++;
  }
  return currentServoPosition;  
}

int turnToDown(Servo servo, int currentServoPosition, int servoRightBorder){
  if (currentServoPosition > servoRightBorder){
    servo.write(currentServoPosition);
    delay(turnTimeout);
    currentServoPosition--;
  }
  return currentServoPosition;
}
