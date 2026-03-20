#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int vibPin = A0;
int sensorValue = 0;

int vibValue = 0;     // Counter
int threshold = 200;  // Vibration-detect limit

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(vibPin, INPUT);

  lcd.setCursor(0, 0);
  lcd.print("Vibration Count");
  delay(1200);
  lcd.clear();
}

void loop() {

  sensorValue = analogRead(vibPin);

  // ===== LCD shows YES → Increase =====
  if (sensorValue > threshold) {

    if (vibValue < 24) {
      vibValue++;     // Increase value
    }
    delay(200);
  }

  // ===== LCD shows NO → Decrease =====
  else {

    if (vibValue > 0) {
      vibValue--;     // Decrease value
    }
    delay(200);
  }

  // ===== LCD Display =====

  lcd.setCursor(0, 0);
  lcd.print("Vibration: ");
  if (sensorValue > threshold) lcd.print("YES ");
  else lcd.print("NO  ");

  lcd.setCursor(0, 1);
  lcd.print("Voltage: ");
  lcd.print(vibValue);
  lcd.print(" mV  ");

  delay(80);
}





// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// LiquidCrystal_I2C lcd(0x27, 16, 2);

// const int vibPin = A0;
// int sensorValue = 0;

// int vibValue = 0;     // Counter
// int threshold = 200;  // Vibration-detect limit

// void setup() {
//   lcd.init();
//   lcd.backlight();
//   pinMode(vibPin, INPUT);

//   lcd.setCursor(0, 0);
//   lcd.print("Vibration Count");
//   delay(1200);
//   lcd.clear();
// }

// void loop() {

//   sensorValue = analogRead(vibPin);

//   // ===== Vibration detected =====
//   if (sensorValue > threshold) {
//     if (vibValue < 24) {  // Limit to 24 maximum
//       vibValue++;
//     } else {
//       vibValue--;
//     }
//     delay(300);  // Debounce
//   }
//   // ===== No vibration =====
//   // else {
//   //   vibValue--;  // Reset counter to zero
//   // }

//   // ===== LCD Display =====

//   lcd.setCursor(0, 0);
//   lcd.print("Vibration: ");
//   if (sensorValue > threshold) lcd.print("YES ");
//   else lcd.print("NO  ");

//   lcd.setCursor(0, 1);
//   lcd.print("Count: ");
//   lcd.print(vibValue);
//   lcd.print(" mV  ");  // clear extra chars

//   delay(120);
// }
