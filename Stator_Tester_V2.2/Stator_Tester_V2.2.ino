//STATOR TESTER 2.0

#include <AccelStepper.h>                         //stepper library that is good at controlling accelleration and runs a bit smoother than others
#define stepPinX 3                                //Driver X pusle pin
#define dirPinX 2                                 //Driver X direction pin
#define en_X 4                                    //Driver X enable pin
#define stepPinY 6                                //Driver Y pusle pin
#define dirPinY 5                                 //Driver Y direction pin
#define en_Y 7                                    //Driver Y enable pin
#define SwX 12                                    //X limit switch pin
#define SwY 13                                    //Y limit switch pin
#define motorInterfaceType 1                      //interface type for the accel stepper library, works with TB6600 stepper drivers​
#define relay1 11                                 //pins to relays, active low turns them on
#define relay2 10
#define relay3 9
#define relay4 8
#define relay5 A3
#define relay6 A2
#define relay7 A1
#define relay8 A0

AccelStepper stepperX = AccelStepper(motorInterfaceType, stepPinX, dirPinX);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepPinY, dirPinY);

int sensorA5;                               //ID resistor variable
int ID;                                     //ID resistor ADC value (A5)
int sensorA4;                               //hall sensor (A4)
int hall;                                   //hall sensor variable
int valSwX;                                 //variable for X limit switch
int valSwY;                                 //variable for y limit switch
int valX;                                   //variable for X enable and LED
int valY;                                   //variable for Y enable and LED
unsigned long millisK;                      //timer for 500ms sampling
int magNum = 0;                             //variable for number of magnets a rotor has
int xSteps;                                 //variable for number of steps between magnets in that rotor
int ySteps = 0;                             //variable for moving sensor along y axis, initial set so it can home on startup
int flagX1 = 0;                             //Flags for homing X on the leading edge of X limit switch
int flagX2 = 0;                             //Flags for homing X on the leading edge of X limit switch
int i = 0;                                  //used in runXab and runXcb for while loops
int j = 0;                                  //used in runXab and runXcb for while loops
int k = 0;                                  //used in runXab and runXcb for while loops
int h = 0;                                  //used in runXab and runXcb for while loops h = magNum / (abSteps + cbSteps);
char startCond;                             // Start condition ":" to initiate comms
String inputStr;
char startChar;
boolean magNumCollected = false;            //flags for when all 4 variables are recieved from python program or user it initiates testing
boolean yStepsCollected = false;
boolean cbStepsCollected = false;
boolean abStepsCollected = false;
boolean poleOffsetCollected = false;
boolean stepOffsetCollected = false;
int abSteps = 0;                            //(A to B pole variable) example: if stator pattern is AabBCc  aa=4
int cbSteps = 0;                            //(C pole variable) example: if stator pattern is AabBCc  ca=2
int poleOffset = 0;
int stepOffset = 0;
int totOffset = 0;
boolean syncDone;                           //used to sync comms
boolean abRelays = false;                   //bool variable used to turn on A to B phase LCR test relays 2,3, and 5
boolean bcRelays = false;                   //bool variable used to turn on B to C phase LCR test relays
boolean acRelays = false;                   //bool variable used to turn on A to C phase LCR test relays
boolean hpRelays = false;                   //bool variable used to turn on Hi-pot phase LCR test relays
boolean pwrRelayA = false;                  //bool variable used to power coils phase A to B  for calibration only
boolean pwrRelayB = false;                  //bool variable used to power coils phase A to B  for calibration only

void setup() {
  pinMode (en_X, OUTPUT);                   //X Driver enable and LED
  digitalWrite(en_X, HIGH);                 //TB6600 driver enable needs pull up resistor and high on start up otherwise it draws 2Amps each
  pinMode (en_Y, OUTPUT);                   //X Driver enable and LED
  digitalWrite(en_Y, HIGH);                 //TB6600 driver enable needs pull up resistor and high on start up otherwise it draws 2Amps each
  pinMode(SwX, INPUT);                      //X limit Switch
  pinMode(SwY, INPUT);                      //Y limit Switch
  stepperX.setCurrentPosition(0);
  stepperX.setAcceleration(1000000);
  stepperX.setMaxSpeed(1000);
  stepperY.setCurrentPosition(0);
  stepperY.setAcceleration(1000000);
  stepperY.setMaxSpeed(1000);
  pinMode (relay1, OUTPUT);                 //outputs to relays, active low turns them on, initially set high
  pinMode (relay2, OUTPUT);
  pinMode (relay3, OUTPUT);
  pinMode (relay4, OUTPUT);
  pinMode (relay5, OUTPUT);
  pinMode (relay6, OUTPUT);
  pinMode (relay7, OUTPUT);
  pinMode (relay8, OUTPUT);
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);
  digitalWrite(relay5, HIGH);
  digitalWrite(relay6, HIGH);
  digitalWrite(relay7, HIGH);
  digitalWrite(relay8, HIGH);
  Serial.begin(115200);
  Serial.setTimeout(50);                                        //set to 50 because it adds too much delay to relay responce
  millisK = millis() + 500;                                     //millisK for timed if statements, can bug out after while i++ loops, so copy this to an exit of a while loop
}

