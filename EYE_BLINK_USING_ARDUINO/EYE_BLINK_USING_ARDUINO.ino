#define EYE_SENSOR A0
#define BUZZER_PIN 8

// Adjust after checking Serial Monitor
int blinkThreshold = 500;

// Eye must remain closed this long before alarm
unsigned long eyeCloseTime = 1500;   // 1.5 seconds

unsigned long closedStartTime = 0;
bool eyeClosed = false;

void setup()
{
  Serial.begin(9600);

  pinMode(EYE_SENSOR, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);

  Serial.println("Eye Blink Goggles Started");
}

void loop()
{
  int eyeValue = analogRead(EYE_SENSOR);

  Serial.print("Eye Sensor: ");
  Serial.println(eyeValue);

  // Usually sensor value changes when eye closes.
  // Change < to > if your sensor works opposite.
  if (eyeValue < blinkThreshold)
  {
    if (!eyeClosed)
    {
      eyeClosed = true;
      closedStartTime = millis();
    }

    // Eye closed continuously for more than 1.5 sec
    if (millis() - closedStartTime >= eyeCloseTime)
    {
      digitalWrite(BUZZER_PIN, HIGH);
      Serial.println("WARNING: Eye Closed - BUZZER ON");
    }
  }
  else
  {
    eyeClosed = false;
    closedStartTime = 0;

    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(50);
}