#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <NewPing.h>
#include <limits.h>

// ------------------ Adafruit IO credentials ------------------
#define AIO_USERNAME "Transformer140925"
const char *AIO_SERVER = "io.adafruit.com";
const char *AIO_KEY    = "aio_cNXq83u3ZIOG0JOTX5U5LsPmnoAD"; 
const int PORT         = 1883;

// ------------------ Hardware Pins ------------------
const uint8_t LM35_OIL_PIN   = 34;   // Oil temp LM35
const uint8_t LM35_TRANS_PIN = 35;   // Transformer temp LM35
const uint8_t TRIG_PIN  = 26;        // HC-SR04 trig
const uint8_t ECHO_PIN  = 27;        // HC-SR04 echo
const uint8_t BUZZER_PIN = 25;       // Buzzer

NewPing sonar(TRIG_PIN, ECHO_PIN, 200);

// ------------------ LCD ------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ------------------ WiFi ------------------
const char *WLAN_SSID = "Robotutor";
const char *WLAN_PASS = "Robotutor";

// ------------------ MQTT Setup ------------------
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, PORT, AIO_USERNAME, AIO_KEY);

// Feeds
Adafruit_MQTT_Publish oilTempFeed   = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/oil-temp");
Adafruit_MQTT_Publish transTempFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/transformer-temp");
Adafruit_MQTT_Publish levelFeed     = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/oil-level");
Adafruit_MQTT_Publish alarmFeed     = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/alarm");

// ------------------ Cached values ------------------
int lastOilTemp   = INT_MIN;
int lastTransTemp = INT_MIN;
int lastLevel     = INT_MIN;
int lastAlarm     = -1;

// ------------------ Function Declarations ------------------
void connectToWiFiAndMQTT();
float readLM35(uint8_t pin);
int   readOilLevelBucket();   // <-- updated
void longBeep();
void shortBeep();

// ===========================================================
// SETUP
// ===========================================================
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Transformer Mon");
  delay(1500);
}

// ===========================================================
// LOOP
// ===========================================================
void loop() {
  connectToWiFiAndMQTT();

  float oilTempC   = readLM35(LM35_OIL_PIN);
  float transTempC = readLM35(LM35_TRANS_PIN);
  int levelBucket  = readOilLevelBucket();   // now bucketed (0,25,50,75,100)

  // Thresholds
  bool overheatingOil   = oilTempC > 60.0;
  bool overheatingTrans = transTempC > 70.0;
  bool lowLevel         = (levelBucket <= 25);
  bool critical         = overheatingOil || overheatingTrans || (levelBucket == 0);

  // Buzzer Alerts
  if (critical)
    longBeep();
  else if (overheatingOil || overheatingTrans || lowLevel)
    shortBeep();

  // LCD Display
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Oil:");
  lcd.print((int)round(oilTempC));
  lcd.print("C ");
  lcd.print(levelBucket);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Tr:");
  lcd.print((int)round(transTempC));
  lcd.print("C ");
  if (critical)
    lcd.print("ALRT");
  else if (overheatingOil || overheatingTrans || lowLevel)
    lcd.print("WARN");
  else
    lcd.print("OK");

  // MQTT publish only on change
  int oInt      = (int)round(oilTempC);
  int tInt      = (int)round(transTempC);
  int alarmCode = critical ? 2 : (overheatingOil || overheatingTrans || lowLevel ? 1 : 0);

  if (lastOilTemp != oInt) {
    oilTempFeed.publish((int32_t)oInt);
    lastOilTemp = oInt;
  }
  if (lastTransTemp != tInt) {
    transTempFeed.publish((int32_t)tInt);
    lastTransTemp = tInt;
  }
  if (lastLevel != levelBucket) {
    levelFeed.publish((int32_t)levelBucket);
    lastLevel = levelBucket;
  }
  if (lastAlarm != alarmCode) {
    alarmFeed.publish((int32_t)alarmCode);
    lastAlarm = alarmCode;
  }

  delay(2000);
}

// ===========================================================
// FUNCTIONS
// ===========================================================
float readLM35(uint8_t pin) {
  int raw = analogRead(pin);
  float voltage = (raw / 4095.0f) * 3.3f;      // ESP32 ADC
  float tempC   = (voltage * 1000.0f) / 10.0f; // 10mV per °C
  if (tempC < -10) tempC = -10;
  if (tempC > 150) tempC = 150;
  return tempC;
}

// Your 25 cm container bucket logic
int readOilLevelBucket() {
  unsigned int distCm = sonar.ping_cm();
  if (distCm == 0) return lastLevel;  // keep last if no echo

  int bucket = 0;
  if (distCm > 25)                bucket = 0;
  else if (distCm > 20 && distCm <= 25) bucket = 25;
  else if (distCm > 15 && distCm <= 20) bucket = 50;
  else if (distCm > 10 && distCm <= 15) bucket = 75;
  else if (distCm > 0  && distCm <= 10) bucket = 100;

  return bucket;
}

void longBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(600);
  digitalWrite(BUZZER_PIN, LOW);
}

void shortBeep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(120);
  digitalWrite(BUZZER_PIN, LOW);
}

void connectToWiFiAndMQTT() {
  // WiFi
  if (!WiFi.isConnected()) {
    WiFi.begin(WLAN_SSID, WLAN_PASS);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting WiFi");

    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      delay(500);
      Serial.print(".");
    }
    Serial.println();
  }

  // MQTT
  if (!mqtt.connected()) {
    int8_t ret;
    uint8_t retries = 3;
    while ((ret = mqtt.connect()) != 0 && retries > 0) {
      Serial.println(mqtt.connectErrorString(ret));
      Serial.println("Retrying MQTT...");
      mqtt.disconnect();
      delay(5000);
      retries--;
    }
  }
}
