#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>


// ==========================
// LCD
// ==========================
LiquidCrystal_I2C lcd(0x27, 16, 2);   // Try 0x3F if 0x27 not working

// ==========================
// SIM800L Pins
// ==========================
// SIM800L TX -> Arduino D10
// SIM800L RX -> Arduino D11 through voltage divider
SoftwareSerial sim800(10, 11); // RX, TX

String mobileNumber = "+919936706276";   // Change your mobile number

// ==========================
// Relay Pins
// ==========================
#define RELAY_1 7
#define RELAY_2 6
#define RELAY_3 5

// ==========================
// Buzzer Pin
// ==========================
#define BUZZER_PIN 8

// ==========================
// Flex Sensor Pins
// ==========================
#define FLEX_1_PIN A0
#define FLEX_2_PIN A1
#define FLEX_3_PIN A2

// ==========================
// Threshold
// ==========================
int flexThreshold1 = 600;
int flexThreshold2 = 600;
int flexThreshold3 = 600;

// ==========================
// Relay states
// ==========================
bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;

// Startup control
bool firstReadingDone = false;

unsigned long lastFlexRead = 0;
const unsigned long flexInterval = 500;

// ==========================
// Relay Control
// ON = HIGH, OFF = LOW
// If relay works opposite, change HIGH/LOW here
// ==========================
void setRelay(uint8_t relayPin, bool state) {
  digitalWrite(relayPin, state ? HIGH : LOW);
}

// ==========================
// Read SIM800L Response
// ==========================
void readSIM800Response(unsigned long timeout) {
  unsigned long startTime = millis();

  while (millis() - startTime < timeout) {
    while (sim800.available()) {
      char c = sim800.read();
      Serial.write(c);
    }
  }
}

// ==========================
// Check SIM Status
// ==========================
void checkSIM800L() {
  Serial.println();
  Serial.println("Checking SIM800L...");

  sim800.println("AT");
  readSIM800Response(2000);

  sim800.println("AT+CPIN?");
  readSIM800Response(2000);

  sim800.println("AT+CSQ");
  readSIM800Response(2000);

  sim800.println("AT+CREG?");
  readSIM800Response(2000);

  sim800.println("AT+COPS?");
  readSIM800Response(2000);

  Serial.println("SIM800L Check Complete");
  Serial.println();
}

// ==========================
// Send SMS Function
// ==========================
void sendSMS(String message) {
  Serial.println();
  Serial.println("Sending SMS...");

  sim800.println("AT");
  readSIM800Response(1000);

  sim800.println("AT+CMGF=1");   // SMS text mode
  readSIM800Response(1000);

  sim800.println("AT+CSCS=\"GSM\"");
  readSIM800Response(1000);

  sim800.print("AT+CMGS=\"");
  sim800.print(mobileNumber);
  sim800.println("\"");
  readSIM800Response(3000);

  sim800.print(message);
  delay(500);

  sim800.write(26);  // CTRL+Z
  readSIM800Response(10000);

  Serial.println();
  Serial.println("SMS command completed");
}

// ==========================
// KM value function
// Same value shown on LCD
// ==========================
String getKMValue(int flexValue) {
  if (flexValue >= 350 && flexValue <= 430) {
    return "1KM";
  } else if (flexValue <= 500) {
    return "2KM";
  } else if (flexValue <= 550) {
    return "3KM";
  } else {
    return "0KM";
  }
}

// ==========================
// LCD Update
// ==========================
void updateLCD(int flex1, int flex2, int flex3) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(" R    Y    B");

  lcd.setCursor(0, 1);
  lcd.print(getKMValue(flex1));

  lcd.setCursor(5, 1);
  lcd.print(getKMValue(flex2));

  lcd.setCursor(10, 1);
  lcd.print(getKMValue(flex3));
}

// ==========================
// Create Fault SMS Message
// Sends flex raw value + same LCD value
// ==========================
String createFaultMessage(String phaseName, int flexValue) {
  String distanceValue = getKMValue(flexValue);

  String msg = "";

  msg += "Cable Fault Alert!\n";
  msg += "Fault Phase: " + phaseName + "\n";
  msg += "Flex Value: " + String(flexValue) + "\n";
  msg += "GPS Location: https://share.google/j5Jnd1fGkRRI4LMxO";

  return msg;
}

