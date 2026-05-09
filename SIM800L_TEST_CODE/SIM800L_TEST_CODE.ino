#include <SoftwareSerial.h>

SoftwareSerial sim800(10, 11); // RX, TX

void setup() {
  Serial.begin(9600);
  sim800.begin(9600);

  Serial.println("SIM800L Test Starting...");
  delay(2000);
}

void loop() {
  // Send data from Serial Monitor to SIM800L
  if (Serial.available()) {
    char c = Serial.read();
    sim800.write(c);
  }

  // Read data from SIM800L and print to Serial Monitor
  if (sim800.available()) {
    char c = sim800.read();
    Serial.write(c);
  }
}


//type the AT command on serial moniter



// #include <SoftwareSerial.h>

// SoftwareSerial sim800(10, 11); // RX, TX

// void setup() {
//   Serial.begin(9600);
//   sim800.begin(9600);

//   delay(3000); // wait 3 seconds

//   Serial.println("Sending SMS...");

//   sim800.println("AT"); 
//   delay(1000);

//   sim800.println("AT+CMGF=1"); // Set SMS mode to text
//   delay(1000);

//   sim800.println("AT+CMGS=\"+919919505589\""); // Replace with your number
//   delay(1000);

//   sim800.print("Welcome! System is ready."); // Message
//   delay(500);

//   sim800.write(26); // CTRL+Z to send
//   delay(5000);

//   Serial.println("SMS Sent");
// }

// void loop() {
//   // nothing here
// }