#include <SoftwareSerial.h>

byte out_message [64];
int out_message_length = 0;

byte in_message [64];
int in_message_type = 0;
int in_message_length = 0;
int in_mailbox_length = 0;
int in_value_length = 0;
char in_mailbox [16];
boolean in_value_logic;
float in_value_numeric;
char in_value_chars [64];
String in_mailbox_text;
String in_value_text;

int xPin = A1;
int yPin = A0;
int xPosition = 0;
int yPosition = 0;
int xValueToOut = 0;
int yValueToOut = 0;
int buttonPin = 2;
int buttonState = 0;
int motorMaxVals[4] = {25, 50, 75, 100};
int motorMaxNegVals[4] = {-25, -50, -75, -100};
int currentTransmissionPosition = 0;

// к 11 пину подключаем Bluetooth
SoftwareSerial mySerial(10, 11); // TX, RX 

void setup()  
{
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); 
  // Открываем Bluetooth соединение
  mySerial.begin(9600);
 
  // Немного ждем
  delay(1000);
  Serial.begin(9600);
}

union u_fi
{
  unsigned  char c[4];
  float f;
} ieee752;

union l_en
{
  unsigned  char e[2];
  unsigned int l;
} little_endian;

// Send Packets functions

void MakeLogicEV3Message(String tmp_text, boolean value_logic) {
  char mailbox[tmp_text.length()+1];
  tmp_text.toCharArray(mailbox, tmp_text.length()+1) ;  
  int PacketLength = 5 + sizeof(mailbox) + 3;
  little_endian.l=(int)PacketLength;
  out_message[0] = little_endian.e[0];
  out_message[1] = little_endian.e[1];
  out_message[2] = 0x01;
  out_message[3] = 0x00;
  out_message[4] = 0x81;
  out_message[5] = 0x9E;
  out_message[6] = sizeof(mailbox);  
  int i=7;
  while (i<6+sizeof(mailbox)){
    out_message[i]=mailbox[i-7];
    i++;
  }
  out_message[i] = 0x00;
  out_message[i+1] = 0x01;
  out_message[i+2] = 0x00;
  out_message[i+3] = value_logic;   
  out_message_length = PacketLength + 2;
  mySerial.write(out_message, out_message_length);
}

void MakeNumericEV3Message(String tmp_text, float value_numeric) {
  char mailbox[tmp_text.length()+1];
  tmp_text.toCharArray(mailbox, tmp_text.length()+1) ;   
  int PacketLength = 5 + sizeof(mailbox) + 6;
  little_endian.l=(int)PacketLength;
  out_message[0] = little_endian.e[0];
  out_message[1] = little_endian.e[1];
  out_message[2] = 0x01;
  out_message[3] = 0x00;
  out_message[4] = 0x81;
  out_message[5] = 0x9E;
  out_message[6] = sizeof(mailbox);  
  int i=7;
  while (i<6+sizeof(mailbox)){
    out_message[i]=mailbox[i-7];
    i++;
  }
  out_message[i] = 0x00;
  out_message[i+1] = 0x04;
  out_message[i+2] = 0x00;
  ieee752.f=(float)value_numeric;
  out_message[i+3] = ieee752.c[0];
  out_message[i+4] = ieee752.c[1];
  out_message[i+5] = ieee752.c[2];
  out_message[i+6] = ieee752.c[3]; 
  out_message_length = PacketLength + 2;
  mySerial.write(out_message, out_message_length);
}

void MakeTextEV3Message(String tmp_text, String tmp_text2) {
  char mailbox[tmp_text.length()+1];
  tmp_text.toCharArray(mailbox, tmp_text.length()+1) ; 
  char value_text[tmp_text2.length()+1];
  tmp_text2.toCharArray(value_text, tmp_text2.length()+1) ;   
  int PacketLength = 5 + sizeof(mailbox) + 2 + sizeof(value_text);
  little_endian.l=(int)PacketLength;
  out_message[0] = little_endian.e[0];
  out_message[1] = little_endian.e[1];
  out_message[2] = 0x01;
  out_message[3] = 0x00;
  out_message[4] = 0x81;
  out_message[5] = 0x9E;
  out_message[6] = sizeof(mailbox);  
  int i=7;
  while (i<6+sizeof(mailbox)){
    out_message[i]=mailbox[i-7];
    i++;
  }
  out_message[i] = 0x00;
  little_endian.l=(int)sizeof(value_text);
  out_message[i+1] = little_endian.e[0];
  out_message[i+2] = little_endian.e[1];
  int j=i+3;
  while (j<i+2+sizeof(value_text)){
    out_message[j]=value_text[j-i-3];
    j++;
  }
  out_message[j] = 0x00;    
  out_message_length = PacketLength + 2;
  mySerial.write(out_message, out_message_length);
}



