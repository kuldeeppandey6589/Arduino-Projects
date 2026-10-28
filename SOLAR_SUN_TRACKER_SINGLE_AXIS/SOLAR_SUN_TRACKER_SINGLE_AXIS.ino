// ==========================
// SOLAR TRACKER (DIGITAL LDR)
// ==========================

// Digital LDR pins
const int ldrLeft  = 2;
const int ldrRight = 3;

// L298N motor driver pins
const int ENA = 9;
const int IN1 = 7;
const int IN2 = 8;

int motorSpeed = 150;

void setup() {
  Serial.begin(9600);

  pinMode(ldrLeft, INPUT);
  pinMode(ldrRight, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  stopMotor();
}

void loop() {
  int leftState  = digitalRead(ldrLeft);
  int rightState = digitalRead(ldrRight);

  Serial.print("Left: ");
  Serial.print(leftState);
  Serial.print("  Right: ");
  Serial.println(rightState);

  // Case 1: Left gets more light
  if (leftState == LOW && rightState == HIGH) {
    rotateLeft();
  }
  // Case 2: Right gets more light
  else if (leftState == HIGH && rightState == LOW) {
    rotateRight();
  }
  // Case 3: Both same (aligned or dark)
  else {
    stopMotor();
  }

  delay(200);
}

// ==========================
// FUNCTIONS
// ==========================

void rotateLeft() {
  analogWrite(ENA, motorSpeed);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Moving Left");
}

void rotateRight() {
  analogWrite(ENA, motorSpeed);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Moving Right");
}

void stopMotor() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  Serial.println("Stop");
}