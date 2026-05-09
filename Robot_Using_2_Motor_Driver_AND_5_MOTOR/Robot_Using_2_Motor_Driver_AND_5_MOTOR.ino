/* BLUETOOTH CAR WITH ARDUINO UNO AND HC-05
   Car control + one extra single motor using second L298N

   Commands:
   F = Forward
   B = Backward
   R = Right
   L = Left
   C = Extra motor clockwise
   A = Extra motor anti-clockwise
   O = LED ON
   s = LED OFF
   S = Stop all motors
*/

#define led1 13

// Car motor driver (L298N-1)
#define in1 12
#define in2 11
#define in3 10
#define in4 9

// Extra single motor driver (L298N-2)
#define m2_in1 7
#define m2_in2 6

void stopCar() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void stopExtraMotor() {
  digitalWrite(m2_in1, LOW);
  digitalWrite(m2_in2, LOW);
}

void setup() {
  Serial.begin(9600);
  Serial.println("Start");

  pinMode(led1, OUTPUT);

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  pinMode(m2_in1, OUTPUT);
  pinMode(m2_in2, OUTPUT);

  stopCar();
  stopExtraMotor();
  digitalWrite(led1, LOW);
}

void loop() {
  if (Serial.available() > 0) {
    char inputvalue = Serial.read();

    // =========================
    // Car movement
    // =========================
    if (inputvalue == 'F') {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Forward");
    }

    else if (inputvalue == 'B') {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("Backward");
    }

    else if (inputvalue == 'R') {
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      Serial.println("Right");
    }

    else if (inputvalue == 'L') {
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      Serial.println("Left");
    }

    // =========================
    // Extra single motor control
    // =========================
    else if (inputvalue == 'C') {
      digitalWrite(m2_in1, HIGH);
      digitalWrite(m2_in2, LOW);
      Serial.println("Extra Motor Clockwise");
    }

    else if (inputvalue == 'A') {
      digitalWrite(m2_in1, LOW);
      digitalWrite(m2_in2, HIGH);
      Serial.println("Extra Motor AntiClockwise");
    }

    // =========================
    // LED control
    // =========================
    else if (inputvalue == 'O') {
      digitalWrite(led1, HIGH);
      Serial.println("LED ON");
    }

    else if (inputvalue == 's') {
      digitalWrite(led1, LOW);
      Serial.println("LED OFF");
    }

    // =========================
    // Stop all
    // =========================
    else if (inputvalue == 'S') {
      stopCar();
      stopExtraMotor();
      Serial.println("All Stop");
    }
  }
}