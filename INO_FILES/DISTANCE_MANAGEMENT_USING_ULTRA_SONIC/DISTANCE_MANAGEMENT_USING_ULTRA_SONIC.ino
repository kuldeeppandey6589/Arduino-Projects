// #include <Wire.h>
// #include <LiquidCrystal_I2C.h>

// #define TRIG_PIN 9
// #define ECHO_PIN 10

// LiquidCrystal_I2C lcd(0x27, 16, 2); // Adjust address if needed

// void setup() {
//   // lcd.begin(9600);
//   lcd.backlight();
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
// }

// void loop() {
//   long duration;
//   float distance;

//   // Trigger the ultrasonic pulse
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   // Measure the echo time
//   duration = pulseIn(ECHO_PIN, HIGH);

//   // Calculate distance in cm
//   distance = duration * 0.034 / 2;

//   // Display on LCD
//   lcd.clear();
//   lcd.setCursor(0, 0);
//   lcd.print("Distance:");
//   lcd.setCursor(0, 1);
//   lcd.print(distance);
//   lcd.print(" cm");

//   delay(500);
// }



char command;

void setup() {
  Serial.begin(9600);

  // Motor A
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);

  // Motor B
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();

    switch (command) {
      case 'F': // Forward
        digitalWrite(5, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        break;

      case 'B': // Backward
        digitalWrite(5, LOW);
        digitalWrite(6, HIGH);
        digitalWrite(9, LOW);
        digitalWrite(10, HIGH);
        break;

      case 'L': // Left
        digitalWrite(5, LOW);
        digitalWrite(6, HIGH);
        digitalWrite(9, HIGH);
        digitalWrite(10, LOW);
        break;

      case 'R': // Right
        digitalWrite(5, HIGH);
        digitalWrite(6, LOW);
        digitalWrite(9, LOW);
        digitalWrite(10, HIGH);
        break;

      case 'S': // Stop
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(9, LOW);
        digitalWrite(10, LOW);
        break;
    }
  }
}
