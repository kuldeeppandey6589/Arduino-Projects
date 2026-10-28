// RC Car Code using Arduino + FS-i6/FS-CT6B Receiver + 2 BTS7960 + 4 Motors

// ==========================
// Receiver Pins
// ==========================
#define CH1_PIN 2   // Steering
#define CH2_PIN 3   // Throttle

// ==========================
// Left BTS7960 Pins
// ==========================
#define LEFT_RPWM 6
#define LEFT_LPWM 5
#define LEFT_REN  7
#define LEFT_LEN  8

// ==========================
// Right BTS7960 Pins
// ==========================
#define RIGHT_RPWM 9
#define RIGHT_LPWM 10
#define RIGHT_REN  11
#define RIGHT_LEN  12

// ==========================
// Settings
// ==========================
#define MIN_PULSE 1150
#define MID_PULSE 1490
#define MAX_PULSE 1870

#define DEADZONE 40          // joystick center dead zone
#define SIGNAL_TIMEOUT 30000 // microseconds

int throttleValue = 0;
int steeringValue = 0;

void setup() {
  Serial.begin(9600);

  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);

  pinMode(LEFT_RPWM, OUTPUT);
  pinMode(LEFT_LPWM, OUTPUT);
  pinMode(LEFT_REN, OUTPUT);
  pinMode(LEFT_LEN, OUTPUT);

  pinMode(RIGHT_RPWM, OUTPUT);
  pinMode(RIGHT_LPWM, OUTPUT);
  pinMode(RIGHT_REN, OUTPUT);
  pinMode(RIGHT_LEN, OUTPUT);

  // Enable BTS7960 drivers
  digitalWrite(LEFT_REN, HIGH);
  digitalWrite(LEFT_LEN, HIGH);
  digitalWrite(RIGHT_REN, HIGH);
  digitalWrite(RIGHT_LEN, HIGH);

  stopMotors();

  Serial.println("RC Car Ready");
}

void loop() {
  int ch1 = pulseIn(CH1_PIN, HIGH, SIGNAL_TIMEOUT); // Steering
  int ch2 = pulseIn(CH2_PIN, HIGH, SIGNAL_TIMEOUT); // Throttle

  // Safety: if receiver signal lost, stop car
  if (ch1 == 0 || ch2 == 0) {
    stopMotors();
    Serial.println("Signal Lost - Motors Stopped");
    delay(100);
    return;
  }

  // Convert receiver PWM value to -255 to +255
  steeringValue = mapChannel(ch1);
  throttleValue = mapChannel(ch2);

  // Mixing logic
  int leftSpeed  = throttleValue + steeringValue;
  int rightSpeed = throttleValue - steeringValue;

  // Limit speed between -255 and 255
  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  setMotorLeft(leftSpeed);
  setMotorRight(rightSpeed);

  Serial.print("CH1: ");
  Serial.print(ch1);
  Serial.print("  CH2: ");
  Serial.print(ch2);
  Serial.print("  Left: ");
  Serial.print(leftSpeed);
  Serial.print("  Right: ");
  Serial.println(rightSpeed);

  delay(20);
}

// ==========================
// Convert RC PWM to motor speed
// ==========================
int mapChannel(int pulse) {
  pulse = constrain(pulse, MIN_PULSE, MAX_PULSE);

  if (abs(pulse - MID_PULSE) < DEADZONE) {
    return 0;
  }

  int value = map(pulse, MIN_PULSE, MAX_PULSE, -255, 255);
  return value;
}

// ==========================
// speed: -255 to +255
// ==========================
void setMotorLeft(int speedValue) {
  speedValue = constrain(speedValue, -255, 255);

  if (speedValue > 0) {
    analogWrite(LEFT_RPWM, speedValue);
    analogWrite(LEFT_LPWM, 0);
  } 
  else if (speedValue < 0) {
    analogWrite(LEFT_RPWM, 0);
    analogWrite(LEFT_LPWM, abs(speedValue));
  } 
  else {
    analogWrite(LEFT_RPWM, 0);
    analogWrite(LEFT_LPWM, 0);
  }
}

// ==========================
// Right Motor Control
// speed: -255 to +255
// ==========================
void setMotorRight(int speedValue) {
  speedValue = constrain(speedValue, -255, 255);

  if (speedValue > 0) {
    analogWrite(RIGHT_RPWM, speedValue);
    analogWrite(RIGHT_LPWM, 0);
  } 
  else if (speedValue < 0) {
    analogWrite(RIGHT_RPWM, 0);
    analogWrite(RIGHT_LPWM, abs(speedValue));
  } 
  else {
    analogWrite(RIGHT_RPWM, 0);
    analogWrite(RIGHT_LPWM, 0);
  }
}

// ==========================
// Stop All Motors
// ==========================
void stopMotors() {
  analogWrite(LEFT_RPWM, 0);
  analogWrite(LEFT_LPWM, 0);
  analogWrite(RIGHT_RPWM, 0);
  analogWrite(RIGHT_LPWM, 0);
}
