#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR_PIN 2   // IR sensor output pin

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long lastMillis = 0;
unsigned long timeCount = 0;
unsigned long unitCount = 0;


void setup() {
  pinMode(IR_PIN, INPUT);

  lcd.init(); 
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Unit Counter Start ");
  lcd.setCursor(0, 1);
  lcd.print("Waiting...");
  delay(2000);
  lcd.clear();
}

void loop() {
  int irState = digitalRead(IR_PIN);

  // Object detected (IR gives LOW)
  if (irState == LOW) {
    unsigned long currentMillis = millis();

    if (currentMillis - lastMillis >= 1) {  // 1 millisecond
      lastMillis = currentMillis;
      timeCount++;
      delay(1800);
      unitCount++;
      delay(1800);
    }
  }

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Units:     ");
  lcd.setCursor(7, 0);
  lcd.print(timeCount);

  lcd.setCursor(0, 1);
  lcd.print("Price:       ");
  lcd.setCursor(7, 1);
  lcd.print(unitCount*3.4);
}
 