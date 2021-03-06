#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

RF24 radio(9, 10);
int data[3];
Servo servo1;
int servoPin = 3;

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 255, revValueSpeed = 100, useCamera = -1;
boolean isCameraLeft = false, isCameraRight = false, isCameraCenter = true;
boolean isServoAttached = false;
int centerPoint = 95, rightPoint = 8, leftPoint = 180, turnTimeout = 60, currentPosition = 0;

void setup() {
  Serial.begin(9600);
  delay(100);

  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, 0x1234567890LL);
  radio.startListening();
  
  pinMode(enG1, OUTPUT);
  pinMode(enG2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  servo1.attach(servoPin);
  delay(200);
  servo1.write(centerPoint);
  delay(200);
  currentPosition = centerPoint;
  servo1.detach();
}

void loop() {
  if (radio.available()){
    radio.read(&data, sizeof(data));

    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];

    if (useCamera == -1){
      if (isServoAttached == true){
          servo1.detach();
          isServoAttached = false;
      }

         if (abs(valueX)<8){
          valueX = 0;
        }
    
        if (abs(valueY)<8){
          valueY = 0;
          stopEngine();
        }
        
        switch (valueX){
        case 10: 
          rightEngine();
          break;
        case -10: 
          leftEngine();
          break;
        }
    
        switch (valueY){
        case 10:
            forwardEngine();
          break;
        case -10: 
            backwardEngine();
          break;
        }
    } else{
        if (isServoAttached == false){
          servo1.attach(servoPin);
          isServoAttached = true;
          centerCamera();
          }
       if (abs(valueX)<8){
          valueX = 0;
        }

        if (abs(valueY)<8){
          valueY = 0;
        }
              
        switch (valueX){
        case 10: 
          rightCamera();
          break;
        case -10: 
          leftCamera();
          break;
        }
    
        switch (valueY){
        case 10: 
            centerCamera();
          break;
        }
    }
   
  }

}

void forwardEngine(){
  analogWrite(enG1, valueSpeed * 0.8);
  analogWrite(enG2, valueSpeed * 0.8);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
    
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine(){
  analogWrite(enG1, valueSpeed * 0.8);
  analogWrite(enG2, valueSpeed * 0.8);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void leftEngine(){
  analogWrite(enG1, revValueSpeed);
  analogWrite(enG2, valueSpeed);
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void rightEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, revValueSpeed);
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void servoSlowForward( Servo num, int startPos, int endPos, int time)
{ 
  num.write(startPos);
  int duration;
  delay(20);  
  for ( int i=startPos; i<endPos; i++) 
      {  num.write(i);
         delay(time);
      }
}

void servoSlowBackward( Servo num, int startPos, int endPos, int time)
{ 
  num.write(startPos);
  int duration;
  delay(20);  
  for ( int i=startPos; i>endPos; i--) 
      {  num.write(i);
         delay(time);
      }
}

void centerCamera(){
  if (currentPosition > centerPoint){
    servoSlowBackward(servo1,currentPosition, centerPoint-1, turnTimeout);
  } else if (currentPosition < centerPoint){
    servoSlowForward(servo1,currentPosition, centerPoint+3, turnTimeout);
   }
   currentPosition = centerPoint;
 }

void leftCamera(){
  if (currentPosition < leftPoint){
     servo1.write(currentPosition);
     delay(turnTimeout);
     currentPosition++;
  }  
}


void rightCamera(){
  if (currentPosition > rightPoint){
    servo1.write(currentPosition);
    delay(turnTimeout);
    currentPosition--;
  }
}
