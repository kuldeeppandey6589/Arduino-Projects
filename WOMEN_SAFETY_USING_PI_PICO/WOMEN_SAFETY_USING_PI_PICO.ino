#include <Arduino.h>
#include <TinyGPSPlus.h>

// ==========================
// Phone Number
// ==========================
String phoneNumber = "+918090107996";   // Change number if needed

// ==========================
// SIM800L Pins
// Pico GP4 TX -> SIM800L RX through voltage divider
// Pico GP5 RX <- SIM800L TX
// ==========================
#define GSM_TX_PIN 4
#define GSM_RX_PIN 5
#define GSM Serial2

// ==========================
// GPS Pins
// Pico GP0 TX -> GPS RX
// Pico GP1 RX <- GPS TX
// ==========================
#define GPS_TX_PIN 0
#define GPS_RX_PIN 1
#define GPS Serial1

// ==========================
// Push Button
// Button one side -> GP15
// Button other side -> GND
// ==========================
#define BUTTON_PIN 15

TinyGPSPlus gps;

int smsCount = 1;
bool buttonLocked = false;

// ==========================
// Clear GSM Buffer
// ==========================
void clearGSM() {
  while (GSM.available()) {
    GSM.read();
  }
}

// ==========================
// Read GSM Response
// ==========================
String readGSM(unsigned long timeoutMs) {
  String response = "";
  unsigned long startTime = millis();

  while (millis() - startTime < timeoutMs) {
    while (GSM.available()) {
      char c = GSM.read();
      response += c;
      Serial.write(c);
    }
    delay(1);
  }

  return response;
}

// ==========================
// Send AT Command
// ==========================
bool sendAT(String command, String expected, unsigned long timeoutMs) {
  clearGSM();

  Serial.print("\nSending: ");
  Serial.println(command);

  GSM.println(command);

  String response = readGSM(timeoutMs);

  if (response.indexOf(expected) != -1) {
    Serial.println("\nResult: OK");
    return true;
  }

  Serial.println("\nResult: FAILED");
  Serial.print("Expected: ");
  Serial.println(expected);
  return false;
}

// ==========================
// GSM Network Check
// ==========================
bool checkNetwork() {
  Serial.println("\nChecking GSM Network...");

  if (!sendAT("AT", "OK", 3000)) {
    Serial.println("Error: SIM800L not responding.");
    return false;
  }

  if (!sendAT("AT+CPIN?", "READY", 3000)) {
    Serial.println("Error: SIM card not ready.");
    return false;
  }

  sendAT("AT+CSQ", "OK", 3000);

  clearGSM();
  GSM.println("AT+CREG?");
  String response = readGSM(3000);

  if (response.indexOf("+CREG: 0,1") != -1 || response.indexOf("+CREG: 0,5") != -1) {
    Serial.println("\nNetwork registered.");
    return true;
  }

  Serial.println("\nError: Network not registered.");
  Serial.println("AT+CREG? should show 0,1 or 0,5");
  return false;
}

// ==========================
// Send SMS
// ==========================
bool sendSMS(String number, String message) {
  Serial.println("\n========== Sending SMS ==========");

  if (!checkNetwork()) {
    Serial.println("SMS stopped: GSM network problem.");
    return false;
  }

  if (!sendAT("AT+CMGF=1", "OK", 3000)) {
    Serial.println("Error: Cannot set SMS text mode.");
    return false;
  }

  if (!sendAT("AT+CSCS=\"GSM\"", "OK", 3000)) {
    Serial.println("Warning: Character set command failed.");
  }

  clearGSM();

  Serial.print("\nSending SMS to: ");
  Serial.println(number);

  GSM.print("AT+CMGS=\"");
  GSM.print(number);
  GSM.println("\"");

  String response = readGSM(7000);

  if (response.indexOf(">") == -1) {
    Serial.println("\nError: SIM800L did not give SMS prompt >");
    Serial.println("Possible reasons:");
    Serial.println("1. SIM card problem");
    Serial.println("2. Network not registered");
    Serial.println("3. Low power supply");
    Serial.println("4. Wrong baud rate");
    return false;
  }

  Serial.println("\nWriting message...");
  GSM.print(message);
  delay(500);

  GSM.write(26);   // CTRL + Z
  Serial.println("CTRL+Z sent. Waiting for final response...");

  response = readGSM(25000);

  if (response.indexOf("+CMGS:") != -1 && response.indexOf("OK") != -1) {
    Serial.println("\nSMS sent successfully.");
    return true;
  }

  Serial.println("\nSMS failed.");
  Serial.println("Possible reasons:");
  Serial.println("1. SIM800L power supply weak");
  Serial.println("2. No balance / SMS pack");
  Serial.println("3. Network signal weak");
  Serial.println("4. SIM not registered on network");
  return false;
}

