#include <SoftwareSerial.h>

// ==========================================
// HC-05 Bluetooth
// ==========================================
SoftwareSerial Bluetooth(2, 3);
// Arduino RX = D2  <- HC-05 TX
// Arduino TX = D3  -> HC-05 RX


// ==========================================
// LEFT MOTOR - BTS7960 #1
// ==========================================
#define LEFT_RPWM 5
#define LEFT_LPWM 6


// ==========================================
// RIGHT MOTOR - BTS7960 #2
// ==========================================
#define RIGHT_RPWM 9
#define RIGHT_LPWM 10


// ==========================================
// Motor Speed
// Range: 0 - 255
// ==========================================
int motorSpeed = 180;


// ==========================================
// SETUP
// ==========================================
void setup() {

  Serial.begin(9600);
  Bluetooth.begin(9600);

  pinMode(LEFT_RPWM, OUTPUT);
  pinMode(LEFT_LPWM, OUTPUT);

  pinMode(RIGHT_RPWM, OUTPUT);
  pinMode(RIGHT_LPWM, OUTPUT);

  stopCar();

  Serial.println("Bluetooth Robot Car Ready");
  Serial.println("Commands: F B L R S");
}


// ==========================================
// LOOP
// ==========================================
void loop() {

  if (Bluetooth.available()) {

    char command = Bluetooth.read();

    Serial.print("Command: ");
    Serial.println(command);

    switch (command) {

      case 'F':
      case 'f':
        forward();
        break;

      case 'B':
      case 'b':
        backward();
        break;

      case 'L':
      case 'l':
        turnLeft();
        break;

      case 'R':
      case 'r':
        turnRight();
        break;

      case 'S':
      case 's':
        stopCar();
        break;
    }
  }
}


// ==========================================
// LEFT MOTOR FORWARD
// ==========================================
void leftMotorForward(int speedValue) {

  analogWrite(LEFT_RPWM, speedValue);
  analogWrite(LEFT_LPWM, 0);
}


// ==========================================
// LEFT MOTOR BACKWARD
// ==========================================
void leftMotorBackward(int speedValue) {

  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, speedValue);
}


// ==========================================
// RIGHT MOTOR FORWARD
// ==========================================
void rightMotorForward(int speedValue) {

  analogWrite(RIGHT_RPWM, speedValue);
  analogWrite(RIGHT_LPWM, 0);
}


// ==========================================
// RIGHT MOTOR BACKWARD
// ==========================================
void rightMotorBackward(int speedValue) {

  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, speedValue);
}


// ==========================================
// STOP LEFT MOTOR
// ==========================================
void stopLeftMotor() {

  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 0);
}


// ==========================================
// STOP RIGHT MOTOR
// ==========================================
void stopRightMotor() {

  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 0);
}


// ==========================================
// CAR FORWARD
// ==========================================
void forward() {

  leftMotorForward(motorSpeed);
  rightMotorForward(motorSpeed);

  Serial.println("FORWARD");
}


// ==========================================
// CAR BACKWARD
// ==========================================
void backward() {

  leftMotorBackward(motorSpeed);
  rightMotorBackward(motorSpeed);

  Serial.println("BACKWARD");
}


// ==========================================
// TURN LEFT
// Pivot Turn
// ==========================================
void turnLeft() {

  leftMotorBackward(motorSpeed);
  rightMotorForward(motorSpeed);

  Serial.println("LEFT");
}


// ==========================================
// TURN RIGHT
// Pivot Turn
// ==========================================
void turnRight() {

  leftMotorForward(motorSpeed);
  rightMotorBackward(motorSpeed);

  Serial.println("RIGHT");
}


// ==========================================
// STOP CAR
// ==========================================
void stopCar() {

  stopLeftMotor();
  stopRightMotor();

  Serial.println("STOP");
}