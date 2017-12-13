/*
Corvette Revamp One (CR-1)

AUTHOR:
- Corvette Revamp One (CR-1) Team

Should you decide to use this code for your project, please credit the CR-1 Team
of the Mechatronics Research Lab.

Please note that all distances listed in this program are in inches.
*/

#define ir 5 //IR Sensor for steering; declaration
#include <Servo.h> 

//General System variables
boolean loopFrst = true;
int prgmState = 0;
/* Program states:
0 - ENV SPACE LIM.
1 - FL, FR < 20
2 - FL < 20
3 - FR < 20
4 - RL, RR < 20
5 - RL < 20
6 - RR < 20
7 - L < 20
8 - R < 20
*/
//Fwd/Rev variables:
Servo drvMotors;  // create servo object to control drive motors 
int varSpeed = 0; //Variable speed variable; used in methods
int drvState = 0;
int spdDelay = 10; //loop delay for slowdown
int revSpeed = 115;
int fwdSpeed = 70;
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
int strState = 0;
int leftSpd =135;
int rightSpd = 40;
int maxDist = 5;
int sensorDelay = 150;
int strDelay = 500;
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
const int pingFL = 2; //Front left ping sensor pin
const int pingFR = 3; //Front right
const int pingL = 4; //Left-side
const int pingR = 5; //Right-side
const int pingRL = 6; //Rear left
const int pingRR = 7; //Rear right
long timeFL, inchFL, timeFR, inchFR, timeL, inchL, timeR, inchR, timeRL, inchRL, timeRR, inchRR; //measured distance for ping

void setup() {
  Serial.begin(115200); //begin printing in Serial monitor at 115200 frequency
  analogReference(DEFAULT);
  Serial.print("CR-1 Setup... ");
  drvMotors.attach(12); //connect drive motors pin
  strMotors.attach(13); //connect steering motor pin
  pinMode (ir, INPUT);
  
  raw = analogRead(ir);
  volt = map(raw, 0, 1023, 0, 6000);
  cm = (21.61/(volt-0.1696))*maxDist;
  
  val_new = cm;
  val_old = val_new;
  Serial.print("Complete!");
  Serial.println();
  Serial.println("Exiting Setup; Starting CR-1");
}

void loop() {


    getPingData(); //receives all ping data
    Serial.print("inchFL: ");
    Serial.print(inchFL);
    Serial.println();
    Serial.print("inchFR: ");
    Serial.print(inchFR);
    Serial.println();
    Serial.print("inchL: ");
    Serial.print(inchL);
    Serial.println();
    Serial.print("inchR: ");
    Serial.print(inchR);
    Serial.println();
    Serial.print("inchRL: ");
    Serial.print(inchRL);
    Serial.println();
    Serial.print("inchRR: ");
    Serial.print(inchRR);
    Serial.println();
    
    if (loopFrst) {
      forward();
      loopFrst = false;
    }
    
    if (inchFL < 20 && inchFR < 20 && inchRL < 20 && inchRR < 20) { //state 0
      Serial.println("ENVIRONMENT SPACE IS TOO LIMITED");
      stopMtrs();

    }
    
    //FRONT PING CHECKS:
    if (inchFL < 20 && inchFR < 20) { //front is getting too close; state 1
      if (prgmState != 1) {
        prgmState = 1;
        if (drvState != 2) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 12 && inchL > 12 && inchRL > 20 && inchRR > 20) {
          center();
          reverse();
        }
      }
      
    }
    else if (inchFL < 20) { 
      //object is too close to front left of car; need to back right; state 2
      if (prgmState != 2) {
        prgmState = 2;
        if (drvState != 2) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 12 && inchL > 6 && inchRL > 20 && inchRR > 20) {
          right();
          reverse();
        }
      }
      
    }
    
    else if (inchFR < 20) { 
      //object is too close to front right of car; need to back left; state 3
      if (prgmState != 3) {
        prgmState = 3;
        if (drvState != 2) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 6 && inchL > 12 && inchRL > 20 && inchRR > 20) {
          left();
          reverse();
        }
      }
      
    }
    
    //REAR PING CHECKS:
    if (inchRL < 20 && inchRR < 20) { //rear is getting too close; state 4
      if (prgmState != 4) {
        prgmState = 4;
        if (drvState != 1) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 12 && inchL > 12 && inchFL > 20 && inchFR > 20) {
          center();
          forward();
        }
      }
      
    } 
    else if (inchRL < 20) { 
      //object is too close to rear left of car; need to forward right; state 5
      if (prgmState != 5) {
        prgmState = 5;
        if (drvState != 1) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 12 && inchL > 6 && inchFL > 20 && inchFR > 20) {
          right();
          forward();
        }
      }
      
    }
    
    else if (inchRR < 20) { 
      //object is too close to rear right of car; need to forward left; state 6
      if (prgmState != 6) {
        prgmState = 6;
        if (drvState != 1) {
          stopMtrs();
          delay(1000);
        }
        if (inchR > 6 && inchL > 12 && inchFL > 20 && inchFR > 20) {
          left();
          forward();
        }
      }
      
    }
    
    //SIDE DISTANCE CHECKS, while moving:
    else if (inchL < 15) { //If car is getting too close on the left; state 7
      if (prgmState != 7) {
        prgmState = 7;
        if (inchR > 12) {
          drvMotors.write(0);
          right();
          delay(1000);
        } if (drvState == 1) {
          forward();
        } else if (drvState == 2) {
          reverse();}
      }
      
    }
    
    else if (inchR < 15) { //If car is getting too close on the right; state 8
      if (prgmState != 8) {
        prgmState = 8;
        if (inchL > 12) {
          drvMotors.write(0);
          left();
          delay(1000);
        } if (drvState == 1) {
          forward();
        } else if (drvState == 2) {
          reverse();}
      }
      
    }
  delay(100);
}
 
