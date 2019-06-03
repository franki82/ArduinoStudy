#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Ultrasonic.h>

RF24 radio(A0, A1);
int data[8], dataTelemetry[4];
Servo servo1, servo2, servo3;
Ultrasonic ultrasonic(A2,A3);
int servoPin = 10;
int servoPin2 = 9;
int servoPin3 = 3;
int pressAnalogPin = A5;

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int trigPin = A2;
int echoPin = A3;

int valueX, valueY, valueSpeed = 0, revValueSpeed = 0, useClaws = -1, useCamera = -1; //set revValueSpeed = 100 for low batery
boolean isCameraLeft = false, isCameraRight = false, isCameraCenter = true;
boolean isServoAttached = false;
int centerPoint = 93, rightPoint = 75, leftPoint = 128, turnTimeout = 30, currentPosition = 0, pressLimit = 400; //claws
int centerHandPoint = 85, downHandPoint = 45, upHandPoint = 130, currentHandPosition = 0; //hand
int cervoCenterSee = 83, servoRightSee = 20, servoLeftSee = 165, currentSeePosition = 0; //camera
unsigned long CTime01;
unsigned long LTime01;
int distance;
int pressReading;

void setup() {
  Serial.begin(9600);
  delay(100);

  radio.begin();
  radio.enableAckPayload();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, 0x1234567895LL);
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
  servo2.attach(servoPin2);
  delay(200);
  servo2.write(cervoCenterSee);
  delay(200);
  currentSeePosition = cervoCenterSee;
  currentPosition = centerPoint;
  servo1.detach();
  servo2.detach();
  servo3.attach(servoPin3);
  servo3.write(centerHandPoint);
  delay(200);
  currentHandPosition = centerHandPoint;
}

void loop() {
//  Serial.println("Press value");
//  Serial.println(pressReading);
  int distance1=0, distance2=0, distance3=0;
  distance1 = ultrasonic.Ranging(CM);
  distance2 = ultrasonic.Ranging(CM);
  distance3 = ultrasonic.Ranging(CM);
  distance = (distance1 + distance2 + distance3)/3;
  pressReading = analogRead(pressAnalogPin);

  dataTelemetry[0] = distance;
  dataTelemetry[1] = cervoCenterSee - currentSeePosition;
  dataTelemetry[2] = currentHandPosition;
  dataTelemetry[3] = currentPosition;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));
  
  if (radio.available()){
    radio.read(&data, sizeof(data));    
    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];
    useClaws = data[3];
    valueSpeed = data[4];
    revValueSpeed = data[4];
    

    if (useCamera == -1 && useClaws == -1){
      if (isServoAttached == true){
          servo1.detach();
          servo2.detach();
          isServoAttached = false;
      }

      if (valueX == 0 && valueY == 0){
            stopEngine();
      } else if (valueX == 0 && valueY == 10){
            forwardEngine();
      } else if (valueX == 0 && valueY == -10){
            backwardEngine();  
      } else if (valueX == 10 && valueY == 0){
            rightEngine();  
      } else if (valueX == -10 && valueY == 0){
            leftEngine();
      }

    } else if (useClaws == 1){
        useCamera = -1;
        if (isServoAttached == false){
          servo1.attach(servoPin);
          isServoAttached = true;
          }
       if (abs(valueX)<8){
          valueX = 0;
        }

        if (abs(valueY)<8){
          valueY = 0;
        }
              
        switch (valueX){
        case 10: 
          openClaws();
          break;
        case -10: 
          closeClaws();
        break;
        }
    
        switch (valueY){
        case 10: 
            upHand();
          break;
        case -10: 
            downHand();
          break;
        }
    } else if (useCamera == 1){
        useClaws = -1;
        if (isServoAttached == false){
          servo2.attach(servoPin2);
          isServoAttached = true;
          }
       if (abs(valueX)<8){
          valueX = 0;
        }

        if (abs(valueY)<8){
          valueY = 0;
        }
              
        switch (valueX){
        case 10: 
          cameraToRight();
          break;
        case -10: 
          cameraToLeft();
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

void clawsToMedium(){
  if (currentPosition > centerPoint){
    servoSlowBackward(servo1,currentPosition, centerPoint, turnTimeout);
  } else if (currentPosition < centerPoint){
    servoSlowForward(servo1,currentPosition, centerPoint+8, turnTimeout);
   }
   currentPosition = centerPoint;
 }

void closeClaws(){
  if (currentPosition < leftPoint){
    if (pressReading < pressLimit){
     //Serial.println("Press value");
     //Serial.println(pressReading);
     servo1.write(currentPosition);
     delay(turnTimeout);
     currentPosition++;
    }
  }  
}


void openClaws(){
  if (currentPosition > rightPoint){
    servo1.write(currentPosition);
    delay(turnTimeout);
    currentPosition--;
  }
}

void cameraToLeft(){
  if (currentSeePosition < servoLeftSee){
     servo2.write(currentSeePosition);
     delay(turnTimeout);
     currentSeePosition++;
  }  
}

void cameraToRight(){
  if (currentSeePosition > servoRightSee){
    servo2.write(currentSeePosition);
    delay(turnTimeout);
    currentSeePosition--;
  }
}

void centerCamera(){
  if (currentSeePosition > cervoCenterSee){
    servoSlowBackward(servo2,currentSeePosition, cervoCenterSee-2, turnTimeout);
  } else if (currentSeePosition < cervoCenterSee){
    servoSlowForward(servo2,currentSeePosition, cervoCenterSee+2, turnTimeout);
   }
   currentSeePosition = cervoCenterSee;
 }

 void downHand(){
  if (currentHandPosition > downHandPoint){
    servo3.write(currentHandPosition);
    delay(turnTimeout);
    currentHandPosition--;
  }
}

void upHand(){
  if (currentHandPosition < upHandPoint){
     servo3.write(currentHandPosition);
     delay(turnTimeout);
     currentHandPosition++;
  }  
}
  
