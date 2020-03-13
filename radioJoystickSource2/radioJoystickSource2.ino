#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_SSD1306.h>

RF24 radio(A2, A3);
int data[8], dataTelemetry[5];
Adafruit_SSD1306 display(4);

int xPin = A1;
int yPin = A0;
int xPosition = 0;
int yPosition = 0;
int xValueToOut = 0;
int yValueToOut = 0;
int buttonStatePin = 8, buttonState = 0, useCamera = -1;
int buttonClawsPin = 7, buttonClawsState = 0, useClaws = -1;
unsigned long CTime01;
unsigned long LTime01;
int speedPin = A6, mySpeed = 0;
int buttonState2Pin = 6, buttonState2State = 0, useState2 = -1;
int buttonState3Pin = 5, buttonState3State = 0, useState3 = -1;
int buttonState4Pin = 4, buttonState4State = 0, useState4 = -1;

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(buttonStatePin, INPUT_PULLUP);
  pinMode(buttonClawsPin, INPUT_PULLUP);
  pinMode(buttonState2Pin, INPUT_PULLUP);
  pinMode(buttonState3Pin, INPUT_PULLUP);
  pinMode(buttonState4Pin, INPUT_PULLUP);
  pinMode(speedPin, INPUT);
  radio.begin();
  radio.enableAckPayload();
  radio.setChannel(5);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(0x1234567895LL);
}

void loop() {
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonStatePin);
  buttonClawsState = digitalRead(buttonClawsPin);
  buttonState2State = digitalRead(buttonState2Pin);
  buttonState3State = digitalRead(buttonState3Pin);
  buttonState4State = digitalRead(buttonState4Pin);
  mySpeed = 255-analogRead(speedPin)/4;

  if (buttonState == LOW){
    useCamera = -useCamera;
    delay(500);
  }

  if (buttonClawsState == LOW){
    useClaws = 1;
  } else if (buttonClawsState == HIGH){
    useClaws = -1;
  }

  if (buttonState2State == LOW){
    useState2 = 1;
  } else if (buttonState2State == HIGH){
    useState2 = -1;
  }

  if (buttonState3State == LOW){
    useState3 = 1;
  } else if (buttonState3State == HIGH){
    useState3 = -1;
  }

  if (buttonState4State == LOW){
    useState4 = 1;
  } else if (buttonState4State == HIGH){
    useState4 = -1;
  }
  

  if (xPosition==515){
    xValueToOut = 0;
  } else if (xPosition<515){
    xValueToOut = map(xPosition, 514, 3, 0, -10);
  } else if (xPosition>515){
    xValueToOut = map(xPosition, 516, 1000, 0, 10);
  }

//  Serial.print("XPos:");
//  Serial.println(xPosition);
//  Serial.print("X:");
//  Serial.println(xValueToOut);

  if (yPosition==503){
    yValueToOut = 0;
  } else if (yPosition<503){
    yValueToOut = map(yPosition, 503, 3, 0, 10);
  } else if (yPosition>503){
    yValueToOut = map(yPosition, 504, 1000, 0, -10);
  }

//  Serial.print("YPos:");
//  Serial.println(yPosition);
//  Serial.print("Y:");
//  Serial.println(yValueToOut);

  data[0] = xValueToOut;
  data[1] = yValueToOut;
  data[2] = useCamera;
  data[3] = useClaws;
  data[4] = mySpeed;
  data[5] = useState2;
  data[6] = useState3;
  data[7] = useState4;
  radio.write(&data, sizeof(data));

  if ( radio.isAckPayloadAvailable() ) {
    radio.read(&dataTelemetry, sizeof(dataTelemetry));
    int duration, angleSee, handPosition, clawsPosition, voltageValue;
    duration = dataTelemetry[0];
    angleSee = dataTelemetry[1];
    handPosition = dataTelemetry[2];
    clawsPosition = dataTelemetry[3];
    voltageValue = dataTelemetry[4];
    if (duration > 0){
          Serial.println("----------read-telemetry------------");
          Serial.println(duration);
          Serial.println(angleSee);
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setCursor(3,0);
          display.setTextSize(1);
          display.print("Distance: ");
          display.print(duration);
          display.print(" / ");
          display.print(angleSee);
          display.setCursor(3,9);
          display.setTextSize(1);
          display.print(mySpeed);
          display.print("/");
          display.print(handPosition);
          display.print("/");
          display.print(clawsPosition);
          display.setCursor(3,17);
          display.setTextSize(1);
          display.print("Voltage: ");
          display.print(voltageValue);
          display.setCursor(3,25);
          display.setTextSize(1);
          display.print("Activ: ");
          if (useCamera == -1 && useClaws == -1 && useState2 == -1 && useState3 == -1 && useState4 == -1){
            display.print("movement");
          } else if (useClaws == 1 && useCamera == -1 && useState2 == -1 && useState3 == -1 && useState4 == -1){
            display.print("st1");
          } else if (useCamera == 1 && useClaws == -1 && useState2 == -1 && useState3 == -1 && useState4 == -1){
            display.print("ult view");
          } else if (useState2 == 1 && useCamera == -1 && useClaws == -1 && useState3 == -1 && useState4 == -1){
            display.print("st2");
          } else if (useState3 == 1 && useCamera == -1 && useClaws == -1 && useState2 == -1 && useState4 == -1){
            display.print("st3");
          } else if (useState4 == 1 && useCamera == -1 && useClaws == -1 && useState2 == -1 && useState3 == -1){
            display.print("st4");
          }
          display.display();
       }
  }   
  
}
