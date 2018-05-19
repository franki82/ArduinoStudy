#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(3, 4);
int data[2];

int enG1 = 8, enG2 = 9;

int in1 = 46;
int in2 = 47;

int in3 = 48;
int in4 = 49;

int valueX, valueY, valueSpeed = 255, revValueSpeed = 100;

void setup() {
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openReadingPipe(1, 0x1234567890LL);
  radio.startListening();
}

void loop() {
  if(radio.available()){
//    analogWrite(enG1, valueSpeed);
//    analogWrite(enG2, valueSpeed);
    radio.read(&data, sizeof(data));

    valueX = data[0];
    valueY = data[1];
    if (abs(valueX)<8){
      valueX = 0;
    }

    if (abs(valueY)<8){
      valueY = 0;
      stopEngine();
    }
    
    switch (valueX){
    case 10:
    case 9:
      rightEngine();
      break;
    case -10:
    case -9: 
      leftEngine();
      break;
    }

    switch (valueY){
    case 10:
    case 9: 
        forwardEngine();
      break;
    case -10:
    case -9: 
        backwardEngine();
      break;
    }
   }
  
}

void forwardEngine(){
  analogWrite(enG1, valueSpeed * 0.8);
  analogWrite(enG2, valueSpeed * 0.8);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void backwardEngine(){
  analogWrite(enG1, valueSpeed * 0.8);
  analogWrite(enG2, valueSpeed * 0.8);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}


void rightEngine(){
  analogWrite(enG1, revValueSpeed);
  analogWrite(enG2, valueSpeed);
  
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void leftEngine(){
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, revValueSpeed);
  
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
//  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void stopEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
