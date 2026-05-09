/*************************************************
 ESP8266 Overvoltage & Undervoltage Protection
 3 Relay + 10K Pot + LCD + Adafruit IO
*************************************************/

#include <ESP8266WiFi.h>
#include "AdafruitIO_WiFi.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- Adafruit IO ----------
#define IO_USERNAME  "overvolt260426"
#define IO_KEY       "aio_slWN51T53uL86sqDm1Hm7gokbCI4"

#define WIFI_SSID    "homeiot"
#define WIFI_PASS    "homeiot123"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

// ---------- Adafruit IO Feeds ----------
AdafruitIO_Feed *voltageFeed = io.feed("voltage");
AdafruitIO_Feed *statusFeed  = io.feed("voltage-status");
AdafruitIO_Feed *relay1Feed  = io.feed("relay-1");
AdafruitIO_Feed *relay2Feed  = io.feed("relay-2");
AdafruitIO_Feed *relay3Feed  = io.feed("relay-3");

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Pins ----------
#define POT_PIN A0

#define RELAY1 D5
#define RELAY2 D6
#define RELAY3 D7

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ---------- Voltage Limits ----------
float maxVoltage = 250.0;
float underVoltage = 120.0;
float overVoltage  = 230.0;

// ---------- Timers ----------
unsigned long lastSend = 0;
unsigned long sendInterval = 15000;

unsigned long lastLCD = 0;
unsigned long lcdInterval = 500;

unsigned long relay3StartTime = 0;
bool relay3Active = false;
bool faultAlreadyTriggered = false;

String status = "";
float voltage = 0;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);

  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);
  digitalWrite(RELAY3, RELAY_OFF);

  Wire.begin(D2, D1);   // SDA = D2, SCL = D1
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Voltage System");
  lcd.setCursor(0, 1);
  lcd.print("Connecting...");

  io.connect();

  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Adafruit IO");
  lcd.setCursor(0, 1);
  lcd.print("Connected");
  delay(1500);
  lcd.clear();
}

void loop() {
  io.run();

  int adcValue = analogRead(POT_PIN);
  voltage = (adcValue / 1023.0) * maxVoltage;

  if (voltage < underVoltage) {
    status = "UNDER VOLT";
    faultCondition();
  }
  else if (voltage > overVoltage) {
    status = "OVER VOLT";
    faultCondition();
  }
  else {
    status = "NORMAL";
    normalCondition();
  }

  if (relay3Active && millis() - relay3StartTime >= 3000) {
    digitalWrite(RELAY3, RELAY_OFF);
    relay3Active = false;
  }

  if (millis() - lastLCD >= lcdInterval) {
    lastLCD = millis();
    displayLCD();
  }

  if (millis() - lastSend >= sendInterval) {
    lastSend = millis();
    sendToAdafruit();
  }

  Serial.print("Voltage: ");
  Serial.print(voltage, 1);
  Serial.print(" V | Status: ");
  Serial.println(status);

  delay(200);
}

void normalCondition() {
  digitalWrite(RELAY1, RELAY_ON);
  digitalWrite(RELAY2, RELAY_ON);
  digitalWrite(RELAY3, RELAY_OFF);

  relay3Active = false;
  faultAlreadyTriggered = false;
}

void faultCondition() {
  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);

  if (faultAlreadyTriggered == false) {
    digitalWrite(RELAY3, RELAY_ON);
    relay3StartTime = millis();
    relay3Active = true;
    faultAlreadyTriggered = true;
  }
}

void displayLCD() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Voltage:");
  lcd.print(voltage, 1);
  lcd.print("V");

  lcd.setCursor(0, 1);
  lcd.print("Status:");
  lcd.print(status);
}

void sendToAdafruit() {
  voltageFeed->save(voltage);
  statusFeed->save(status);

  if (status == "NORMAL") {
    relay1Feed->save("ON");
    relay2Feed->save("ON");
    relay3Feed->save("OFF");
  } 
  else {
    relay1Feed->save("OFF");
    relay2Feed->save("OFF");

    if (relay3Active) {
      relay3Feed->save("ON");
    } else {
      relay3Feed->save("OFF");
    }
  }
}