void HALL() {                                                   //Analog read of hall sensor located on back iron
  sensorA4 = analogRead(A4);
  hall = map(sensorA4, 0, 1023, -508, 514);                     //+ numbers are south poles and - numbers are north poles
  //  if (hall < -2) {                                              //printing polarity is optional
  //    Serial.print("N");
  //  }
  //  if (hall > 4) {                                               //printing polarity is optional
  //    Serial.print("S");
  //  }
  if ((hall > 4) || (hall < -2)) {
    Serial.println(hall);                                         //prints ADC value of hall sensor, + numbers are south poles and - numbers are north poles
  }
  else {
    Serial.println("0");                                        //numbers between -2 and +4 represent NO magnetic field detected, so -2 to +4 = 0
  }
}
void ident() {
  sensorA5 = analogRead(A5);                                    //Identification resistor located on back iron
  ID = map(sensorA5, 0, 1023, 0, 100);                          //a value of 100 means back iron isnt installed or isnt seated correctly
  Serial.print("ID, ");
  Serial.println(ID);                                           //prints ADC value of ID resistor (voltage divider) PC will determine what ADC value belongs to what back iron
}
void homeX() {
  flagX1 = 1;                                                   //Flags for homing X on the leading edge of X limit switch
  flagX2 = 1;                                                   //Flags for homing X on the leading edge of X limit switch
  digitalWrite(en_X, LOW);
  while ((flagX1 == 1) || (flagX2 == 1)) {
    valSwX = digitalRead(SwX);                                    //read X limit switch
    valX = digitalRead(en_X);                                     //read X driver enable
    if ((valSwX == HIGH) && (flagX1 == 1) && (flagX2 == 1)) {
      stepperX.setCurrentPosition(0);                             //this if function moves x out of limit switch actuation to make sure it starts on leading edge
      Serial.println("homingX,");
      stepperX.runToNewPosition(500);                             //360 is about how many steps X limit switch is actuated for
      stepperX.setCurrentPosition(0);
      valSwX = digitalRead(SwX);                                     //read X limit switch
      while ((valSwX == HIGH) && (flagX1 == 1) && (flagX2 == 1)) {   //fails if stepper doesnt kick back all the way out of limit switch, reset required
        if (millis() > millisK) {                                    //countdown timer. try to keep things in if statements to reduce processing power
          millisK = millis() + 500;
          Serial.println("failedHomingX,");
        }
      }
    }
    if ((valSwX == LOW) && (flagX1 == 1) && (flagX2 == 1)) {      //if limit switch is acuated set X1 flag false to move out of limit
      flagX1 = 0;
    }
    if ((valSwX == LOW) && (flagX1 == 0) && (flagX2 == 1)) {       //when or if limit switch is not actuated find its leading edge
      stepperX.moveTo(-14800);                                     //make steps positive if you want to go opposite way
      stepperX.run();
      if (millis() > millisK) {                                    //countdown timer. try to keep things in if statements to reduce processing power
        millisK = millis() + 500;
        if (stepperX.isRunning()) {                                 //non blocking function, checks to see if arduino is still pulsing steppers
          Serial.println("homingX,");
        }
        else {
          Serial.println("failedHomingX,");                      //if arduino is done pulsing stepper but hasnt reached limit switch then failure happens
        }
      }
    }
    if ((valSwX == HIGH) && (flagX1 == 0) && (flagX2 == 1)) {      //if X limit switch is reach and FlagX1 is already set to off (ensures leading edge of limit switch is found.)
      flagX2 = 0;                                                 //exit while loop by turning final X limit flag off
    }
  }
}
void runXab() {
  homeX();
  i = 0;
  h = magNum / (abSteps + cbSteps);                 //h = loop number variable
  xSteps = -14400 / magNum;                         //stator needs to go counter clockwise for test which is negative steps
  offset();
  digitalWrite (relay1, LOW);                       //needs to be 1,2,4,5  to safely turn on
  delay(50);                                        //50ms was chosen to ensure relay contact bouncing wont create any shorts between input devices
  digitalWrite (relay2, LOW);
  delay(50);
  digitalWrite (relay3, LOW);
  delay(50);

  while (i < h) {                                     //example: h = AabBCc which would be one i loop
    while (j < abSteps) {                             //A to B flux test. example: AabB
      Serial.print("runAB, ");
      HALL();
      stepperX.setCurrentPosition(0);                //ensure stepper position is set to 0 before moving
      stepperX.runToNewPosition(xSteps);             //14400steps/48magnets=300steps/magnet(blocking function is used so when hall is sampled it wont sometimes drop out due to interupts)
      j++;                                           //loop determined by aa serial input example: aa4
    }
    while (k < cbSteps) {                            //C to B flux test, hall sampling  is skipped during runXab() to keep relay actuation to a minimum
      //HALL();
      //Serial.println("Skip");
      stepperX.setCurrentPosition(0);                //not using run() funtion because hall sampling sometimes drops out
      stepperX.runToNewPosition(xSteps);             //14400steps/48magnets=300steps/magnet(blocking function is used so when hall is sampled it wont sometimes drop out due to interupts)
      k++;
    }

    k = 0;                                          //k and j get reset back to zero before i loops again
    j = 0;
    i++;
  }
  digitalWrite (en_X, HIGH);                         //enable X
  i = 0;                                           //all veriables reset before exiting runXab() function
  j = 0;
  k = 0;
  h = 0;
  digitalWrite (relay3, HIGH);                  //needs to be turned off in order 5,4,2,1, opposite of on order
  delay(50);
  digitalWrite (relay2, HIGH);
  delay(50);
  digitalWrite (relay1, HIGH);
  delay(50);
  //millisK for timed functions, can bug out after while i++ loops, so copy this to an exit of a while loop
}
void runXcb() {
  homeX();
  h = magNum / (abSteps + cbSteps);               //h = loop number variable
  xSteps = -14400 / magNum;                       //stator needs to go counter clockwise for test which is negative steps
  stepperX.setCurrentPosition(0);                //offset
  stepperX.runToNewPosition(totOffset);
  digitalWrite (relay3, LOW);                     //needs to be in order 4,5,6,7  to safely turn on
  delay(50);                                      //50ms was chosen to ensure relay contact bouncing wont create any shorts between input devices
  digitalWrite (relay6, LOW);
  delay(50);
  digitalWrite (relay7, LOW);
  delay(50);
  while (i < h) {                                     //example: h = AabBCc which would be one i loop
    while (j < abSteps) {                             //A to B flux test, hall sampling  is skipped during runXab() to keep relay actuation to a minimum
      //HALL();
      //Serial.println("Skip");
      stepperX.setCurrentPosition(0);                //ensure stepper position is set to 0 before moving
      stepperX.runToNewPosition(xSteps);             //14400steps/48magnets=300steps/magnet(blocking function is used so when hall is sampled it wont sometimes drop out due to interupts)
      j++;
    }
    while (k < cbSteps) {                            //C to B flux test example: Cc (Bb poles do not need to be retested because they were already tested in runXab())
      Serial.print("runBC, ");
      HALL();

      stepperX.setCurrentPosition(0);                //ensure stepper position is set to 0 before moving
      stepperX.runToNewPosition(xSteps);             //14400steps/48magnets=300steps/magnet(blocking function is used so when hall is sampled it wont sometimes drop out due to interupts)
      k++;
    }

    k = 0;                                           //k and j get reset back to zero before i loops again
    j = 0;
    i++;
  }
  digitalWrite(en_X, HIGH);
  i = 0;                                            //all veriables reset before exiting runXcb() function
  j = 0;
  k = 0;
  h = 0;
  homeX();
  digitalWrite (en_X, HIGH);
  digitalWrite (relay7, HIGH);                      //needs to be 7,6,5,4  to safely turn off, opposite of on order
  delay(50);
  digitalWrite (relay6, HIGH);
  delay(50);
  digitalWrite (relay3, HIGH);
  delay(50);
  //millisK for timed functions, can bug out after while i++ loops, so copy this to an exit of a while loop
}

