#include <Servo.h>

// ----- Motor Driver Pins (L298N / L293D style) -----
// const int ENA = 5;   // PWM speed left motor
// const int ENB = 6;   // PWM speed right motor

const int IN1 = 9;
const int IN2 = 10;
const int IN3 = 11;
const int IN4 = 12;

// ----- One Servo -----
Servo servo1;
const int SERVO1_PIN = 7;   // changed from 6 to 7 (no conflict)

// Servo angles
const int S_INIT = 0;   // initial position (change if you want)
const int S_45   = 45;
const int S_90   = 90;
const int S_180  = 180;


Servo servo2;
const int SERVO2_PIN = 6;   // use any free digital pin

// Speed (0-255)
int speedVal = 180;

void setup() {
  Serial.begin(9600);

  // pinMode(ENA, OUTPUT);
  // pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  servo1.attach(SERVO1_PIN);
  servo1.write(S_INIT);


  servo2.attach(SERVO2_PIN);
  servo2.write(0);   // start at 0 degree

  // setSpeed(speedVal);
  stopCar();
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    // ---- Speed control (0..9) ----
    if (cmd >= '0' && cmd <= '9') {
      int level = cmd - '0';                 // 0..9
      speedVal = map(level, 0, 9, 80, 255);  // adjust if needed
      // setSpeed(speedVal);
      return;
    }

    switch (cmd) {
      // ----- Car control -----
      case 'F': forward(); break;
      case 'B': backward(); break;
      case 'L': left(); break;
      case 'R': right(); break;
      case 'S': stopCar(); break;

      // ----- Servo control -----
      case 'a': servo1.write(S_45);  break;  // 45°
      case 'b': servo1.write(S_90);  break;  // 90°
      case 'c': servo1.write(S_180); break;  // 180°
      case 'd': servo1.write(S_INIT); break; // initial position

      // Servo  2 control
      // ----- Servo2 control (0–90 only) -----
      case 'e': servo2.write(0);  break;   // 0°
      case 'f': servo2.write(90); break;   // 90°

    }
  }
}

// ---------- Helpers ----------
// void setSpeed(int spd) {
//   analogWrite(ENA, spd);
//   analogWrite(ENB, spd);
// }

void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void left() {
  // your left logic kept same style as your code
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);
}

void right() {
  // your right logic kept same style as your code
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}
