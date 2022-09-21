//#include <MemoryFree.h>                     //checks free memory
#include <AccelStepper.h>
#include <LiquidCrystal.h>


#define dirPin 3
#define stepPin 2
#define motorInterfaceType 1
#define but_1 4
#define en_1 24
#define en_LED 25

AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
LiquidCrystal lcd(43, 42, 41, 40, 39, 38);

int sensorReading1;
float f;
int sensorReading;
float motorSpeed;
float steps;
float t;
int sensorReading2;
int a;
long spd;
long tme;
unsigned long millisK;
unsigned int seconds;
unsigned int minutes;
int val1;

void setup() {
  //Serial.print("a");                //foot print, mark these over code to tell where things crash
  pinMode (but_1, INPUT);
  pinMode (en_1, OUTPUT);
  digitalWrite(en_1, HIGH);
  pinMode (en_LED, OUTPUT);
  digitalWrite(en_LED, LOW);
  
  lcd.begin(16, 2);                   //LCD one time set up
  lcd.setCursor(0, 0);
  lcd.print("RPM:");
  lcd.setCursor(11, 0);
  lcd.print("A:");
  lcd.setCursor(0, 1);
  lcd.print("RunTime:");
  
  lcd.setCursor(10, 1);
  lcd.print("min");
  
  stepper.setCurrentPosition(0);
  //Serial.begin(57600);

}

void loop() {

  sensorReading1 = analogRead(A1);                     //TIME
  f = map(sensorReading1, 0, 1023, 0, 17472);          //max steps   (194steps/min * 90min = 17472)
  sensorReading = analogRead(A0);                      //RPM
  motorSpeed = map(sensorReading, 0, 1023, 0, 944);   //1002=531RPM     1350=715RPM    (RPM * 1.887 = value)
  steps = motorSpeed / 3.34 * f;                       //194f about equals 1 minute so 8685 lasts 45 minutes
  t = steps / motorSpeed / 3.34 / 17.29;
  sensorReading2 = analogRead(A2);                    //Accel
  a = map(sensorReading2, 0, 1023, 0, 99);
  stepper.setAcceleration(a);


  lcd.setCursor(4, 0);                            //LCD RPM
  spd = motorSpeed / 3.34 * 1.77;
  if (spd < 10) {
    lcd.print("0");
  }
  if (spd < 100) {
    lcd.print("0");
  }
  lcd.print(spd);

  lcd.setCursor(13, 0);                         //LCD Acceleration
  if (a < 10) {
    lcd.print("0");
  }
  lcd.print(a);

  tme = f / 194;                                //LCD run time
  lcd.setCursor(8, 1);
  if (tme < 10) {
    lcd.print("0");
  }
  lcd.print(tme);

  val1 = digitalRead(but_1);

  if (val1 == LOW) {
    digitalWrite(en_1, HIGH);
    digitalWrite(en_LED, LOW);
    stepper.setCurrentPosition(0);
  }

  if (val1 == HIGH) {
    stepper.setCurrentPosition(0);
    digitalWrite(en_1, LOW);
    digitalWrite(en_LED, HIGH);
    stepper.setAcceleration(a);
    stepper.setMaxSpeed(motorSpeed);

    stepper.moveTo(steps);                //make steps negative if you want to go opposite way
    stepper.run();

    millisK = millis() + 1000;


    while (stepper.isRunning()) {

      stepper.run();

      if (millis() > millisK) {         //countdown timer. try to keep things in if statements to reduce processing power
        seconds += 1;
        millisK = millisK + 1000;
      }
      if (seconds == 60) {
        seconds = 0;
        minutes += 1;
        lcd.setCursor(8, 1);
        if ((tme - minutes) < 10) {
          lcd.print("0");
        }
        lcd.print(tme - minutes);
        //Serial.print(tme - minutes);                //Serial print
        //Serial.print("mins");
        //Serial.print("\r\n");                      //Return
        //Serial.print("freeMemory()=");           //Frememory library prints how much ram is left
        //Serial.println(freeMemory());

      }
    }
  }
}
