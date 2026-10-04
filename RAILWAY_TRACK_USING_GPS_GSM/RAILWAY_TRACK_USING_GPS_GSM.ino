#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Initialize LCD (Address 0x27, 16 columns, 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Software Serial configurations for modules
SoftwareSerial sim800l(2, 3); // RX = Pin 2 (Connect to SIM800L TX), TX = Pin 3 (Connect to SIM800L RX via voltage divider)
SoftwareSerial gpsSerial(0, 1); // RX = Pin 10 (Connect to GPS TX), TX = Pin 11 (Connect to GPS RX)

// **REASSIGNED MOTOR PINS TO FREE UP SERIAL PINS**
const int in1 = 10;
const int in2 = 11;
const int in3 = 12;
const int in4 = 13;

// Ultrasonic Sensor pins
const int trigPin = 8;
const int echoPin = 9;

// IR Sensor pins
const int irPin1 = 6;
const int irPin2 = 7;

// Buzzer pin
const int buzzerPin = 5;

const int stopDistance = 20; 
const int irDetectedState = LOW; 

// Replace with your target mobile number (include country code)
const String PHONE_NUMBER = "+918960383086"; 

// Tracking variables
String lastLcdMessage1 = "";
String lastLcdMessage2 = "";
bool smsSent = false; // Prevents spamming SMS while stopped

void setup() {
  // Set reassigned motor pins as outputs
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  // Set sensor and buzzer pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  updateLCD("Car Initialized", "System Ready");
  
  // Initialize Serial Connections
  Serial.begin(9600); 
  sim800l.begin(9600);
  gpsSerial.begin(9600);
  
  delay(1000);
  initGSM();
}

void loop() {
  long duration;
  int distance;

  // 1. Trigger the Ultrasonic Sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // 2. Read the state of both IR sensors
  int irState1 = digitalRead(irPin1);
  int irState2 = digitalRead(irPin2);
  bool irDetectingBoth = (irState1 == irDetectedState && irState2 == irDetectedState);

  // 3. Core Driving Logic with LCD, Buzzer, and SMS Alert
  if (!irDetectingBoth) {
    stopCar();
    digitalWrite(buzzerPin, HIGH); 
    updateLCD("CAR STOPPED", "Fault on Track");
    
    if (!smsSent) {
      sendAlertSMS("Fault on Track detected!");
      smsSent = true;
    }
  } 
  else if (distance > 0 && distance <= stopDistance) {
    stopCar();
    digitalWrite(buzzerPin, HIGH); 
    updateLCD("CAR STOPPED", "Object Detected");
    
    if (!smsSent) {
      sendAlertSMS("Obstacle detected ahead!");
      smsSent = true;
    }
  } 
  else {
    digitalWrite(buzzerPin, LOW);  
    updateLCD("CAR RUNNING", "Path Clear");
    moveForward();
    smsSent = false; // Reset token once path is clear and moving again
  }
  
  delay(100); 
}

void moveForward() {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopCar() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void updateLCD(String line1, String line2) {
  if (line1 != lastLcdMessage1 || line2 != lastLcdMessage2) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
    lastLcdMessage1 = line1;
    lastLcdMessage2 = line2;
  }
}

void initGSM() {
  sim800l.listen();
  sim800l.println("AT"); 
  delay(1000);
  sim800l.println("AT+CMGF=1"); // Set SMS system to text mode
  delay(1000);
}

void sendAlertSMS(String reason) {
  String googleMapsLink = "Waiting for GPS Fix...";
  
  // Listen to the GPS module to pull structural coordinate string
  gpsSerial.listen();
  unsigned long start = millis();
  
  // Read GPS stream for up to 2 seconds to parse location data
  while (millis() - start < 2000) {
    while (gpsSerial.available()) {
      char c = gpsSerial.read();
      // Raw parsing snippet (Looks for standard NMEA $GPRMC or $GPGGA patterns)
      // For highly precise parsing, libraries like TinyGPS++ are recommended.
      Serial.write(c); // Print raw GPS data to serial monitor for debugging
    }
  }
  
  // Note: To dynamically unpack standard NMEA coordinates into an absolute URL, 
  // you can tie in a helper parser library like TinyGPS++. Below uses a placeholder fallback layout.
  googleMapsLink = "https://google.com"; // Static fallback example

  // Return communication focus to the GSM Module
  sim800l.listen();
  sim800l.println("AT+CMGS=\"" + PHONE_NUMBER + "\""); 
  delay(1000);
  
  sim800l.print("ALERT: Emergency stop triggered! Reason: ");
  sim800l.println(reason);
  sim800l.print("Location: ");
  sim800l.println(googleMapsLink);
  delay(100);
  
  sim800l.write(26); // Send ASCII character 26 (Ctrl+Z) to finalize and send SMS
  delay(5000); 
}
