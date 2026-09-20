#include <SoftwareSerial.h>

SoftwareSerial sim800(10, 11); 
// Arduino D10 = RX  connect to SIM800L TX
// Arduino D11 = TX  connect to SIM800L RX through voltage divider

String phoneNumber = "+919999999999";  // Change your number

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);

  Serial.println("SIM800L Test Started");
  delay(3000);

  sendAT("AT", 2000);
  sendAT("ATE0", 2000);
  sendAT("AT+CPIN?", 2000);
  sendAT("AT+CSQ", 2000);
  sendAT("AT+CREG?", 2000);
  sendAT("AT+CMGF=1", 2000);

  sendSMS(phoneNumber, "Hello, SIM800L test SMS from Arduino");
}

void loop() {
  // Read SIM800L response and print on Serial Monitor
  while (sim800.available()) {
    Serial.write(sim800.read());
  }

  // Send command from Serial Monitor to SIM800L
  while (Serial.available()) {
    sim800.write(Serial.read());
  }
}

void sendAT(String command, int waitTime) {
  Serial.print("Sending: ");
  Serial.println(command);

  sim800.println(command);
  delay(waitTime);

  while (sim800.available()) {
    Serial.write(sim800.read());
  }

  Serial.println();
}

void sendSMS(String number, String message) {
  Serial.println("Sending SMS...");

  sim800.println("AT+CMGF=1");
  delay(1000);

  sim800.print("AT+CMGS=\"");
  sim800.print(number);
  sim800.println("\"");
  delay(1000);

  sim800.print(message);
  delay(500);

  sim800.write(26);   // CTRL+Z
  delay(7000);

  while (sim800.available()) {
    Serial.write(sim800.read());
  }

  Serial.println("SMS function complete");
}

// #include <HardwareSerial.h>

// // SIM800L pins
// // SIM800L TX -> ESP32 GPIO16
// // SIM800L RX -> ESP32 GPIO17
// #define SIM800_RX 16
// #define SIM800_TX 17

// HardwareSerial sim800(1);

// // Change this mobile number
// String phoneNumber = "+918960383086";

// // Default message
// String defaultMessage = "Hello, this is ESP32 SIM800L test message.";

// unsigned long lastSmsTime = 0;
// const unsigned long smsInterval = 2000;  // 2 seconds

// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   sim800.begin(9600, SERIAL_8N1, SIM800_RX, SIM800_TX);
//   delay(3000);

//   Serial.println("SIM800L SMS Test Started");

//   sendAT("AT", 1000);
//   sendAT("AT+CMGF=1", 1000);       // SMS text mode
//   sendAT("AT+CSCS=\"GSM\"", 1000); // GSM character set
// }

// void loop() {
//   if (millis() - lastSmsTime >= smsInterval) {
//     lastSmsTime = millis();

//     sendSMS(phoneNumber, defaultMessage);
//   }
// }

// void sendAT(String command, int waitTime) {
//   sim800.println(command);
//   delay(waitTime);

//   while (sim800.available()) {
//     Serial.write(sim800.read());
//   }
// }

// void sendSMS(String number, String message) {
//   Serial.println("Sending SMS...");

//   sim800.println("AT+CMGF=1");
//   delay(1000);

//   sim800.print("AT+CMGS=\"");
//   sim800.print(number);
//   sim800.println("\"");
//   delay(1000);

//   sim800.print(message);
//   delay(500);

//   sim800.write(26);  // CTRL + Z
//   delay(5000);

//   Serial.println("SMS Sent");
// }