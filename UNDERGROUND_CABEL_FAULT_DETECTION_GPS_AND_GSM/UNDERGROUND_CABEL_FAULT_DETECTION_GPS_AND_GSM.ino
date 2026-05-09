#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==========================
// Relay Pins (UNO)
// ==========================
#define RELAY_1 7
#define RELAY_2 6
#define RELAY_3 5

// ==========================
// Flex Sensor Pins
// ==========================
#define FLEX_1_PIN A0
#define FLEX_2_PIN A1
#define FLEX_3_PIN A2

int flexThreshold1 = 600;
int flexThreshold2 = 600;
int flexThreshold3 = 600;

bool relayState1 = false;
bool relayState2 = false;
bool relayState3 = false;

unsigned long lastFlexRead = 0;
const unsigned long flexInterval = 200;

void setRelay(uint8_t relayPin, bool state) {
  digitalWrite(relayPin, state ? LOW : HIGH);  // Active LOW
}

void updateLCD(int flex1, int flex2, int flex3) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(" R     Y      B");

  // -------- LINE 2 --------
  lcd.setCursor(0, 1);

  // R value
  if (flex1 >= 330 && flex1 <= 435) {
    lcd.print("1KM");
  } else if (flex1 <= 490) {
    lcd.print("2KM");
  } else if (flex1 <= 550) {
    lcd.print("3KM");
  } else {
    lcd.print("0KM");
  }

  lcd.print("   "); // spacing

  // Y value
  if (flex2 >= 330 && flex2 <= 435) {
    lcd.print("1KM");
  } else if (flex2 <= 490) {
    lcd.print("2KM");
  } else if (flex2 <= 550) {
    lcd.print("3KM");
  } else {
    lcd.print("0KM");
  }

  lcd.print("    "); // spacing

  // B value
  if (flex3 >= 330 && flex3 <= 435) {
    lcd.print("1KM");
  } else if (flex3 <= 490) {
    lcd.print("2KM");
  } else if (flex3 <= 550) {
    lcd.print("3KM");
  } else {
    lcd.print("0KM");
  }

  
}

void updateRelay1(bool state) {
  relayState1 = state;
  setRelay(RELAY_1, state);
  Serial.print("Relay 1 -> ");
  Serial.println(state ? "ON" : "OFF");
}

void updateRelay2(bool state) {
  relayState2 = state;
  setRelay(RELAY_2, state);
  Serial.print("Relay 2 -> ");
  Serial.println(state ? "ON" : "OFF");
}

void updateRelay3(bool state) {
  relayState3 = state;
  setRelay(RELAY_3, state);
  Serial.print("Relay 3 -> ");
  Serial.println(state ? "ON" : "OFF");
}

void readFlexSensors() {
  int flex1 = analogRead(FLEX_1_PIN);
  int flex2 = analogRead(FLEX_2_PIN);
  int flex3 = analogRead(FLEX_3_PIN);



  Serial.print("F1=");
  Serial.print(flex1);
  Serial.print("  F2=");
  Serial.print(flex2);
  Serial.print("  F3=");
  Serial.println(flex3);

  bool newState1 = (flex1 > flexThreshold1);
  bool newState2 = (flex2 > flexThreshold2);
  bool newState3 = (flex3 > flexThreshold3);

  if (newState1 != relayState1) updateRelay1(newState1);
  if (newState2 != relayState2) updateRelay2(newState2);
  if (newState3 != relayState3) updateRelay3(newState3);

  updateLCD(flex1, flex2, flex3);
}

void setup() {
  Serial.begin(9600);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Arduino UNO");
  lcd.setCursor(0, 1);
  lcd.print("System Started");
  delay(2000);
  lcd.clear();

  Serial.println("Arduino UNO System Started");
}

void loop() {
  if (millis() - lastFlexRead >= flexInterval) {
    lastFlexRead = millis();
    readFlexSensors();
  }
}