#include <SoftwareSerial.h>

// -------- Bluetooth --------
SoftwareSerial bt(2, 3); // RX, TX

// -------- Motor Pins --------
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// -------- Ultrasonic Pins --------
#define TRIG 6
#define ECHO 7

String command = "";

void setup() {
  bt.begin(9600);
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  stopCar();
  Serial.println("Voice Control Car Ready");
}

void loop() {

  long distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // -------- Obstacle Detection --------
  if (distance > 0 && distance <= 20) {
    stopCar();
    delay(300);

    moveForward();
    delay(1000);

    stopCar();
    delay(500);
    return;
  }

  // -------- Voice Control --------
  if (bt.available()) {
    command = bt.readStringUntil('\n');
    command.trim();          // remove spaces
    command.toLowerCase();   // convert to lowercase

    Serial.println(command);

    if (command == "backward") moveForward();
    else if (command == "forward") moveBackward();
    else if (command == "right") turnLeft();
    else if (command == "left") turnRight();
    else if (command == "stop") stopCar();
  }
}

// -------- Distance Function --------
long getDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 25000);
  long distance = duration * 0.034 / 2;
  return distance;
}

// -------- Motor Functions --------
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
