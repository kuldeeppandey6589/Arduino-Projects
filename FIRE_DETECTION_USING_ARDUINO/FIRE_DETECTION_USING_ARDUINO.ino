#include <Servo.h>

const int flamePin = 2;   // Digital output from flame sensor
const int servoPin = 9;   // Servo signal pin
const int buzzer = 8;
      
Servo myServo;

void setup() {
  pinMode(flamePin, INPUT);   // Some modules need INPUT_PULLUP, see note below
  pinMode(buzzer, OUTPUT);
  myServo.attach(servoPin);

  // Start at 0 degrees
  myServo.write(0);
}

void loop() {
  int flameState = digitalRead(flamePin);

  // If your module works opposite, just swap the if/else bodies.
  
  if (flameState == LOW) {      // Flame detected
    myServo.write(90);          // Turn to 90 degrees   [']
    digitalWrite(buzzer, HIGH);
  } else {                      // No flame
    myServo.write(0);           // Back to 0 degrees
    digitalWrite(buzzer, LOW);
  }

  delay(100); // small delay for stability
}
