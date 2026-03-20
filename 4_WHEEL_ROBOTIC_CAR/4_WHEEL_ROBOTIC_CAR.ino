/* BLUETOOTH CAR WITH ARDUINO UNO AND HC-05

   THIS CODE DEMONSTRATES CONTROLLING A CAR VIA SMARTPHONE USING BLUETOOTH COMMUNICATION.

   NOTE-IT IS ADVISED THAT BLUETOOTH MODULE SHOULD BE ATTACHED TO ARDUINO BOARD AFTER
        UPLOADING CODE TO AVOID ERRORS AND PREVENT POSSIBLE DAMAGE TO BLUETOOTH MODULE.

   CODE CREATED BY - AMEYA ANGADI
   LAST MODIFIED ON - 26/07/2024
   VERSION - 1.3
*/

#define led1 = 13
#define in1 = 12
#define in2 = 11
#define in3 = 10-
#define in4 = 9

void setup() {
  Serial.begin(9600);
  Serial.print("start");
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) {
    char inputvalue = char(Serial.read());
    if (inputvalue == 'F') {
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(10, HIGH);
      digitalWrite(9, LOW);
      Serial.println("forward");
      
    }
    else if (inputvalue == 'B') {
      digitalWrite(12, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(10, LOW);
      digitalWrite(9, HIGH);
      Serial.print("Backward");
    }

    else if (inputvalue == 'R') {
      digitalWrite(12, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(10, HIGH);
      digitalWrite(9, LOW);
    }

    else if (inputvalue == 'L') {
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(10, LOW);
      digitalWrite(9, HIGH);
    }

    else if (inputvalue == 'C') {
      digitalWrite(12, LOW);
      digitalWrite(11, HIGH);
      digitalWrite(10, HIGH);
      digitalWrite(9, LOW);
    }

    else if (inputvalue == 'A') {
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(10, LOW);
      digitalWrite(9, HIGH);
    }

    else if (inputvalue == 'O') {
      digitalWrite(13, HIGH);
    }

    else if (inputvalue == 's') {
      digitalWrite(13, LOW);
    }

    else if (inputvalue == 'S') {
      digitalWrite(12, LOW);
      digitalWrite(11, LOW);
      digitalWrite(10, LOW);
      digitalWrite(9, LOW);

    }
  }
}