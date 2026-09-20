// L298N Motor Pins
#define IN1 9
#define IN2 10
#define IN3 11
#define IN4 12

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.println("Bluetooth Car Ready");
}

void loop() {

  if (Serial.available()) {

    char cmd = Serial.read();

    Serial.print("Command: ");
    Serial.println(cmd);

    // Forward
    if (cmd == 'F') {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      Serial.println("Forward");
    }

    // Backward
    else if (cmd == 'B') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      Serial.println("Backward");
    }

    // Left
    else if (cmd == 'L') {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);

  Serial.println("LEFT");
}
    // Right
    else if (cmd == 'R') {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      Serial.println("Right");
    }

    // Stop
    else if (cmd == 'S') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      Serial.println("Stop");
    }
  }
}