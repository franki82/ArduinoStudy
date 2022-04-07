#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>


RF24 radio(A0, A1);
int data[8], dataTelemetry[5];

int enG1 = 5, enG2 = 6;
int in1 = A2;
int in2 = A3;
int in3 = A4;
int in4 = A5;

int enG3 = 9, enG4 = 10;
int in5 = 2;
int in6 = 4;
int in7 = 7;
int in8 = 8;

int valueX, valueY, valueSpeed = 0, omniTurn = -1;
//int analogVoltmeterInput = A2;
int vin = 1;
float R1 = 30000.0, R2 = 7500.0;


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

  pinMode(enG3, OUTPUT);
  pinMode(enG4, OUTPUT);
  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);
  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);
  //pinMode(analogVoltmeterInput, INPUT);
}

void loop() {
  delay(5);
  //vin = ((analogRead(analogVoltmeterInput) * 50.0) / 1024.0)/(R2/(R1+R2));

  dataTelemetry[0] = 1;
  dataTelemetry[1] = 1;
  dataTelemetry[2] = 1;
  dataTelemetry[3] = 1;
  dataTelemetry[4] = vin;

  radio.writeAckPayload(1, &dataTelemetry, sizeof(dataTelemetry));

  //  Serial.print("Voltage:");
  //  Serial.println(vin);

  if (radio.available()) {
    radio.read(&data, sizeof(data));
    valueX = data[0];
    valueY = data[1];
    omniTurn = data[3];
    valueSpeed = data[4];

    if (valueX == 0 && valueY == 0) {
      stopEngine();
    } else if (valueX == 0 && valueY == 10) {
      forwardEngine();
    } else if (valueX == 0 && valueY == -10) {
      backwardEngine();
    } else if (valueX == 10 && valueY == 0) {
      rightEngine();
    } else if (valueX == -10 && valueY == 0) {
      leftEngine();
    } else if (valueX == 0 && valueY > 2 && valueY < 10) {
      valueSpeed = valueSpeed * valueY / 10;
      if (valueSpeed < 70) {
        valueSpeed = 70;
      }
      forwardEngine();
    } else if (valueX == 0 && valueY < -2 && valueY > -10) {
      int absValueY = abs(valueY);
      valueSpeed = valueSpeed * valueY / 10;
      if (valueSpeed < 70) {
        valueSpeed = 70;
      }
      backwardEngine();
    } else if (valueY == 0 && valueX > 2 && valueX < 10) {
      valueSpeed = valueSpeed * valueX / 10;
      rightEngine();
    } else if (valueY == 0 && valueX < -2 && valueX > -10) {
      int absValueX = abs(valueX);
      valueSpeed = valueSpeed * absValueX / 10;
      leftEngine();
    }

  }
}

void firstForward(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
}

void firstBackward(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}

void secondForward(){
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void secondBackward(){
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void thirdForward(){
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
}

void thirdBackward(){
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
}

void fothForward(){
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);
}

void fothBackward(){
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
}


void forwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  analogWrite(enG3, valueSpeed);
  analogWrite(enG4, valueSpeed);
  firstForward();
  secondForward();
  thirdForward();
  fothForward();
}

void backwardEngine() {
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  analogWrite(enG3, valueSpeed);
  analogWrite(enG4, valueSpeed);
  firstBackward();
  secondBackward();
  thirdBackward();
  fothBackward();
}


void leftEngine() {
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, valueSpeed);
    analogWrite(enG3, valueSpeed);
    analogWrite(enG4, valueSpeed);
  if (omniTurn == 1) {
    firstBackward();
    secondForward();
    thirdForward();
    fothBackward();
  } else {
    firstBackward();
    secondForward();
    thirdBackward();
    fothForward();
  }
}

void rightEngine() {
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, valueSpeed);
    analogWrite(enG3, valueSpeed);
    analogWrite(enG4, valueSpeed);
  if (omniTurn == 1) {
    firstForward();
    secondBackward();
    thirdBackward();
    fothForward();
  } else {
    firstForward();
    secondBackward();
    thirdForward();
    fothBackward();
  }
}

void stopEngine() {
  analogWrite(enG1, 0);
  analogWrite(enG2, 0);
  analogWrite(enG3, 0);
  analogWrite(enG4, 0);

  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);

  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);

  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);
}
