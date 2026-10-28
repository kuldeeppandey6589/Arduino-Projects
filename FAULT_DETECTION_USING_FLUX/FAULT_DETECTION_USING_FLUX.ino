#include <Arduino.h>

// Relay Pins
#define RELAY_1   8
#define RELAY_2   9
#define RELAY_3   10

// Fault Pins
#define FAULT_1   2
#define FAULT_2   3
#define FAULT_3   4

// Relay Logic (Active LOW)
#define RELAY_ON    LOW
#define RELAY_OFF   HIGH

void setup() {
  Serial.begin(9600);

  // Relay Pins
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  // Fault Pins
  pinMode(FAULT_1, INPUT);
  pinMode(FAULT_2, INPUT);
  pinMode(FAULT_3, INPUT);

  // Initial OFF
  digitalWrite(RELAY_1, RELAY_OFF);
  digitalWrite(RELAY_2, RELAY_OFF);
  digitalWrite(RELAY_3, RELAY_OFF);

  Serial.println("System Started");
}

void loop() {
  // Read Faults
  int f1 = digitalRead(FAULT_1);
  int f2 = digitalRead(FAULT_2);
  int f3 = digitalRead(FAULT_3);

  // Relay 1 Control
  if (f1 == HIGH) {
    digitalWrite(RELAY_1, RELAY_ON);
    Serial.println("FAULT 1 -> Relay 1 ON");
  } else {
    digitalWrite(RELAY_1, RELAY_OFF);
    Serial.println("No Fault 1 -> Relay 1 OFF");
  }

  // Relay 2 Control
  if (f2 == HIGH) {
    digitalWrite(RELAY_2, RELAY_ON);
    Serial.println("FAULT 2 -> Relay 2 ON");
  } else {
    digitalWrite(RELAY_2, RELAY_OFF);
    Serial.println("No Fault 2 -> Relay 2 OFF");
  }

  // Relay 3 Control
  if (f3 == HIGH) {
    digitalWrite(RELAY_3, RELAY_ON);
    Serial.println("FAULT 3 -> Relay 3 ON");
  } else {
    digitalWrite(RELAY_3, RELAY_OFF);
    Serial.println("No Fault 3 -> Relay 3 OFF");
  }

  Serial.println("----------------------");
  delay(500);
}