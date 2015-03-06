/*
Corvette Revamp One (CR-1)

AUTHOR:
- James Brancale

CHANGELOG:
3/5/2015
- Merged CR-1 ping fwd and rev test code with CR-1 Steering IR test code
- Created methods for full vehicle functionality, full ping sensor detection
*/
// Powering a servo from the arduino usually *DOES NOT WORK*.

#define ir 5 //IR Sensor for steering; declaration
#include <Servo.h> 

//Fwd/Rev variables:
Servo drvMotors;  // create servo object to control drive motors 
int varSpeed = 0; //Variable speed variable; used in methods
int drvState = 0;
int spdDelay = 10; //loop delay for slowdown
/*
Status of the DRIVE MOTORS
0 -> stopped
1 -> Fwd movement
2 -> Rev movement
*/
//Steering variables:
Servo strMotors;
int val_new;
int val_old;
int raw;
int volt;
int cm;
int turns = 0; //variable for number of turns
int strState = 1;
int leftSpd = 40;
int rightSpd = 135;
int maxDist = 5;
/*
Status of the STEERING MOTORS
1 -> Straight
0 -> Left
2 -> Right
*/
//Ping sensor variables:
/*
Ping sensor configuration & corresponding pins:
       _1__FRONT___2_
       |            |
LEFT   3            4   RIGHT
       |            |
       |5___BACK___6|
*/
const int pingFL = 1; //Front left ping sensor pin
long timeFL, inchFL; //measured distance for ping
const int pingFR = 2; //Front right
long timeFR, inchFR;
const int pingL = 3; //Left-side
long timeL, inchL;
const int pingR = 4; //Right-side
long timeR, inchR;
const int pingRL = 1; //Rear left
long timeRL, inchRL;
const int pingRR = 2; //Rear right
long timeRR, inchRR;

void setup() {
  analogReference(DEFAULT);
  Serial.begin(9600); //begin printing in Serial monitor at 9600 frequency
  Serial.print("CR-1 Setup... ");
  drvMotors.attach(9); //connect drive motors pin
  strMotors.attach(10); //connect steering motor pin
  pinMode (ir, INPUT);
  
  raw = analogRead(ir);
  volt = map(raw, 0, 1023, 0, 6000);
  cm = (21.61/(volt-0.1696))*maxDist;
  
  val_new = cm;
  val_old = val_new;
  Serial.print("Complete!");
  Serial.println("Exiting Setup; Starting CR-1");
}

void loop() {

  while(true) { //main for loop for car's functionality
    delay(10);
    getPingData(); //receives all ping data
    
    if (inchFL < 20 && inchFR < 20 && inchRL < 20 && inchRR < 20) {
      Serial.println("ENVIRONMENT SPACE IS TOO LIMITED");
      stopMtrs();
      delay(1000);
      continue;
    }
    
    //FRONT PING CHECKS:
    if (inchFL < 20 && inchFR < 20) { //front is getting too close
      stopMtrs();
      delay(1000);
      if (inchR > 12 && inchL > 12 && inchRL > 20 && inchRR > 20) {
        center();
        reverse();
      }
    }
    else if (inchFL < 20) { 
      //object is too close to front left of car; need to back right
      stopMtrs();
      delay(1000);
      if (inchR > 12 && inchL > 6 && inchRL > 20 && inchRR > 20) {
        right();
        reverse();
      }
    }
    
    else if (inchFR < 20) { 
      //object is too close to front right of car; need to back left
      stopMtrs();
      delay(1000);
      if (inchR > 6 && inchL > 12 && inchRL > 20 && inchRR > 20) {
        left();
        reverse();
      }
    }
    
    //REAR PING CHECKS:
    if (inchRL < 20 && inchRR < 20) { //rear is getting too close
      stopMtrs();
      delay(1000);
      if (inchR > 12 && inchL > 12 && inchFL > 20 && inchFR > 20) {
        center();
        reverse();
      }
    } 
    else if (inchRL < 20) { 
      //object is too close to rear left of car; need to forward right
      stopMtrs();
      delay(1000);
      if (inchR > 12 && inchL > 6 && inchFL > 20 && inchFR > 20) {
        right();
        forward();
      }
    }
    
    else if (inchRR < 20) { 
      //object is too close to rear right of car; need to forward left
      stopMtrs();
      delay(1000);
      if (inchR > 6 && inchL > 12 && inchFL > 20 && inchFR > 20) {
        left();
        forward();
      }
    }
    
    //SIDE DISTANCE CHECKS, while moving:
    else if (inchL < 8) { //If car is getting too close on the left
      if (inchR > 12) {
        stopMtrs();
        right();
      } if (drvState == 1) {
        forward();
      } if (drvState == 2) {
        reverse();}
    }
    
    else if (inchR < 8) { //If car is getting too close on the right
      if (inchL > 12) {
        stopMtrs();
        left();
      } if (drvState == 1) {
        forward();
      } if (drvState == 2) {
        reverse();}
    }
  }
}
 
