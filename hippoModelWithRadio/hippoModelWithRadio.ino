#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

RF24 radio(A0, A1);
int data[8], dataTelemetry[5];

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, revValueSpeed = 5, shortTurn = -1, correctSpeed = -1, correctTurn = -1;
int analogVoltmeterInput = A2;
int vin = 1; 
float R1 = 20000.0, R2 = 10000.0;
int leftMotorPersentage = 100, rightMotorPersentage = 100;
int turnTimeout = 20;

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
  pinMode(analogVoltmeterInput, INPUT);
}

void loop() {
  delay(5);
  vin = ((analogRead(analogVoltmeterInput) * 50.0) / 1024.0)/(R2/(R1+R2));
  
  dataTelemetry[0] = leftMotorPersentage;
  dataTelemetry[1] = rightMotorPersentage;
  dataTelemetry[2] = valueSpeed;
  dataTelemetry[3] = revValueSpeed;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

  if (radio.available()){
    radio.read(&data, sizeof(data));    
    valueX = data[0];
    valueY = data[1];
    correctSpeed = data[3];
    valueSpeed = data[4];
    correctTurn = data[5];
    shortTurn = data[6];
    
    if (correctSpeed == -1 && correctTurn == -1){
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
        } else if (valueX == 0 && valueY > 2 && valueY < 9){
            valueSpeed = valueSpeed * valueY / 10;
            if (valueSpeed < 70){
              valueSpeed = 70;
              }
            forwardEngine();
        } else if (valueX == 0 && valueY < -2 && valueY > -9) {
          valueSpeed = valueSpeed * valueY / 10;
          if (valueSpeed < 70) {
            valueSpeed = 70;
          }
          backwardEngine();
      }
        
    } 
    else if (correctSpeed == 1 && correctTurn == -1){
      if (valueX == 10 && valueY == 0){
        rightEnginePowerChange();
      } else if (valueX == -10 && valueY == 0){
        leftEnginePowerChange();
      } else if (valueX == 0 && valueY == 10){
        enginePowerChangeToFull();
      }
    } else if (correctSpeed == -1 && correctTurn == 1){
      if (valueX == 10 && valueY == 0){
        enlargeTurnValue();
      } else if (valueX == -10 && valueY == 0){
        decreaseTurnValue();
      }
    }
   
  }
}


void forwardEngine(){
  analogWrite(enG1, valueSpeed * leftMotorPersentage / 100);
  analogWrite(enG2, valueSpeed * rightMotorPersentage / 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine(){
  analogWrite(enG1, valueSpeed * leftMotorPersentage / 100);
  analogWrite(enG2, valueSpeed * rightMotorPersentage / 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void leftEngine(){
  if (shortTurn == 1) {
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, valueSpeed);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else {
    analogWrite(enG1, revValueSpeed);
    analogWrite(enG2, valueSpeed);  
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
}

void rightEngine(){
  if (shortTurn == 1) {
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, valueSpeed);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else{ 
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, revValueSpeed); 
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
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

void rightEnginePowerChange(){
    if (rightMotorPersentage <= 100 && rightMotorPersentage > 50){
        rightMotorPersentage--;
    } else {
        rightMotorPersentage = 100;
        delay(2000);
    }
  }

  void leftEnginePowerChange(){
    if (leftMotorPersentage <= 100 && leftMotorPersentage > 50){
        leftMotorPersentage--;
    } else {
        leftMotorPersentage = 100;
        delay(2000);
    }
  }

  void enginePowerChangeToFull(){
    rightMotorPersentage = 100;
    leftMotorPersentage = 100;
    delay(2000);
  }

  void enlargeTurnValue(){
    if (revValueSpeed < 255 && revValueSpeed >= 0){
        revValueSpeed++;
    } else {
        revValueSpeed = 255;
        delay(2000);
    }
  }

  void decreaseTurnValue(){
    if (revValueSpeed <= 255 && revValueSpeed > 0){
        revValueSpeed--;
    } else {
        revValueSpeed = 0;
        delay(2000);
    }
  }
  
