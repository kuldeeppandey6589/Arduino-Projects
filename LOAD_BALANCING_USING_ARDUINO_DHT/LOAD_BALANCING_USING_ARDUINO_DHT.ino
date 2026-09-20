#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// =====================================================
// LCD
// =====================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================================
// DHT11
// =====================================================
#define DHT_PIN  2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

// =====================================================
// Analog inputs
// =====================================================
#define POT_PIN      A0
#define CURRENT_PIN  A1

// =====================================================
// Relay pins
// =====================================================
#define RELAY1 8
#define RELAY2 9
#define RELAY3 10

// Active LOW relay module
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// =====================================================
// Arduino ADC settings
// =====================================================
const float ADC_REFERENCE = 5.0;
const int ADC_MAX = 1023;

// =====================================================
// Potentiometer settings
// Maximum value measured from your potentiometer
// =====================================================
const int POT_MAX_VALUE = 556;

// =====================================================
// Simulated voltage settings
// Voltage increases according to load percentage
// 0% load   = 0V
// 100% load = 25V
// =====================================================
const float MAX_DISPLAY_VOLTAGE = 25.0;

// =====================================================
// ACS712 current sensor settings
//
// ACS712-5A  = 0.185 V/A
// ACS712-20A = 0.100 V/A
// ACS712-30A = 0.066 V/A
// =====================================================
const float ACS_SENSITIVITY = 0.100;

// ACS712 output voltage when current is zero
float currentZeroVoltage = 2.50;

// Remove small noise around zero current
const float CURRENT_NOISE_LIMIT = 0.10;

// =====================================================
// Load limits
// =====================================================
const int NORMAL_LIMIT = 70;
const int CRITICAL_LIMIT = 90;

// =====================================================
// Variables
// =====================================================
int potValue = 0;
int loadPercent = 0;

float displayedVoltage = 0.0;
float measuredCurrent = 0.0;
float temperature = 0.0;
float humidity = 0.0;

bool emergencyTriggered = false;

// =====================================================
// LCD timing
// =====================================================
byte lcdPage = 0;

unsigned long previousLCDTime = 0;
const unsigned long LCD_PAGE_INTERVAL = 2000;

// =====================================================
// Read average analog value
// =====================================================
float readAverageADC(int pin, int samples)
{
  unsigned long total = 0;

  for (int i = 0; i < samples; i++)
  {
    total += analogRead(pin);
    delayMicroseconds(300);
  }

  return (float)total / samples;
}

// =====================================================
// Read current from ACS712
// =====================================================
float readCurrent()
{
  float adcValue = readAverageADC(CURRENT_PIN, 100);

  float sensorVoltage =
      (adcValue * ADC_REFERENCE) / ADC_MAX;

  float current =
      (sensorVoltage - currentZeroVoltage) /
      ACS_SENSITIVITY;

  // Display positive current
  if (current < 0)
  {
    current = -current;
  }

  // Remove small sensor noise
  if (current < CURRENT_NOISE_LIMIT)
  {
    current = 0.0;
  }

  return current;
}

// =====================================================
// Turn all relays OFF
// =====================================================
void allRelaysOff()
{
  digitalWrite(RELAY1, RELAY_OFF);
  digitalWrite(RELAY2, RELAY_OFF);
  digitalWrite(RELAY3, RELAY_OFF);
}

// =====================================================
// Read potentiometer, current and DHT11
// =====================================================
void readSensors()
{
  potValue = analogRead(POT_PIN);

  potValue = constrain(
      potValue,
      0,
      POT_MAX_VALUE
  );

  // Convert potentiometer into 0-100% load
  loadPercent = map(
      potValue,
      0,
      POT_MAX_VALUE,
      0,
      100
  );

  loadPercent = constrain(
      loadPercent,
      0,
      100
  );

  // Voltage increases according to load percentage
  displayedVoltage =
      (loadPercent / 100.0) *
      MAX_DISPLAY_VOLTAGE;

  // Read actual current from ACS712
  measuredCurrent = readCurrent();

  // Read DHT11
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
}

