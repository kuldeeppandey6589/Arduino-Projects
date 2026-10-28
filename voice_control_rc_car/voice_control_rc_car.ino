#include <SoftwareSerial.h>

SoftwareSerial BT(10, 11); // RX, TX

// Motor pins
int IN1 = 2;
int IN2 = 3;
int IN3 = 4;
int IN4 = 5;
int ENA = 6;
int ENB = 9;

// Ultrasonic pins
int trigPin = 7;
int echoPin = 8;

String command = "";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         

long duration;
int distance;

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  analogWrite(ENA, 200);
  analogWrite(ENB, 200);

  Serial.println("Car Ready");
}

void loop() {

  // 🔹 Ultrasonic Distance Measure
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  if (duration == 0) {
    distance = 999; // no reading
  } else {
    distance = duration * 0.034 / 2;
  }

  Serial.print("Distance: ");
  Serial.println(distance);

  // 🚨 Obstacle Detection (20 cm se kam → STOP)
  if (distance > 2 && distance <= 10) {
    Serial.println("Obstacle Detected - STOP");
    stopMotor();
    delay(100);   // stability delay
    return;
  }

  // 🔹 Bluetooth Command Read
  if (BT.available()) {
    command = BT.readString();
    command.trim();
    command.toLowerCase();

    Serial.print("Command: ");
    Serial.println(command);

    if (command == "forward") {
      forward();
    } 
    else if (command == "backward") {
      backward();
    } 
    else if (command == "left") {
      left();
    } 
    else if (command == "right") {
      right();
    } 
    else if (command == "stop") {
      stopMotor();
    }
  }
}

// 🚗 Movement Functions

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Forward");
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Backward");
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Left");
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Right");
}

void stopMotor() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Stopped");
}
