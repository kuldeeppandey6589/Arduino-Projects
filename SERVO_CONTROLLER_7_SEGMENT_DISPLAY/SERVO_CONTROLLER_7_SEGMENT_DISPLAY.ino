#include <TM1637Display.h>
#include <Servo.h>

// TM1637 pins
#define CLK 2
#define DIO 3

// Buzzer & Servo
#define BUZZER 8
#define SERVO_PIN 9

TM1637Display display(CLK, DIO);
Servo myServo;

// Timer variables
unsigned long previousMillis = 0;
int seconds = 0;
int millisPart = 0;

void setup() {
  display.setBrightness(7);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  myServo.attach(SERVO_PIN);
  myServo.write(0);
}

void loop() {
  unsigned long currentMillis = millis();

  // Update every 10 ms
  if (currentMillis - previousMillis >= 10) {
    previousMillis = currentMillis;

    millisPart++;

    if (millisPart >= 100) {  // 100 x 10ms = 1 second
      millisPart = 0;
      seconds++;
    }

    if (seconds > 9) {
      seconds = 0;
    }

    // Combine seconds and milliseconds
    int displayValue = (seconds * 100) + millisPart;

    display.showNumberDecEx(displayValue, 0b01000000, true);
  }

  // Buzzer at 7 seconds
  if (seconds >= 7 && seconds <= 9) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // Servo at 9 seconds
  if (seconds == 9) {
    myServo.write(90);
  } else {
    myServo.write(0);
  }
}