//Driving related functions below:
 
void forward() {// turns on drive motor in forward and leaves it on
  drvMotors.write(60); //write max speed to motors
  drvState = 1;
}
void reverse() {// turns on drive motor in reverse and leaves it on
  drvMotors.write(125); //write max speed to motors
  drvState = 2;
}

void stopMtrs() {//stop both motors
  drvMotors.write(0); //write 0 to stop
  drvState = 0;
}

void getPingFL() { //get distance from FL ping
      pinMode(pingFL, OUTPUT);
      digitalWrite(pingFL, LOW);
      delayMicroseconds(1);
      digitalWrite(pingFL, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingFL, LOW);
      pinMode(pingFL, INPUT);
      timeFL = pulseIn(pingFL, HIGH);
      inchFL = microsecondsToInches(timeFL);
}

void getPingFR() { //get distance from FR ping
      pinMode(pingFR, OUTPUT);
      digitalWrite(pingFR, LOW);
      delayMicroseconds(1);
      digitalWrite(pingFR, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingFR, LOW);
      pinMode(pingFR, INPUT);
      timeFR = pulseIn(pingFR, HIGH);
      inchFR = microsecondsToInches(timeFR);
}

void getPingL() { //get distance from L ping
      pinMode(pingL, OUTPUT);
      digitalWrite(pingL, LOW);
      delayMicroseconds(1);
      digitalWrite(pingL, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingL, LOW);
      pinMode(pingL, INPUT);
      timeL = pulseIn(pingL, HIGH);
      inchL = microsecondsToInches(timeL);
}

void getPingR() { //get distance from R ping
      pinMode(pingR, OUTPUT);
      digitalWrite(pingR, LOW);
      delayMicroseconds(1);
      digitalWrite(pingR, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingR, LOW);
      pinMode(pingR, INPUT);
      timeR = pulseIn(pingR, HIGH);
      inchR = microsecondsToInches(timeR);
}

void getPingRL() { //get distance from RL ping
      pinMode(pingRL, OUTPUT);
      digitalWrite(pingRL, LOW);
      delayMicroseconds(1);
      digitalWrite(pingRL, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingRL, LOW);
      pinMode(pingRL, INPUT);
      timeRL = pulseIn(pingRL, HIGH);
      inchRL = microsecondsToInches(timeRL);
}

void getPingRR() { //get distance from RR ping
      pinMode(pingRR, OUTPUT);
      digitalWrite(pingRR, LOW);
      delayMicroseconds(1);
      digitalWrite(pingRR, HIGH);
      delayMicroseconds(1);
      digitalWrite(pingRR, LOW);
      pinMode(pingRR, INPUT);
      timeRR = pulseIn(pingRR, HIGH);
      inchRR = microsecondsToInches(timeRR);
}

void getPingData() { //collects all ping data for interpretation
  Serial.println("------------"); //indicator that data has been collected
  getPingFL();
  delay(2);
  getPingFR();
  delay(2);
  getPingL();
  delay(2);
  getPingR();
  delay(2);
  getPingRL();
  delay(2);
  getPingRR();
  delay(2);
}

/* Conversion of time to inches for ping sensors
Explained here: http://learn.parallax.com/kickstart/28015
*/
long microsecondsToInches(long microseconds) {
    return microseconds / 74 / 2;
}

//Steering related functions below:

void readIRData() {
  raw = analogRead(ir);
  volt = map(raw, 0, 1023, 0, 6000);
  cm = (21.61/(volt-0.1696))*maxDist;
  val_new = cm;
}

void left() {
  Serial.print("LEFT TURN");
  turns = 0;
  if(strState == 1) { //if center, then left
    while(turns != 1) {
      readIRData();
      delay(100);
      strMotors.write(leftSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.print("CtoL TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 2) { //if right, then left
    center();
    left();
  }
  strMotors.write(0);
  strState = 0;
}

void right() {
  Serial.print("RIGHT TURN");
  turns = 0;
  if(strState == 1) { //if center, then right
    while(turns != 8) {
      readIRData();
      delay(100);
      strMotors.write(rightSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.print("CtoR TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 0) { //if left, then right
    center();
    right();
  }
  strMotors.write(0);
  strState = 2;
}

void center() {
  Serial.print("CENTER");
  turns = 0;
  if (strState == 1) {
    strMotors.write(0);
    strState = 1;
    Serial.println("STOP!!");
  } else if(strState == 2) { //if right, then center
  Serial.println("LEFT!!");
    while(turns != 2) {
      readIRData();
      delay(500);
      strMotors.write(leftSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.print("RtoC TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 0) { //if left, then center
    while(turns != 3) {
      readIRData();
      delay(500);
      strMotors.write(rightSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.print("LtoC TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  }
  strMotors.write(0);
  strState = 1;
}