// =====================================================
// Relay control
// =====================================================
void controlRelays()
{
  // ---------------------------------------------------
  // Load below 70%
  // Relay 1 ON
  // ---------------------------------------------------
  if (loadPercent < NORMAL_LIMIT)
  {
    emergencyTriggered = false;

    digitalWrite(RELAY1, RELAY_ON);
    digitalWrite(RELAY2, RELAY_OFF);
    digitalWrite(RELAY3, RELAY_OFF);
  }

  // ---------------------------------------------------
  // Load 70% to below 90%
  // Relay 1 and Relay 2 ON
  // ---------------------------------------------------
  else if (loadPercent < CRITICAL_LIMIT)
  {
    emergencyTriggered = false;

    digitalWrite(RELAY1, RELAY_ON);
    digitalWrite(RELAY2, RELAY_ON);
    digitalWrite(RELAY3, RELAY_OFF);
  }

  // ---------------------------------------------------
  // Load 90% or above
  //
  // All relays ON for 3 seconds
  // Then Relay 1 and Relay 2 OFF
  // Relay 3 remains ON
  // ---------------------------------------------------
  else
  {
    if (!emergencyTriggered)
    {
      digitalWrite(RELAY1, RELAY_ON);
      digitalWrite(RELAY2, RELAY_ON);
      digitalWrite(RELAY3, RELAY_ON);

      lcd.clear();

      lcd.setCursor(0, 0);
      lcd.print("Load:");
      lcd.print(loadPercent);
      lcd.print("%");

      lcd.setCursor(0, 1);
      lcd.print("ALL RELAYS ON");

      delay(3000);

      emergencyTriggered = true;
    }

    digitalWrite(RELAY1, RELAY_OFF);
    digitalWrite(RELAY2, RELAY_OFF);
    digitalWrite(RELAY3, RELAY_ON);
  }
}

// =====================================================
// LCD page 1: Load and condition
// =====================================================
void displayLoadCondition()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Load:");
  lcd.print(loadPercent);
  lcd.print("%");

  lcd.setCursor(0, 1);

  if (loadPercent < NORMAL_LIMIT)
  {
    lcd.print("T1 NORMAL");
  }
  else if (loadPercent < CRITICAL_LIMIT)
  {
    lcd.print("T1+T2 OVERLOAD");
  }
  else
  {
    lcd.print("CRITICAL");
  }
}

// =====================================================
// LCD page 2: Voltage and current
// =====================================================
void displayVoltageCurrent()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Voltage:");
  lcd.print(displayedVoltage, 1);
  lcd.print("V");

  lcd.setCursor(0, 1);
  lcd.print("Current:");
  lcd.print(measuredCurrent/10, 2);
  lcd.print("A");
}

// =====================================================
// LCD page 3: Temperature and humidity
// =====================================================
void displayTemperatureHumidity()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Temp:");

  if (isnan(temperature))
  {
    lcd.print("Error");
  }
  else
  {
    lcd.print(temperature, 1);
    lcd.print((char)223);
    lcd.print("C");
  }

  lcd.setCursor(0, 1);
  lcd.print("Humidity:");

  if (isnan(humidity))
  {
    lcd.print("Error");
  }
  else
  {
    lcd.print(humidity, 0);
    lcd.print("%");
  }
}

// =====================================================
// Update LCD pages
// =====================================================
void updateLCD()
{
  if (lcdPage == 0)
  {
    displayLoadCondition();
  }
  else if (lcdPage == 1)
  {
    displayVoltageCurrent();
  }
  else
  {
    displayTemperatureHumidity();
  }

  lcdPage++;

  if (lcdPage > 2)
  {
    lcdPage = 0;
  }
}

// =====================================================
// Serial Monitor
// =====================================================
void printSerialData()
{
  Serial.print("Pot: ");
  Serial.print(potValue);

  Serial.print(" | Load: ");
  Serial.print(loadPercent);
  Serial.print("%");

  Serial.print(" | Voltage: ");
  Serial.print(displayedVoltage, 2);
  Serial.print("V");

  Serial.print(" | Current: ");
  Serial.print(measuredCurrent, 2);
  Serial.print("A");

  Serial.print(" | Temperature: ");

  if (isnan(temperature))
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(temperature, 1);
    Serial.print("C");
  }

  Serial.print(" | Humidity: ");

  if (isnan(humidity))
  {
    Serial.print("Error");
  }
  else
  {
    Serial.print(humidity, 1);
    Serial.print("%");
  }

  Serial.print(" | Condition: ");

  if (loadPercent < NORMAL_LIMIT)
  {
    Serial.println("NORMAL");
  }
  else if (loadPercent < CRITICAL_LIMIT)
  {
    Serial.println("OVERLOAD");
  }
  else
  {
    Serial.println("CRITICAL");
  }
}

// =====================================================
// Setup
// =====================================================
void setup()
{
  Serial.begin(9600);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);

  allRelaysOff();

  lcd.init();
  lcd.backlight();

  dht.begin();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("2 Transformer");

  lcd.setCursor(0, 1);
  lcd.print("Load Balancing");

  delay(2000);

  readSensors();
  controlRelays();
  displayLoadCondition();

  previousLCDTime = millis();
}

// =====================================================
// Main loop
// =====================================================
void loop()
{
  readSensors();

  controlRelays();

  printSerialData();

  if (millis() - previousLCDTime >= LCD_PAGE_INTERVAL)
  {
    previousLCDTime = millis();
    updateLCD();
  }

  delay(300);
}
