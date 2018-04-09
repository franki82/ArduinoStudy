int rulePin = 12;
byte incomingByte;
int returnedFromPin = 0;

void setup() {
  pinMode(rulePin, INPUT_PULLUP); //INPUT use embedded resistor 100MOm (for any sensors), INPUT_PULLUP - without embedded resistor (for LED)
  pinMode(rulePin, OUTPUT);
  Serial.begin(9600);
  digitalWrite(rulePin, LOW);
}

void loop() {
  if (Serial.available() > 0){
    returnedFromPin = digitalRead(rulePin);
    incomingByte = Serial.read();
    if(incomingByte == '1' && returnedFromPin == LOW){
      digitalWrite(rulePin, HIGH);
      Serial.println("On");
    } else if (incomingByte == '0' && returnedFromPin == HIGH) {
      digitalWrite(rulePin, LOW);
      Serial.println("Off");
    }
    Serial.print("From keyboard: ");
    Serial.println(incomingByte);
    Serial.print("From pin before verification: ");
    Serial.println(returnedFromPin);
  }
}
