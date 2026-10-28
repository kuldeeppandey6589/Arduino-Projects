// Motor slider with 2 IR sensors and L298N (without enable pin)

int ir1 = 4;    // IR sensor 1
int ir2 = 3;    // IR sensor 2
int in1 = 8;    // L298N IN1
int in2 = 9;    // L298N IN2

void setup() {
  pinMode(ir1, INPUT);
  pinMode(ir2, INPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (digitalRead(ir1) == HIGH) {
    Serial.println("IR1 HIGH → Motor CW for 3s");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    delay(1000); // run for 3 seconds
    stopMotor();
  }

  if (digitalRead(ir2) == HIGH) {
    Serial.println("IR2 HIGH → Motor CCW for 3s");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    delay(1000); // run for 3 seconds
    stopMotor();
  }
}

void stopMotor() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  Serial.println("Motor Stopped");
}




// int ir1 = 4;    // IR sensor 1
// int ir2 = 3;    // IR sensor 2
// int in1 = 8;    // L298N IN1
// int in2 = 9;    // L298N IN2

// unsigned long motorStartTime = 0;
// int motorState = 0; // 0 = stopped, 1 = CW (IR1), 2 = CCW (IR2)
// const unsigned long motorRunTime = 3000; // 3 seconds

// void setup() {
//   pinMode(ir1, INPUT);
//   pinMode(ir2, INPUT);
//   pinMode(in1, OUTPUT);
//   pinMode(in2, OUTPUT);
//   Serial.begin(9600);
//   stopMotor();
// }

// void loop() {
//   // If the motor is stopped, look for triggers
//   if (motorState == 0) {
//     if (digitalRead(ir1) == HIGH) {
//       Serial.println("IR1 HIGH → Motor CW for 3s");
//       digitalWrite(in1, HIGH);
//       digitalWrite(in2, LOW);
//       delay(1000);
//       motorState = 1;
//       motorStartTime = millis();
//     } else if (digitalRead(ir2) == HIGH) {
//       Serial.println("IR2 HIGH → Motor CCW for 3s");
//       digitalWrite(in1, LOW);
//       digitalWrite(in2, HIGH);
//       delay(1000);
//       motorState = 2;
//       motorStartTime = millis();
//     }
//   } else {
//     // If running, check if time has elapsed
//     if (millis() - motorStartTime >= motorRunTime) {
//       stopMotor();
//     }
//   }
// }

// void stopMotor() {
//   digitalWrite(in1, LOW);
//   digitalWrite(in2, LOW);
//   motorState = 0;
//   Serial.println("Motor Stopped");
// }
