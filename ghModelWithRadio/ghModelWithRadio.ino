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

int valueX, valueY, valueSpeed = 0, isTurn = -1, isCorrectRightServo = -1, isCorrectLeftServo = -1, isForvardEngine = -1;
int analogVoltmeterInput = A2;
int vin = 1;
float R1 = 30000.0, R2 = 7500.0;
int rightServoPin = 9, rightServoStart = 90, rightCurrentPosition = 90, rightServoRightBorder = 35, rightServoLeftBorder = 145;
int leftServoPin = 10, leftServoStart = 90, leftCurrentPosition = 90, leftServoRightBorder = 35, leftServoLeftBorder = 145;
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

  dataTelemetry[0] = leftCurrentPosition;
  dataTelemetry[1] = rightCurrentPosition;
  dataTelemetry[2] = leftServoStart;
  dataTelemetry[3] = rightServoStart;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    valueX = data[0];
    valueY = data[1];
    isTurn = data[3];
    valueSpeed = data[4];
    isCorrectLeftServo = data[5];
    isCorrectRightServo = data[6];
    isForvardEngine = data[7];

    if (isTurn == -1 && isCorrectRightServo == -1 && isCorrectLeftServo == -1 && isForvardEngine == -1) {
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
    } else if (isTurn == -1 && isCorrectRightServo == 1 && isCorrectLeftServo == -1 && isForvardEngine == -1) {
      rightServoCentralPointChange();
    } else if (isTurn == -1 && isCorrectRightServo == -1 && isCorrectLeftServo == 1 && isForvardEngine == -1) {
      leftServoCentralPointChange();
    } else if (isTurn == 1 && isCorrectRightServo == -1 && isCorrectLeftServo == -1 && isForvardEngine == -1) {
      if (valueX <= -8 && valueY == 0) {
        rightCurrentPosition = turnToRight(servoRight, rightCurrentPosition, rightServoRightBorder);
        leftCurrentPosition = turnToRight(servoLeft, leftCurrentPosition, leftServoRightBorder);
      } else if (valueX >= 8 && valueY == 0) {
        rightCurrentPosition = turnToLeft(servoRight, rightCurrentPosition, rightServoLeftBorder);
        leftCurrentPosition = turnToLeft(servoLeft, leftCurrentPosition, leftServoLeftBorder);
      } else if (valueY >= 8 && valueX == 0){
          weelsToCenter();
      }
    } else if (isTurn == -1 && isCorrectRightServo == -1 && isCorrectLeftServo == -1 && isForvardEngine == 1){
      forwardEngine();
    }
  }
}


void forwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
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

void rightServoCentralPointChange() {
  if (valueSpeed > 80 && valueSpeed < 100){
    rightServoStart = valueSpeed;
  } else {
    rightServoStart = 90;
  }
}

void leftServoCentralPointChange() {
  if (valueSpeed > 80 && valueSpeed < 100){
    leftServoStart = valueSpeed;
  } else {
    leftServoStart = 90;
  }
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
  if(rightCurrentPosition <= rightServoStart){
    rightCurrentPosition++;
    servoRight.write(rightCurrentPosition);
    delay(turnTimeout);
  } 
  if (leftCurrentPosition <= leftServoStart){
    leftCurrentPosition++;
    servoLeft.write(leftCurrentPosition);
    delay(turnTimeout);
  }
  if (rightCurrentPosition >= rightServoStart+1){
    rightCurrentPosition--;
    servoRight.write(rightCurrentPosition);
    delay(turnTimeout);
  } if (leftCurrentPosition >= leftServoStart+1){
    leftCurrentPosition--;
    servoLeft.write(leftCurrentPosition);
    delay(turnTimeout);
  }
}
