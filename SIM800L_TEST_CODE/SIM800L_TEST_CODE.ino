#include <SoftwareSerial.h>

SoftwareSerial sim800(7, 8);   // Arduino RX, TX

// =================================================
// SETUP
// =================================================

void setup() {

  Serial.begin(9600);
  sim800.begin(9600);

  delay(3000);

  Serial.println(F(""));
  Serial.println(F("================================"));
  Serial.println(F("       SIM800L TEST"));
  Serial.println(F("================================"));
  Serial.println(F(""));

  // -----------------------------------------------
  // 1. AT TEST
  // -----------------------------------------------

  Serial.println(F("[1] Testing SIM800L..."));

  if (sendCommand("AT", 3000)) {

    Serial.println(F("OK - SIM800L is responding"));

  } else {

    Serial.println(F("ERROR - SIM800L not responding"));
    Serial.println(F(""));
    Serial.println(F("CHECK:"));
    Serial.println(F("1. SIM800L power"));
    Serial.println(F("2. VCC should be about 4V"));
    Serial.println(F("3. Power supply current"));
    Serial.println(F("4. TX/RX wiring"));
    Serial.println(F("5. Common GND"));
    Serial.println(F("6. Baud rate"));
    Serial.println(F(""));

    return;
  }


  // -----------------------------------------------
  // 2. ECHO OFF
  // -----------------------------------------------

  Serial.println(F("[2] Turning echo OFF..."));

  sim800.println(F("ATE0"));
  delay(500);

  clearBuffer();


  // -----------------------------------------------
  // 3. MODULE INFORMATION
  // -----------------------------------------------

  Serial.println(F("[3] Module information"));

  sim800.println(F("ATI"));

  readResponse(3000);


  // -----------------------------------------------
  // 4. SIM CARD
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("[4] Checking SIM card..."));

  sim800.println(F("AT+CPIN?"));

  if (waitFor("READY", 5000)) {

    Serial.println(F("OK - SIM card is READY"));

  } else {

    Serial.println(F("ERROR - SIM card problem"));
    Serial.println(F("Check SIM insertion / SIM PIN"));
  }


  // -----------------------------------------------
  // 5. SIGNAL
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("[5] Checking signal..."));

  sim800.println(F("AT+CSQ"));

  readResponse(3000);


  // -----------------------------------------------
  // 6. NETWORK REGISTRATION
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("[6] Checking network registration..."));

  sim800.println(F("AT+CREG?"));

  readResponse(3000);


  // -----------------------------------------------
  // 7. OPERATOR
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("[7] Checking network operator..."));

  sim800.println(F("AT+COPS?"));

  readResponse(5000);


  // -----------------------------------------------
  // 8. SMS MODE
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("[8] Setting SMS text mode..."));

  sim800.println(F("AT+CMGF=1"));

  if (waitFor("OK", 3000)) {

    Serial.println(F("OK - SMS mode enabled"));

  } else {

    Serial.println(F("ERROR - SMS mode failed"));
  }


  // -----------------------------------------------
  // COMPLETE
  // -----------------------------------------------

  Serial.println(F(""));
  Serial.println(F("================================"));
  Serial.println(F("       TEST COMPLETE"));
  Serial.println(F("================================"));
  Serial.println(F(""));
  Serial.println(F("Type AT commands in Serial Monitor."));
}


// =================================================
// LOOP
// =================================================

void loop() {

  // Arduino -> SIM800L

  if (Serial.available()) {

    while (Serial.available()) {

      char c = Serial.read();

      sim800.write(c);
    }
  }

  // SIM800L -> Arduino

  if (sim800.available()) {

    while (sim800.available()) {

      Serial.write(sim800.read());
    }
  }
}


// =================================================
// SEND COMMAND
// =================================================

bool sendCommand(const char *command, unsigned long timeout) {

  clearBuffer();

  sim800.println(command);

  return waitFor("OK", timeout);
}


// =================================================
// WAIT FOR RESPONSE
// =================================================

bool waitFor(const char *target, unsigned long timeout) {

  unsigned long start = millis();

  uint8_t matched = 0;

  while (millis() - start < timeout) {

    if (sim800.available()) {

      char c = sim800.read();

      Serial.write(c);

      if (c == target[matched]) {

        matched++;

        if (target[matched] == '\0') {

          return true;
        }

      } else {

        matched = 0;
      }
    }
  }

  return false;
}


// =================================================
// READ RESPONSE
// =================================================

void readResponse(unsigned long timeout) {

  unsigned long start = millis();

  while (millis() - start < timeout) {

    while (sim800.available()) {

      Serial.write(sim800.read());

      start = millis();
    }
  }

  Serial.println();
}


// =================================================
// CLEAR SERIAL BUFFER
// =================================================

void clearBuffer() {

  while (sim800.available()) {

    sim800.read();
  }
}