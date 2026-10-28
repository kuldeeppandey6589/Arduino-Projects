#include <SoftwareSerial.h>

// =====================================
// L298N MOTOR DRIVER PINS
// =====================================
// IN1 & IN2 control Left side motors
// IN3 & IN4 control Right side motors
#define IN1 7
#define IN2 8
#define IN3 9
#define IN4 10

// =====================================
// HC-05 BLUETOOTH PINS
// HC-05 TX -> Arduino D2
// HC-05 RX -> Arduino D3
// =====================================
SoftwareSerial Bluetooth(2, 3);

// =====================================
// GLOBAL VARIABLES
// =====================================
char command = 'S'; // Default state is STOP

// =====================================
// SETUP
// =====================================
void setup() {
  // Initialize serial communications at 9600 baud
  Serial.begin(9600);
  Bluetooth.begin(9600);
  
  // Set motor control pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Ensure the car is stationary at boot
  stopCar();
  
  Serial.println("4 Wheel Robotic Car Ready");
  Bluetooth.println("Robotic Car Ready");
}

// =====================================
// MAIN LOOP
// =====================================
void loop() {
  // Check if any data is coming from the Bluetooth module
  if (Bluetooth.available()) {
    command = Bluetooth.read();
    Serial.print("Command Received: ");
    Serial.println(command);
  }

  // Execute direction based on the current command
  switch (command) {
    case 'F': case 'f': forward();  break;
    case 'B': case 'b': backward(); break;
    case 'L': case 'l': left();     break;
    case 'R': case 'r': right();    break;
    case 'S': case 's': stopCar();  break;
    default:            stopCar();  break;
  }
  
  delay(30); // Tiny delay to keep serial buffer stable
}

// =====================================
// DIRECTION FUNCTIONS
// =====================================

void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("FORWARD");
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("BACKWARD");
}

/* 
 NOTE ON TURNING: 
 If your car goes RIGHT when you press LEFT, simply swap the 
 HIGH and LOW values between the left() and right() functions below.
*/

void left() {
  // Left wheels go backward, Right wheels go forward -> Sharp Left Turn
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("LEFT");
}

void right() {
  // Left wheels go forward, Right wheels go backward -> Sharp Right Turn
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("RIGHT");
}

void stopCar() {
  // Cut power to all motors
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("STOP");
}
