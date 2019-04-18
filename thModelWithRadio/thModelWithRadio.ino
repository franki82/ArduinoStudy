#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

RF24 radio(A0, A1);
int data[5], dataTelemetry[4];
Servo servo2;
int servoPin2 = 9;

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, revValueSpeed = 0, longTurn = -1, useCamera = -1; //set revValueSpeed = 100 for low batery
boolean isCameraLeft = false, isCameraRight = false, isCameraCenter = true;
boolean isServoAttached = false;
int cervoCenterSee = 90, servoRightSee = 20, servoLeftSee = 165, turnTimeout = 30, currentSeePosition = 0; //camera
unsigned long CTime01;
unsigned long LTime01;
VL53L0X sensor;

void setup() {
  Serial.begin(9600);
  Wire.begin();
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

  servo2.attach(servoPin2);
  delay(200); 
  servo2.write(cervoCenterSee);
  delay(200);
  servo2.detach(); 
  currentSeePosition = cervoCenterSee;

  sensor.init();
  sensor.setTimeout(500);
  sensor.setSignalRateLimit(0.1);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensor.setMeasurementTimingBudget(20000);
  sensor.setMeasurementTimingBudget(40000);
}

void loop() {
  //delay(10);
  int primaryDistanseMM = 0;
  if (sensor.timeoutOccurred()) { 
    primaryDistanseMM = 10; 
  } else{
    primaryDistanseMM = sensor.readRangeSingleMillimeters();
  }
  dataTelemetry[0] = primaryDistanseMM / 10;
  dataTelemetry[1] = cervoCenterSee - currentSeePosition;
  dataTelemetry[2] = 1;
  dataTelemetry[3] = 1;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));
  
  if (radio.available()){
    radio.read(&data, sizeof(data));    
    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];
    longTurn = data[3];
    valueSpeed = data[4];
    revValueSpeed = data[4];
    

    if (useCamera == -1){
        if (isServoAttached == true){
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
            if (longTurn == 1){
              revValueSpeed = 50;
            }
            rightEngine();  
        } else if (valueX == -10 && valueY == 0){
            if (longTurn == 1){
              revValueSpeed = 50;
            }
            leftEngine();
        }
        
    } 
    else if (useCamera == 1){
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
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed * 0.75);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
    
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed * 0.75);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void leftEngine(){
  analogWrite(enG1, revValueSpeed);
  analogWrite(enG2, valueSpeed);

  if (longTurn == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else {  
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
}

void rightEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, revValueSpeed);

  if (longTurn == 1) {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else{  
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  }
}

void stopEngine(){
  analogWrite(enG1, 0);
  analogWrite(enG2, 0);
  
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
    servoSlowForward(servo2,currentSeePosition, cervoCenterSee, turnTimeout);
   }
   currentSeePosition = cervoCenterSee;
 }
  
