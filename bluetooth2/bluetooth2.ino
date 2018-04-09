#include <SoftwareSerial.h>
int val;
int LED = 12;
char myChar;
SoftwareSerial mySerial(10, 11); // RX, TX 
void setup() {
  mySerial.begin(9600);
  delay(100);
  mySerial.println("Hello, world?");
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}

void loop() {
 if (mySerial.available())
  {
    val = mySerial.read();
    // При символе "1" включаем светодиод
    if (val == '1')
    {
      digitalWrite(LED, HIGH);
      myChar = 'E';
      mySerial.write(myChar);
    }
    // При символе "0" выключаем светодиод
    if ( val == '0')
    {
      digitalWrite(LED, LOW);
      myChar = 'D';
      mySerial.write(myChar);
    }
  }
}
