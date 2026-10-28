#include <LiquidCrystal_I2C.h>

const int soil = A0;
const int ir = A1;
const int buzzer = 9;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  pinMode(buzzer, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!!");
  lcd.setCursor(0, 1);
}

void loop() {
  bool isIrOn = digitalRead(ir);
  int moisture = map(analogRead(soil), 0, 1024, 100, 0);
  digitalWrite(buzzer, isIrOn || moisture > 40);
  lcd.print("Moisture: " + String(moisture) + "%   ");
}
