#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 10);
int data[3];

int xPin = A1;
int yPin = A0;
int xPosition = 0;
int yPosition = 0;
int xValueToOut = 0;
int yValueToOut = 0;
int buttonStatePin = 8, buttonState = 0, useCamera = -1;

void setup() {
  Serial.begin(9600);
  pinMode(buttonStatePin, INPUT_PULLUP);
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(0x1234567890LL);
}

void loop() {
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonStatePin);

  if (buttonState == LOW){
    useCamera = -useCamera;
    delay(500);
  }
  
  if (xPosition==515){
    xValueToOut = 0;
  } else if (xPosition<515){
    xValueToOut = map(xPosition, 514, 0, 0, -10);
  } else if (xPosition>515){
    xValueToOut = map(xPosition, 516, 1023, 0, 10);
  }

  if (yPosition==503){
    yValueToOut = 0;
  } else if (yPosition<503){
    yValueToOut = map(yPosition, 503, 0, 0, 10);
  } else if (xPosition>503){
    yValueToOut = map(yPosition, 504, 1023, 0, -10);
  }

  data[0] = xValueToOut;
  data[1] = yValueToOut;
  data[2] = useCamera;
  radio.write(&data, sizeof(data));

}
