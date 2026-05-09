#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Sensor pins
const int sensorOnPin  = 2;
const int sensorOffPin = 3;

// Relay pins
const int relay1 = 8;
const int relay2 = 9;
const int relay3 = 10;
const int relay4 = 11;

// Relay state array
int relayPins[4] = {relay1, relay2, relay3, relay4};
bool relayState[4] = {false, false, false, false};

// Counters
int onCount = 0;
int offCount = 0;

// Previous sensor states
bool lastOnSensorState = HIGH;
bool lastOffSensorState = HIGH;

// Debounce time
unsigned long lastOnTriggerTime = 0;
unsigned long lastOffTriggerTime = 0;
const unsigned long debounceDelay = 500;

// Startup delay
unsigned long startupDelayEnd = 0;

void showLCDMessage(String line1, String line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], HIGH);   // active LOW relay OFF
    relayState[i] = false;
  }

  pinMode(sensorOnPin, INPUT_PULLUP);
  pinMode(sensorOffPin, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();
  showLCDMessage("System Ready", "All Relay OFF");

  lastOnSensorState = digitalRead(sensorOnPin);
  lastOffSensorState = digitalRead(sensorOffPin);

  startupDelayEnd = millis() + 2000;

  Serial.println("System Ready");
}

void loop() {
  if (millis() < startupDelayEnd) return;

  bool currentOnSensorState = digitalRead(sensorOnPin);
  bool currentOffSensorState = digitalRead(sensorOffPin);

  // SENSOR 1 -> TURN ON RELAYS ONE BY ONE
  if (lastOnSensorState == HIGH && currentOnSensorState == LOW) {
    if (millis() - lastOnTriggerTime > debounceDelay) {
      lastOnTriggerTime = millis();

      // whenever ON mode starts, reset OFF counter
      offCount = 0;

      if (onCount < 4) {
        if (!relayState[onCount]) {
          turnRelayOn(onCount);
          onCount++;
        }
      } else {
        Serial.println("All relays already ON");
        showLCDMessage("All Relay ON", "Cycle Complete");
      }
    }
  }

  // SENSOR 2 -> TURN OFF RELAYS ONE BY ONE
  if (lastOffSensorState == HIGH && currentOffSensorState == LOW) {
    if (millis() - lastOffTriggerTime > debounceDelay) {
      lastOffTriggerTime = millis();

      // whenever OFF mode starts, reset ON counter
      onCount = 0;

      if (offCount < 4) {
        if (relayState[offCount]) {
          turnRelayOff(offCount);
          offCount++;
        }
      } else {
        Serial.println("All relays already OFF");
        showLCDMessage("All Relay OFF", "Cycle Complete");
      }
    }
  }

  lastOnSensorState = currentOnSensorState;
  lastOffSensorState = currentOffSensorState;

  // Optional auto reset after full ON
  if (allRelaysOn()) {
    onCount = 4;
  }

  // Optional auto reset after full OFF
  if (allRelaysOff()) {
    offCount = 4;
  }
}

void turnRelayOn(int index) {
  if (index >= 0 && index < 4) {
    digitalWrite(relayPins[index], LOW);   // active LOW relay ON
    relayState[index] = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED : ");
    lcd.print(index + 1);
    lcd.print(" ON");
    lcd.setCursor(0, 1);
    lcd.print("On Mode Active.");

    Serial.print("Relay ");
    Serial.print(index + 1);
    Serial.println(" ON");
  }
}

void turnRelayOff(int index) {
  if (index >= 0 && index < 4) {
    digitalWrite(relayPins[index], HIGH);   // active LOW relay OFF
    relayState[index] = false;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("LED : ");
    lcd.print(index + 1);
    lcd.print(" OFF");
    lcd.setCursor(0, 1);
    lcd.print("Off Mode Active.");

    Serial.print("Relay ");
    Serial.print(index + 1);
    Serial.println(" OFF");
  }
}

bool allRelaysOn() {
  for (int i = 0; i < 4; i++) {
    if (!relayState[i]) return false;
  }
  return true;
}

bool allRelaysOff() {
  for (int i = 0; i < 4; i++) {
    if (relayState[i]) return false;
  }
  return true;
}