#include <LiquidCrystal.h>
#include <Ultrasonic.h>

//int echoPin = 9; 
//int trigPin = 8; 

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Ultrasonic ultrasonic(8,9);
int rulePin = 6; 

void setup()
{
  //pinMode(trigPin, OUTPUT); 
  //pinMode(echoPin, INPUT); 
  
  lcd.begin(16, 2); //Initialize the 16x2 LCD
  lcd.clear();  //Clear any old data displayed on the LCD
  lcd.print("Distance(cm):"); // Display a message on the LCD!
  analogWrite(rulePin,120);
}

void loop()
{
  int duration,cm; 
  
//  digitalWrite(trigPin, LOW); 
//  delayMicroseconds(2); 
//  digitalWrite(trigPin, HIGH); 
//  delayMicroseconds(10); 
//  digitalWrite(trigPin, LOW); 
//  duration = pulseIn(echoPin, HIGH); 
//  cm = duration / 58;

  duration = ultrasonic.Ranging(CM);
  lcd.setCursor(0, 6);
  lcd.print("      ");
  lcd.setCursor(0, 1);
  lcd.print(duration);
//  lcd.print(cm);
  delay(1000);
}
  
