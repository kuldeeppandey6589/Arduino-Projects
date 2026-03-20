#include <Servo.h>

const int rainSensor = A0;
const int buzzer = 2;
const int servoPin = 3;0

Servo servo;
bool lastStateIsRaining = false;


void setup() {
  servo.attach(servoPin);
  pinMode(rainSensor, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  bool isRaining = digitalRead(rainSensor);
  if(lastStateIsRaining != isRaining){
    digitalWrite(buzzer, HIGH);
    servo.write(isRaining ? 90: 0);
    delay(1000);
    digitalWrite(buzzer, LOW);
    lastStateIsRaining = isRaining;
  }
}
