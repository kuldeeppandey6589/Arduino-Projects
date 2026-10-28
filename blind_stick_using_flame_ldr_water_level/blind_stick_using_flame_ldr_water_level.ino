// Blind Stick using Ultrasonic, Fire Sensor, LDR, Water Sensor and Buzzer

// Ultrasonic Sensor Pins
#define TRIG_PIN 9
#define ECHO_PIN 8

// Sensor Pins
#define FIRE_SENSOR 2       // Digital pin
#define LDR_SENSOR A0       // Analog pin
#define WATER_SENSOR A1     // Analog pin

// Buzzer Pin
#define BUZZER 4

// Threshold values
int objectDistance = 30;    // cm, buzzer ON if object is closer than 50 cm
int ldrThreshold = 400;     // adjust after testing
int waterThreshold = 400;   // adjust after testing

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(FIRE_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);
}

void loop() {
  long distance = getDistance();

  int fireValue = digitalRead(FIRE_SENSOR);
  int ldrValue = analogRead(LDR_SENSOR);
  int waterValue = analogRead(WATER_SENSOR);

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm | Fire: ");
  Serial.print(fireValue);
  Serial.print(" | LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Water: ");
  Serial.println(waterValue);

  bool objectDetected = distance > 0 && distance <= objectDistance;

  // Most fire sensor modules give LOW when fire is detected
  bool fireDetected = fireValue == LOW;

  // LDR value depends on module/wiring.
  // Usually lower value means darkness in voltage divider circuit.
  bool darknessDetected = ldrValue < ldrThreshold;

  bool waterDetected = waterValue > waterThreshold;

  if (objectDetected || fireDetected || darknessDetected || waterDetected) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  delay(100);
}

long getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1;
  }

  long distance = duration * 0.034 / 2;
  return distance;
}