// ==========================
// Read GPS Data
// ==========================
void readGPS(unsigned long durationMs) {
  unsigned long startTime = millis();

  while (millis() - startTime < durationMs) {
    while (GPS.available()) {
      gps.encode(GPS.read());
    }
    delay(1);
  }
}

// ==========================
// Create Location Message
// ==========================
String createLocationMessage() {
  Serial.println("\nReading GPS location for 10 seconds...");

  readGPS(10000);

  String message = "Emergency Alert!\n";
  message += "Help Needed.\n";
  message += "SMS Count: ";
  message += String(smsCount);
  message += "\n";

  if (gps.location.isValid()) {
    double latitude = gps.location.lat();
    double longitude = gps.location.lng();

    message += "Latitude: ";
    message += String(latitude, 6);
    message += "\n";

    message += "Longitude: ";
    message += String(longitude, 6);
    message += "\n";

    message += "Map: https://maps.google.com/?q=";
    message += String(latitude, 6);
    message += ",";
    message += String(longitude, 6);

    Serial.println("GPS location found.");
    Serial.print("Latitude: ");
    Serial.println(latitude, 6);
    Serial.print("Longitude: ");
    Serial.println(longitude, 6);
  } else {
    message += "GPS Location : https://maps.app.goo.gl/NJGHivhi4beDJAdKA?g_st=ac";

    Serial.println("GPS location not fixed.");
    Serial.print("Satellites: ");
    Serial.println(gps.satellites.value());
  }

  return message;
}

// ==========================
// GSM Setup Check
// ==========================
void checkGSMStartup() {
  Serial.println("\n========== GSM Startup Check ==========");

  sendAT("AT", "OK", 3000);
  sendAT("ATE0", "OK", 3000);
  sendAT("AT+CPIN?", "READY", 3000);
  sendAT("AT+CSQ", "OK", 3000);
  sendAT("AT+CREG?", "OK", 3000);
  sendAT("AT+CMGF=1", "OK", 3000);

  Serial.println("=======================================");
}

// ==========================
// Setup
// ==========================
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // GSM Serial
  GSM.setTX(GSM_TX_PIN);
  GSM.setRX(GSM_RX_PIN);
  GSM.begin(9600);

  // GPS Serial
  GPS.setTX(GPS_TX_PIN);
  GPS.setRX(GPS_RX_PIN);
  GPS.begin(9600);

  Serial.println("\nPico W SIM800L + GPS + Button SMS System Starting...");
  Serial.println("Press button to send emergency SMS with GPS location.");

  delay(5000);

  checkGSMStartup();

  Serial.println("\nSystem Ready.");
}

// ==========================
// Loop
// ==========================
void loop() {
  // Keep reading GPS continuously
  while (GPS.available()) {
    gps.encode(GPS.read());
  }

  int buttonState = digitalRead(BUTTON_PIN);

  // Button pressed
  if (buttonState == LOW && buttonLocked == false) {
    buttonLocked = true;

    Serial.println("\nButton Pressed!");
    Serial.println("Preparing location SMS...");

    String smsMessage = createLocationMessage();

    Serial.println("\nSMS Message:");
    Serial.println(smsMessage);

    if (sendSMS(phoneNumber, smsMessage)) {
      smsCount++;
      Serial.println("Emergency SMS sent.");
    } else {
      Serial.println("Emergency SMS failed.");
    }
  }

  // Button released
  if (buttonState == HIGH) {
    buttonLocked = false;
  }

  delay(50);
}