const int button1Pin = 2;  // pushbutton 1 pin
const int button2Pin = 3;  // pushbutton 2 pin
const int ledPin1 =  12;
const int ledPin2 =  11;

int button1State, button2State;  // variables to hold the pushbutton states


void setup()
{
  // Set up the pushbutton pins to be an input:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT);

  // Set up the LED pin to be an output:
  pinMode(ledPin1, OUTPUT);      
  pinMode(ledPin2, OUTPUT);  
}

void loop()
{
  button1State = digitalRead(button1Pin);
  button2State = digitalRead(button2Pin);

  if (button1State == LOW)
  {
    digitalWrite(ledPin1, HIGH);
  }
  else if (button2State == LOW)
  {
    digitalWrite(ledPin2, HIGH);
  } else {
    digitalWrite(ledPin1, LOW);
    digitalWrite(ledPin2, LOW);
  }
}  
