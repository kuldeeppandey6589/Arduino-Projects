#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ---------- CONFIG ----------
const uint8_t PHASE_IN   = A0;
const uint8_t PHASE_LED  = 10;
const uint8_t PHASE_RELAY = 5;
const uint8_t BUZZER     = 4;

const int GND_THRESHOLD = 50;    // Line-Ground / Open fault
const int HIGH_THRESHOLD = 700;  // Short / abnormal fault

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- SETUP ----------
void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(PHASE_LED, OUTPUT);
  pinMode(PHASE_RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(PHASE_LED, LOW);
  digitalWrite(PHASE_RELAY, HIGH); // Relay OFF (active LOW)
  digitalWrite(BUZZER, LOW);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Single Phase");
  lcd.setCursor(0, 1);
  lcd.print("Fault System");
  delay(1500);
  lcd.clear();
}

// ---------- LOOP ----------
void loop() {
  int phaseVal = analogRead(PHASE_IN);

  Serial.print("ADC Value: ");
  Serial.println(phaseVal);

  lcd.clear();

  if (phaseVal < GND_THRESHOLD) {
    // LINE TO GROUND / OPEN FAULT
    lcd.print("Fault Detected");
    lcd.setCursor(0, 1);
    lcd.print("Phase R");

    digitalWrite(PHASE_LED, HIGH);
    digitalWrite(PHASE_RELAY, LOW);  // Trip
    digitalWrite(BUZZER, HIGH);
  }

  // else if (phaseVal > HIGH_THRESHOLD) {
  //   // SHORT / ABNORMAL FAULT
  //   lcd.print("SHORT FAULT");
  //   lcd.setCursor(0, 1);
  //   //lcd.print("Phase R");

  //   digitalWrite(PHASE_LED, HIGH);
  //   digitalWrite(PHASE_RELAY, LOW);  // Trip
  //   digitalWrite(BUZZER, HIGH);
  // }
  else {
    // NORMAL
    lcd.print("NO FAULT");
    lcd.setCursor(0, 1);
    lcd.print("LINE OK");

    digitalWrite(PHASE_LED, LOW);
    digitalWrite(PHASE_RELAY, HIGH);
    digitalWrite(BUZZER, LOW);
  }

  delay(500);
}
