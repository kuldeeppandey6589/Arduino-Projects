#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address is usually 0x27 or 0x3F
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ==========================
// Phase sensing pins
// ==========================
#define R_PHASE_SENSOR 2
#define Y_PHASE_SENSOR 3
#define B_PHASE_SENSOR 4

// ==========================
// Relay output pins
// ==========================
#define R_PHASE_RELAY 8
#define Y_PHASE_RELAY 9
#define B_PHASE_RELAY 10

// Most relay modules are ACTIVE LOW
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

unsigned long lastSwitchTime = 0;
const unsigned long switchDelay = 3000; // 3 seconds

int currentPhase = 0;
// 0 = no phase
// 1 = R phase
// 2 = Y phase
// 3 = B phase

void setup() {
  Serial.begin(9600);

  pinMode(R_PHASE_SENSOR, INPUT);
  pinMode(Y_PHASE_SENSOR, INPUT);
  pinMode(B_PHASE_SENSOR, INPUT);

  pinMode(R_PHASE_RELAY, OUTPUT);
  pinMode(Y_PHASE_RELAY, OUTPUT);
  pinMode(B_PHASE_RELAY, OUTPUT);

  allRelaysOff();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("3 Phase Load");
  lcd.setCursor(0, 1);
  lcd.print("Shifting System");
  delay(2000);
  lcd.clear();

  Serial.println("3 Phase Load Shifting System Started");
}

void loop() {
  bool rAvailable = digitalRead(R_PHASE_SENSOR);
  bool yAvailable = digitalRead(Y_PHASE_SENSOR);
  bool bAvailable = digitalRead(B_PHASE_SENSOR);

  Serial.print("R: ");
  Serial.print(rAvailable);
  Serial.print("  Y: ");
  Serial.print(yAvailable);
  Serial.print("  B: ");
  Serial.println(bAvailable);

  int requiredPhase = 0;

  // Priority based load shifting
  if (rAvailable == HIGH) {
    requiredPhase = 1;
  } 
  else if (yAvailable == HIGH) {
    requiredPhase = 2;
  } 
  else if (bAvailable == HIGH) {
    requiredPhase = 3;
  } 
  else {
    requiredPhase = 0;
  }

  // Switch only if required phase changed
  if (requiredPhase != currentPhase) {
    if (millis() - lastSwitchTime >= switchDelay) {
      switchToPhase(requiredPhase);
      currentPhase = requiredPhase;
      lastSwitchTime = millis();
    }
  }

  updateLCD(rAvailable, yAvailable, bAvailable, currentPhase);

  delay(500);
}

void allRelaysOff() {
  digitalWrite(R_PHASE_RELAY, RELAY_OFF);
  digitalWrite(Y_PHASE_RELAY, RELAY_OFF);
  digitalWrite(B_PHASE_RELAY, RELAY_OFF);
}

void switchToPhase(int phase) {
  // First turn everything OFF
  allRelaysOff();

  // Small delay for safety before changing phase
  delay(500);

  if (phase == 1) {
    digitalWrite(R_PHASE_RELAY, RELAY_ON);
    Serial.println("Load shifted to R Phase");
  } 
  else if (phase == 2) {
    digitalWrite(Y_PHASE_RELAY, RELAY_ON);
    Serial.println("Load shifted to Y Phase");
  } 
  else if (phase == 3) {
    digitalWrite(B_PHASE_RELAY, RELAY_ON);
    Serial.println("Load shifted to B Phase");
  } 
  else {
    Serial.println("No phase available. Load OFF");
  }
}

void updateLCD(bool r, bool y, bool b, int phase) {
  lcd.setCursor(0, 0);
  lcd.print("R:");
  lcd.print(r);
  lcd.print(" Y:");
  lcd.print(y);
  lcd.print(" B:");
  lcd.print(b);
  lcd.print("   ");

  lcd.setCursor(0, 1);

  if (phase == 1) {
    lcd.print("LOAD: R PHASE  ");
  } 
  else if (phase == 2) {
    lcd.print("LOAD: Y PHASE  ");
  } 
  else if (phase == 3) {
    lcd.print("LOAD: B PHASE  ");
  } 
  else {
    lcd.print("LOAD: OFF      ");
  }
}