void loop() 
{

  MakeNumericEV3Message("MyNumeric", 100);
  delay(100);

  buttonState = digitalRead(buttonPin);
  if (buttonState == 0){
    currentTransmissionPosition++;
  }
  if (currentTransmissionPosition>3){
    currentTransmissionPosition = 0;
  }


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
    yValueToOut = map(yPosition, 503, 0, 0, motorMaxNegVals[currentTransmissionPosition]);
  } else if (xPosition>503){
    yValueToOut = map(yPosition, 503, 1023, 0, motorMaxVals[currentTransmissionPosition]);
  }

MakeNumericEV3Message("MyState", buttonState);
delay(100);
MakeNumericEV3Message("MyY", yValueToOut);
delay(100);
MakeNumericEV3Message("MyX", xValueToOut);
delay(100);

//AskForEV3Message();
//if (in_message_type != 0) Serial.println(in_mailbox);
//if (in_message_type == 1) Serial.println(in_value_logic);
//if (in_message_type == 2) Serial.println(in_value_numeric);
//if (in_message_type == 3) Serial.println(in_value_chars);
//delay(1000);
}

//void AskForEV3Message() {
//in_message_length = 0;
//in_value_length = 0;
//in_message_type = 0;
//
//  boolean InMessageTrue = false;
//  Serial.println("Call");
//  while (mySerial.available() && !InMessageTrue) // до тех пор, пока приходят данные по bluetooth
//    {
//    Serial.println("Incoming Message");
//    little_endian.e[0] = mySerial.read();
//    little_endian.e[1] = mySerial.read();
//    
//    in_message[0] = little_endian.e[0];
//    in_message[1] = little_endian.e[1];
//      
//    for (int i=0; i<little_endian.l; i++) {
//      if (mySerial.available()) {
//        in_message[i+2] = mySerial.read();
//        in_message_length = i+3;
//        InMessageTrue = true;
//      }
//      else {
//        InMessageTrue = false;
//        in_message_type = 0;
//      }
//    }
//      
//  }
//
//  if (in_message[4] != 0x81 || in_message[5] != 0x9E) InMessageTrue = false;
//
//  if (InMessageTrue) {
//    memset(in_mailbox, 0, sizeof(in_mailbox));
//    memset(in_value_chars, 0, sizeof(in_value_chars));
//    in_mailbox_length = (int)in_message[6]-1;
//    little_endian.e[0] = in_message[6+in_mailbox_length+2];
//    little_endian.e[1] = in_message[6+in_mailbox_length+3];
//    in_value_length = (int)little_endian.l;
//    
//    for (int i=0; i<in_mailbox_length; i++) {
//    in_mailbox[i] = (char)in_message[7+i];  
//    }
//  
//    in_message_type = 3; // default as text
//    if (in_value_length == 1) in_message_type = 1; // logic
//    if (in_value_length == 4) in_message_type = 2; // numeric
//    
//    if (in_message_type == 1 && (int)in_message[6+in_mailbox_length+4] == 1) in_value_logic = true;
//    if (in_message_type == 1 && (int)in_message[6+in_mailbox_length+4] == 0) in_value_logic = false;  
//  
//    if (in_message_type == 2) {
//    ieee752.c[0] = in_message[6+in_mailbox_length+4];
//    ieee752.c[1] = in_message[6+in_mailbox_length+5];
//    ieee752.c[2] = in_message[6+in_mailbox_length+6];
//    ieee752.c[3] = in_message[6+in_mailbox_length+7]; 
//    in_value_numeric = ieee752.f;
//    }
//    
//    if (in_message_type == 3) {
//      for (int i=0; i<in_value_length-1; i++) {
//      in_value_chars[i] = (char)in_message[6+in_mailbox_length+4+i];
//       
//      }    
//    }
//        
//    }
//}

