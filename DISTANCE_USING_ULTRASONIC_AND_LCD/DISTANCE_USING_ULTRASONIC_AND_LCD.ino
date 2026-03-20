#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD address 0x27, 16 columns and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Ultrasonic sensor pins
const int trigPin = 9;
const int echoPin = 10;

void setup() {
  lcd.init();            // Initialize the LCD
  lcd.backlight();       // Turn on the backlight
  lcd.setCursor(0, 0);
  lcd.print("Distance:");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  // Trigger the ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the echo time
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in cm
  float distance = duration * 0.034 / 2;

  // Display on LCD
  lcd.setCursor(0, 1);
  lcd.print("Dist: ");
  lcd.print(distance);
  lcd.print(" cm  ");

  // Optional: print to Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);
}
