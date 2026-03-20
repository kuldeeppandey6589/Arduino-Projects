// #include <SoftwareSerial.h>

// // ----------- SIM800L & GPS -------------
// SoftwareSerial sim800(2, 3);   // SIM800L RX, TX
// SoftwareSerial gps(4, 5);      // GPS RX, TX

// // ----------- BUTTON --------------------
// #define BUTTON_PIN 6

// // ----------- PHONE NUMBER --------------
// String phone = "+919XXXXXXXXX";   // Replace with your number

// String latitude = "";
// String longitude = "";

// void setup() {
//   pinMode(BUTTON_PIN, INPUT_PULLUP);

//   Serial.begin(9600);
//   sim800.begin(9600);
//   gps.begin(9600);

//   delay(3000);

//   sim800.println("AT");
//   delay(1000);
//   sim800.println("AT+CMGF=1");  // SMS text mode
//   delay(1000);

//   Serial.println("Women Safety Band Ready");
// }

// void loop() {
//   readGPS();

//   if (digitalRead(BUTTON_PIN) == LOW) {
//     Serial.println("Emergency Button Pressed");

//     sendSMS();
//     makeCall();

//     delay(15000);  // Avoid repeat triggering
//   }
// }

// // ----------- READ GPS ------------------
// void readGPS() {
//   while (gps.available()) {
//     String data = gps.readStringUntil('\n');

//     if (data.startsWith("$GPGGA")) {
//       String parts[15];
//       int index = 0;

//       for (int i = 0; i < data.length(); i++) {
//         if (data[i] == ',' || data[i] == '*') {
//           index++;
//         } else {
//           parts[index] += data[i];
//         }
//       }

//       if (parts[2].length() > 0 && parts[4].length() > 0) {
//         latitude = parts[2];
//         longitude = parts[4];
//       }
//     }
//   }
// }

// // ----------- SEND SMS ------------------
// void sendSMS() {
//   sim800.println("AT+CMGS=\"" + phone + "\"");
//   delay(1000);

//   sim800.print("HELP! I am in danger.\n");
//   sim800.print("Location:\n");
//   sim800.print("https://maps.app.goo.gl/4jDreaE3HYJKWCK97?g_st=aw");
//   sim800.print(latitude);
//   sim800.print(",");
//   sim800.print(longitude);

//   delay(500);
//   sim800.write(26);  // CTRL+Z
//   delay(5000);

//   Serial.println("SMS Sent");
// }

// // ----------- MAKE CALL -----------------
// void makeCall() {
//   sim800.println("ATD" + phone + ";");
//   delay(15000);
//   sim800.println("ATH");
// }




#include <SoftwareSerial.h>

// ----------- SIM800L & GPS -------------
SoftwareSerial sim800(2, 3);   // SIM800L RX, TX
SoftwareSerial gps(4, 5);      // GPS RX, TX

// ----------- BUTTON --------------------
#define BUTTON_PIN 6

// ----------- PHONE NUMBER --------------
String phone = "+916307159040";   // Replace with your number

String latitude = "";
String longitude = "";

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  sim800.begin(9600);
  gps.begin(9600);

  Serial.println("================================");
  Serial.println(" Women Safety Band Starting ");
  Serial.println("================================");

  delay(3000);

  sim800.println("AT");
  delay(1000);
  Serial.println("SIM800L AT command sent");

  sim800.println("AT+CMGF=1");  // SMS text mode
  delay(1000);
  Serial.println("SIM800L SMS Text Mode Enabled");

  Serial.println("System Ready...");
}

void loop() {

  Serial.println("--------------------------------");
  Serial.println("Loop Running...");
  
  // Read GPS continuously
  readGPS();

  Serial.print("Button State: ");
  Serial.println(digitalRead(BUTTON_PIN));

  if (digitalRead(BUTTON_PIN) == LOW) {
    Serial.println("!!! EMERGENCY BUTTON PRESSED !!!");

    sendSMS();
    makeCall();

    Serial.println("Waiting before next trigger...");
    delay(15000);  // Avoid repeat triggering
  }

  delay(1000); // Loop delay for readability
}

// ----------- READ GPS ------------------
void readGPS() {
  Serial.println("Reading GPS Data...");

  while (gps.available()) { 
    String data = gps.readStringUntil('\n');

    if (data.startsWith("$GPGGA")) {
      Serial.println("GPGGA Data Found:");
      Serial.println(data);

      String parts[15];
      int index = 0;

      for (int i = 0; i < data.length(); i++) {
        if (data[i] == ',' || data[i] == '*') {
          index++;
        } else {
          parts[index] += data[i];
        }
      }

      if (parts[2].length() > 0 && parts[4].length() > 0) {
        latitude = parts[2];
        longitude = parts[4];

        Serial.print("Latitude: ");
        Serial.println(latitude);
        Serial.print("Longitude: ");
        Serial.println(longitude);
      } else {
        Serial.println("GPS Fix Not Ready");
      }
    }
  }
}

// ----------- SEND SMS ------------------
void sendSMS() {
  Serial.println("Sending SMS...");

  sim800.println("AT+CMGS=\"" + phone + "\"");
  delay(1000);

  sim800.print("HELP! I am in danger.\n");
  sim800.print("Location:\n");
  sim800.print("https://maps.app.goo.gl/4jDreaE3HYJKWCK97?g_st=aw");
  sim800.print(latitude);
  sim800.print(",");
  sim800.print(longitude);

  delay(500);
  sim800.write(26);  // CTRL+Z
  delay(5000);

  Serial.println("SMS Sent Successfully");
}

// ----------- MAKE CALL -----------------
void makeCall() {
  Serial.println("Making Emergency Call...");

  sim800.println("ATD" + phone + ";");
  delay(15000);

  sim800.println("ATH");
  Serial.println("Call Ended");
}


