int enG1 = 2;
int enG2 = 3;

int in1 = 22;
int in2 = 23;

int in3 = 24;
int in4 = 25;

String stringFromPhone = "";
boolean bool1 = true, bool2 = false;
int valueX, valueY, valueSpeed, valueHead, turnValueSpeed;

void setup() {
  Serial3.begin(9600);
  delay(100);
  
  pinMode(enG1, OUTPUT);
  pinMode(enG2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  while (Serial3.available() > 0){
    char c = Serial3.read();
    if(c!='\n')    stringFromPhone += c; //Если не конец строки, добавляем символ  
    else bool2=true;   
  }
 
  if (bool2){ //Если конец строки, то разбираем ее на ключ и значение
    bool2=false;
   
    String key = "", value = "";

    key = stringFromPhone.substring(0,stringFromPhone.indexOf(' '));
    value = stringFromPhone.substring(stringFromPhone.indexOf(' ')+1);
    stringFromPhone = "";

    Serial3.print("KEY:");
    Serial3.println(key);

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

//    if (valueX == 0 and valueY == 0){
//      stopEngine();
//    }
//    else if (valueX >0 and valueX <10 and abs(valueY)==10){
//      analogWrite(enG1, valueSpeed*(0.5+valueX/10));
//      analogWrite(enG2, valueSpeed);
//    }

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
    analogWrite(enG1, valueSpeed);
    analogWrite(enG2, valueSpeed);
    turnValueSpeed = valueSpeed + 50;
    if (turnValueSpeed > 255){
      turnValueSpeed = 255;
    }
   }
  }
   
}

void forwardEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void backwardEngine(){
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}


void leftEngine(){
  analogWrite(enG1, turnValueSpeed);
  analogWrite(enG2, turnValueSpeed * 0.5);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
}

void rightEngine(){
  analogWrite(enG1, turnValueSpeed * 0.5);
  analogWrite(enG2, turnValueSpeed);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
    
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enG1, valueSpeed);
  analogWrite(enG2, valueSpeed);
}

void stopEngine(){
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
