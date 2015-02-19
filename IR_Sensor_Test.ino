#define IOP 8 //This may be the IR Sensor pin - James
#define PWM 3

//Code Resource: http://forum.arduino.cc/index.php?topic=8666.0

int val_new;
int val_old;
int clicks = 0;
int turns = 0;

void setup() {
     Serial.begin(115200);
     pinMode(IOP, INPUT);

     val_new = digitalRead(IOP);
     val_old = val_new;
}

void loop() {
     analogWrite(PWM, 80);

     val_new = digitalRead(IOP);
     
     if(val_new != val_old) {
           if(clicks == 40) {
                 clicks = 1;
                 turns++;

                 Serial.print("TURNS: ");
                 Serial.println(turns);   
           }
           else clicks++;
           
           Serial.print("CLICKS: ");
           Serial.println(clicks);

           val_old = val_new;
     }
}
