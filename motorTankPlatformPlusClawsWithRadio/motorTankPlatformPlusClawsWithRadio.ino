#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Ultrasonic.h>

RF24 radio(A0, A1);
int data[4], dataTelemetry[2];
Servo servo1, servo2, servo3;
Ultrasonic ultrasonic(A2,A3);
int servoPin = 10;
int servoPin2 = 9;
int servoPin3 = 3;

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int trigPin = A2;
int echoPin = A3;

int valueX, valueY, valueSpeed = 255, revValueSpeed = 255, useClaws = -1, useCamera = -1; //set revValueSpeed = 100 for low batery
boolean isCameraLeft = false, isCameraRight = false, isCameraCenter = true;
boolean isServoAttached = false;
int centerPoint = 93, rightPoint = 74, leftPoint = 131, turnTimeout = 30, currentPosition = 0;
int centerHandPoint = 90, downHandPoint = 55, upHandPoint = 135, currentHandPosition = 0;
int cervoCenterSee = 86, servoRightSee = 20, servoLeftSee = 165, currentSeePosition = 0;
unsigned long CTime01;
unsigned long LTime01;
int distance;

void setup() {
  Serial.begin(9600);
  delay(100);

  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, 0x1234567895LL);
  radio.openWritingPipe(0x1234567895LL);
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
  currentSeePosition = cervoCenterSee;
  currentPosition = centerPoint;
  servo1.detach();
  servo2.detach();
  servo3.attach(servoPin3);
  servo3.write(centerHandPoint);
  currentHandPosition = centerHandPoint;
}

void loop() {
  if (radio.available()){
    radio.read(&data, sizeof(data));
    distance = ultrasonic.Ranging(CM);

    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];
    useClaws = data[3];

    if (useCamera == -1 && useClaws == -1){
      if (isServoAttached == true){
          servo1.detach();
          servo2.detach();
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
        case 9: 
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

  CTime01 = millis();
    if (CTime01 >= (LTime01 +120)) //Периодичность отправки пакетов
    {
       Serial.println("----------write-telemetry------------");
       Serial.println(distance);  
       radio.stopListening();  //Перестаем слушать
       dataTelemetry[0] = distance;
       dataTelemetry[1] = cervoCenterSee - currentSeePosition;
       radio.write(&dataTelemetry, sizeof(dataTelemetry)); // Отправляем ответ
       radio.startListening();
       LTime01 = CTime01;
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
     servo1.write(currentPosition);
     delay(turnTimeout);
     currentPosition++;
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
    servoSlowBackward(servo2,currentSeePosition, cervoCenterSee, turnTimeout);
  } else if (currentSeePosition < cervoCenterSee){
    servoSlowForward(servo2,currentSeePosition, cervoCenterSee, turnTimeout);
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
  
