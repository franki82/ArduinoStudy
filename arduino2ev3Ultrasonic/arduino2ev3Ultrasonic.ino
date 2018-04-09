#include <SoftwareSerial.h>
#include <Ultrasonic.h> 

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

// к 11 пину подключаем Bluetooth
SoftwareSerial mySerial(10, 11); // RX, TX 
Ultrasonic ultrasonic(8,9); 
int LED = 12;
void setup()  
{
  // Открываем Bluetooth соединение
  mySerial.begin(9600);
 
  // Немного ждем
  delay(1000);
  digitalWrite(LED, HIGH);
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
  int duration;
if (mySerial.available()>0){
  digitalWrite(LED, HIGH);
} else {
  digitalWrite(LED, LOW);
  }

duration = ultrasonic.Ranging(CM); 
 

MakeNumericEV3Message("MyNumeric", duration);

delay(500);

}
