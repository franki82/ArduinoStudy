#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Servo.h>

Servo servoRight, servoLeft;

RF24 radio(A0, A1);
int data[8], dataTelemetry[5];

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, isTurn = -1, isCorrectData = -1;
int analogVoltmeterInput = A2;
int vin = 1;
float R1 = 30000.0, R2 = 7500.0;
int leftMotorPersentage = 100, rightMotorPersentage = 100;
int rightServoPin = 9, rightServoStart = 90, rightCurrentPosition = 90, rightServoRightBorder = 35, rightServoLeftBorder = 145;
int leftServoPin = 10, leftServoStart = 93, leftCurrentPosition = 93, leftServoRightBorder = 35, leftServoLeftBorder = 145;
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

  servoRight.attach(rightServoPin);
  delay(500);
  servoRight.write(rightServoStart);
  delay(500);
  servoLeft.attach(leftServoPin);
  delay(500);
  servoLeft.write(leftServoStart);
  delay(500);
}

void loop() {
  delay(10);
  vin = ((analogRead(analogVoltmeterInput) * 50.0) / 1024.0) / (R2 / (R1 + R2));

  dataTelemetry[0] = 1;
  dataTelemetry[1] = rightCurrentPosition;
  dataTelemetry[2] = leftMotorPersentage;
  dataTelemetry[3] = rightMotorPersentage;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    valueX = data[0];
    valueY = data[1];
    isTurn = data[3];
    valueSpeed = data[4];
    isCorrectData = data[5];

    if (isTurn == -1 && isCorrectData == -1) {
      if (valueX == 0 && valueY == 0) {
        stopEngine();
      } else if (valueX == 0 && valueY == 10) {
        forwardEngine();
      } else if (valueX == 0 && valueY == -10) {
        backwardEngine();
      } else if (valueX == 0 && valueY > 2 && valueY < 10) {
        valueSpeed = valueSpeed * valueY / 10;
        if (valueSpeed < 70) {
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
    } else if (isTurn == -1 && isCorrectData == 1) {
      if (valueX >= 8 && valueY == 0) {
        rightEnginePowerChange();
      } else if (valueX <= -8 && valueY == 0) {
        leftEnginePowerChange();
      } else if (valueX == 0 && valueY >= 8) {
        enginePowerChangeToFull();
      }

    } else if (isTurn == 1 && isCorrectData == -1) {
      if (valueX <= -8 && valueY == 0) {
        rightCurrentPosition = turnToRight(servoRight, rightCurrentPosition, rightServoRightBorder);
        leftCurrentPosition = turnToRight(servoLeft, leftCurrentPosition, leftServoRightBorder);
      } else if (valueX >= 8 && valueY == 0) {
        rightCurrentPosition = turnToLeft(servoRight, rightCurrentPosition, rightServoLeftBorder);
        leftCurrentPosition = turnToLeft(servoLeft, leftCurrentPosition, leftServoLeftBorder);
      } else if (valueY >= 8 && valueX == 0){
          weelsToCenter();
      }
    } 
  }
}


void forwardEngine() {
  analogWrite(enG1, valueSpeed * leftMotorPersentage / 100);
  analogWrite(enG2, valueSpeed * rightMotorPersentage / 100);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine() {
  analogWrite(enG1, valueSpeed * leftMotorPersentage / 100);
  analogWrite(enG2, valueSpeed * rightMotorPersentage / 100);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopEngine() {
  analogWrite(enG1, 0);
  analogWrite(enG2, 0);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void rightEnginePowerChange() {
  if (rightMotorPersentage <= 100 && rightMotorPersentage > 50) {
    rightMotorPersentage--;
  } else {
    rightMotorPersentage = 100;
    delay(2000);
  }
}

void leftEnginePowerChange() {
  if (leftMotorPersentage <= 100 && leftMotorPersentage > 50) {
    leftMotorPersentage--;
  } else {
    leftMotorPersentage = 100;
    delay(2000);
  }
}

void enginePowerChangeToFull() {
  rightMotorPersentage = 100;
  leftMotorPersentage = 100;
  delay(2000);
}

int turnToLeft(Servo servo, int currentServoPosition, int servoLeftBorder) {
  if (currentServoPosition < servoLeftBorder) {
    currentServoPosition++;
    servo.write(currentServoPosition);
    delay(turnTimeout);
  }
  return currentServoPosition;
}

int turnToRight(Servo servo, int currentServoPosition, int servoRightBorder) {
  if (currentServoPosition > servoRightBorder) {
    currentServoPosition--;
    servo.write(currentServoPosition);
    delay(turnTimeout);
  }
  return currentServoPosition;
}

void weelsToCenter(){
  if(rightCurrentPosition < rightServoStart && leftCurrentPosition < leftServoStart){
    rightCurrentPosition++;
    leftCurrentPosition++;
    servoRight.write(rightCurrentPosition);
    servoLeft.write(leftCurrentPosition);
    delay(turnTimeout);
  } else if (rightCurrentPosition > rightServoStart && leftCurrentPosition > leftServoStart){
    rightCurrentPosition--;
    leftCurrentPosition--;
    servoRight.write(rightCurrentPosition);
    servoLeft.write(leftCurrentPosition);
    delay(turnTimeout);
  }
}
