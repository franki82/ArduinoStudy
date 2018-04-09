#include <Ultrasonic.h>

Ultrasonic ultrasonic(8,9);

void setup()
{
Serial.begin(9600);
}

void loop()
{
  int duration; 
  
  duration = ultrasonic.Ranging(CM);

  Serial.println(duration);
  delay(1000);
}
  
