#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// =============================
// PIN DEFINITIONS
// =============================

#define PIR_PIN       2
#define DHT_PIN       3
#define RELAY1_PIN    7
#define RELAY2_PIN    8
#define LDR_PIN       A0

// =============================
// DHT11
// =============================

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// =============================
// LCD I2C
// =============================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =============================
// SETTINGS
// =============================

// LDR threshold
int ldrThreshold = 500;

// Temperature threshold
float temperatureLimit = 34.0;

// Temperature at which relay 2 turns OFF
float temperatureOffLimit = 34.0;

// =============================
// RELAY TYPE
// =============================

// Most relay modules are ACTIVE LOW
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// =============================
// VARIABLES
// =============================

int motionState = 0;
int lastMotionState = -1;

int ldrValue = 0;

float temperature = 0;
float humidity = 0;

bool isDark = true;

bool relay1State = false;
bool relay2State = false;

unsigned long lastDHTRead = 0;
unsigned long lastLCDChange = 0;

int lcdPage = 0;

// =============================
// SETUP
// =============================

void setup() {

  Serial.begin(9600);

  // Sensor pins
  pinMode(PIR_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  // Relay pins
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);

  // Relays OFF initially
  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);

  // Start DHT11
  dht.begin();

  // Start LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Smart Home");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  Serial.println();
  Serial.println("============================");
  Serial.println(" Smart Automation System");
  Serial.println("============================");

  Serial.println("System Starting...");
  Serial.println("Waiting for PIR sensor...");

  delay(2000);

  lcd.clear();
}

// =============================
// MAIN LOOP
// =============================

void loop() {

  // =============================
  // READ PIR SENSOR
  // =============================

  motionState = digitalRead(PIR_PIN);

  // Print immediately when PIR changes
  if (motionState != lastMotionState) {

    Serial.print("PIR Status: ");

    if (motionState == HIGH) {

      Serial.println("MOTION DETECTED");

    } else {

      Serial.println("NO MOTION");
    }

    lastMotionState = motionState;
  }

  // =============================
  // READ LDR
  // =============================

  ldrValue = analogRead(LDR_PIN);

  /*
     Current wiring assumption:

     Bright = High analog value
     Dark   = Low analog value

     Example:

     Dark = 200
     Bright = 800

     Threshold = 500
  */

  isDark = (ldrValue < ldrThreshold);

  // =============================
  // RELAY 1 CONTROL
  //
  // MOTION OR DARK
  // =============================

  if (motionState == HIGH || isDark == false) {

    relay1State = true;

    digitalWrite(RELAY1_PIN, RELAY_ON);

  } else {

    relay1State = false;

    digitalWrite(RELAY1_PIN, RELAY_OFF);
  }

  // =============================
  // READ DHT11
  // =============================

  if (millis() - lastDHTRead >= 2000) {

    lastDHTRead = millis();

    float newTemperature = dht.readTemperature();
    float newHumidity = dht.readHumidity();

    // =============================
    // CHECK TEMPERATURE
    // =============================

    if (!isnan(newTemperature)) {

      temperature = newTemperature;

    } else {

      Serial.println("ERROR: DHT11 Temperature Read Failed!");
    }

    // =============================
    // CHECK HUMIDITY
    // =============================

    if (!isnan(newHumidity)) {

      humidity = newHumidity;

    } else {

      Serial.println("ERROR: DHT11 Humidity Read Failed!");
    }

    // =============================
    // RELAY 2
    //
    // TEMPERATURE CONTROL
    // =============================

    // Above 34 C = ON
    if (temperature > temperatureLimit) {

      relay2State = true;

      digitalWrite(RELAY2_PIN, RELAY_ON);

    }

    // Below 33 C = OFF
    else if (temperature < temperatureOffLimit) {

      relay2State = false;

      digitalWrite(RELAY2_PIN, RELAY_OFF);
    }

    // =============================
    // SERIAL MONITOR
    // =============================

    Serial.println();
    Serial.println("----------------------------");

    // PIR
    Serial.print("PIR Status     : ");

    if (motionState == HIGH) {

      Serial.println("MOTION DETECTED");

    } else {

      Serial.println("NO MOTION");
    }

    // LDR
    Serial.print("LDR Value      : ");
    Serial.println(ldrValue);

    Serial.print("Light Status   : ");

    if (isDark) {

      Serial.println("DARK");

    } else {

      Serial.println("BRIGHT");
    }

    // Temperature
    Serial.print("Temperature    : ");
    Serial.print(temperature, 1);
    Serial.println(" C");

    // Humidity
    Serial.print("Humidity       : ");
    Serial.print(humidity, 1);
    Serial.println(" %");

    // Relay 1
    Serial.print("Relay 1        : ");

    if (relay1State) {

      Serial.println("ON");

    } else {

      Serial.println("OFF");
    }

    // Relay 2
    Serial.print("Relay 2        : ");

    if (relay2State) {

      Serial.println("ON");

    } else {

      Serial.println("OFF");
    }

    Serial.println("----------------------------");
  }

  // =============================
  // LCD DISPLAY
  // =============================

  if (millis() - lastLCDChange >= 2000) {

    lastLCDChange = millis();

    lcd.clear();

    // =============================
    // PAGE 1
    // TEMPERATURE + HUMIDITY
    // =============================

    if (lcdPage == 0) {

      lcd.setCursor(0, 0);

      lcd.print("Temp:");
      lcd.print(temperature, 1);
      lcd.print((char)223);
      lcd.print("C");

      lcd.setCursor(0, 1);

      lcd.print("Hum:");
      lcd.print(humidity, 0);
      lcd.print("%");

      lcdPage = 1;
    }

    // =============================
    // PAGE 2
    // PIR + LDR
    // =============================

    else if (lcdPage == 1) {

      lcd.setCursor(0, 0);

      lcd.print("Motion:");

      if (motionState == HIGH) {

        lcd.print("YES");

      } else {

        lcd.print("NO");
      }

      lcd.setCursor(0, 1);

      lcd.print("Light:");

      if (isDark) {

        lcd.print("DARK");

      } else {

        lcd.print("BRIGHT");
      }

      lcdPage = 2;
    }

    // =============================
    // PAGE 3
    // RELAY STATUS
    // =============================

    else {

      lcd.setCursor(0, 0);

      lcd.print("Relay1:");

      if (relay1State) {

        lcd.print("ON");

      } else {

        lcd.print("OFF");
      }

      lcd.setCursor(0, 1);

      lcd.print("Relay2:");

      if (relay2State) {

        lcd.print("ON");

      } else {

        lcd.print("OFF");
      }

      lcdPage = 0;
    }
  }
}