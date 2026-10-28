#include <Servo.h>

// =====================================================
// Flame sensor pins
// =====================================================
#define LEFT_FLAME    A0
#define CENTER_FLAME  A1
#define RIGHT_FLAME   A2

// =====================================================
// L298N motor driver pins
// =====================================================
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// =====================================================
// Pump relay and servo
// =====================================================
#define PUMP_RELAY 12
#define SERVO_PIN  13

// Most relay modules are active LOW
#define PUMP_ON  LOW
#define PUMP_OFF HIGH

Servo waterServo;

// A lower analog value means stronger flame
const int FLAME_DETECT_LIMIT = 500;

// Flame must rise above this value before the system
// is armed for the next fire
const int FLAME_RESET_LIMIT = 600;

// Number of samples used for stable sensor readings
const int SENSOR_SAMPLES = 5;

// Prevent the same flame from immediately starting
// another extinguishing cycle
bool fireSystemArmed = true;

// =====================================================
// Setup
// =====================================================
void setup()
{
  Serial.begin(9600);

  pinMode(LEFT_FLAME, INPUT);
  pinMode(CENTER_FLAME, INPUT);
  pinMode(RIGHT_FLAME, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(PUMP_RELAY, OUTPUT);
  digitalWrite(PUMP_RELAY, PUMP_OFF);

  waterServo.attach(SERVO_PIN);
  waterServo.write(90);

  stopRobot();

  Serial.println("Fire-fighting robot started");
}

// =====================================================
// Main loop
// =====================================================
void loop()
{
  int leftFire   = readFlameSensor(LEFT_FLAME);
  int centerFire = readFlameSensor(CENTER_FLAME);
  int rightFire  = readFlameSensor(RIGHT_FLAME);

  printSensorValues(leftFire, centerFire, rightFire);

  bool leftDetected =
      leftFire < FLAME_DETECT_LIMIT;

  bool centerDetected =
      centerFire < FLAME_DETECT_LIMIT;

  bool rightDetected =
      rightFire < FLAME_DETECT_LIMIT;

  bool anyFireDetected =
      leftDetected || centerDetected || rightDetected;

  // Re-arm only after the flame has completely disappeared
  if (!fireSystemArmed)
  {
    if (leftFire > FLAME_RESET_LIMIT &&
        centerFire > FLAME_RESET_LIMIT &&
        rightFire > FLAME_RESET_LIMIT)
    {
      fireSystemArmed = true;

      pumpOff();
      stopRobot();
      waterServo.write(90);

      Serial.println("System re-armed for next fire");
      delay(300);
    }
    else
    {
      stopRobot();
      pumpOff();
    }

    delay(100);
    return;
  }

  // Center sensor has highest priority
  if (centerDetected)
  {
    Serial.println("Fire detected in CENTER");

    moveTowardFire();

    stopRobot();

    extinguishFire();

    // Wait until the current fire disappears before
    // allowing another complete extinguishing cycle
    fireSystemArmed = false;
  }
  else if (leftDetected)
  {
    Serial.println("Fire detected on LEFT");

    turnLeft();
    delay(100);
  }
  else if (rightDetected)
  {
    Serial.println("Fire detected on RIGHT");

    turnRight();
    delay(100);
  }
  else if (!anyFireDetected)
  {
    stopRobot();
    pumpOff();
    waterServo.write(90);
  }

  delay(50);
}

// =====================================================
// Average flame sensor reading
// =====================================================
int readFlameSensor(int sensorPin)
{
  long total = 0;

  for (int i = 0; i < SENSOR_SAMPLES; i++)
  {
    total += analogRead(sensorPin);
    delay(2);
  }

  return total / SENSOR_SAMPLES;
}

// =====================================================
// Display sensor readings
// =====================================================
void printSensorValues(int leftValue,
                       int centerValue,
                       int rightValue)
{
  Serial.print("Left: ");
  Serial.print(leftValue);

  Serial.print("  Center: ");
  Serial.print(centerValue);

  Serial.print("  Right: ");
  Serial.println(rightValue);
}

// =====================================================
// Move closer to the flame
// =====================================================
void moveTowardFire()
{
  forward();

  // Move in small steps instead of continuously
  for (int i = 0; i < 5; i++)
  {
    delay(100);

    int centerValue =
        readFlameSensor(CENTER_FLAME);

    // Stop approaching if the flame is very strong
    if (centerValue < 250)
    {
      break;
    }
  }

  stopRobot();
  delay(200);
}

// =====================================================
// Motor control
// =====================================================
void forward()
{
  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft()
{
  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right motor forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight()
{
  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right motor backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// =====================================================
// Pump control
// =====================================================
void pumpOn()
{
  digitalWrite(PUMP_RELAY, PUMP_ON);
}

void pumpOff()
{
  digitalWrite(PUMP_RELAY, PUMP_OFF);
}

// =====================================================
// Fire extinguishing
// =====================================================
void extinguishFire()
{
  Serial.println("Extinguishing fire");

  stopRobot();
  pumpOn();

  const int maximumSweeps = 5;

  for (int sweep = 0; sweep < maximumSweeps; sweep++)
  {
    // Sweep left to right
    for (int position = 50; position <= 130; position += 5)
    {
      waterServo.write(position);
      delay(60);
    }

    // Sweep right to left
    for (int position = 130; position >= 50; position -= 5)
    {
      waterServo.write(position);
      delay(60);
    }

    int leftValue =
        readFlameSensor(LEFT_FLAME);

    int centerValue =
        readFlameSensor(CENTER_FLAME);

    int rightValue =
        readFlameSensor(RIGHT_FLAME);

    printSensorValues(
        leftValue,
        centerValue,
        rightValue
    );

    // Stop spraying when no flame remains
    if (leftValue > FLAME_RESET_LIMIT &&
        centerValue > FLAME_RESET_LIMIT &&
        rightValue > FLAME_RESET_LIMIT)
    {
      Serial.println("Fire extinguished");
      break;
    }
  }

  pumpOff();
  waterServo.write(90);
  delay(300);
}