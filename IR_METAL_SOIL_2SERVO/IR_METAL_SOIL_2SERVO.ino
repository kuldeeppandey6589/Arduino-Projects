#include <Servo.h>

// ----------- SERVO OBJECTS ------------
Servo servo1;  // Proximity + Soil
Servo servo2;  // Extra / optional

// ----------- PIN DEFINITIONS ----------
#define IR_PIN 2
#define PROX_PIN 3
#define SOIL_PIN 4

#define SERVO1_PIN 9
#define SERVO2_PIN 10

// ----------- SOIL THRESHOLD -----------
int soilThreshold = 500;  // adjust after testing

void setup() {
  pinMode(IR_PIN, INPUT);
  pinMode(PROX_PIN, INPUT);
  pinMode(SOIL_PIN, INPUT);

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  servo1.write(0);  // initial position
  servo2.write(0);

  Serial.begin(9600);
  Serial.println("System Ready");
}

void loop() {

  int proxState = digitalRead(PROX_PIN);
  int irState = digitalRead(IR_PIN);
  int soilValue = digitalRead(SOIL_PIN);

  Serial.print("Proximity: ");
  Serial.print(proxState);
  Serial.print(" | Soil: ");
  Serial.println(soilValue);

  if (irState == HIGH) {
    Serial.println("IR HIGH → IR Servo OPEN");
    servo1.write(90);
    delay(2000);
  } else {
    Serial.println("IR LOW → IR Servo CLOSE");
    servo1.write(0);
  }

  // ----------- MAIN CONDITION ----------
  // if (proxState == HIGH) {  // object detected
  //   servo2.write(90);
  //   delay(2000);
  // } else {
  //   servo2.write(0);
  //   delay(2000);
  // }

  // if (soilValue == HIGH) {
  //   servo2.write(-90);
  //   delay(1000);
  // } else {
  //   servo2.write(0);
  //   delay(2000);
  // }

  if (proxState == HIGH && soilValue == LOW) {
    Serial.println("Servo +90");
    servo2.write(180);   // +90
    delay(3000);
    servo2.write(90);    // back to center
  }

  // -------- -90 DEGREE --------
  else if (soilValue == HIGH && proxState == LOW) {
    Serial.println("Servo -90");
    servo2.write(0);     // -90
    delay(3000);
    servo2.write(90);    // back to center
  }
}
