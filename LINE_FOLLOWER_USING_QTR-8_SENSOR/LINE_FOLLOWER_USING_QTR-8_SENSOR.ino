#include <QTRSensors.h>

// =====================================================
// QTR-8RC SENSOR
// =====================================================

QTRSensors qtr;

const uint8_t SensorCount = 8;

uint16_t sensorValues[SensorCount];

// Sensor pins from LEFT to RIGHT
const uint8_t sensorPins[SensorCount] =
{
  A0, A1, A2, A3, A4, A5, 10, 11
};

// =====================================================
// L298N MOTOR DRIVER
// =====================================================

// Left Motor
#define ENA 5
#define IN1 2
#define IN2 3

// Right Motor
#define ENB 6
#define IN3 7
#define IN4 8


// =====================================================
// SPEED SETTINGS
// =====================================================

int baseSpeed = 130;

int maxSpeed = 200;

int minSpeed = 0;


// =====================================================
// LINE FOLLOWING SETTINGS
// =====================================================

// QTR-8 sensor position:
// 0 ----------------------------- 7000
//               3500
//
// 3500 = center

float Kp = 0.05;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);

  // -----------------------------
  // Motor pins
  // -----------------------------

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);


  // Stop motors
  stopMotors();


  // -----------------------------
  // Configure QTR-8RC
  // -----------------------------

  qtr.setTypeRC();

  qtr.setSensorPins(sensorPins, SensorCount);


  // ===================================================
  // SENSOR CALIBRATION
  // ===================================================

  Serial.println("QTR Calibration Starting");

  Serial.println("Move sensor over BLACK and WHITE surface");

  delay(1000);


  for (uint16_t i = 0; i < 200; i++)
  {
    qtr.calibrate();

    delay(10);
  }


  Serial.println("Calibration Complete");

  delay(1000);
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Read position of BLACK line
  //
  // Left     = 0
  // Center   = 3500
  // Right    = 7000

  uint16_t position = qtr.readLineBlack(sensorValues);


  // Calculate error
  int error = position - 3500;


  // Proportional correction
  int correction = Kp * error;


  // Calculate motor speeds
  int leftSpeed  = baseSpeed + correction;

  int rightSpeed = baseSpeed - correction;


  // Limit speeds
  leftSpeed = constrain(leftSpeed, minSpeed, maxSpeed);

  rightSpeed = constrain(rightSpeed, minSpeed, maxSpeed);


  // Drive motors
  setMotorSpeed(leftSpeed, rightSpeed);


  // -----------------------------
  // Serial Monitor
  // -----------------------------

  Serial.print("Position: ");
  Serial.print(position);

  Serial.print("   Error: ");
  Serial.print(error);

  Serial.print("   L: ");
  Serial.print(leftSpeed);

  Serial.print("   R: ");
  Serial.println(rightSpeed);
}


// =====================================================
// MOTOR SPEED
// =====================================================

void setMotorSpeed(int leftSpeed, int rightSpeed)
{
  // -----------------------------
  // LEFT MOTOR FORWARD
  // -----------------------------

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  analogWrite(ENA, leftSpeed);


  // -----------------------------
  // RIGHT MOTOR FORWARD
  // -----------------------------

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  analogWrite(ENB, rightSpeed);
}


// =====================================================
// STOP MOTOR
// =====================================================

void stopMotors()
{
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}