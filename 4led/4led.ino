//led clock
#include <dht.h>
#include "TM1637.h"
#include <stdio.h>
#include <DS1302.h>
 
#define dht_pin 2 // Pin sensor is connected to
#define CLK 9//Pins for TM1637       
#define DIO 8
 
TM1637 tm1637(CLK,DIO);
dht DHT;

const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
const int kSclkPin = 7;  // Serial Clock
const int buttonPin = 10, buttonPinDisable = 11;
const int relayPort = 4;
int buttonState = 0, buttonDisableState = 0;
boolean readOtherValues = false;
int disableLed = -1;
int disableLight = 1;
int soundSensorPin = A0;
int soundSensorValue = 0;

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

int myHour, myMinutes, myDate, myMonth, myYear;
 
void setup(){
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPinDisable, INPUT_PULLUP);
  pinMode(soundSensorPin,INPUT);
  pinMode(relayPort, OUTPUT);
  digitalWrite(relayPort, HIGH);
  rtc.halt(false); //запуск часов
  rtc.writeProtect(false); //снять защиту от записи
//
//  // Make a new time object to set the date and time.
//  // Sunday, September 22, 2013 at 01:38:50.
//  Time t(2018, 4, 30, 22, 15, 00, Time::kMonday);
// 
//  /* Set the time and date on the chip */
//  rtc.time(t);
  
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

  if(soundSensorValue > 50){
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

//  if (buttonState == LOW && buttonDisableState == LOW){
//    disableLight = - disableLight;
//    delay(10);
//  }

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
      DHT.read11(dht_pin);
      delay (500);
      int temp = DHT.temperature;
      int humidity = DHT.humidity;
      int digitoneT = temp / 10;
      int digittwoT = temp % 10;
      int digitoneH = humidity / 10;
      int digittwoH = humidity % 10;
      int digitOneDate = myDate / 10;
      int digitTwoDate = myDate % 10;
      int digitOneMonth = myMonth / 10;
      int digitTwoMonth = myMonth % 10;
      int digitOneYear  = myYear / 1000;
      int digitTwoYear = (myYear / 100)%10;
      int digitThreeYear = (myYear % 100)/10;
      int digitFourYear = myYear % 10;;
      tm1637.point(POINT_OFF);
//      if (myDate < 10){
//        tm1637.display(0,25); 
//        tm1637.display(1,digitOneDate);
//       } else {
//        tm1637.display(0,digitOneDate);
//        tm1637.display(1,digitTwoDate); 
//      }
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
      tm1637.display(0,25); 
      tm1637.display(1,digitoneT); 
      tm1637.display(2,digittwoT);
      tm1637.display(3,12);
      delay (2000);
      tm1637.display(1,25); 
      tm1637.display(2,digitoneH); 
      tm1637.display(3,digittwoH);
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

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}

void getHoursAndMinutes() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  myHour = t.hr;
  myMinutes = t.min;
  myDate = t.date;
  myMonth = t.mon;
  myYear = t.yr;
}

