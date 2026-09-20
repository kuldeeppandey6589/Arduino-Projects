#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pin setup
#define DHTPIN 2
#define DHTTYPE DHT11
#define MQ135_PIN A0
#define BUZZER_PIN 8

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // If not working, try 0x3F

// Alert limits
float tempLimit = 40.0;
int gasLimit = 400;

void setup() {
  Serial.begin(9600);

  dht.begin();

  lcd.init();
  lcd.backlight();

  pinMode(MQ135_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("Temp Gas Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();

  Serial.println("Temperature and MQ135 Monitoring Started");
}

void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int gasValue = analogRead(MQ135_PIN);

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT Sensor Error!");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("DHT Sensor");
    lcd.setCursor(0, 1);
    lcd.print("Error!");

    digitalWrite(BUZZER_PIN, LOW);
    delay(1000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" C | Humidity: ");
  Serial.print(humidity);
  Serial.print(" % | MQ135 Value: ");
  Serial.println(gasValue);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Gas:");
  lcd.print(gasValue);

  if (temperature >= tempLimit || gasValue >= gasLimit) {
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.print(" ALERT");
    Serial.println("ALERT! Temperature or Gas Level High");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    lcd.print(" Normal");
    Serial.println("Status: Normal");
  }

  delay(1000);
}