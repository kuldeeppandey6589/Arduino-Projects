#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// --------------------
// LCD
// --------------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --------------------
// DHT11
// --------------------
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --------------------
// Pins
// --------------------
#define SOIL_PIN A0
#define RELAY_PIN 7

// --------------------   
// Settings
// --------------------
int soilValue = 0;
int moisturePercent = 0;

// Change this after testing
int dryThreshold = 40;   // if moisture below 40%, pump ON

void setup() {
  Serial.begin(9600);

  pinMode(SOIL_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Relay OFF at start
  digitalWrite(RELAY_PIN, HIGH);

  dht.begin();
  lcd.init();909;
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Soil Irrigation");
  lcd.setCursor(0, 1);
  lcd.print("System Start");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Read soil sensor
  soilValue = analogRead(SOIL_PIN);

  // Convert raw value to moisture percentage
  // Dry soil ~1023, Wet soil ~300
  moisturePercent = map(soilValue, 1023, 300, 0, 100);

  // Limit value between 0 and 100
  moisturePercent = constrain(moisturePercent, 0, 100);

  // Read DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check DHT read
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("DHT11 not reading");
    lcd.setCursor(0, 0);
    lcd.print("DHT Error       ");
    lcd.setCursor(0, 1);
    lcd.print("Check Sensor    ");
    delay(2000);
    return;
  }

  // Pump control
  if (moisturePercent < dryThreshold) {
    digitalWrite(RELAY_PIN, LOW);   // Pump ON (active LOW relay)
  } else {
    digitalWrite(RELAY_PIN, HIGH);  // Pump OFF
  }

  // Serial Monitor
  Serial.print("Soil Raw: ");
  Serial.print(soilValue);
  Serial.print("  Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("%  Temp: ");
  Serial.print(temperature);
  Serial.print(" C  Humidity: ");
  Serial.print(humidity);
  Serial.print("%  Pump: ");
  if (moisturePercent < dryThreshold) {
    Serial.println("ON");
  } else {
    Serial.println("OFF");
  }

  // LCD display
  lcd.setCursor(0, 0);
  lcd.print("M:");
  lcd.print(moisturePercent);
  lcd.print("% ");

  lcd.print("T:");
  lcd.print(temperature, 0);
  // lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.print(humidity, 0);
  lcd.print("% ");

  if (moisturePercent < dryThreshold) {
    lcd.print("P:ON ");
  } else {
    lcd.print("P:OFF");
  }

  delay(2000);
}