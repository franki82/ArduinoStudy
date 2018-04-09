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

int myBorder = 30, speedB = 0, speedC = 0;
int generalSpeed = 75, turnSpeed = 25;
boolean isWaitForTurn = false;
unsigned long startWaitingTime;
int waitingBorder = 0;//1200;

int xPin = A1;
int yPin = A0;
int xPosition = 0;
int yPosition = 0;
int xValueToOut = 0;
int yValueToOut = 0;
int buttonPin = 2;
int buttonState = 0;
boolean isUseSavedPath = false;
int currentSaveIndex = 0;
int motorMaxVal = 75;
int motorMaxNegVal = -75;
int pathArray[3][3]={{-75,-75,3000},{-25,25,500},{-75,-75,2000}};
unsigned long startTime = 0;


void setup()  
{
  Serial.begin(9600);
  delay(1000);
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
  Serial.write(out_message, out_message_length);
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
  Serial.write(out_message, out_message_length);
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
  Serial.write(out_message, out_message_length);
}



void loop() 
{
  MakeNumericEV3Message("MyConnection", myBorder);
  delay(100);

//  buttonState = digitalRead(buttonPin);
//  if (buttonState == LOW){
//    delay(100);
//    isUseSavedPath = true;
//    currentSaveIndex = 0;
//    startTime = millis();
//  } 
//  if (isUseSavedPath){
//    unsigned long currentTime = millis();
//    int borderTime = pathArray[currentSaveIndex][2];
//    if (currentTime - startTime <= borderTime){
//        speedB = pathArray[currentSaveIndex][0];
//        speedC = pathArray[currentSaveIndex][1];
//    } else {
//            currentSaveIndex++;
//            if (currentSaveIndex > 2){
//              isUseSavedPath = false;
//              currentSaveIndex = 0;
//              speedB = 0;
//              speedC = 0;
//            }
//      }
//    
//    } 
//    else {
//      xPosition = analogRead(xPin);
//      yPosition = analogRead(yPin);
//      if (xPosition==515){
//        xValueToOut = 0;
//      } else if (xPosition<515){
//        xValueToOut = map(xPosition, 514, 0, 0, motorMaxNegVal);
//      } else if (xPosition>515){
//        xValueToOut = map(xPosition, 516, 1023, 0, motorMaxVal);
//      }
//    
//      if (yPosition==503){
//        yValueToOut = 0;
//      } else if (yPosition<503){
//        yValueToOut = map(yPosition, 503, 0, 0, motorMaxNegVal);
//      } else if (xPosition>503){
//        yValueToOut = map(yPosition, 503, 1023, 0, motorMaxVal);
//      }
//    
//      if (abs(xValueToOut)==0){
//        speedB = yValueToOut;
//        speedC = yValueToOut;
//      } else {
//        if (xValueToOut > 0){
//          speedB = abs(xValueToOut);
//          speedC = -abs(xValueToOut);
//        } else {
//          speedB = -abs(xValueToOut);
//          speedC = abs(xValueToOut);
//        }
//      }
//    }

      xPosition = analogRead(xPin);
      yPosition = analogRead(yPin);
      if (xPosition==515){
        xValueToOut = 0;
      } else if (xPosition<515){
        xValueToOut = map(xPosition, 514, 0, 0, motorMaxNegVal);
      } else if (xPosition>515){
        xValueToOut = map(xPosition, 516, 1023, 0, motorMaxVal);
      }
    
      if (yPosition==503){
        yValueToOut = 0;
      } else if (yPosition<503){
        yValueToOut = map(yPosition, 503, 0, 0, motorMaxNegVal);
      } else if (xPosition>503){
        yValueToOut = map(yPosition, 503, 1023, 0, motorMaxVal);
      }
    
      if (abs(xValueToOut)==0){
        speedB = yValueToOut;
        speedC = yValueToOut;
      } else {
        if (xValueToOut > 0){
          speedB = abs(xValueToOut);
          speedC = -abs(xValueToOut);
        } else {
          speedB = -abs(xValueToOut);
          speedC = abs(xValueToOut);
        }
      }
  

  MakeNumericEV3Message("DSB", speedB);
  MakeNumericEV3Message("DSC", speedC);
  delay(100);
  
}

int returnTimeToOnePhaseBig(int powerEngine, int correction){
  int returnTime = 0;
  returnTime = ((1/((powerEngine*1.7)/60))*1000)-correction;
  return returnTime;
}

 int returnTimeToOnePhaseMedium(int powerEngine, int correction){
  int returnTime = 0;
  returnTime = ((1/((powerEngine*2.67)/60))*1000)-correction;
  return returnTime;
}

