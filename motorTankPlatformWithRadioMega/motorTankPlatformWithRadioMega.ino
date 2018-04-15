#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(3, 4);
int data[2];

int enG = 2;

int in1 = 46;
int in2 = 47;

int in3 = 48;
int in4 = 49;

int valueX, valueY, valueSpeed = 250;

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
    analogWrite(enG, valueSpeed);
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
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void backwardEngine(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}


void leftEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void rightEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
//  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stopEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
