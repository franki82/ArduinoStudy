int sensorPin = A0; // суда подключаем А0
int sensorValue =0 ; // начальный уровень
int digitalInPin = 2; 
void setup() {
Serial.begin(9600);
pinMode(sensorPin,INPUT);
Serial.println("audio test");
}
void loop() {
  sensorValue = analogRead(sensorPin);
  if(sensorValue>40){
    Serial.println(sensorValue);
    sensorValue=0;
    delay(1000);
  }
}

