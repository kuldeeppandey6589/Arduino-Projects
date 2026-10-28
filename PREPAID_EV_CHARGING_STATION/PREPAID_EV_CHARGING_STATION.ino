#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

Servo gateServo;
#define SERVO_PIN 3

#define IR_ENTRY 4
#define IR_EXIT 5
#define RELAY_PIN 6

#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define OBJECT_DETECTED LOW

bool chargingOn = false;

void setup() {
  Serial.begin(9600);

  pinMode(IR_ENTRY, INPUT);
  pinMode(IR_EXIT, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, RELAY_OFF);

  gateServo.attach(SERVO_PIN);
  gateServo.write(0);

  lcd.init();
  lcd.backlight();

  SPI.begin();
  rfid.PCD_Init();

  lcd.setCursor(0, 0);
  lcd.print("Prepaid EV");
  lcd.setCursor(0, 1);
  lcd.print("Charging Station");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome !");
}

void loop() {

  // IR1 detect -> gate open then close

  if (digitalRead(IR_ENTRY) == OBJECT_DETECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vehicle Detect");
    lcd.setCursor(0, 1);
    lcd.print("RFID Ready");

    gateServo.write(90);
    delay(3000);          // gate open time

    delay(1000);
  }

  // RFID -> Relay ON
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("RFID Verified");
    lcd.setCursor(0, 1);
    lcd.print("Charging ON");

    digitalWrite(RELAY_PIN, RELAY_ON);
    chargingOn = true;

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();

    delay(1000);
  }

  // IR2 detect -> Relay OFF
  if (digitalRead(IR_EXIT) == OBJECT_DETECTED) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vehicle Exit");

    if (chargingOn) {
      digitalWrite(RELAY_PIN, RELAY_OFF);
      chargingOn = false;

      lcd.setCursor(0, 1);
      lcd.print("Charging OFF");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("Relay Already OFF");
    }

    delay(2000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ready Next");
  }
}