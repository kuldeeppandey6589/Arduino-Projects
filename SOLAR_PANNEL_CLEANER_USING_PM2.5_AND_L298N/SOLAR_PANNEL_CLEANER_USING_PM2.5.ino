// GP2Y1010AU0F + Relay + L298N Motor Control

const int dustLedPin = 2;    // Dust sensor LED pin
const int dustVoPin  = A0;   // Dust sensor analog output

const int pumpRelay  = 7;    // Water pump relay

// L298N motor driver pins
const int ENA = 9;           // PWM enable pin
const int IN1 = 10;          // Motor direction 1
const int IN2 = 11;          // Motor direction 2

const int rawThreshold = 600;   // Adjust after testing

void setup() {
  Serial.begin(9600);

  pinMode(dustLedPin, OUTPUT);
  pinMode(dustVoPin, INPUT);

  pinMode(pumpRelay, OUTPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  digitalWrite(dustLedPin, HIGH);

  // Pump OFF at start
  digitalWrite(pumpRelay, LOW);

  // Motor OFF at start
  stopMotor();

  Serial.println("Dust Sensor + Pump + L298N Motor System Started...");
}

void loop() {
  int rawValue = 0;
  float voltage = 0;
  float dustDensity = 0;

  // Read dust sensor
  digitalWrite(dustLedPin, LOW);
  delayMicroseconds(280);

  rawValue = analogRead(dustVoPin);

  delayMicroseconds(40);
  digitalWrite(dustLedPin, HIGH);
  delayMicroseconds(9680);

  voltage = rawValue * (5.0 / 1024.0);
  dustDensity = 0.17 * voltage - 0.1;
  if (dustDensity < 0) dustDensity = 0;

  Serial.print("RAW: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage);
  Serial.print(" V | Dust Density: ");
  Serial.print(dustDensity);
  Serial.print(" mg/m3");

  if (rawValue > rawThreshold) {
    Serial.println(" | Dust Detected -> Pump ON + Motor Forward/Backward");

    // Pump ON
    digitalWrite(pumpRelay, HIGH);

    // Motor forward for 2 sec
    motorForward();
    delay(2300);

    // Stop for small safety gap
    stopMotor();
    delay(500);

    // Motor backward for 2 sec
    motorBackward();
    delay(3000);

    // Stop for small safety gap
    stopMotor();
    delay(500);
  }
  else {
    Serial.println(" | Clean -> Pump OFF + Motor STOP");

    digitalWrite(pumpRelay, LOW);
    stopMotor();

    delay(1000);
  }
}

// ======================
// Motor Functions
// ======================
void motorForward() {
  analogWrite(ENA, 200);   // Speed 0 to 255
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorBackward() {
  analogWrite(ENA, 200);   // Speed 0 to 255
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void stopMotor() {
  analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
}