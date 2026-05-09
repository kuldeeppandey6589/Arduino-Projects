
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int vibPin = A0;

int vibValue = 0;     // Counter
int threshold = 200;


int sensorValue = 0;
int prevValue = 0;   // Store previous value

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  pinMode(vibPin, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Vibration Read");
  delay(1200);
  lcd.clear();
}

void loop() {

  sensorValue = analogRead(vibPin);

// ===== LCD shows YES → Increase =====
  if (sensorValue > threshold) {

    if (vibValue < 24) {
      vibValue++;  // Increase value
    }
    delay(200);
  }

  // ===== LCD shows NO → Decrease =====
  else {

    if (vibValue > 0) {
      vibValue--;  // Decrease value
    }

    delay(200);
  }

  // ===== LCD Display =====
  lcd.setCursor(0, 0);
  lcd.print("Value: ");
  lcd.print(sensorValue);
  lcd.print("   ");  // clear extra digits

  lcd.setCursor(0, 1);
  lcd.print("Voltage: ");
  lcd.print(vibValue);
  lcd.print(" mV  ");

  // ===== Serial Monitor =====
  Serial.print("Current: ");
  Serial.print(sensorValue);
  Serial.print(" | Previous: ");
  Serial.println(prevValue);

  // Update previous value
  prevValue = sensorValue;

  delay(200);
}