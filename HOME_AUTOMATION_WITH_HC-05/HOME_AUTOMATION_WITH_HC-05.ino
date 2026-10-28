#include <SoftwareSerial.h>

// HC-05 Bluetooth
SoftwareSerial bt(10, 11); // RX, TX

// Relay pins
#define R1 2
#define R2 3
#define R3 4
#define R4 5

char command;

void setup() {
  pinMode(R1, OUTPUT);
  pinMode(R2, OUTPUT);
  pinMode(R3, OUTPUT);
  pinMode(R4, OUTPUT);

  // Relays OFF initially (Active LOW)
  digitalWrite(R1, HIGH);
  digitalWrite(R2, HIGH);
  digitalWrite(R3, HIGH);
  digitalWrite(R4, HIGH);

  Serial.begin(9600);
  bt.begin(9600);

  Serial.println("Bluetooth Home Automation Ready");
}

void loop() {
  if (bt.available()) {
    command = bt.read();
    Serial.println(command);

    switch (command) {
      case 'A': digitalWrite(R1, LOW); break;
      case 'a': digitalWrite(R1, HIGH); break;

      case 'B': digitalWrite(R2, LOW); break;
      case 'b': digitalWrite(R2, HIGH); break;

      case 'C': digitalWrite(R3, LOW); break;
      case 'c': digitalWrite(R3, HIGH); break;

      case 'D': digitalWrite(R4, LOW); break;
      case 'd': digitalWrite(R4, HIGH); break;
    }
  }
}



