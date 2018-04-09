int xPin = A1;
int yPin = A0;
int xPosition = 0;
int yPosition = 0;
int xValueToOut = 0;
int yValueToOut = 0;
void setup() {
  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
}

void loop() {
  xPosition = analogRead(xPin);
  yPosition = analogRead(yPin);
  if (xPosition==515){
    xValueToOut = 0;
  } else if (xPosition<515){
    xValueToOut = map(xPosition, 514, 0, 0, -100);
  } else if (xPosition>515){
    xValueToOut = map(xPosition, 516, 1023, 0, 100);
  }

  if (yPosition==503){
    yValueToOut = 0;
  } else if (yPosition<503){
    yValueToOut = map(yPosition, 503, 0, 0, 100);
  } else if (xPosition>503){
    yValueToOut = map(yPosition, 503, 1023, 0, -100);
  }
  
  Serial.print("X: ");
  Serial.print(xValueToOut);
  Serial.print(" | Y: ");
  Serial.print(yValueToOut);
  Serial.println("");
  delay(1000);
}
