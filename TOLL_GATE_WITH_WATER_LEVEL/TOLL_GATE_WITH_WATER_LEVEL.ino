// #include <Servo.h>

// Servo gateServo;

// // Pin configuration
// const int trigPin = 7;     // Ultrasonic trigger
// const int echoPin = 6;     // Ultrasonic echo
// const int waterPin = 8;    // Water level sensor
// const int buzzerPin = 9;   // Buzzer

// // Parameters
// const int distanceThreshold = 20;  // Distance (cm) for vehicle detection
// long duration;
// int distance;
// int waterState = 0;

// void setup() {
//   Serial.begin(9600);

//   pinMode(trigPin, OUTPUT);
//   pinMode(echoPin, INPUT);
//   pinMode(waterPin, INPUT);
//   pinMode(buzzerPin, OUTPUT);

//   gateServo.attach(5);     // Servo signal pin
//   gateServo.write(0);      // Start with gate closed

//   Serial.println("🚧 Smart Toll Gate System Initialized 🚧");
// }

// void loop() {
//   // Read water level sensor
//   waterState = digitalRead(waterPin);

//   if (waterState == HIGH) {
//     // Water detected
//     Serial.println("⚠️ Water detected! Gate closing & buzzer ON.");

//     gateServo.write(0);     // Close gate
//     digitalWrite(buzzerPin, HIGH);  // Turn buzzer ON
//     delay(2000);            // Buzzer for 2 seconds
//     digitalWrite(buzzerPin, LOW);   // Turn buzzer OFF

//     delay(1000);            // Small pause before next reading
//   } 
//   else {
//     // Water level normal — operate toll based on vehicle detection
//     digitalWrite(buzzerPin, LOW);

//     // Ultrasonic distance measurement
//     digitalWrite(trigPin, LOW);
//     delayMicroseconds(2);
//     digitalWrite(trigPin, HIGH);
//     delayMicroseconds(10);
//     digitalWrite(trigPin, LOW);

//     duration = pulseIn(echoPin, HIGH);
//     distance = duration * 0.034 / 2;  // Convert time to distance (cm)

//     Serial.print("Distance: ");
//     Serial.print(distance);
//     Serial.println(" cm");

//     if (distance < distanceThreshold) {
//       Serial.println("🚗 Vehicle detected — Opening gate...");
//       gateServo.write(90);  // Open gate
//       delay(3000);          // Keep gate open for 3 seconds
//       gateServo.write(0);   // Close gate
//       Serial.println("Gate closed.");
//     } else {
//       gateServo.write(0);   // Keep gate closed
//     }

//     delay(1000);
//   }
// }






#include <Servo.h>

// --- Servo Objects ---
Servo gate1Servo;
Servo gate2Servo;

// --- Pin Configuration ---
const int trigPin = 7;     // Ultrasonic Trigger
const int echoPin = 6;     // Ultrasonic Echo
const int waterPin = 8;    // Water Level Sensor
const int buzzerPin = 9;   // Buzzer

// --- Parameters ---
const int distanceThreshold = 20;  // Vehicle detection distance (cm)
long duration;
int distance;
int waterState = 0;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(waterPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Attach servos
  gate1Servo.attach(5);   // Gate 1 servo on pin 5
  gate2Servo.attach(4);   // Gate 2 servo on pin 4

  // Start with gates closed
  gate1Servo.write(0);
  gate2Servo.write(0);

  Serial.println("🚧 Smart Toll Gate System Initialized 🚧");
}

void loop() {
  // --- Measure Distance using Ultrasonic Sensor ---
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;  // Convert to cm

  // --- Read Water Level Sensor ---
  waterState = digitalRead(waterPin);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Water: ");
  Serial.println(waterState == HIGH ? "DETECTED" : "LOW");

  // --- Condition 1: Water Detected Only ---
  if (waterState == HIGH && distance >= distanceThreshold) {
    Serial.println("⚠️ Water detected — Closing gates & buzzer ON.");
    gate1Servo.write(0);
    gate2Servo.write(0);
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    digitalWrite(buzzerPin, LOW);
  }

  // --- Condition 2: Vehicle Detected, Water Normal ---
  else if (waterState == LOW && distance < distanceThreshold) {
    Serial.println("🚗 Vehicle detected — Opening gates...");
    gate1Servo.write(90);   // Open both gates
    gate2Servo.write(90);
    delay(3000);            // Keep open for 3 sec
    gate1Servo.write(0);
    gate2Servo.write(0);    // Close both gates
    Serial.println("Gates closed.");
  }

  // --- Condition 3: Vehicle + Water Detected Together ---
  else if (waterState == HIGH && distance < distanceThreshold) {
    Serial.println("🚨 ALERT: Vehicle + Water detected!");
    digitalWrite(buzzerPin, HIGH);
    delay(2000);
    digitalWrite(buzzerPin, LOW);

    Serial.println("Closing gates after 3 seconds...");
    delay(3000);
    gate1Servo.write(0);
    gate2Servo.write(0);
    Serial.println("Gates closed due to combined condition!");
  }

  // --- Default: Safe State ---
  else {
    gate1Servo.write(0);
    gate2Servo.write(0);
    digitalWrite(buzzerPin, LOW);
  }

  delay(1000); // Stability delay
}



