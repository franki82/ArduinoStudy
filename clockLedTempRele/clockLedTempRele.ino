//led clock
#include <DS3231.h>
#include <Wire.h>
#include "TM1637.h"
 
#define CLK 9//Pins for TM1637       
#define DIO 8
 
TM1637 tm1637(CLK,DIO);

const int buttonPin = 10, buttonPinDisable = 11;
const int relayPort = 4;
int buttonState = 0, buttonDisableState = 0;
boolean readOtherValues = false;
int disableLed = -1;
int disableLight = 1;
int soundSensorPin = A0;
int soundSensorValue = 0;

DS3231 Clock;
bool Century=false, h12, PM;

int myHour, myMinutes, myDate, myMonth, myYear;
 
void setup(){
  Serial.begin(9600);
  Wire.begin();
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPinDisable, INPUT_PULLUP);
  pinMode(soundSensorPin,INPUT);
  pinMode(relayPort, OUTPUT);
  digitalWrite(relayPort, HIGH);
//    Clock.setClockMode(false);
//    Clock.setYear(18);
//    Clock.setMonth(05);
//    Clock.setDate(17);
//    Clock.setHour(21);
//    Clock.setMinute(55);
//    Clock.setSecond(30);
  
  tm1637.init();
  tm1637.set(BRIGHT_DARKEST); 
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  
  delay(500);
 
 }
 
void loop(){
  soundSensorValue = analogRead(soundSensorPin);
  buttonState = digitalRead(buttonPin);
  buttonDisableState = digitalRead(buttonPinDisable);

  getHoursAndMinutes();

  if(soundSensorValue > 200){
    disableLed = 1;
    disableLight = 1;
    soundSensorValue=0;
    delay(500);
  }

    
  
  if (buttonState == LOW && buttonDisableState == HIGH){
    readOtherValues = true;
    disableLight = - disableLight;
    delay(10);
  } else {
    readOtherValues = false;
  }

  if (buttonDisableState == LOW && buttonState == HIGH){
    disableLed = - disableLed;
    disableLight = 1;
    delay(500);
  }


  if (disableLight == -1){
    digitalWrite(relayPort, LOW);
  } else {
    digitalWrite(relayPort, HIGH);
  }

  if (myHour == 6 && myMinutes == 0){
    disableLed = -1;
  }

  if (disableLed == -1){
  
    //getHoursAndMinutes();
   
    int digitoneHour = myHour / 10;
    int digittwoHour = myHour % 10;
    int digitoneMin = myMinutes / 10;
    int digittwoMin = myMinutes % 10;
  
    if (readOtherValues){
      delay (100);
      int temp = Clock.getTemperature();
      int digitoneT = temp / 10;
      int digittwoT = temp % 10;
      int digitOneDate = myDate / 10;
      int digitTwoDate = myDate % 10;
      int digitOneMonth = myMonth / 10;
      int digitTwoMonth = myMonth % 10;
      int digitOneYear  = myYear / 1000;
      int digitTwoYear = (myYear / 100)%10;
      int digitThreeYear = (myYear % 100)/10;
      int digitFourYear = myYear % 10;;
      tm1637.point(POINT_OFF);

      tm1637.display(0,digitOneDate);
      tm1637.display(1,digitTwoDate); 
      
      tm1637.display(2,digitOneMonth); 
      tm1637.display(3,digitTwoMonth);
      delay (2000);
      tm1637.display(0,digitOneYear); 
      tm1637.display(1,digitTwoYear); 
      tm1637.display(2,digitThreeYear);
      tm1637.display(3,digitFourYear);
      delay (2000);
      tm1637.clearDisplay();
      tm1637.display(0,digitoneT); 
      tm1637.display(1,digittwoT); 
      tm1637.display(2,12);
      delay (2000);
      readOtherValues = false;
    } else {
      tm1637.point(POINT_ON);
      int digitoneHour = myHour / 10;
      int digittwoHour = myHour % 10;
      int digitoneMin = myMinutes / 10;
      int digittwoMin = myMinutes % 10;
      tm1637.display(0,digitoneHour);
      tm1637.display(1,digittwoHour); 
      tm1637.display(2,digitoneMin); 
      tm1637.display(3,digittwoMin);
  }
  } else {
    tm1637.point(POINT_OFF);
    tm1637.clearDisplay();
  }
  
 
}


void getHoursAndMinutes() {
  myHour = Clock.getHour(h12, PM);
  myMinutes = Clock.getMinute();
  myDate = Clock.getDate();
  myMonth = Clock.getMonth(Century);
  myYear = 2000 + Clock.getYear();
}

