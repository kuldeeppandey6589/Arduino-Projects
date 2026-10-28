#include <Arduino.h>
#include <Servo.h>

// =====================================================
// SENSOR PINS
// =====================================================

// Flame sensors
#define FLAME_SENSOR_1  2
#define FLAME_SENSOR_2  7

// Gas sensor
#define MQ_SENSOR       3

// Buzzer
#define BUZZER          4

// =====================================================
// L298N MOTOR DRIVER
// =====================================================

#define IN1 5
#define IN2 6
#define IN3 9
#define IN4 10

// =====================================================
// WATER PUMP RELAY
// =====================================================

#define PUMP_RELAY 11

// Most relay modules are ACTIVE LOW.
// If your relay works opposite, change these two values.
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// =====================================================
// SERVO
// =====================================================

#define SERVO_PIN 8

Servo waterServo;

// Servo positions
#define SERVO_LEFT    50
#define SERVO_CENTER  90
#define SERVO_RIGHT   130

// =====================================================
// MOTOR FUNCTIONS
// =====================================================

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void moveForward() {
  // Left motor
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Optional functions for future use

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// =====================================================
// PUMP FUNCTIONS
// =====================================================

void pumpON() {
  digitalWrite(PUMP_RELAY, RELAY_ON);
}

void pumpOFF() {
  digitalWrite(PUMP_RELAY, RELAY_OFF);
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(9600);

  // Sensors
  pinMode(FLAME_SENSOR_1, INPUT);
  pinMode(FLAME_SENSOR_2, INPUT);
  pinMode(MQ_SENSOR, INPUT);

  // Buzzer
  pinMode(BUZZER, OUTPUT);

  // Motors
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Relay
  pinMode(PUMP_RELAY, OUTPUT);

  // Servo
  waterServo.attach(SERVO_PIN);

  // Initial states
  stopCar();

  digitalWrite(BUZZER, LOW);

  pumpOFF();

  // Center water nozzle
  waterServo.write(SERVO_CENTER);

  delay(500);

  Serial.println("-----------------------------");
  Serial.println("Fire Fighting Robot Started");
  Serial.println("-----------------------------");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // Most flame sensor modules give LOW when flame detected
  bool flame1Detected =
      (digitalRead(FLAME_SENSOR_1) == LOW);

  bool flame2Detected =
      (digitalRead(FLAME_SENSOR_2) == LOW);

  // MQ digital output normally becomes LOW when threshold reached
  bool gasDetected =
      (digitalRead(MQ_SENSOR) == LOW);

  // ===================================================
  // FLAME DETECTION
  // ===================================================

  if (flame1Detected && flame2Detected) {

    // Fire detected by BOTH sensors
    Serial.println("FIRE CENTER");

    moveForward();

    // Aim nozzle straight
    waterServo.write(SERVO_CENTER);

    // Start water pump
    pumpON();
  }

  else if (flame1Detected) {

    Serial.println("FIRE SENSOR 1 DETECTED");

    moveForward();

    // Aim towards sensor 1 side
    waterServo.write(SERVO_LEFT);

    // Start pump
    pumpON();
  }

  else if (flame2Detected) {

    Serial.println("FIRE SENSOR 2 DETECTED");

    moveForward();

    // Aim towards sensor 2 side
    waterServo.write(SERVO_RIGHT);

    // Start pump
    pumpON();
  }

  else {

    // No fire detected

    Serial.println("NO FIRE - CAR STOPPED");

    stopCar();

    // Water pump OFF
    pumpOFF();

    // Return nozzle to center
    waterServo.write(SERVO_CENTER);
  }

  // ===================================================
  // GAS DETECTION
  // ===================================================

  if (gasDetected) {

    digitalWrite(BUZZER, HIGH);

    Serial.println("GAS DETECTED - BUZZER ON");
  }

  else {

    digitalWrite(BUZZER, LOW);
  }

  // ===================================================
  // DEBUG INFORMATION
  // ===================================================

  Serial.print("Flame 1: ");
  Serial.print(flame1Detected ? "FIRE" : "NORMAL");

  Serial.print(" | Flame 2: ");
  Serial.print(flame2Detected ? "FIRE" : "NORMAL");

  Serial.print(" | Gas: ");
  Serial.println(gasDetected ? "DETECTED" : "NORMAL");

  delay(100);
}





// #include <Arduino.h>
// #include <Servo.h>

// // =============================
// // SENSOR PINS
// // =============================
// #define FLAME1 2
// #define MQ_SENSOR 3
// #define FLAME2 7

// // =============================
// // BUZZER
// // =============================
// #define BUZZER 4

// // =============================
// // L298N MOTOR DRIVER
// // =============================
// #define IN1 5
// #define IN2 6
// #define IN3 9
// #define IN4 10

// // =============================
// // SERVO AND WATER PUMP
// // =============================
// #define SERVO_PIN 8
// #define RELAY_PIN 11

// // Change to HIGH if your sensor
// // outputs HIGH when triggered.
// const int FLAME_ACTIVE = LOW;
// const int MQ_ACTIVE = LOW;

// // Most relay modules are active LOW.
// // Change to HIGH for active-HIGH relay.
// const int RELAY_ON = LOW;
// const int RELAY_OFF = HIGH;

// Servo fireServo;

// void stopCar() {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);
// }

// void moveForward() {
//   digitalWrite(IN1, HIGH);
//   digitalWrite(IN2, LOW);

//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
// }

// void setup() {
//   Serial.begin(9600);

//   pinMode(FLAME1, INPUT);
//   pinMode(FLAME2, INPUT);
//   pinMode(MQ_SENSOR, INPUT);

//   pinMode(BUZZER, OUTPUT);

//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);

//   // Set relay OFF before operation
//   digitalWrite(RELAY_PIN, RELAY_OFF);
//   pinMode(RELAY_PIN, OUTPUT);

//   digitalWrite(BUZZER, LOW);
//   stopCar();

//   fireServo.attach(SERVO_PIN);
//   fireServo.write(90);

//   Serial.println("Fire Fighting Robot Ready");
// }

// void loop() {
//   bool flame1Detected =
//       (digitalRead(FLAME1) == FLAME_ACTIVE);

//   bool flame2Detected =
//       (digitalRead(FLAME2) == FLAME_ACTIVE);

//   bool gasDetected =
//       (digitalRead(MQ_SENSOR) == MQ_ACTIVE);

//   bool fireDetected =
//       flame1Detected || flame2Detected;

//   // =============================
//   // GAS DETECTION AND BUZZER
//   // =============================
//   digitalWrite(BUZZER, gasDetected ? HIGH : LOW);

//   if (gasDetected) {
//     Serial.println("GAS DETECTED - BUZZER ON");
//   }

//   // =============================
//   // FIRE DETECTION
//   // =============================
//   if (fireDetected) {
//     Serial.println("FIRE DETECTED");

//     // Move robot toward the detected fire
//     moveForward();

//     // Turn ON water pump
//     digitalWrite(RELAY_PIN, RELAY_ON);

//     // Sweep servo nozzle
//     // Stop briefly at each position
//     for (int angle = 0; angle <= 180; angle += 5) {
//       fireServo.write(angle);
//       delay(40);
//     }

//     for (int angle = 180; angle >= 0; angle -= 5) {
//       fireServo.write(angle);
//       delay(40);
//     }

//   } else {
//     Serial.println("NO FIRE");

//     // Stop robot and water pump
//     stopCar();
//     digitalWrite(RELAY_PIN, RELAY_OFF);

//     // Center servo
//     fireServo.write(90);
//   }

//   delay(100);
// }
