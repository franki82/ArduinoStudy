const int prx_pin = 15;
const int led_pin = 13;
int val;

void setup() {
    Serial.begin(9600);
    pinMode(prx_pin, INPUT);
   // pinMode(led_pin, OUTPUT);
}

void loop() {
    val = digitalRead(prx_pin);

    Serial.println(val);

    if(val == LOW)
        //digitalWrite(led_pin, HIGH);
        Serial.println("detect");
    //else
        //digitalWrite(led_pin, LOW);
}
