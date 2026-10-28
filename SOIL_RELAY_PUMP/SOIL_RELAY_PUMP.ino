#include <LiquidCrystal_I2C.h>

int S1 = A0;
int R1 = 7;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);

  pinMode(S1, INPUT);
  pinMode(R1, OUTPUT);

  digitalWrite(R1, HIGH);  // Pump OFF at start (active LOW relay)

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Soil Irrigation");
  lcd.setCursor(0, 1);
  lcd.print("System Start");
  delay(2000);
  lcd.clear();
}

void loop() {
  int soilValue = analogRead(S1);   // read once

  Serial.print("Soil Value: ");
  Serial.println(soilValue);

  lcd.setCursor(0, 0);
  lcd.print("Soil  Value :");
  lcd.print(soilValue);
  lcd.print("   "); // clear extra chars

  lcd.setCursor(0, 1);

  if (soilValue < 512) {
    digitalWrite(R1, LOW);   // Pump ON
    lcd.print("Pump: ON  ");
  } else {
    digitalWrite(R1, HIGH);  // Pump OFF
    lcd.print("Pump: OFF ");
  }

  delay(1000);
}