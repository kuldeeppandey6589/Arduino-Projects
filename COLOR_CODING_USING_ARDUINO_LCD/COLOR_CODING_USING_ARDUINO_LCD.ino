#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// TCS3200 Pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define OUT_PIN 8

// Push Button
#define BUTTON_PIN 2

String bandColors[4];
int bandIndex = 0;

// Color names
String colorNames[] = {
  "BLACK", "BROWN", "RED", "ORANGE", "YELLOW",
  "GREEN", "BLUE", "VIOLET", "GREY", "WHITE",
  "GOLD", "SILVER"
};

// First and second band digits
int digitValues[] = {
  0, 1, 2, 3, 4,
  5, 6, 7, 8, 9,
  -1, -1
};

// Third band multiplier
float multiplierValues[] = {
  1, 10, 100, 1000, 10000,
  100000, 1000000, 10000000, 100000000, 1000000000,
  0.1, 0.01
};

// Fourth band tolerance
float toleranceValues[] = {
  0, 1, 2, 0, 0,
  0.5, 0.25, 0.1, 0.05, 0,
  5, 10
};

void setup() {
  Serial.begin(9600);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT_PIN, INPUT);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // TCS3200 frequency scaling 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Resistor Color");
  lcd.setCursor(0, 1);
  lcd.print("Code Reader");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place Band 1");
  lcd.setCursor(0, 1);
  lcd.print("Press Button");
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(250);

    String detectedColor = detectColor();
    bandColors[bandIndex] = detectedColor;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Band ");
    lcd.print(bandIndex + 1);

    lcd.setCursor(0, 1);
    lcd.print(detectedColor);

    Serial.print("Band ");
    Serial.print(bandIndex + 1);
    Serial.print(": ");
    Serial.println(detectedColor);

    bandIndex++;

    delay(1500);

    if (bandIndex < 4) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place Band ");
      lcd.print(bandIndex + 1);
      lcd.setCursor(0, 1);
      lcd.print("Press Button");
    } else {
      calculateResistance();
      bandIndex = 0;

      delay(6000);

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Place Band 1");
      lcd.setCursor(0, 1);
      lcd.print("Press Button");
    }

    while (digitalRead(BUTTON_PIN) == LOW);
  }
}

// =============================
// Detect Color
// =============================
String detectColor() {
  int r = readRed();
  int g = readGreen();
  int b = readBlue();

  Serial.print("R=");
  Serial.print(r);
  Serial.print(" G=");
  Serial.print(g);
  Serial.print(" B=");
  Serial.println(b);

  /*
    Important:
    In TCS3200, lower value means stronger color.
    You may need to adjust these values according to your sensor.
  */

  if (r > 250 && g > 250 && b > 250) {
    return "BLACK";
  }

  if (r < 60 && g < 60 && b < 60) {
    return "WHITE";
  }

  if (r < g && r < b) {
    if (g < 120 && b > 130) {
      return "YELLOW";
    } 
    else if (g < 170 && b < 170) {
      return "ORANGE";
    } 
    else {
      return "RED";
    }
  }

  if (g < r && g < b) {
    return "GREEN";
  }

  if (b < r && b < g) {
    return "BLUE";
  }

  if (r < 150 && b < 150 && g > 180) {
    return "VIOLET";
  }

  if (r > 100 && r < 230 && g > 100 && g < 230 && b > 100 && b < 230) {
    return "BROWN";
  }

  return "UNKNOWN";
}

// =============================
// TCS3200 Reading Functions
// =============================
int readRed() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(100);
  return pulseIn(OUT_PIN, LOW, 100000);
}

int readGreen() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(100);
  return pulseIn(OUT_PIN, LOW, 100000);
}

int readBlue() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(100);
  return pulseIn(OUT_PIN, LOW, 100000);
}

// =============================
// Find color index
// =============================
int getColorIndex(String color) {
  for (int i = 0; i < 12; i++) {
    if (color == colorNames[i]) {
      return i;
    }
  }
  return -1;
}

// =============================
// Calculate Resistance
// =============================
void calculateResistance() {
  int c1 = getColorIndex(bandColors[0]);
  int c2 = getColorIndex(bandColors[1]);
  int c3 = getColorIndex(bandColors[2]);
  int c4 = getColorIndex(bandColors[3]);

  lcd.clear();

  if (c1 == -1 || c2 == -1 || c3 == -1 || c4 == -1) {
    lcd.setCursor(0, 0);
    lcd.print("Color Error");
    lcd.setCursor(0, 1);
    lcd.print("Scan Again");
    return;
  }

  if (digitValues[c1] == -1 || digitValues[c2] == -1) {
    lcd.setCursor(0, 0);
    lcd.print("Invalid Band");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
    return;
  }

  int firstDigit = digitValues[c1];
  int secondDigit = digitValues[c2];

  float multiplier = multiplierValues[c3];
  float tolerance = toleranceValues[c4];

  float resistance = ((firstDigit * 10) + secondDigit) * multiplier;

  lcd.setCursor(0, 0);
  lcd.print("Value:");

  lcd.setCursor(0, 1);
  printResistance(resistance);
  lcd.print(" Ohm ");
  lcd.print(tolerance);
  lcd.print("%");

  Serial.print("Resistance = ");
  Serial.print(resistance);
  Serial.print(" Ohm, Tolerance = ");
  Serial.print(tolerance);
  Serial.println("%");
}

// =============================
// Display Value
// =============================
void printResistance(float value) {
  if (value >= 1000000) {
    lcd.print(value / 1000000);
    lcd.print("M");
  } 
  else if (value >= 1000) {
    lcd.print(value / 1000);
    lcd.print("K");
  } 
  else {
    lcd.print(value);
  }
}