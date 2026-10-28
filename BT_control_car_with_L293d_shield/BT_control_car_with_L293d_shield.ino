
#include <Servo.h>
int d;
int trigPin = 11; //Trig - green Jumper
int echoPin = 9; //Echo - yellow Jumper
long duration, cm, inches;


Servo myservo;  // create servo object to control a servo



// twelve servo objects can be created on most boards
int pos1=0;
int pos = 0;    // variable to store the servo position


void setup() {
Serial.begin(115200);
  delay(10);
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
myservo.attach(8);  // attaches the servo on pin 9 to the servo object
}

void loop() {
digitalWrite(trigPin, LOW);
delayMicroseconds(5);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
pinMode(echoPin, INPUT);
duration = pulseIn(echoPin, HIGH);
// convert the time into a distance
//Serial.println("cm");
//Serial.println();

cm = (duration/2) / 29.1;
d=cm;
inches = (duration/2) / 74;
Serial.print(inches);
Serial.println("person ");
Serial.println(d);
Serial.print("cm");
Serial.println();
delay(150);
if(d<=30)
{
  pos=270;
 // for (pos = -80; pos <= 80; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos); 
    // tell servo to go to position in variable 'pos'
    Serial.println("Person distance");
    Serial.println(d);
    
  }

if(d>=30)
{
  pos1=-80;

  //for (pos = 80; pos >= -80; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos1);              // tell servo to go to position in variable 'pos'
    Serial.println("person distance");
    Serial.println(d);
}
}
