// voice control home automation

int L1 = 11;
int L2 = 12;
int L3 = 13;

String voice = "";

void setup() {
  Serial.begin(9600);

  pinMode(L1, OUTPUT);
  pinMode(L2, OUTPUT);
  pinMode(L3, OUTPUT);

  // Turn all OFF at start (change HIGH/LOW if your relay is active LOW)
  digitalWrite(L1, HIGH);
  digitalWrite(L2, HIGH);
  digitalWrite(L3, HIGH);
}

void loop() {

  if (Serial.available() > 0) {
    voice = Serial.readString();
    voice.trim();          // remove \n and spaces
    Serial.println(voice); // print correctly
  }

  // Relay 1
  if (voice == "L1 on") {
    digitalWrite(L1, LOW);   // active LOW relay
  } 
  else if (voice == "L1 of") {
    digitalWrite(L1, HIGH);
  }

  // Relay 2
  else if (voice == "L2 on") {
    digitalWrite(L2, LOW);
  } 
  else if (voice == "L2 of") {
    digitalWrite(L2, HIGH);
  }

  // Relay 3
  else if (voice == "L3 on") {
    digitalWrite(L3, LOW);
  } 
  else if (voice == "L3 of") {
    digitalWrite(L3, HIGH);
  }

  voice = "";  // clear after execution
}