// ==========================
// Read Flex Sensors
// ==========================
void readFlexSensors() {
  int flex1 = analogRead(FLEX_1_PIN);
  int flex2 = analogRead(FLEX_2_PIN);
  int flex3 = analogRead(FLEX_3_PIN);

  Serial.print("F1=");
  Serial.print(flex1);
  Serial.print("  F2=");
  Serial.print(flex2);
  Serial.print("  F3=");
  Serial.println(flex3);

  bool newState1 = (flex1 > flexThreshold1);
  bool newState2 = (flex2 > flexThreshold2);
  bool newState3 = (flex3 > flexThreshold3);

  // ==========================
  // FIRST READING
  // No SMS at startup
  // ==========================
  if (firstReadingDone == false) {
    relayState1 = newState1;
    relayState2 = newState2;
    relayState3 = newState3;

    setRelay(RELAY_1, relayState1);
    setRelay(RELAY_2, relayState2);
    setRelay(RELAY_3, relayState3);

    if (relayState1 || relayState2 || relayState3) {
      digitalWrite(BUZZER_PIN, HIGH);
    } else {
      digitalWrite(BUZZER_PIN, LOW);
    }

    firstReadingDone = true;

    Serial.println("First reading done. Startup SMS skipped.");

    updateLCD(flex1, flex2, flex3);
    return;
  }

  // ==========================
  // SMS only when relay changes OFF to ON
  // ==========================
  if (newState1 == true && relayState1 == false) {
    Serial.println("R Phase fault detected. Relay ON. Sending SMS...");
    sendSMS(createFaultMessage("R Phase", flex1));
  }

  if (newState2 == true && relayState2 == false) {
    Serial.println("Y Phase fault detected. Relay ON. Sending SMS...");
    sendSMS(createFaultMessage("Y Phase", flex2));
  }

  if (newState3 == true && relayState3 == false) {
    Serial.println("B Phase fault detected. Relay ON. Sending SMS...");
    sendSMS(createFaultMessage("B Phase", flex3));
  }

  // ==========================
  // Update relay states
  // ==========================
  if (newState1 != relayState1) {
    relayState1 = newState1;
    setRelay(RELAY_1, relayState1);
    Serial.print("Relay 1 -> ");
    Serial.println(relayState1 ? "ON" : "OFF");
  }

  if (newState2 != relayState2) {
    relayState2 = newState2;
    setRelay(RELAY_2, relayState2);
    Serial.print("Relay 2 -> ");
    Serial.println(relayState2 ? "ON" : "OFF");
  }

  if (newState3 != relayState3) {
    relayState3 = newState3;
    setRelay(RELAY_3, relayState3);
    Serial.print("Relay 3 -> ");
    Serial.println(relayState3 ? "ON" : "OFF");
  }

  // ==========================
  // Buzzer
  // ==========================
  if (relayState1 || relayState2 || relayState3) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  updateLCD(flex1, flex2, flex3);
}

// ==========================
// Setup
// ==========================
void setup() {
  Serial.begin(9600);
  sim800.begin(9600);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Relay OFF at start
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Cable Fault");
  lcd.setCursor(0, 1);
  lcd.print("System Start");
  delay(2000);
  lcd.clear();

  Serial.println("System Started");
  Serial.println("SIM800L Starting...");
  delay(3000);

  sim800.println("AT");
  readSIM800Response(1000);

  sim800.println("AT+CMGF=1");
  readSIM800Response(1000);

  checkSIM800L();

  Serial.println("SIM800L Ready");

  lcd.setCursor(0, 0);
  lcd.print("System Ready");
  lcd.setCursor(0, 1);
  lcd.print("Waiting Fault");
  delay(1500);
  lcd.clear();
}

// ==========================
// Loop
// ==========================
void loop() {
  if (millis() - lastFlexRead >= flexInterval) {
    lastFlexRead = millis();
    readFlexSensors();
  }
}
