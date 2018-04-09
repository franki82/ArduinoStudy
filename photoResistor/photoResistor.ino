const int sensorPin = 0;
const int ledPin = 9;

int lightLevel;
int calibratedlightLevel; // used to store the scaled / calibrated lightLevel

void setup()
{
  pinMode(ledPin, OUTPUT);    // Set up the LED pin to be an output.
  Serial.begin(9600);
}

void loop()
{
  lightLevel = analogRead(sensorPin);  // reads the voltage on the sensorPin
  Serial.print(lightLevel);
  Serial.print(" ");
  //calibratedlightLevel = map(lightLevel, 0, 1023, 0, 255);
  calibratedlightLevel = map(lightLevel, 0, 1023, 255, 0); //reverse
  calibratedlightLevel = constrain(calibratedlightLevel, 0, 255);
  Serial.print(" new level:");
  Serial.print(calibratedlightLevel);
  Serial.println(""); 
  delay(1000);
  analogWrite(ledPin, calibratedlightLevel); 
}  
