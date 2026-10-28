// ESP8266 (ESP12E) version
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>               // ESP8266 WiFi
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"
#include "MAX30100_PulseOximeter.h"

// === USER PINS / CONFIG ===
// NOTE: If your LCD uses default I2C pins (SDA=GPIO4 D2, SCL=GPIO5 D1)
// and you wired DHT to GPIO5 (D1), that's a conflict.
// Move DHT to another free pin (e.g. D6) or rewire I2C.
#define DHTPIN 7       // currently GPIO5 (D1) — change if conflict with I2C
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// I2C LCD (address may be 0x27 or 0x3F) — change if needed
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pulse Oximeter
#define REPORTING_PERIOD_MS 5000
PulseOximeter pox;
uint32_t tsLastReport = 0;

// sensor values
int HeartRate = 0;
float SpO = -1.0f;
int32_t lastHeartRate = -1;
float lastSpO = -1.0f;

float temperature = NAN;
float humidity = NAN;
float lasttemperature = NAN;
float lasthumidity = NAN;

// WiFi / Adafruit IO
#define WLAN_SSID       "Robotutor"
#define WLAN_PASS       "Robotutor"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "health110"
#define AIO_KEY         "aio_bgus1060THvGsaRFDfRT9VvjFXz7"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temp_feed      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Publish humidity_feed  = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");
Adafruit_MQTT_Publish spo2_feed      = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/spo2");
Adafruit_MQTT_Publish heartrate_feed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/heartrate");

void MQTT_connect();
void onBeatDetected() { Serial.println("Beat detected!"); }

void setup() {
  Serial.begin(115200);
  delay(50);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Booting...");

  // I2C - default pins on ESP8266 (SDA=4=D2, SCL=5=D1)
  Wire.begin(); // If you must change pins: Wire.begin(sdaPin, sclPin);

  // DHT
  dht.begin();

  // Pulse Oximeter (MAX30100)
  Serial.print("Initializing POX.. ");
  if (!pox.begin()) {
    Serial.println("FAILED");
    lcd.clear(); lcd.setCursor(0,0); lcd.print("POX init FAILED");
    // We'll continue but POX values may be invalid
  } else {
    Serial.println("OK");
    pox.setOnBeatDetectedCallback(onBeatDetected);
  }

  // WiFi connect
  Serial.print("Connecting to WiFi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
    // optional timeout handling
    if (millis() - start > 20000) {
      Serial.println();
      Serial.println("WiFi connect timeout, retrying...");
      start = millis();
    }
  }
  Serial.println();
  Serial.print("WiFi connected, IP: "); Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  // Force first-publish by making last values impossible
  lastHeartRate = -1;
  lastSpO = -1.0f;
  lasttemperature = NAN;
  lasthumidity = NAN;

  // Connect MQTT
  MQTT_connect();
}

void loop() {
  // keep MQTT alive
  if (!mqtt.connected()) MQTT_connect();
  mqtt.processPackets(10);
  mqtt.ping();

  // update pox sensor
  pox.update();

  // reporting loop
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    float t = dht.readTemperature(); // C
    float h = dht.readHumidity();

    int hr = pox.getHeartRate();
    float spo2 = pox.getSpO2();

    Serial.print("Read T="); Serial.print(t);
    Serial.print(" H="); Serial.print(h);
    Serial.print(" HR="); Serial.print(hr);
    Serial.print(" SpO2="); Serial.println(spo2);

    // LCD display (guard against NaN)
    lcd.clear();
    lcd.setCursor(0,0);
    if (!isnan(t)) { lcd.print("T:"); lcd.print(t,1); lcd.print("C "); }
    else { lcd.print("T: --C "); }
    if (!isnan(h)) { lcd.print("H:"); lcd.print(h,0); lcd.print("%"); }
    else { lcd.print("H: --%"); }

    lcd.setCursor(0,1);
    if (spo2 > 0.0f && spo2 <= 100.0f) { lcd.print("O2:"); lcd.print(spo2,0); lcd.print("% "); } 
    else { lcd.print("O2: --% "); }
    if (hr > 0 && hr < 300) { lcd.print("HR:"); lcd.print(hr); } 
    else { lcd.print("HR: --"); }

    // ---------- Publish (explicit casts to resolve overloads) ----------
    // Temperature (double overload, 1 decimal place)
    if (!isnan(t)) {
      if (isnan(lasttemperature) || fabs(t - lasttemperature) >= 0.1f) {
        if (temp_feed.publish((double)t, 1, false)) {
          Serial.println("Temperature published");
          lasttemperature = t;
        } else {
          Serial.println("Temperature publish FAILED");
        }
      }
    }

    // Humidity
    if (!isnan(h)) {
      if (isnan(lasthumidity) || fabs(h - lasthumidity) >= 0.5f) {
        if (humidity_feed.publish((double)h, 1, false)) {
          Serial.println("Humidity published");
          lasthumidity = h;
        } else {
          Serial.println("Humidity publish FAILED");
        }
      }
    }

    // SpO2
    if (spo2 > 0.0f && spo2 <= 100.0f) {
      if (lastSpO < 0.0f || fabs(spo2 - lastSpO) >= 0.5f) {
        if (spo2_feed.publish((double)spo2, 1, false)) {
          Serial.println("SpO2 published");
          lastSpO = spo2;
        } else {
          Serial.println("SpO2 publish FAILED");
        }
      }
    }

    // Heart rate (int32_t overload)
    if (hr > 0 && hr < 300) {
      if (lastHeartRate != hr) {
        if (heartrate_feed.publish((int32_t)hr, false)) {
          Serial.println("Heart rate published");
          lastHeartRate = hr;
        } else {
          Serial.println("Heart rate publish FAILED");
        }
      }
    }
    // -------------------------------------------------------------------

  } // end reporting

  delay(10);
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) return;

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 5;
  while ((ret = mqtt.connect()) != 0) {
    Serial.print(mqtt.connectErrorString(ret));
    Serial.print(" - retrying in 2s...");
    mqtt.disconnect();
    delay(2000);
    if (--retries == 0) {
      Serial.println();
      Serial.println("MQTT connect failed - restarting WiFi and retrying");
      WiFi.disconnect();
      WiFi.reconnect();
      retries = 5;
    }
  }
  Serial.println("MQTT Connected!");
}
