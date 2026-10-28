#include <Servo.h>

// -------- Servo Objects ----------
Servo dryServo;
Servo irServo;
Servo metalServo;

// -------- Pin Definitions --------
int inputPin = 2;   // Digital sensor / switch
int IrPin    = 3;   // IR sensor
int SoilPin    = 4;   // IR sensor

void setup() {
  pinMode(inputPin, INPUT_PULLUP);  
  pinMode(IrPin, INPUT);
  pinMode(SoilPin, INPUT);

  dryServo.attach(9);   // Servo 1
  irServo.attach(10);   // Servo 2
  metalServo.attach(11);   // Servo 2

  Serial.begin(9600);
  Serial.println("Servo Test Started");
}

void loop() {

  int pinState = digitalRead(inputPin);
  int IRState  = digitalRead(IrPin);
  int metalState  = digitalRead(SoilPin);

  // -------- Control Dry Servo --------
  if (pinState == HIGH) {
    Serial.println("InputPin HIGH → Dry Servo CLOSE");
    dryServo.write(0);
    // delay(3000);
  } else {
    Serial.println("InputPin LOW → Dry Servo OPEN");
    dryServo.write(90);
    delay(3000);
  }

  // -------- Control IR Servo --------
  if (IRState == HIGH) {
    Serial.println("IR HIGH → IR Servo OPEN");
    irServo.write(90);
  } else {
    Serial.println("IR LOW → IR Servo CLOSE");
    irServo.write(0);
  }

  if (metalState == HIGH) {
    Serial.println("IR HIGH → IR Servo OPEN");
    metalServo.write(90);
    // delay(3000);
  } else {
    Serial.println("IR LOW → IR Servo CLOSE");
    metalServo.write(0);
    delay(3000);
  }
  delay(500);
}
