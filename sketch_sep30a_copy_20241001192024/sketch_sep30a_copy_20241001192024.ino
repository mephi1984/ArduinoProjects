const int buttonPin = 2; 
const int ledPin =  13; 
 
int buttonState = 0; 

bool ledState = true;
bool ledHold = false;
 
void setup() {
  pinMode(ledPin, OUTPUT);     
  pinMode(buttonPin, INPUT);   
}
 
void loop(){
  buttonState = digitalRead(buttonPin);
 
  if (buttonState == HIGH) {   

    if (!ledHold)
    {
        ledState = !ledState;
        ledHold = true;
    }
  }
  else {
    ledHold = false;
  }
  if (ledState)
  {
    digitalWrite(ledPin, HIGH); 
  }
  else
  {
    digitalWrite(ledPin, LOW); 
  }
}
