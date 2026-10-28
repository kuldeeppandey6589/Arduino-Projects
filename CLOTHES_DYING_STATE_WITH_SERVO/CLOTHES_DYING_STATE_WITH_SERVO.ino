#include <Servo.h>

Servo myServo;  // Create a Servo object
int rainSensorPin = A0;  // Define the analog pin for the raindrop sensor
int buzzer = 7;
int servoPin = 8;  // Define the digital pin for the servo motor
int threshold = 700;  // Set the threshold value to detect rain

void setup() {
  myServo.attach(servoPin);  // Attach the servo to the specified pin
  myServo.write(0);  // Initialize the servo position at 0 degrees
  pinMode(rainSensorPin, INPUT);  // Set the raindrop sensor pin as input
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);  // Initialize serial communication for debugging
}

void loop() {
  int sensorValue = analogRead(rainSensorPin);  // Read the value from the raindrop sensor
  Serial.println(sensorValue);  // Print the sensor value for debugging

  int currentPos = myServo.read();  // Get the current position of the servo

  if (sensorValue < threshold) {  // If rain is detected
    for (int pos = currentPos; pos <= 90; pos++) {  // Gradually move to 90 degrees
      myServo.write(pos);
      digitalWrite(buzzer, HIGH);
      delay(15);  // Small delay for smooth movement
    }
  } else {
    for (int pos = currentPos; pos >= 0; pos--) {  // Gradually move back to 0 degrees
      myServo.write(pos);
      digitalWrite(buzzer, LOW);
      delay(15);  // Small delay for smooth movement
    }
  }

  delay(1000);  // Wait for a second before reading the sensor again
}
