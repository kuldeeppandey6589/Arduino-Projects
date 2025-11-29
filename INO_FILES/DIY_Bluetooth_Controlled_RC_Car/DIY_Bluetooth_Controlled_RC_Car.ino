// const int motor1 = 9;
// const int motor2 = 10;
// const int motor3 = 11;
// const int motor4 = 12;
// char command;


// void setup() {
//   // put your setup code here, to run once:
//   //Serial.begin(9600);
//   pinMode(motor1, OUTPUT);
//   pinMode(motor2, OUTPUT);
//   pinMode(motor3, OUTPUT);
//   pinMode(motor4, OUTPUT);

// }

// void loop() {             // put your main code here, to run repeatedly:
 
//   if (Serial.available() > 0) 
//   {
//     command = Serial.read();

//     if (command == "F") {
//       digitalWrite(motor1, HIGH);
//       digitalWrite(motor2, LOW);
//       digitalWrite(motor3, HIGH);
//       digitalWrite(motor4, LOW);
//     }

//     if (command == "B") {
//       digitalWrite(motor1, LOW);
//       digitalWrite(motor2, HIGH);
//       digitalWrite(motor3, LOW);
//       digitalWrite(motor4, HIGH);
//     }

//     if (command == "L") {
//       digitalWrite(motor1, HIGH);
//       digitalWrite(motor2, LOW);
//       digitalWrite(motor3, LOW);
//       digitalWrite(motor4, LOW);
//     }

//     if (command == "R") {
//       digitalWrite(motor1, LOW);
//       digitalWrite(motor2, LOW);
//       digitalWrite(motor3, HIGH);
//       digitalWrite(motor4, LOW);
//     }

//     if (command == "S") {
//       digitalWrite(motor1, LOW);
//       digitalWrite(motor2, LOW);
//       digitalWrite(motor3, LOW);
//       digitalWrite(motor4, LOW);
//     }
//   }
// }



// Motor pin definitions
const int ENA = 6;
const int IN1 = 8;
const int IN2 = 9;
const int ENB = 7;
const int IN3 = 10;
const int IN4 = 11;

char command = 'S'; // Default to Stop

void setup() {
  Serial.begin(9600); // HC-05 default baud rate
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  if (Serial.available()) {
    command = Serial.read();
  }

  switch (command) {
    case 'F':
      moveForward();
      break;
    case 'B':
      moveBackward();
      break;
    case 'L':
      turnLeft();
      break;
    case 'R':
      turnRight();
      break;
    case 'S':
      stopCar();
      break;
  }
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 200);
  analogWrite(ENB, 200);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 150);
  analogWrite(ENB, 150);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
