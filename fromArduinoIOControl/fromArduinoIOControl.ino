#include <SoftwareSerial.h>
int val;
int LED = 12;
char myChar;
String stringFromPhone = "";
boolean bool1 = true, bool2 = false;
int valueX, valueY, valueSpeed, valueHead;
SoftwareSerial mySerial(10, 11); // TX, RX 
void setup() {
  mySerial.begin(9600);
  Serial.begin(9600);
  delay(100);
  Serial.println("Hello, world");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  valueX = 0;
  valueY = 0;
  valueSpeed = 0;
  valueHead = -1;
}

void loop() {
valueHead = -1;
while (mySerial.available() > 0){
    char c = mySerial.read();
    if(c!='\n')    stringFromPhone += c; //Если не конец строки, добавляем символ  
    else bool2=true;   
  }
 
  if (bool2){ //Если конец строки, то разбираем ее на ключ и значение
    bool2=false;
   
    String key = "", value = "";

    key = stringFromPhone.substring(0,stringFromPhone.indexOf(' '));
    value = stringFromPhone.substring(stringFromPhone.indexOf(' ')+1);
    stringFromPhone = "";

    Serial.print("KEY:");
    Serial.println(key);

   if (key.equals("MyJoyValue")) {
    String valueXString = value.substring(0,value.indexOf(' '));
    String valueYString = value.substring(value.indexOf(' ')+1);
    valueX = valueXString.toInt();
    valueY = valueYString.toInt();
    if (abs(valueX)<10){
      valueX = 0;
    }
    if (abs(valueY)<10){
      valueY = 0;
    }
    switch (valueX){
    case 10: 
      valueX = 100;
      break;
    case -10: 
      valueX = -100;
      break;
    }

    switch (valueY){
    case 10: 
      valueY = -valueSpeed;
      
      break;
    case -10: 
      valueY = valueSpeed;
      break;
    }
   }
   else if (key.equals("maxSpeed")){
    valueSpeed = value.toInt();
   }
   else if (key.equals("isPress")){
    valueHead = 0;
   }
  }
  Serial.print("Speed:");
  Serial.println(valueSpeed);
  Serial.print("X:");
  Serial.println(valueX);
  Serial.print("Y:");
  Serial.println(valueY);
  Serial.print("HEAD:");
  Serial.println(valueHead);
  delay(1000);
}
