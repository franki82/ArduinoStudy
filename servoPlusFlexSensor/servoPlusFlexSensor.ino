#include <Servo.h>
Servo servo1;
const int flexPin = A0; 

void setup() {
  Serial.begin(9600);
  servo1.attach(9);
  servo1.write(0);
  delay(1000);
}

void loop() {
  int flexPosition;    // Input value from the analog pin.
  int servoPosition;   // Output value to the servo.

  // Read the position of the flex sensor (0 to 1023):
  
  flexPosition = analogRead(flexPin);


  servoPosition = map(flexPosition, 600, 900, 0, 180);
  servoPosition = constrain(servoPosition, 0, 180);

  // Now we'll command the servo to move to that position:

  servo1.write(servoPosition);

  
  Serial.print("sensor: ");
  Serial.print(flexPosition);
  Serial.print("  servo: ");
  Serial.println(servoPosition);
  
  
  delay(20);  // wait 20ms between servo updates 
  delay(1000);
}
