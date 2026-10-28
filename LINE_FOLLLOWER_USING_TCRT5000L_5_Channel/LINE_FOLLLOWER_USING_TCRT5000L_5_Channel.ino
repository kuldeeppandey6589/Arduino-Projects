
// =====================================================
// MOTOR PINS
// =====================================================

// Motor 1 - Left Motor
#define IN1 7
#define IN2 8

// Motor 2 - Right Motor
#define IN3 11
#define IN4 12

// Enable Pins
#define EN1 9
#define EN2 10


// =====================================================
// SENSOR PINS
// =====================================================

#define S1 2   // Far Left
#define S2 3   // Left
#define S3 4   // Center
#define S4 5   // Right
#define S5 6   // Far Right


// =====================================================
// SPEED SETTINGS
// =====================================================

int forwardSpeed = 80;
int turnSpeed = 65;
int hardTurnSpeed = 80;


// =====================================================
// SENSOR VARIABLES
// =====================================================

int s1, s2, s3, s4, s5;


// Remember last direction
// -1 = Left
//  0 = Center
//  1 = Right
int lastDirection = 0;


// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);

  // Motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  // Sensor pins
  pinMode(S1, INPUT);
  pinMode(S2, INPUT);
  pinMode(S3, INPUT);
  pinMode(S4, INPUT);
  pinMode(S5, INPUT);

  stopMotors();

  delay(1000);
}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Read sensors
  s1 = digitalRead(S1);
  s2 = digitalRead(S2);
  s3 = digitalRead(S3);
  s4 = digitalRead(S4);
  s5 = digitalRead(S5);


  // Print sensor values
  Serial.print(s1);
  Serial.print(" ");
  Serial.print(s2);
  Serial.print(" ");
  Serial.print(s3);
  Serial.print(" ");
  Serial.print(s4);
  Serial.print(" ");
  Serial.println(s5);


  // ===================================================
  // CENTER
  // 1 1 0 1 1
  // ===================================================

  if (s1 == 1 && s2 == 1 && s3 == 0 &&
      s4 == 1 && s5 == 1)
  {
    forward();
    lastDirection = 0;
  }


  // ===================================================
  // SLIGHT LEFT
  //
  // 1 0 1 1 1
  // 1 0 0 1 1
  // ===================================================

  else if (
    (s1 == 1 && s2 == 0 && s3 == 1 && s4 == 1 && s5 == 1) ||
    (s1 == 1 && s2 == 0 && s3 == 0 && s4 == 1 && s5 == 1)
  )
  {
    slightLeft();
    lastDirection = -1;
  }


  // ===================================================
  // HARD LEFT
  //
  // 0 1 1 1 1
  // 0 0 1 1 1
  // 0 0 0 1 1
  // ===================================================

  else if (
    (s1 == 0 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 1) ||
    (s1 == 0 && s2 == 0 && s3 == 1 && s4 == 1 && s5 == 1) ||
    (s1 == 0 && s2 == 0 && s3 == 0 && s4 == 1 && s5 == 1)
  )
  {
    hardLeft();
    lastDirection = -1;
  }


  // ===================================================
  // SLIGHT RIGHT
  //
  // 1 1 1 0 1
  // 1 1 0 0 1
  // ===================================================

  else if (
    (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0 && s5 == 1) ||
    (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0 && s5 == 1)
  )
  {
    slightRight();
    lastDirection = 1;
  }


  // ===================================================
  // HARD RIGHT
  //
  // 1 1 1 1 0
  // 1 1 1 0 0
  // 1 1 0 0 0
  // ===================================================

  else if (
    (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 1 && s5 == 0) ||
    (s1 == 1 && s2 == 1 && s3 == 1 && s4 == 0 && s5 == 0) ||
    (s1 == 1 && s2 == 1 && s3 == 0 && s4 == 0 && s5 == 0)
  )
  {
    hardRight();
    lastDirection = 1;
  }


  // ===================================================
  // ALL BLACK
  // 0 0 0 0 0
  //
  // Could be intersection / finish line
  // ===================================================

  else if (s1 == 0 && s2 == 0 && s3 == 0 &&
           s4 == 0 && s5 == 0)
  {
    stopMotors();
  }


  // ===================================================
  // ALL WHITE - LINE LOST
  // 1 1 1 1 1
  //
  // Search in last known direction
  // ===================================================

  else if (s1 == 1 && s2 == 1 && s3 == 1 &&
           s4 == 1 && s5 == 1)
  {
    if (lastDirection == -1)
    {
      hardLeft();
    }

    else if (lastDirection == 1)
    {
      hardRight();
    }

    else
    {
      stopMotors();
    }
  }


  // ===================================================
  // OTHER SENSOR COMBINATIONS
  // ===================================================

  else
  {
    forward();
  }

  delay(5);
}


// =====================================================
// FORWARD
// =====================================================

void forward()
{
  analogWrite(EN1, forwardSpeed);
  analogWrite(EN2, forwardSpeed);

  // Left Motor Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right Motor Forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// SLIGHT LEFT
// =====================================================

void slightLeft()
{
  // Left motor slower
  analogWrite(EN1, 70);

  // Right motor faster
  analogWrite(EN2, turnSpeed);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// HARD LEFT
// =====================================================

void hardLeft()
{
  analogWrite(EN1, hardTurnSpeed);
  analogWrite(EN2, hardTurnSpeed);

  // Left Motor Backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  // Right Motor Forward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// SLIGHT RIGHT
// =====================================================

void slightRight()
{
  // Left motor faster
  analogWrite(EN1, turnSpeed);

  // Right motor slower
  analogWrite(EN2, 70);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}


// =====================================================
// HARD RIGHT
// =====================================================

void hardRight()
{
  analogWrite(EN1, hardTurnSpeed);
  analogWrite(EN2, hardTurnSpeed);

  // Left Motor Forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  // Right Motor Backward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// BACKWARD
// =====================================================

void back()
{
  analogWrite(EN1, forwardSpeed);
  analogWrite(EN2, forwardSpeed);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}


// =====================================================
// STOP
// =====================================================

void stopMotors()
{
  analogWrite(EN1, 0);
  analogWrite(EN2, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}