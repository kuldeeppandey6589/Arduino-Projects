//LED with arduino steet light
const byte GREEN_LED = 8;     // LED anode connected here
const byte RED_LED = 9;
const byte Yellow_LED = 12;

void setup() {
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(Yellow_LED, OUTPUT);
    // turn LED OFF
}

void loop() {
  digitalWrite(GREEN_LED, HIGH); // turn LED ON
  digitalWrite(RED_LED, LOW);
  digitalWrite(Yellow_LED, LOW);
  delay(5000);                   // wait 5 000 ms (5 s)
  digitalWrite(Yellow_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  delay(2000);
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(Yellow_LED, LOW);
  delay(5000);
  // nothing more to do – LED stays OFF
}