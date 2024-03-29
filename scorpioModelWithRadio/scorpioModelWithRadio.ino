#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Ultrasonic.h>

RF24 radio(A0, A1);
Ultrasonic ultrasonic(A3, A4);
int data[8], dataTelemetry[5];

int enG1 = 5, enG2 = 9;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, lightTurn = -1, useCamera = -1, autoForward = -1, shortTurn = -1;
int analogVoltmeterInput = A2;
int vin = 1;
int distance;
float R1 = 30000.0, R2 = 7500.0;
int digitalPowerLed = 6;
int correctedPersMax = 40, correctedPersActual = 0;

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
  pinMode(digitalPowerLed, OUTPUT);
  pinMode(analogVoltmeterInput, INPUT);
}

void loop() {
  distance = ultrasonic.Ranging(CM);
  vin = ((analogRead(analogVoltmeterInput) * 50.0) / 1024.0) / (R2 / (R1 + R2));

  dataTelemetry[0] = distance;
  dataTelemetry[1] = 1;
  dataTelemetry[2] = valueSpeed;
  dataTelemetry[3] = 1;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    valueX = data[0];
    valueY = data[1];
    useCamera = data[2];
    lightTurn = data[3];
    valueSpeed = data[4];
    shortTurn = data[5];
    autoForward = data[7];

    if(lightTurn == 1){
      digitalWrite(digitalPowerLed, HIGH);
    } else if (lightTurn == -1){
       digitalWrite(digitalPowerLed, LOW);
    }

    if (autoForward == 1){
      if (distance > 15){
        forwardEngine();
      } else{
        stopEngine();
      }
    } else {
      if (valueX == 0 && valueY == 0) {
        stopEngine();
      } else if (valueX == 0 && valueY == 10) {
        forwardEngine();
      } else if (valueX == 0 && valueY == -10) {
        backwardEngine();
      } else if (valueX == 10 && valueY == 0) {
        correctedPersActual = correctedPersMax;
        rightEngine();
      } else if (valueX == -10 && valueY == 0) {
        correctedPersActual = correctedPersMax;
        leftEngine();
      } else if (valueX == 0 && valueY > 2 && valueY < 10) {
        valueSpeed = valueSpeed * valueY / 10;
        if (valueSpeed < 70) {
          valueSpeed = 70;
        }
        forwardEngine();
      } else if (valueX == 0 && valueY < -2 && valueY > -10) {
        valueSpeed = valueSpeed * valueY / 10;
        if (valueSpeed < 70) {
          valueSpeed = 70;
        }
        backwardEngine();
      }
      else if (valueY == 0 && valueX > 2 && valueX < 10) {
        correctedPersActual = map(valueX, 2, 9, 100, correctedPersMax);
        rightEngine();
      } else if (valueY == 0 && valueX < -2 && valueX > -10) {
        correctedPersActual = map(valueX, -2, -9, 100, correctedPersMax);
        leftEngine();
      }
    }

  }
}


void forwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void leftEngine() {
  if (shortTurn == 1){
      analogWrite(enG1, valueSpeed);
      analogWrite(enG2, valueSpeed);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
  } else{
      int correctedValue = (valueSpeed * correctedPersActual)/100;
      analogWrite(enG1, correctedValue);
      analogWrite(enG2, valueSpeed);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }
}

void rightEngine() {
  if (shortTurn == 1){
      analogWrite(enG1, valueSpeed);
      analogWrite(enG2, valueSpeed);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
  } else{
      int correctedValue = (valueSpeed * correctedPersActual)/100;
      analogWrite(enG1, valueSpeed);
      analogWrite(enG2, correctedValue);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
    }
}

void stopEngine() {
  analogWrite(enG1, 0);
  analogWrite(enG2, 0);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
