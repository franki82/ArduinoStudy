#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Servo.h>

Servo servoHorizontal, servoVertical;

RF24 radio(A0, A1);
int data[8], dataTelemetry[5];

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, revValueSpeed = 0, longTurn = -1, useCamera = -1, correctSpeed = -1, turnTurrel = -1, fireState = -1;
int analogVoltmeterInput = A2;
int vin = 1; 
float R1 = 30000.0, R2 = 7500.0;
int leftMotorPersentage = 100, rightMotorPersentage = 100;
int analogPower = 3, lowPower = 20, maximumPower = 255;
int horizontalServoPin = 9, horizontalServoStart = 90, horizontalCurrentPosition = 90, horizontalServoRightBorder = 15, horizontalServoLeftBorder = 175;
int verticalServoPin = 10, verticalServoStart = 90, verticalCurrentPosition = 90, verticalServoRightBorder = 15, verticalServoLeftBorder = 175;
int turnTimeout = 10;

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
  
  pinMode(analogPower, OUTPUT);
  analogWrite(analogPower, 0);

  servoHorizontal.attach(horizontalServoPin);
  delay(500);
  servoHorizontal.write(horizontalServoStart);
  delay(500);
  servoVertical.attach(verticalServoPin);
  delay(500);
  servoVertical.write(verticalServoStart);
  delay(500);
}

void loop() {
  delay(10);
  vin = ((analogRead(analogVoltmeterInput) * 50.0) / 1024.0)/(R2/(R1+R2));
  
  dataTelemetry[0] = leftMotorPersentage;
  dataTelemetry[1] = rightMotorPersentage;
  dataTelemetry[2] = 1;
  dataTelemetry[3] = 1;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

//  Serial.print("Voltage:");
//  Serial.println(vin);
  
  if (radio.available()){
    radio.read(&data, sizeof(data));    
    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];
    longTurn = data[3];
    valueSpeed = data[4];
    revValueSpeed = data[4];
    correctSpeed = data[5];
    turnTurrel = data[6];
    fireState = data[7];
    

    if (correctSpeed == -1 && turnTurrel == -1){
        
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
        } else if (valueX == 0 && valueY > 2 && valueY < 10){
            valueSpeed = valueSpeed * valueY / 10;
            if (valueSpeed < 70){
              valueSpeed = 70;
              }
            forwardEngine();
        } else if (valueY == 0 && valueX > 2 && valueX < 10){
            valueSpeed = valueSpeed * valueX / 10;
            if (longTurn == 1){
              revValueSpeed = 50;
            }
            rightEngine();  
        } else if (valueY == 0 && valueX < -2 && valueX > -10){
            int absValueX = abs(valueX);
            valueSpeed = valueSpeed * absValueX / 10;
            if (longTurn == 1){
              revValueSpeed = 50;
            }
            leftEngine();
        }
        analogWrite(analogPower, 0);

//          Serial.print("Speed:");
//          Serial.println(valueSpeed);

    } else if (correctSpeed == 1 && turnTurrel == -1){
      if (valueX == 10 && valueY == 0){
        rightEnginePowerChange();
      } else if (valueX == -10 && valueY == 0){
        leftEnginePowerChange();
      } else if (valueX == 0 && valueY == 10){
        enginePowerChangeToFull();
      }
      analogWrite(analogPower, 0);
    } else if (correctSpeed == -1 && turnTurrel == 1){
      if(fireState == 1){
        analogWrite(analogPower, maximumPower);
      } else if (fireState == -1){
        analogWrite(analogPower, lowPower);
      }
      //move turrel code
      if (valueX == -10 && valueY == 0){
        horizontalCurrentPosition = turnToUp(servoHorizontal, horizontalCurrentPosition, horizontalServoLeftBorder);
      } else if (valueX == 10 && valueY == 0){
        horizontalCurrentPosition = turnToDown(servoHorizontal, horizontalCurrentPosition, horizontalServoRightBorder);
      } else if (valueX == 0 && valueY == -10){
        verticalCurrentPosition = turnToDown(servoVertical, verticalCurrentPosition, verticalServoRightBorder);
      } else if (valueX == 0 && valueY == 10){
        verticalCurrentPosition = turnToUp(servoVertical, verticalCurrentPosition, verticalServoLeftBorder);
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
  analogWrite(enG1, revValueSpeed);
  analogWrite(enG2, valueSpeed);

  if (longTurn == 1) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else {  
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  }
}

void rightEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, revValueSpeed);

  if (longTurn == 1) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
  } else{  
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
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
  
