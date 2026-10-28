// GP2Y1010AU0F Calibration Code + 2 Channel Relay Control

const int dustLedPin = 2;   // Dust sensor LED control
const int dustVoPin  = A0;  // Dust sensor analog output

const int pumpRelay  = 7;   // Relay channel for water pump
const int motorRelay = 8;   // Relay channel for motor

const int rawThreshold = 600;   // Trigger value

void setup() {
  Serial.begin(9600);

  pinMode(dustLedPin, OUTPUT);
  pinMode(pumpRelay, OUTPUT);
  pinMode(motorRelay, OUTPUT);

  digitalWrite(dustLedPin, HIGH);

  // Relay OFF at start (active LOW relay module)
  digitalWrite(pumpRelay, HIGH);
  digitalWrite(motorRelay, HIGH);

  Serial.println("Dust Sensor + Pump + Motor System Started...");
}

void loop() {
  int rawValue = 0;
  float voltage = 0;
  float dustDensity = 0;

  // Turn ON IR LED
  digitalWrite(dustLedPin, LOW);
  delayMicroseconds(280);

  // Read analog value
  rawValue = analogRead(dustVoPin);

  delayMicroseconds(40);

  // Turn OFF IR LED
  digitalWrite(dustLedPin, HIGH);
  delayMicroseconds(9680);

  // Convert to voltage
  voltage = rawValue * (5.0 / 1024.0);

  // Convert to dust density
  dustDensity = 0.17 * voltage - 0.1;
  if (dustDensity < 0) dustDensity = 0;

  // Print values
  Serial.print("RAW: ");
  Serial.print(rawValue);

  Serial.print(" | Voltage: ");
  Serial.print(voltage);

  Serial.print(" V | Dust Density: ");
  Serial.print(dustDensity);
  Serial.print(" mg/m3");

  // Relay control
  if (rawValue > rawThreshold) {
    digitalWrite(pumpRelay, LOW);    // ON
    digitalWrite(motorRelay, LOW);   // ON
    Serial.println(" | Dust Detected -> Pump ON, Motor ON");
  } else {
    digitalWrite(pumpRelay, HIGH);   // OFF
    digitalWrite(motorRelay, HIGH);  // OFF
    Serial.println(" | Clean -> Pump OFF, Motor OFF");
  }

  delay(1000);
}