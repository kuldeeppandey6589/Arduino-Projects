

/*******************************************************
  TRANSFORMER HEALTH MONITORING USING ARDUINO
********************************************************/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include <DHT.h>

// ----------------- SENSOR PINS -----------------------
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int POT_PIN = A1;

// ----------------- OUTPUT PINS -----------------------
const int RELAY_TEMP = 3;   // 🔥 Temperature relay
const int RELAY_POT  = 5;   // ⚡ POT relay
const int BUZZER_PIN = 4;

// ----------------- LCD -------------------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ----------------- THRESHOLDS ------------------------
const float TEMP_WARN = 30.0;
const float TEMP_TRIP = 33.0;

const int POT_SAFE_LOW  = 160;
const int POT_SAFE_HIGH = 250;

// ----------------- SETUP -----------------------------
void setup() {
  pinMode(RELAY_TEMP, OUTPUT);
  pinMode(RELAY_POT, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(POT_PIN, INPUT);

  digitalWrite(RELAY_TEMP, LOW);
  digitalWrite(RELAY_POT, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();
  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("TRANSFORMER");
  lcd.setCursor(0, 1);
  lcd.print("HEALTH MONITOR");
  delay(2000);
  lcd.clear();
}

// ----------------- MAIN LOOP -------------------------
void loop() {
  float tempC  = dht.readTemperature();
  float hum    = dht.readHumidity();
  int potVal   = analogRead(POT_PIN);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  if (isnan(tempC)) lcd.print("ERR ");
  else {
    lcd.print(tempC, 1);
    lcd.print((char)223);
    lcd.print("C ");
  }

  lcd.print("V:");
  if (potVal < 100) lcd.print(" ");
  if (potVal < 10)  lcd.print(" ");
  lcd.print(potVal);

  lcd.setCursor(0, 1);
  lcd.print("Oil:");
  if (isnan(hum)) lcd.print("ERR ");
  else {
    lcd.print(hum, 0);
    lcd.print("% ");
  }

  updateStatus(tempC, potVal);
  delay(1000);
}

// ----------------- STATUS & CONTROL ------------------
void updateStatus(float tempC, int potValue) {

  bool warning = false;
  bool tempTrip = false;
  bool potTrip  = false;
  String status = "NORMAL      ";

  // -------- Temperature logic (UNCHANGED) --------
  if (!isnan(tempC)) {
    if (tempC >= TEMP_TRIP) {
      tempTrip = true;
      status = "OVER TEMP    ";
    } else if (tempC >= TEMP_WARN) {
      warning = true;
      status = "TEMP WARN    ";
    }
  }

  // -------- POT logic (UNCHANGED) --------
  if (potValue < POT_SAFE_LOW) {
    potTrip = true;
    status = "VOLT LOW     ";
  } else if (potValue > POT_SAFE_HIGH) {
    potTrip = true;
    status = "VOLT HIGH    ";
  }

  // -------- Relay control (SPLIT ONLY) --------
  digitalWrite(RELAY_TEMP, tempTrip ? HIGH : LOW);
  digitalWrite(RELAY_POT,  potTrip  ? HIGH : LOW);

  // -------- Buzzer (original behavior) --------
  if (tempTrip || potTrip) {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else if (warning) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
  }
  else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  lcd.setCursor(6, 1);
  lcd.print(status);
}
