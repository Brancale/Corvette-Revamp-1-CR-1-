//Corvette Revamp One (CR-1)
/*
AUTHOR
- James Brancale

CHANGELOG
- Single ping sensor support
- fwd & rev movement
- int to indicate vehicle state instead of individual booleans
*/
// Powering a servo from the arduino usually *DOES NOT WORK*.

String readString;
#include <Servo.h> 
Servo drvMotors;  // create servo object to control drive motors 
const int pingPin_C = 6;//sets the pin for the PING sensor in the center side of the car
long duration_C, inches_C; //measured distance in inches for ping
int varSpeed = 0; //Variable speed variable; used in methods
int drvState = 0;
/*
Defines the status of the robot's movement for the DRIVE MOTORS
0 --> first time running; prevents motor glitch when not full spd
1 --> stopped
2 --> Fwd movement
3 --> Rev movement
*/
int spdDelay = 10; //loop delay for slowdown

void setup() {
  Serial.begin(9600); //begin printing in Serial monitor at 9600 frequency
  drvMotors.attach(9); //connect drive motor pin
  Serial.println("CR-1 Setup Initiated"); // so I can keep track of what is loaded
}

void loop() {
  //SERVO TEST FROM SERIAL INPUT -- Use for Debug ONLY
  /*while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the String readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }

  if (readString.length() >0) {
    Serial.println(readString);  //so you can see the captured String 
    int n = readString.toInt();  //convert readString into a number
    drvMotors.write(n);
    readString="";
  } */ //SERVO TEST FROM SERIAL INPUT -- Use for Debug ONLY

  while(true) { //main for loop for car's functionality
    getping_all(); //receives all ping data

    //all clear, no obstacles within 12" in any direction
    if(inches_C > 20) { //Will stop if object is > 1 ft. away
      drvState = 1;
      forward(); //stops car
    }
    if(inches_C >= 15 && inches_C < 20) { //if dist. < 1 ft., move fwd
      /*if (drvState == 3) {
        revSlowdown(); //slows down motor from rev to prevent wear
      } else if (drvState == 0 || drvState == 1) {
        forwardStart(); //starts motor fwd movement
      }*/
      if(drvState == 3) {
        Serial.print("FWD STATE STOP");
        stop();
        delay(1000);
        Serial.print("FWD STATE STAHP");
      }
      drvState = 2;
      forward(); //move fwd
      Serial.print("Forward movement");
    }
    if(inches_C < 15) { //if dist. < 6 inches, move rev
      /*if (drvState == 2) {
        fwdSlowdown(); //slows down motor from fwd to prevent wear
      } else if (drvState == 0 || drvState == 1) {
        reverseStart(); //starts motor rev movement
      }*/
      if(drvState == 2) {
        Serial.print("REV STATE STOP");
        stop();
        delay(1000);
        Serial.print("REV STATE STAHP");
      }
      drvState = 3;
      reverse(); //move rev
      Serial.print("Reverse movement");
    }
  }
}
 
void forward()
{// turns on drive motor in forward and leaves it on
     drvMotors.write(60); //write max speed to motors 
}
void reverse()//
{// turns on drive motor in reverse and leaves it on
     drvMotors.write(125); //write max speed to motors
}

void stop()//stop both motors
{
     drvMotors.write(0); //write 0 to stop
}

void fwdSlowdown() //slows down forward motion and begins reverse
{
  if (drvState == 1) { //loop to slow down speed
    Serial.print("Forward SLOWDOWN");
    for(varSpeed; varSpeed < 81; varSpeed++) {
      drvMotors.write(varSpeed);
      delay(spdDelay/4);
    }
  }
  stop(); //stop for temporary amount of time
  delay(1000);
  Serial.print("Reverse SPEEDUP");
  reverseStart();
}

void reverseStart() { //speeds up motors going reverse
  for(varSpeed=105; varSpeed < 139; varSpeed++) {
    drvMotors.write(varSpeed);
    delay(spdDelay);
  }
}

void revSlowdown() //slows down forward motion and begins reverse
{
  if (drvState == 1) { //loop to slow down speed
    Serial.print("Reverse SLOWDOWN");
    for(varSpeed; varSpeed > 105; varSpeed--) {
      drvMotors.write(varSpeed);
      delay(spdDelay);
    }
  }
  stop(); //stop for temporary amount of time
  delay(1000);
  Serial.print("Forward SPEEDUP");
  forwardStart();
}

void forwardStart() { //speeds up motors going forward
  for(varSpeed=80; varSpeed > 45; varSpeed--) {
    drvMotors.write(varSpeed);
    delay(spdDelay);
  }
}

 void getping_C()//get distance from center PING
{
      pinMode(pingPin_C, OUTPUT);
      digitalWrite(pingPin_C, LOW);
      delayMicroseconds(1);
      digitalWrite(pingPin_C, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingPin_C, LOW);
      pinMode(pingPin_C, INPUT);
      duration_C = pulseIn(pingPin_C, HIGH);
      inches_C = microsecondsToInches(duration_C);
}

  void getping_all()
{
  getping_C();
  delay(1);
  /*getping_R();
  delay(2);
  getping_L();
  delay(2);*/
}
  long microsecondsToInches(long microseconds){
    return microseconds / 74 / 2;}

/* The above converts the time of the return to inches as explained here: http://learn.parallax.com/kickstart/28015
*/
