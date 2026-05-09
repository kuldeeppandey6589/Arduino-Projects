#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Ultrasonic Pins
const int trigPin = 9;
const int echoPin = 10;

// DFPlayer
SoftwareSerial mp3Serial(2, 3); // RX, TX
DFRobotDFPlayerMini dfplayer;

int distance;
bool played = false;

void setup() {
  Serial.begin(9600);
  mp3Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("System Starting...");

  if (!dfplayer.begin(mp3Serial)) {
    Serial.println("DFPlayer Error!");
    while (true);
  }

  Serial.println("DFPlayer Ready");
  dfplayer.volume(30);  // Max volume
  delay(1000);
}

void loop() {

  // Ultrasonic trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Condition: less than 50 cm
  if (distance > 0 && distance <= 50) {
    if (!played) {
      Serial.println("Playing Audio...");
      dfplayer.play(1);  // 0001.mp3
      played = true;
    }
  } else {
    played = false; // reset
  }

  delay(300);
}