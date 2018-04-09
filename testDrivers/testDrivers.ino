int enG = 3;

int in1 = 2;
int in2 = 4;

int in3 = 5;
int in4 = 6;

int in5 = 7;
int in6 = 8;

int in7 = 9;
int in8 = 10;

String stringFromPhone = "";
boolean bool1 = true, bool2 = false;
int valueX, valueY, valueSpeed, valueHead;

void setup() {
  Serial.begin(9600);
  delay(100);
  
  pinMode(enG, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(in5, OUTPUT);
  pinMode(in6, OUTPUT);

  pinMode(in7, OUTPUT);
  pinMode(in8, OUTPUT);
}

void loop() {
  while (Serial.available() > 0){
    char c = Serial.read();
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
      stopEngine();
    }
    switch (valueX){
    case 10: 
      rightEngine();
      break;
    case -10: 
      leftEngine();
      break;
    }

    switch (valueY){
    case 10: 
        forwardEngine();
      break;
    case -10: 
        backwardEngine();
      break;
    }
   }
   else if (key.equals("maxSpeed")){
    valueSpeed = value.toInt();
    analogWrite(enG, valueSpeed);
   }
  }
   
//  analogWrite(enG, 100);
//  forwardEngine();
//  delay(2000);
//  backwardEngine();
//  delay(2000);
//  leftEngine();
//  delay(2000);
//  rightEngine();
//  delay(2000);
//  stopEngine();
//  delay(2000);
}

void forwardEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);
}

void backwardEngine(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
}


void leftEngine(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  
  digitalWrite(in5, LOW);
  digitalWrite(in6, HIGH);
  
  digitalWrite(in7, LOW);
  digitalWrite(in8, HIGH);
}

void rightEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  
  digitalWrite(in5, HIGH);
  digitalWrite(in6, LOW);
  
  digitalWrite(in7, HIGH);
  digitalWrite(in8, LOW);
}

void stopEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  
  digitalWrite(in5, LOW);
  digitalWrite(in6, LOW);
  
  digitalWrite(in7, LOW);
  digitalWrite(in8, LOW);
}