// // Import the Servo library
// #include <Servo.h>

// // --- Define Pin Numbers ---
// const int TRIG_PIN = 10;
// const int ECHO_PIN = 9;
// const int SERVO_PIN = 11;
// const int MOISTURE_PIN = A0;

// // --- Define Constants ---
// // Threshold for moisture. You will need to test this value.
// // Wet items will give a lower reading (e.g., < 400-500)
// // Dry items will give a higher reading (e.g., > 600-700)
// const int WET_THRESHOLD = 500;

// // Distance (in cm) to trigger the sensor.  
// const int DISTANCE_THRESHOLD = 20;

// // Servo positions
// const int SERVO_START_POS = 90; // Neutral position
// const int SERVO_DRY_POS = 45;   // Position for dry waste
// const int SERVO_WET_POS = 135;  // Position for wet waste

// // --- Global Variables ---
// long duration;
// int distance;

// // Create a servo object
// Servo sorterServo;

// void setup() {
//   // Initialize Serial Monitor for debugging
//   Serial.begin(9600);

//   // Initialize pins
//   pinMode(TRIG_PIN, OUTPUT);
//   pinMode(ECHO_PIN, INPUT);
  
//   // The moisture sensor pin (A0) is an analog input,
//   // no explicit pinMode is required, but good practice.
//   pinMode(MOISTURE_PIN, INPUT);

//   // Attach the servo to its pin
//   sorterServo.attach(SERVO_PIN);
  
//   // Move servo to the starting neutral position
//   sorterServo.write(SERVO_START_POS);
//   Serial.println("Smart Dustbin Ready!");
//   delay(1000);
// }

// void loop() {
//   // 1. Check for presence using the Ultrasonic Sensor
//   distance = getDistance();
  
//   Serial.print("Distance: ");
//   Serial.print(distance);
//   Serial.println(" cm");

//   // If an object is detected within the threshold
//   if (distance < DISTANCE_THRESHOLD) {
//     Serial.println("Object detected!");
    
//     // Wait a moment for the object to be placed
//     delay(1500); // 1.5 second delay

//     // 2. Check if the waste is wet or dry
//     int moistureValue = analogRead(MOISTURE_PIN);
//     Serial.print("Moisture Value: ");
//     Serial.println(moistureValue);

//     // 3. Sort the waste based on moisture
//     if (moistureValue < WET_THRESHOLD) {
//       // It's WET
//       Serial.println("Result: WET Waste");
//       sorterServo.write(SERVO_WET_POS);
//     } else {
//       // It's DRY
//       Serial.println("Result: DRY Waste");
//       sorterServo.write(SERVO_DRY_POS);
//     }

//     // 4. Wait for the waste to fall
//     delay(3000); // Hold position for 3 seconds

//     // 5. Return to neutral position
//     sorterServo.write(SERVO_START_POS);
//     Serial.println("Returning to neutral. Waiting for next item...");
    
//     // Wait a bit before checking again to avoid multiple triggers
//     delay(2000); 
//   }
  
//   // Small delay to make the loop more stable
//   delay(100); 
// }

// /**
//  * @brief Measures distance using the HC-SR04 sensor.
//  * @return The distance in centimeters (cm).
//  */
// int getDistance() {
//   // Clear the trigPin
//   digitalWrite(TRIG_PIN, LOW);
//   delayMicroseconds(2);

//   // Send a 10 microsecond pulse to trigger the sensor
//   digitalWrite(TRIG_PIN, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(TRIG_PIN, LOW);

//   // Read the echoPin, which returns the sound wave travel time in microseconds
//   duration = pulseIn(ECHO_PIN, HIGH);

//   // Calculate the distance
//   // Speed of sound wave divided by 2 (go and return)
//   // 343 m/s = 0.0343 cm/us
//   distance = duration * 0.0343 / 2;

//   return distance;
// }
