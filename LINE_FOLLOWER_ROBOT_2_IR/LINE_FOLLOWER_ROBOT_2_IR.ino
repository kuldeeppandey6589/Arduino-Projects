// IR Sensor Pins
#define LEFT_SENSOR 2
#define RIGHT_SENSOR 3

// Motor Driver Pins
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7

void setup() 
{
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() 
{
  int left = digitalRead(LEFT_SENSOR);
  int right = digitalRead(RIGHT_SENSOR);

  // Both sensors on line
  if (left == 0 && right == 0)
  {
    forward();
  }
  // Left sensor off line
  else if (left == 1 && right == 0)
  {
    turnLeft();
  }
  // Right sensor off line
  else if (left == 0 && right == 1)
  {
    turnRight();
  }
  // Both off line
  else
  {
    stopRobot();
  }
}

void forward()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight()
{
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stopRobot()
{
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}