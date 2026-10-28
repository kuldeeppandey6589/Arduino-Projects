#include <SoftwareSerial.h>

SoftwareSerial bluetooth(2, 3); // RX, TX

int relayPin = 7;
char command;

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);   // Relay OFF initially
  
  bluetooth.begin(9600);         // HC-05 default baud rate
  Serial.begin(9600);
}

void loop() {
  if (bluetooth.available()) {
    command = bluetooth.read();

    if (command == 'A') {
      digitalWrite(relayPin, HIGH);  // Relay ON
      Serial.println("Relay ON");
    }

    if (command == 'a') {
      digitalWrite(relayPin, LOW);   // Relay OFF
      Serial.println("Relay OFF");
    }
  }
}

