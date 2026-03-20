// // -------- ADXL335 Pins --------
// #define X_PIN A0
// #define Y_PIN A1

// // -------- L298 Motor Pins --------
// #define IN1 8
// #define IN2 9
// #define IN3 10
// #define IN4 11
// #define ENA 5
// #define ENB 6

// // Threshold values (adjust if needed)
// int center = 512;
// int range  = 100;

// void setup() {
//   pinMode(IN1, OUTPUT);
//   pinMode(IN2, OUTPUT);
//   pinMode(IN3, OUTPUT);
//   pinMode(IN4, OUTPUT);
//   pinMode(ENA, OUTPUT);
//   pinMode(ENB, OUTPUT);

//   Serial.begin(9600);

//   analogWrite(ENA, 180); // speed
//   analogWrite(ENB, 180);
// }

// void loop() {

//   int xVal = analogRead(X_PIN);
//   int yVal = analogRead(Y_PIN);

//   Serial.print("X: ");
//   Serial.print(xVal);
//   Serial.print("  Y: ");
//   Serial.println(yVal);

//   // -------- Forward --------
//   if (yVal > center + range) {
//     forward();
//   }
//   // -------- Backward --------
//   else if (yVal < center - range) {
//     backward();
//   }
//   // -------- Right --------
//   else if (xVal > center + range) {
//     rightTurn();
//   }
//   // -------- Left --------
//   else if (xVal < center - range) {
//     leftTurn();
//   }
//   // -------- Stop --------
//   else {
//     stopMotors();
//   }

//   delay(100);
// }

// // -------- Motor Functions --------

// void forward() {
//   digitalWrite(IN1, HIGH);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
// }

// void backward() {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, HIGH);
//   digitalWrite(IN3, LOW);7890-`1

//   digitalWrite(IN4, HIGH);
// }

// void leftTurn() {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, HIGH);
//   digitalWrite(IN4, LOW);
// }

// void rightTurn() {
//   digitalWrite(IN1, HIGH);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);
// }

// void stopMotors() {
//   digitalWrite(IN1, LOW);
//   digitalWrite(IN2, LOW);
//   digitalWrite(IN3, LOW);
//   digitalWrite(IN4, LOW);
// }



 


// -------- ADXL335 --------
#define Xpin A0
#define Ypin A1

// -------- L298N --------
#define IN1 8
#define IN2 9
#define IN3 10
#define IN4 11

// ---- CALIBRATION VALUES (CHANGE IF NEEDED) ----
int Xcenter = 335;
int Ycenter = 340;
int deadZone = 20;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  int x = analogRead(Xpin);
  int y = analogRead(Ypin);

  Serial.print("X:");
  Serial.print(x);
  Serial.print(" Y:");
  Serial.println(y);

  // -------- FORWARD --------
  if (y > Ycenter + deadZone) {
    forward();
  }
  // -------- BACKWARD --------
  else if (y < Ycenter - deadZone) {
    backward();
  }
  // -------- RIGHT --------
  else if (x > Xcenter + deadZone) {
    right();
  }
  // -------- LEFT --------
  else if (x < Xcenter - deadZone) { 
    left();
  }
  // -------- STOP --------
  else {
    stopCar();
  }

  delay(100);
}

// -------- MOTOR FUNCTIONS --------
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
