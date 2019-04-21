#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>

RF24 radio(A0, A1);
int data[5], dataTelemetry[4];

int enG1 = 5, enG2 = 6;

int in1 = 2;
int in2 = 4;

int in3 = 7;
int in4 = 8;

int valueX, valueY, valueSpeed = 0, revValueSpeed = 0, longTurn = -1, useCamera = -1; //set revValueSpeed = 100 for low batery


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
}

void loop() {
  delay(10); 
  
  dataTelemetry[0] = 1;
  dataTelemetry[1] = 1;
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
    
   
  }
}


void forwardEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
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
  