void runY() {
  digitalWrite(en_Y, LOW);                        //enable for Driver Y
  //valY = digitalRead(en_Y);                       //enable variable for Driver Y
  //while (valY == LOW) {                           //while stepper Y is enabled (active low)
  Serial.println("setupY,");
  stepperY.runToNewPosition(ySteps);             //make steps positive if you want to go opposite way
  stepperY.setCurrentPosition(0);
  digitalWrite(en_Y, HIGH);                   //set driver Y enable Low
  //valY = digitalRead(en_Y);                   //read en_y to exit while loop
}

//}

void homeY() {
  valSwY = digitalRead(SwY);                        //Y limit switch variable
  if (valSwY == LOW) {                              //if its low than move to high position
    stepperY.setCurrentPosition(0);                 //make sure stepper Y is set to position 0  before moving
    digitalWrite(en_Y, LOW);                        //if limit switch is not actuatey set driver Y enable low
    valY = digitalRead(en_Y);                       //reads driver Y en
    while (valY == LOW) {                           //enter while loop until driver Y enable goes high
      stepperY.moveTo(-8500);                       //negative makes it go toward limit switch (8500 is about how far it can travel at microstepping x4)
      stepperY.run();                               //not a blocking function

      if (millis() > millisK) {                     //countdown timer. try to keep things in if statements to reduce processing power
        millisK = millis() + 500;
        if (stepperY.isRunning()) {                 //checks to make sure arduino is still sending out pulses every 500ms
          Serial.println("homingY,");
        }
        else {
          Serial.println("failedHomingY,");        //error detection if stepper didnt make it all the way to limit switch
        }
      }
      valSwY = digitalRead(SwY);                    //Driver Y enable Y gets writen High if limit Sw is reached
      if (valSwY == HIGH) {
        digitalWrite(en_Y, HIGH);                   //Driver Y enable high state will exit while loop
      }

      valY = digitalRead(en_Y);                     //read Driver enable Y to exit While loop
    }

    stepperY.setCurrentPosition(0);                 //on exit of while loop set stepper position back to 0
  }
}
void runFluxTest() {
  if ((magNumCollected && yStepsCollected && abStepsCollected && cbStepsCollected && poleOffsetCollected && stepOffsetCollected)) { //these 4 variables are needed to start flux test, set true when mn, ya, aa, and ca are entered
    runY();                                                                             //moves Y to entered stepsY (example: ya4300)
    runXab();                                                                           //loops number of AB poles entered
    runXcb();                                                                           //loops number of C poles entered
    magNumCollected = false;                                                            //sets 4 variables back to false so new mn, ya, aa, and ca variables can be entered
    yStepsCollected = false;
    abStepsCollected = false;
    cbStepsCollected = false;
    poleOffsetCollected = false;
    stepOffsetCollected = false;
    i = 0;
  }
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "ablr:") {                           //serial input A to B LCR relays on/off
    abRelays = input.substring(5).toInt();
  }
  else if (startStr == "bclr:") {                      //serial input B to C LCR relays on/off
    bcRelays = input.substring(5).toInt();
  }
  else if (startStr == "aclr:") {                      //serial input A to C LCR relays on/off
    acRelays = input.substring(5).toInt();
  }
  else if (startStr == "hpot:") {                      //serial input hi pot test relays on/off
    hpRelays = input.substring(5).toInt();
  }
  else if (startStr == "tstA:") {                      //serial input hi pot test relays on/off
    pwrRelayA = input.substring(5).toInt();
  }
  else if (startStr == "tstB:") {                      //serial input hi pot test relays on/off
    pwrRelayB = input.substring(5).toInt();
  }
  else if (startStr == "mnum:") {                      //serial input stator magnet number
    magNum = input.substring(5).toInt();
    magNumCollected = true;
    Serial.print("MagNum, ");
    Serial.println(magNum);
  }
  else if (startStr == "yaxi:") {                      //serial input Y steps number
    ySteps = input.substring(5).toInt();
    if (ySteps < 8500) {                            //if y is microstepping x 4 then 8500 steps is as far as it can travel before hitting rail
      yStepsCollected = true;
      Serial.print("Yaxis, ");
      Serial.println(ySteps);
    }
    else {                                          //if more than 8500 steps is entered then error occurs and variables are auto reset
      ySteps = 0;
      yStepsCollected = false;
      Serial.println("x Too many Y steps entered, limit is 8500.");
    }
  }
  else if (startStr == "apol:") {                      //serial input variable, phase a to b flux test
    abSteps = input.substring(5).toInt();           //flux test needs mn, ya, aa, and ca to function
    abStepsCollected = true;                        //if stator looks like AabBCc aAbBCc with 48 poles then mn=48 aa=4 ca=2
    Serial.print("ABpoles, ");
    Serial.println(abSteps);
  }
  else if (startStr == "cpol:") {                      //serial input variable, phase c to b flux test
    cbSteps = input.substring(5).toInt();
    Serial.print("Cpoles, ");
    Serial.println(cbSteps);
    cbStepsCollected = true;
  }
  else if (startStr == "poff:") {                      //serial input stator magnet number
    poleOffset = input.substring(5).toInt();
    poleOffsetCollected = true;
    Serial.print("poleOffset, ");
    Serial.println(poleOffset);
  }
  else if (startStr == "soff:") {                      //serial input stator magnet number
    stepOffset = input.substring(5).toInt();
    stepOffsetCollected = true;
    Serial.print("stepOffset, ");
    Serial.println(stepOffset);
  }
  else if (startStr == "?") {                       //HELP COMMAND
    Serial.println();
    Serial.println("ablr:1, ablr:0 toggles a to b LCR test relays");
    Serial.println("aclr:1, aclr:0 toggles a to c LCR test relays");
    Serial.println("bclr:1, bclr:0 toggles b to c LCR test relays");
    Serial.println("hpot:1, hpot:0 toggles hi-pot test relays");
    Serial.println("mnum: Magnet Number, enter mnum:48 for 48 stator poles");
    Serial.println("yaxi: Y axis, enter ya1000 for 1000 steps, 8500 max");
    Serial.println("apol: a to b pole number");
    Serial.println("cpol: c pole number");
    Serial.println("mnum: yaxi: apol: and cpol: need to be entered to start flux test");
    Serial.println("Phase pattern example: AabBCc aABbcC...");
    Serial.println("AabB or aABb account for a to b test and Cc or cC account for c to b test.");
    Serial.println("so apol:4 and cpol:2");
    Serial.println("*idn? returns you PN SN and Ver.");
    Serial.println("biid: returns Back Iron ID#.");
    Serial.println("hall: returns a sample from the hall sensor.");
    Serial.println();
  }
  else if (startStr == "*idn?") {
    Serial.print("stator tester, test001, V2.0\r\n");
  }
  else if (startStr == "biid:") {
    ident();
  }
  else if (startStr == "hall:") {
    HALL();
  }
}
void abLRtest() {
  i = 1;
  while (abRelays == 1) {                           //turns on 2,3,5 relays for a to b LCR test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay2, LOW);                     //needs to be in order 2,3,5 to safely turn on
    delay(50);
    digitalWrite (relay5, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("A-B LCR, ");
      Serial.println(abRelays);
      i = 0;
    }
    if (abRelays == 0) {
      digitalWrite (relay5, HIGH);                  //needs to be 5,3,2 to safely turn off
      delay(50);
      digitalWrite (relay2, HIGH);
      delay(50);
      acRelays = 0;
      bcRelays = 0;
      hpRelays = 0;
      i = 0;
      Serial.print("A-B LCR, ");
      Serial.println(abRelays);
      //timed function, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}
void acLRtest() {
  i = 1;
  while (acRelays == 1) {                           //turns on 2,7 relays for a to c LCR test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay2, LOW);                     //any turn on or off order is safe
    delay(50);
    digitalWrite (relay7, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("A-C LCR, ");
      Serial.println(acRelays);
      i = 0;
    }
    if (acRelays == 0) {
      digitalWrite (relay7, HIGH);
      delay(50);
      digitalWrite (relay2, HIGH);
      delay(50);
      abRelays = 0;
      bcRelays = 0;
      hpRelays = 0;
      i = 0;
      Serial.print("A-C LCR, ");
      Serial.println(acRelays);
      //timed if statements, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}
void bcLRtest() {
  i = 1;
  while (bcRelays == 1) {                             //turns on 5,7 relays for b to c LCR test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay4, LOW);                       //any turn on or off order is safe
    delay(50);
    digitalWrite (relay5, LOW);
    delay(50);
    digitalWrite (relay7, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("B-C LCR, ");
      Serial.println(bcRelays);
      i = 0;
    }
    if (bcRelays == 0) {
      digitalWrite (relay7, HIGH);
      delay(50);
      digitalWrite (relay5, HIGH);
      delay(50);
      digitalWrite (relay4, HIGH);                       //any turn on or off order is safe
      delay(50);
      abRelays = 0;
      acRelays = 0;
      hpRelays = 0;
      i = 0;
      Serial.print("B-C LCR, ");
      Serial.println(bcRelays);
      //timed function, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}
void hpTest() {
  i = 1;
  while (hpRelays == 1) {                             //turns on 2,8 relays for hi pot test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay2, LOW);                       //any turn on or off order is safe
    delay(50);
    digitalWrite (relay8, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("HiPot, ");
      Serial.println(hpRelays);
      i = 0;
    }
    if (hpRelays == 0) {
      digitalWrite (relay2, HIGH);
      delay(50);
      digitalWrite (relay8, HIGH);
      delay(50);
      abRelays = 0;
      acRelays = 0;
      bcRelays = 0;
      i = 0;
      Serial.print("HiPot, ");
      Serial.println(hpRelays);
      //timed function, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}

void abPwrTest() {
  i = 1;
  while (pwrRelayA == 1) {                             //turns on 1,2,3 relays for power supply current calibration test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay1, LOW);                       //safe on order = 123
    delay(50);
    digitalWrite (relay2, LOW);
    delay(50);
    digitalWrite (relay3, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("abPower, ");
      Serial.println(pwrRelayA);
      i = 0;
    }
    if (pwrRelayA == 0) {
      digitalWrite (relay3, HIGH);
      delay(50);
      digitalWrite (relay2, HIGH);
      delay(50);
      digitalWrite (relay1, HIGH);
      delay(50);
      abRelays = 0;
      acRelays = 0;
      bcRelays = 0;
      hpRelays = 0;
      i = 0;
      Serial.print("abPower, ");
      Serial.println(pwrRelayA);
      //timed function, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}

void bcPwrTest() {
  i = 1;
  while (pwrRelayB == 1) {                             //turns on 3,6,7 relays for power supply current calibration test
    inputStr = Serial.readStringUntil('\n');
    store(inputStr);
    digitalWrite (relay3, LOW);                       //safe on order = 367
    delay(50);
    digitalWrite (relay6, LOW);
    delay(50);
    digitalWrite (relay7, LOW);
    delay(50);
    if (i > 0) {
      Serial.print("bcPower, ");
      Serial.println(pwrRelayB);
      i = 0;
    }
    if (pwrRelayB == 0) {
      digitalWrite (relay7, HIGH);
      delay(50);
      digitalWrite (relay6, HIGH);
      delay(50);
      digitalWrite (relay3, HIGH);
      delay(50);
      abRelays = 0;
      acRelays = 0;
      bcRelays = 0;
      hpRelays = 0;
      i = 0;
      Serial.print("bcPower, ");
      Serial.println(pwrRelayB);
      //timed function, can bug out after while loops, so copy this to an exit of a while loop
      break;
    }
  }
}

void offset() {
  poleOffset = xSteps * poleOffset;
  totOffset = poleOffset + stepOffset;
  stepperX.setCurrentPosition(0);                //offset
  stepperX.runToNewPosition(totOffset);
}
void loop() {                                         //main loop
  inputStr = Serial.readStringUntil('\n');            //serial input function
  store(inputStr);                                    //serial input function
  abLRtest();                                         //phase A to B LCR test
  acLRtest();                                         //phase A to C LCR test
  bcLRtest();                                         //phase B to C LCR test
  hpTest();                                           //HiPot test, needs to be done after LCR tests to ensure no opens are present in phases of stator.
  abPwrTest();                                        //calibration test for measuring current to phase A to B coils
  bcPwrTest();                                        //calibration test for measuring current to phase B to C coils
  homeY();                                            //By default, Y will return home.
  runFluxTest();                                      //runs magnet pole flux test by turning relays on to power phases A+ to B- with 10ADC, and then C+ to B- with 10ADC
}
