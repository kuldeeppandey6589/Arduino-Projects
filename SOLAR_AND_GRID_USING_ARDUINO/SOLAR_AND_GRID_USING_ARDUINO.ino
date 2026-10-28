#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address: 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define VOLTAGE_PIN A0

#define SOLAR_RELAY 8
#define GRID_RELAY_1 9
#define GRID_RELAY_2 10

int lowVoltageThreshold = 500;

// Most relay modules are Active LOW
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

void setup() {
  Serial.begin(9600);

  pinMode(SOLAR_RELAY, OUTPUT);
  pinMode(GRID_RELAY_1, OUTPUT);
  pinMode(GRID_RELAY_2, OUTPUT);

  digitalWrite(SOLAR_RELAY, RELAY_OFF);
  digitalWrite(GRID_RELAY_1, RELAY_OFF);
  digitalWrite(GRID_RELAY_2, RELAY_OFF);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Solar/Grid Auto");
  lcd.setCursor(0, 1);
  lcd.print("Switching Start");
  delay(2000);
  lcd.clear();

  Serial.println("Solar/Grid Auto Switching Started");
}

void loop() {
  int sensorValue = analogRead(VOLTAGE_PIN);
  float voltage = sensorValue * (5.0 / 1023.0);

  Serial.print("Pot: ");
  Serial.print(sensorValue);
  Serial.print(" Voltage: ");
  Serial.print(voltage);
  Serial.print("V Source: ");

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V:");
  lcd.print(voltage);
  lcd.print(" ADC:");
  lcd.print(sensorValue);

  if (sensorValue < lowVoltageThreshold) {
    // Low voltage: Grid ON, Solar OFF
    solarOff();
    delay(300);
    gridOn();

    lcd.setCursor(0, 1);
    lcd.print("Source: GRID ");
    Serial.println("GRID");
  } 
  else {
    // Normal voltage: Solar ON, Grid OFF
    gridOff();
    delay(300);
    solarOn();

    lcd.setCursor(0, 1);
    lcd.print("Source: SOLAR");
    Serial.println("SOLAR");
  }

  delay(1000);
}

void solarOn() {
  digitalWrite(SOLAR_RELAY, RELAY_ON);
}

void solarOff() {
  digitalWrite(SOLAR_RELAY, RELAY_OFF);
}

void gridOn() {
  digitalWrite(GRID_RELAY_1, RELAY_ON);
  digitalWrite(GRID_RELAY_2, RELAY_ON);
}

void gridOff() {
  digitalWrite(GRID_RELAY_1, RELAY_OFF);
  digitalWrite(GRID_RELAY_2, RELAY_OFF);
}