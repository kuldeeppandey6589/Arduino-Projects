#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// I2C LCD address usually 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int soilPin = A0;
const int relayPin = 7;

int threshold = 500;   // If soil value > 2000 => Pump ON

void setup() {
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // relay OFF for many relay modules (active LOW)

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Soil Monitor");
  delay(1500);
  lcd.clear();
}

void loop() {
  int soilValue = analogRead(soilPin);

  lcd.setCursor(0, 0);
  lcd.print("Soil: ");
  lcd.print(soilValue);
  lcd.print("    "); // clear extra digits

  if (soilValue < threshold) {
    digitalWrite(relayPin, LOW); // relay ON (active LOW)
    lcd.setCursor(0, 1);
    lcd.print("Pump is ON  ");
  } else {
    digitalWrite(relayPin, HIGH); // relay OFF
    lcd.setCursor(0, 1);
    lcd.print("Pump is OFF ");
  }

  delay(500);
}
