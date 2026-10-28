/* ============================================================
   EARTH RESISTANCE + SOIL LEAKAGE MONITOR
   Using SIMPLE & TESTED Adafruit MQTT (from CODE-1)
   + CHANGE-BASED PUBLISHING TO SAVE ADAFRUIT DATA
   ============================================================ */

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

// ---------------- WiFi Credentials ----------------
#define WLAN_SSID   "Robotutor"
#define WLAN_PASS   "Robotutor"

// ---------------- Adafruit IO Settings -------------
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "earth251125"
#define AIO_KEY         "aio_bPeM08gOJyJufEV3gIsWaTuzE3V4"

// MQTT CLIENT
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// MQTT FEEDS
Adafruit_MQTT_Publish feed_leak =
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/earth-leackage");

Adafruit_MQTT_Publish feed_res =
  Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/earth-resistance");

// ---------------- Hardware Pins --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

const float VCC = 3.3;
const float RKNOWN = 1000.0;
const float THRESH_RES_OHM = 50.0;

const uint8_t RELAY_PIN = D5;
const uint8_t RELAY_PIN_2 = D7;
const uint8_t SOIL_DO_PIN = D6;
const uint8_t RELAY_ACTIVE_LOW = 1;

// ---------- CHANGE-DETECTION VARIABLES ----------
float last_rprobe = -1;
bool last_soil_state = false;
const float R_CHANGE_THRESHOLD = 5.0;

// ---------------- FUNCTION DECLARATION --------------
void MQTT_connect();

// ---------------- RESISTANCE FUNCTIONS --------------
float readVadc(int samples = 30) {
  uint32_t sum = 0;
  for (int i = 0; i < samples; i++) {
    sum += analogRead(A0);
    delay(4);
  }
  float avg = (float)sum / samples;
  return (avg / 1023.0) * VCC;
}

float computeResistance(float vprobe) {
  if (vprobe <= 0.0001f) return INFINITY;
  if (vprobe >= VCC - 0.0001f) return 0.0f;
  return (vprobe * RKNOWN) / (VCC - vprobe);
}

// ---------------- SET RELAY -------------------------
void setRelay(bool on) {
  if (RELAY_ACTIVE_LOW) {
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
    digitalWrite(RELAY_PIN_2, on ? HIGH : LOW);
  } else {
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
    digitalWrite(RELAY_PIN_2, on ? LOW : HIGH);
  }
}

// =====================================================
//                     SETUP
// =====================================================
void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RELAY_PIN_2, OUTPUT);
  setRelay(false);

  pinMode(SOIL_DO_PIN, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Earth & Soil Mon");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.println("\nWiFi OK");
}

// =====================================================
//                        LOOP
// =====================================================
void loop() {

  MQTT_connect();  // stable like code-1

  float vprobe = readVadc();
  float rprobe = computeResistance(vprobe);

  bool soil_is_wet = (digitalRead(SOIL_DO_PIN) == LOW);
  bool badEarth = isfinite(rprobe) ? (rprobe > THRESH_RES_OHM) : true;

  bool alarm = badEarth || soil_is_wet;
  setRelay(alarm);

  // ---------------- LCD ----------------
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Earth:");

  if (rprobe < 500) lcd.print("220R ");
  else if (rprobe < 2000) lcd.print("1K  ");
  else if (rprobe < 10000) lcd.print("10K ");
  else if (rprobe < 100000) lcd.print("100K ");
  else if (rprobe < 1000000) lcd.print("1M ");
  else lcd.print(">1M");

  lcd.setCursor(0, 1);
  lcd.print("Leak:");
  lcd.print(soil_is_wet ? "YES" : "NO ");

// ---------------- SERIAL ----------------
  Serial.print("Vprobe: "); Serial.print(vprobe, 3);
  Serial.print("  R: ");
  if (isfinite(rprobe)) Serial.print(rprobe);
  else Serial.print("INF");

  Serial.print("  Soil wet: ");
  Serial.print(soil_is_wet);
  Serial.print(" Alarm: ");
  Serial.println(alarm);

  // =====================================================
  //             CHANGE-BASED MQTT PUBLISH
  // =====================================================

  bool resistance_changed = false;

  if (isfinite(rprobe) && isfinite(last_rprobe)) {
    if (abs(rprobe - last_rprobe) >= R_CHANGE_THRESHOLD)
      resistance_changed = true;
  } else {
    if (rprobe != last_rprobe)
      resistance_changed = true;
  }

  bool leak_changed = (soil_is_wet != last_soil_state);

  if (resistance_changed) {
    char payload[32];
    if (isfinite(rprobe))
      snprintf(payload, sizeof(payload), "%.2f", rprobe);
    else
      snprintf(payload, sizeof(payload), "INF");

    feed_res.publish(payload);
    Serial.println("MQTT: Resistance changed → published");

    last_rprobe = rprobe;
  }

  if (leak_changed) {
    feed_leak.publish(soil_is_wet ? 1 : 0);
    Serial.println("MQTT: Soil leak changed → published");

    last_soil_state = soil_is_wet;
  }

  delay(800);
}

// =====================================================
//             SIMPLE MQTT CONNECT (from CODE-1)
// =====================================================
void MQTT_connect() {
  int8_t ret;

  if (mqtt.connected())
    return;

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
  }

  Serial.println("MQTT Connected!");
}

