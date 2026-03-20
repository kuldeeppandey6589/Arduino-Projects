// #include <DHT.h>
// #include <LiquidCrystal.h>
// #define pin 5
// #define type DHT22

// LiquidCrystal lcd(12,13,5,4,3,2);
// DHT dht(pin,type);

// void setup(){
//   Serial.begin(9600);
//   dht.begin();
//   pinMode(13, OUTPUT);
// }

// void loop() {
//   float hum = dht.readHumidity();
//   float temp = dht.readTemperature();
//   lcd.println(hum);
//   delay(2000);
//   lcd.println(temp);
//   delay(2000);;dzxcxzxxz

//   if (temp>20){
//     digitalWrite(13,HIGH);
//   } else {
//     digitalWrite(13,LOW);
//   }
// }





#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Define LCD (I2C address can be 0x27 or 0x3F depending on your module)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Define DHT sensor
#define DHTPIN 2       // Data pin connected to D2
#define DHTTYPE DHT11  // Change to DHT22 if using that sensor
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  lcd.init();           // Initialize LCD
  lcd.backlight();      // Turn on backlight
  dht.begin();          // Start DHT sensor

  lcd.setCursor(0,0);
  lcd.print("Humidity & Temp");
  delay(2000);
  lcd.clear();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // Celsius
  float f = dht.readTemperature(true); // Fahrenheit

  // Check if reading failed
  if (isnan(h) || isnan(t)) {
    lcd.setCursor(0,0);
    lcd.print("Sensor Error!");
    return;
  }

  // Display Humidity
  lcd.setCursor(0,0);
  lcd.print("Hum: ");
  lcd.print(h);
  lcd.print(" %");

  // Display Temperature
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(t);
  lcd.print(" C");
  
  delay(2000); // Update every 2 seconds
}
