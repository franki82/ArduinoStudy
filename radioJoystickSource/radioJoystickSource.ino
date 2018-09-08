#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_SSD1306.h>

RF24 radio(A2, A3);
int data[5], dataTelemetry[4];
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

void setup() {
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(buttonStatePin, INPUT_PULLUP);
  pinMode(buttonClawsPin, INPUT_PULLUP);
  pinMode(speedPin, INPUT);
  radio.begin();
  radio.setChannel(100);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_HIGH);
  radio.openWritingPipe(0x1234567895LL);
  radio.openReadingPipe(1, 0x1234567895LL);
}

void loop() {
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  buttonState = digitalRead(buttonStatePin);
  buttonClawsState = digitalRead(buttonClawsPin);
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
  data[3] = useClaws;
  data[4] = mySpeed;
  radio.write(&data, sizeof(data));
    
  CTime01 = millis();
  if (CTime01 >= (LTime01 +50)) //read pokets period
  {
     int duration, angleSee, handPosition, clawsPosition;
     radio.startListening();
     delay(20);
     radio.read(&dataTelemetry, sizeof(dataTelemetry));
     duration = dataTelemetry[0];
     angleSee = dataTelemetry[1];
     handPosition = dataTelemetry[2];
     clawsPosition = dataTelemetry[3];
     if (duration > 0){
        Serial.println("----------read-telemetry------------");
        Serial.println(duration);
        Serial.println(angleSee);
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setCursor(0,0);
        display.setTextSize(1);
        display.print("Distance: ");
        display.print(duration);
        display.print(" / ");
        display.print(angleSee);
//        display.setCursor(0,9);
//        display.setTextSize(1);
//        display.print("Ult Angle: ");
//        display.print(angleSee);
//        display.setCursor(0,17);
//        display.setTextSize(1);
//        display.print("Speed: ");
//        display.print(mySpeed);

        display.setCursor(0,9);
        display.setTextSize(1);
        display.print(mySpeed);
        display.print("/");
        display.print(handPosition);
        display.print("/");
        display.print(clawsPosition);

        display.setCursor(0,25);
        display.setTextSize(1);
        display.print("Is Active: ");
        if (useCamera == -1 && useClaws == -1){
          display.print("movement");
        } else if (useClaws == 1){
          display.print("claws");
        } else if (useCamera == 1){
          display.print("ult view");
        }
        display.display();
     }
     LTime01 = CTime01;
     radio.stopListening();
  }
  
}
