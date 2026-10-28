#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ======================
// DHT11 Setup
// ======================
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ======================
// Pulse Sensor Setup
// ======================
#define PULSE_PIN A0

int pulseValue = 0;
int threshold = 550;   // adjust after testing
int bpm = 0;

unsigned long lastBeatTime = 0;
unsigned long interval = 0;

// ======================
// LCD Setup
// ======================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ======================
// Variables
// ======================
float temperature = 0;
float humidity = 0;

unsigned long lastRead = 0;
unsigned long lastDisplay = 0;
int page = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("Health Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);

  lcd.clear();
}

void loop() {

  // ======================
  // Pulse Sensor Reading
  // ======================
  pulseValue = analogRead(PULSE_PIN);

  if (pulseValue > threshold) {
    if (millis() - lastBeatTime > 300) { // debounce
      interval = millis() - lastBeatTime;
      lastBeatTime = millis();
      bpm = 60000 / interval;
    }
  }

  // ======================
  // Read DHT11 every 2 sec
  // ======================
  if (millis() - lastRead > 2000) {
    lastRead = millis();

    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    Serial.println("------ Data ------");
    Serial.print("Temp: ");
    Serial.println(temperature);

    Serial.print("SpO2: ");
    Serial.println(humidity*4.95);

    Serial.print("Pulse: ");
    Serial.println(bpm);
  }

  // ======================
  // LCD Display Change
  // ======================
  if (millis() - lastDisplay > 3000) {
    lastDisplay = millis();

    page++;
    if (page > 1) page = 0;

    lcd.clear();

    if (page == 0) {
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temperature);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("SpO2:");
      lcd.print(humidity*5.2);
      lcd.print("%");
    }
    else if (page == 1) {
      lcd.setCursor(0, 0);
      lcd.print("Pulse Rate:");

      lcd.setCursor(0, 1);
      lcd.print(bpm);
      lcd.print(" BPM");
    }
  }
}