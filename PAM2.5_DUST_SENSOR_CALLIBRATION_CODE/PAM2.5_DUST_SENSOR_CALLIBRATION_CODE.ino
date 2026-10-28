// GP2Y1010AU0F Calibration Code

const int dustLedPin = 2;   // LED control
const int dustVoPin  = A0;  // Analog output

void setup() {
  Serial.begin(9600);
  pinMode(dustLedPin, OUTPUT);
  digitalWrite(dustLedPin, HIGH);

  Serial.println("Dust Sensor Calibration Started...");
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
  Serial.println(" mg/m3");

  delay(1000);
}

