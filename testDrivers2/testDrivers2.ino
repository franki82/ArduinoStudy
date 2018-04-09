int enG = 3;

int in1 = 2;
int in2 = 4;

int in3 = 5;
int in4 = 6;

void setup() {
  pinMode(enG, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  analogWrite(enG, 60);
  //front
//  digitalWrite(in1, LOW);
//  digitalWrite(in2, HIGH);
//  
//  digitalWrite(in3, HIGH);
//  digitalWrite(in4, LOW);

  //back
//  digitalWrite(in1, HIGH);
//  digitalWrite(in2, LOW);
//  
//  digitalWrite(in3, LOW);
//  digitalWrite(in4, HIGH);

  //right
//  digitalWrite(in1, HIGH);
//  digitalWrite(in2, LOW);
//  
//  digitalWrite(in3, HIGH);
//  digitalWrite(in4, LOW);

 //left
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

}
