// Blind Stick Project with Arduino Nano
// Components: 1 Ultrasonic Sensor (HC-SR04), 1 Buzzer

// Pin definitions
const int trigPin = 9;     // Ultrasonic sensor TRIG
const int echoPin = 8;    // Ultrasonic sensor ECHO
const int buzzerPin = 4;   // Buzzer

// Variables
long duration;
int distance;

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
} 

void loop() {
  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance (in cm)
  distance = duration * 0.034 / 2;

  // Print distance for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Trigger buzzer if obstacle is close
  if (distance > 10 && distance <= 50) {   // threshold: 50 cm
    digitalWrite(buzzerPin, HIGH);        // buzzer ON
  } else {
    digitalWrite(buzzerPin, LOW);         // buzzer OFF
  }

  delay(100); // small delay for stability
}
