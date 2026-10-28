/*
  Simple Gesture Control Car
  - Sensor: ADXL335
  - Driver: L298N
  - Motors: 4 DC Motors
*/

// --- Pin Definitions ---
// Accelerometer Pins
const int xPin = A0;
const int yPin = A1;

// Motor Driver Pins (L298N)
// Left Motors
const int IN1 = 8;
const int IN2 = 9;
// Right Motor10
const int IN3 = 10;
const int IN4 = 11;

// --- Threshold Variables ---
// These determine sensitivity. You may need to adjust them based on calibration.
// Analog read is 0-1023. Midpoint (flat) is usually around 330-340 for 3.3V modules.
int xVal = 0;
int yVal = 0;

// Change these limits to make the controller more or less sensitive
const int forwardLimit = 280;   // If Y is lower than this -> Go Forward
const int backwardLimit = 380;  // If Y is higher than this -> Go Backward
const int leftLimit = 280;      // If X is lower than this -> Turn Left
const int rightLimit = 380;     // If X is higher than this -> Turn Right

void setup() {
  // Initialize Motor Pins as Output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Initialize Serial for debugging
  Serial.begin(9600);
}

void loop() {
  // Read sensor values
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);

  // Print values to Serial Monitor to help you calibrate
  Serial.print("X: ");
  Serial.print(xVal);
  Serial.print(" | Y: ");
  Serial.println(yVal);

  // --- Logic Control ---
  
  // 1. MOVE FORWARD
  if (yVal < forwardLimit) {
    moveForward();
  }
  // 2. MOVE BACKWARD
  else if (yVal > backwardLimit) {
    moveBackward();
  }
  // 3. TURN LEFT
  else if (xVal < leftLimit) {
    turnLeft();
  }
  // 4. TURN RIGHT
  else if (xVal > rightLimit) {
    turnRight();
  }
  // 5. STOP (If car is flat/neutral)
  else {
    stopCar();
  }

  delay(100); // Small delay for stability
}

// --- Movement Functions ---

void moveForward() {
  // Left Motors Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right Motors Forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  // Left Motors Backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right Motors Backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  // Left Motors Backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right Motors Forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  // Left Motors Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right Motors Backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}