//Driving related functions below:
 
void reverse() {// turns on drive motor in forward and leaves it on
  drvMotors.write(revSpeed); //write fwd speed to motors
  drvState = 2;
}
void forward() {// turns on drive motor in reverse and leaves it on
  drvMotors.write(fwdSpeed); //write rev speed to motors
  drvState = 1;
}

void stopMtrs() {//stop both motors
  drvMotors.write(0); //write 0 to stop
  drvState = 0;
}

void getPingFL() { //get distance from FL ping
      pinMode(pingFL, OUTPUT);
      digitalWrite(pingFL, LOW);
      delayMicroseconds(2);
      digitalWrite(pingFL, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingFL, LOW);
      pinMode(pingFL, INPUT);
      timeFL = pulseIn(pingFL, HIGH);
      inchFL = microsecondsToInches(timeFL);
}

void getPingFR() { //get distance from FR ping
      pinMode(pingFR, OUTPUT);
      digitalWrite(pingFR, LOW);
      delayMicroseconds(2);
      digitalWrite(pingFR, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingFR, LOW);
      pinMode(pingFR, INPUT);
      timeFR = pulseIn(pingFR, HIGH);
      inchFR = microsecondsToInches(timeFR);
}

void getPingL() { //get distance from L ping
      pinMode(pingL, OUTPUT);
      digitalWrite(pingL, LOW);
      delayMicroseconds(2);
      digitalWrite(pingL, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingL, LOW);
      pinMode(pingL, INPUT);
      timeL = pulseIn(pingL, HIGH);
      inchL = microsecondsToInches(timeL);
}

void getPingR() { //get distance from R ping
      pinMode(pingR, OUTPUT);
      digitalWrite(pingR, LOW);
      delayMicroseconds(2);
      digitalWrite(pingR, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingR, LOW);
      pinMode(pingR, INPUT);
      timeR = pulseIn(pingR, HIGH);
      inchR = microsecondsToInches(timeR);
}

void getPingRL() { //get distance from RL ping
      pinMode(pingRL, OUTPUT);
      digitalWrite(pingRL, LOW);
      delayMicroseconds(2);
      digitalWrite(pingRL, HIGH);
      delayMicroseconds(5);
      digitalWrite(pingRL, LOW);
      pinMode(pingRL, INPUT);
      timeRL = pulseIn(pingRL, HIGH);
      inchRL = microsecondsToInches(timeRL);
}

void getPingRR() { //get distance from RR ping
      pinMode(pingRR, OUTPUT);
      digitalWrite(pingRR, LOW);
      delayMicroseconds(2);
      digitalWrite(pingRR, HIGH);
      delayMicroseconds(5);
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
  Serial.println("LEFT TURN");
  turns = 0;
  if(strState == 1) { //if center, then left
    while(turns < 1) {
      readIRData();
      delay(sensorDelay);
      strMotors.write(leftSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.println("CtoL TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 2) { //if right, then left
    turns = -1;
    center();
    left();
  }
  strMotors.write(leftSpd);
  delay(850);
  strMotors.write(0);
  strState = 0;
}

void right() {
  Serial.println("RIGHT TURN");
  turns = 0;
  if(strState == 1) { //if center, then right
    delay(strDelay);
    strMotors.write(rightSpd);
    while(turns < 4) {
      readIRData();
      delay(sensorDelay);
      strMotors.write(rightSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.println("CtoR TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 0) { //if left, then right
    delay(strDelay);
    strMotors.write(rightSpd);
    center();
    right();
  }
  strMotors.write(0);
  strState = 2;
}

void center() {
  Serial.println("CENTER");
  turns = 0;
  if (strState == 1) {
    strMotors.write(0);
    strState = 1;
    //Serial.println("STOP!!");
  } else if(strState == 2) { //if right, then center
  //Serial.println("LEFT!!");
    delay(strDelay);
    strMotors.write(leftSpd);
    while(turns < 2) {
      readIRData();
      delay(sensorDelay);
      strMotors.write(leftSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.println("RtoC TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  } else if(strState == 0) { //if left, then center
    delay(100);
    strMotors.write(rightSpd);
    while(turns < 1) {
      readIRData();
      delay(sensorDelay);
      strMotors.write(rightSpd);
      if((val_new > maxDist && val_old < maxDist) || (val_new < maxDist 
      && val_old > maxDist)) {
        turns++;
        Serial.println("LtoC TURNS: ");
        Serial.println(turns);
      }
      val_old = val_new;
    }
  }
  strMotors.write(0);
  strState = 1;
}
