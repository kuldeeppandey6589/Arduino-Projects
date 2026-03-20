#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

#include "MAX30100_PulseOximeter.h"

// ===== BMP280 =====
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;  // I2C

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27, 20, 4);   // common address: 0x27 or 0x3F

// ===== DHT =====
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== MAX30100 =====
PulseOximeter pox;

#define REPORTING_PERIOD_MS 1000
uint32_t tsLastReport = 0;

float bpm = 0;
float spo2 = 0;

float tempC = 0;
float hum = 0;

float pressure_hPa = 0;

void onBeatDetected() {
  // optional
}

void setup() {
  Serial.begin(9600);

  // If you are using ESP32 and want to force I2C pins, use:
  // Wire.begin(21, 22);   // SDA=21, SCL=22 (common ESP32 default)
  Wire.begin();

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Health Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");

  // DHT init
  dht.begin();

  // BMP280 init (most modules are 0x76 or 0x77)
  if (!bmp.begin(0x76)) {   // try 0x77 if this fails
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BMP280 FAILED");
    lcd.setCursor(0, 1);
    lcd.print("Try addr 0x77");
    while (1) delay(1000);
  }

  // MAX30100 init
  if (!pox.begin()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("MAX30100 FAILED");
    lcd.setCursor(0, 1);
    lcd.print("Check wiring/I2C");
    while (1) delay(1000);
  }

  pox.setIRLedCurrent(MAX30100_LED_CURR_27_1MA);
  pox.setOnBeatDetectedCallback(onBeatDetected);

  lcd.clear();
}

void loop() {
  // Must be called very frequently for MAX30100 to work
  pox.update();

  // Update display once per second
  if (millis() - tsLastReport > REPORTING_PERIOD_MS) {
    tsLastReport = millis();

    // Read MAX30100 values
    bpm = pox.getHeartRate();
    spo2 = pox.getSpO2();

    // Read DHT11
    hum = dht.readHumidity();
    tempC = dht.readTemperature();

    if (isnan(hum) || isnan(tempC)) {
      hum = -1;
      tempC = -1;
    }

    // Read BMP280 pressure
    pressure_hPa = bmp.readPressure() / 100.0F;  // Pa -> hPa

    // ===== LCD DISPLAY (20x4) =====
    // Line 0: Temp + Hum
    lcd.setCursor(0, 0);
    lcd.print("T:");
    if (tempC >= 0) {
      lcd.print(tempC, 1);
      lcd.print((char)223);
      lcd.print("C ");
    } else {
      lcd.print("Err   ");
    }

    lcd.print("H:");
    if (hum >= 0) {
      lcd.print(hum, 0);
      lcd.print("%   ");
    } else {
      lcd.print("Err   ");
    }

    // Line 1: BPM + SpO2
    lcd.setCursor(0, 1);
    lcd.print(" BPM:");
    if (bpm > 30 && bpm < 220) {
      lcd.print(bpm, 0);
      lcd.print("   ");
    } else {
      lcd.print("--   ");
    }

    lcd.print("SpO2:");
    if (spo2 > 50 && spo2 <= 100) {
      lcd.print(spo2, 0);
      lcd.print("%   ");
    } else {
      lcd.print("--%  ");
    }

    // Line 2: Pressure
    lcd.setCursor(0, 2);
    lcd.print("Pressure: ");
    lcd.print(pressure_hPa, 1);
    lcd.print(" hPa  ");

    // Line 3: (optional) You can show altitude too if you want
    // float altitude = bmp.readAltitude(1013.25); // sea level pressure
    // lcd.setCursor(0, 3);
    // lcd.print("Alt: "); lcd.print(altitude, 1); lcd.print(" m     ");

    lcd.setCursor(0, 3);
    lcd.print("Status: OK          ");

    // Serial debug
    Serial.print("Temp: "); Serial.print(tempC);
    Serial.print("C  Hum: "); Serial.print(hum);
    Serial.print("%  BPM: "); Serial.print(bpm);
    Serial.print("  SpO2: "); Serial.print(spo2);
    Serial.print("  Pressure: "); Serial.print(pressure_hPa);
    Serial.println(" hPa");
  }
}