//Controller Tester V1.0
//Doug Gammill 3/8/22
//Linear Labs Inc.
#include <DigiPotX9Cxxx.h> // digital potentiometer library
#include <FreqCount.h> //to count frequency on pin D47

#define speedIn A0 // manual potentiometer
#define dirHall A4 // determines direction
#define dirTrig 46 // edge trigger direction sample 
#define pGood A8 // measures controller 3.3V
#define brake1 2 // brake input
#define brake2 3 // brake input
#define manuAutoMode 4 // deactivates computer control
#define dir 5 // manual dir switch
#define key 6 // manual key switch
#define keyOut 7 // key to controller
#define dirOut 8 // direction to controller
#define brake2Out 9 // brake 2 to controller
#define brake1Out 10 // brake 1 to controller
#define speedCS 11 // digital potentiometer chip select
#define speedUD 12 // digital potentiometer up/down
#define speedINC 13 // digital potentiometer Increment

DigiPot pot(speedINC, speedUD, speedCS); // start digital potentiometer

String inputStr; // command input
bool keyVal; // key in value
bool dirVal; // dir in value
bool dirTrigVal; // dir trigger value
bool dirTrigIndex = 0; // trigger index for edge triggering
bool manuAutoModeVal; // manual switch value
bool brake2Val; // brake 2 value
bool brake1Val; // brake 1 value
bool dirSample = 0; // direction index for direction while loop
bool modeSafetyIndex = 0; // index to tell arduino not to go into manual mode until auto mode is complete
int speedInVal; // speed in potentiometer adc value
float speedInMap; // speed %
int speedInDif = 0; // used to increment digital pot
int i; // initialized at 80 in setup so digital pot can be turned to 0 on startup
float dirHallVal; // dir input value
float dirHallMap; // dir input is analog
float pGoodVal; // 3.3V read adc val
float pGoodMap; // 3.3V read actual val (5V max)
int inByte; // serial 3 read
//unsigned long prevMillis = 0; // will store last time was updated
//unsigned long currentMillis = 0; // timing functions
float freq; // freq count
float period; // freq count
float rpm; // freq count
float rps; // freq count
unsigned long rampDelay = 30; // ramp up rpm (ms/1%Throttle)
//const long interval = 1000; // interval at which to execute timing functions (milliseconds)

void setup() {
  Serial.begin(57600); // controller serial communicates at 57600
  Serial.setTimeout(20); // set to 20 (acts like delay)
  //Serial3.begin(57600); // controller serial
  //Serial3.setTimeout(20);
  i = 80; // 80 is max digi pot value
  while (i > 0) { // make sure digi pot is set to zero on startup
    pot.decrease(1);
    i--;
  }
  pinMode(key, INPUT_PULLUP);
  pinMode(dir, INPUT_PULLUP);
  pinMode(manuAutoMode, INPUT_PULLUP);
  pinMode(brake1, INPUT_PULLUP);
  pinMode(brake2, INPUT_PULLUP);
  pinMode(dirTrig, INPUT);
  pinMode(keyOut, OUTPUT);
  digitalWrite(keyOut, LOW);
  pinMode(dirOut, OUTPUT);
  digitalWrite(dirOut, LOW);
  pinMode(brake1Out, OUTPUT);
  digitalWrite(brake1Out, LOW);
  FreqCount.begin(1000); // start tachometer read in
}

void store(String input) { // serial commands
  String startStr = input.substring(0, 5); // 5 character commands

  if (startStr == "*idn?") { // request identification
    Serial.print("Controller Tester, V1.0\r\n");
  }
  else if (startStr == "dire?") { // request direction sample
    dirSample = 1;
    directionSample();
  }
  else if ((startStr == "mode?") && (modeSafetyIndex == 0)) { // request direction sample
    manuAutoModeVal = digitalRead(manuAutoMode);
    Serial.println(manuAutoModeVal);
  }
  else if (startStr == "spee?") { // request speed sample
    Serial.println(rpm);
  }
  else if (startStr == "volt?") { // request 3V3 pgood sample
    voltageSample();
  }
  else if (startStr == "print") { // request all input information and measurements
    printInput();
  }
  else if (startStr == "ramp:") { // write key switch
    if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
      rampDelay = input.substring(5).toInt();
      Serial.print("rampDelay: ");
      Serial.println(rampDelay);
    }
    else {
      Serial.println("rampDelay: fail, set rpm 0%");
    }
  }
  if (manuAutoModeVal == 1) {
    if (startStr == "keys:") { // write key switch
      if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
        keyVal = input.substring(5).toInt();
        writeKeySwitch();
        Serial.print("keys: ");
        Serial.println(keyVal);
      }
      else {
        Serial.println("keys: fail, set rpm 0%");
      }
    }
    else if (startStr == "frwd:") { // write forward direction
      if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
        dirVal = 1;
        writeDirectionSwitch();
        Serial.println("Forward");
      }
      else {
        Serial.println("frwd: fail, set rpm 0%");
      }

    }
    else if (startStr == "rvrs:") { // write reverse direction
      if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
        dirVal = 0;
        writeDirectionSwitch();
        Serial.println("Reverse");
      }
      else {
        Serial.println("rvrs: fail, set rpm 0%");
      }
    }
    else if (startStr == "spee:") { // write speed 0-100% (80 is max value)
      speedInMap = input.substring(5).toInt();
      speedInMap = speedInMap / 1.25;
      if ((speedInMap <= 80.00)) {
        Serial.print("spee: ");
        Serial.println(speedInMap * 1.25); // convert 80 to percentage (80=100%)
      }
      else {
        Serial.print("spee: Value too high");
        speedInMap = 0.00;
      }
      while (speedInMap != speedInDif) {
        writeSpeed();
      }
      if (speedInMap == 0.00) { // sets rpm back to 0 so manual mode can be reset
        rpm = 0.00;
      }
    }
    else if (startStr == "brk1:") { // write brake 1 value
      brake1Val = input.substring(5).toInt();
      writeBrake1();
      Serial.print("brk1: ");
      Serial.println(brake1Val);
    }
    else if (startStr == "brk2:") { // write brake 2 value
      brake2Val = input.substring(5).toInt();
      writeBrake2();
      Serial.print("brk2: ");
      Serial.println(brake2Val);
    }
    else if (startStr == "mode:") { // write brake 2 value
      modeSafetyIndex = input.substring(5).toInt();
      Serial.print("mode: ");
      Serial.println(modeSafetyIndex);
    }
  }
}
void writeSpeed() { // writes speed through digi pot
  if (speedInMap <= 80) {
    if (speedInMap > speedInDif) {
      pot.increase(1);
      speedInDif = speedInDif + 1;
    }
    if (speedInMap < speedInDif) {
      pot.decrease(1);
      speedInDif = speedInDif - 1;
    }
  }
  delay(rampDelay); // slows down ramp time
}
void writeOutput() {
  if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
    writeDirectionSwitch();
  }
  writeKeySwitch();
  writeBrake1();
  writeBrake2();
  writeSpeed();
}
void readInput() {
  dirVal = digitalRead(dir);
  brake1Val = digitalRead(brake1);
  brake2Val = digitalRead(brake2);
  speedInVal = analogRead(speedIn);
  speedInMap = map(speedInVal, 0, 1023, 0, 80); // full speed isnt quite 100% @ 3.3V
}

void printInput() {
  //if (currentMillis - prevMillis >= interval) { // Timed Function
  //prevMillis = currentMillis;
  if (manuAutoModeVal == 0) {
    Serial.print ("Manu, ");
  }
  else if (manuAutoModeVal == 1) {
    Serial.print ("Auto, ");
  }
  Serial.print (keyVal);
  Serial.print (", ");
  Serial.print (brake1Val);
  Serial.print (", ");
  Serial.print (brake2Val);
  Serial.print (", ");
  Serial.print (dirVal);
  Serial.print (", ");
  Serial.print (speedInMap * 1.25);
  Serial.print ("%, ");
  pGoodVal = analogRead(pGood);
  pGoodMap = map(pGoodVal, 0, 1023, 0, 515);
  pGoodMap = pGoodMap / 100;
  Serial.print(pGoodMap);
  Serial.print("V, ");
  Serial.print(rpm);
  Serial.print ("RPM, ");
  dirSample = 1;

  directionSample();
}

void writeBrake1() {
  if (brake1Val == 1) {
    digitalWrite(brake1Out, HIGH);
  }
  else {
    digitalWrite(brake1Out, LOW);
  }
}
void writeBrake2() {
  if (brake2Val == 1) {
    digitalWrite(brake2Out, HIGH);
  }
  else {
    digitalWrite(brake2Out, LOW);
  }
}
void writeKeySwitch() {
  if (keyVal == 1) {
    if ((speedInDif == 0.00) && (speedInMap == 0.00)) { // for safety
      digitalWrite(keyOut, HIGH);
    }
  }
  else {
    digitalWrite(keyOut, LOW);
  }
}
void writeDirectionSwitch() {
  if (dirVal == 1) {
    digitalWrite(dirOut, HIGH);
  }
  else {
    digitalWrite(dirOut, LOW);
  }
}

void voltageSample() {
  pGoodVal = analogRead(pGood);
  pGoodMap = map(pGoodVal, 0, 1023, 0, 515);
  pGoodMap = pGoodMap / 100;
  Serial.println(pGoodMap);
}

void speedSample() {
  if (FreqCount.available()) {
    freq = FreqCount.read();
    rps = freq / 26;
    rpm = rps * 60;
    //period = (1 / freq) * 500;
    //Serial.print(freq);
    //Serial.print("f, ");
    //Serial.print(period);
    //Serial.print("ms, ");
  }
}
void directionSample() {
  while (dirSample == 1) {
    dirTrigVal = digitalRead(dirTrig);
    if (dirTrigVal == 1) {
      dirTrigIndex = 1;
    }
    if ((dirTrigVal) == 0 && (dirTrigIndex == 1)) {
      dirHallVal = analogRead(dirHall);
      if (dirHallVal >= 600) {
        Serial.println("Forward");
      }
      else if (dirHallVal <= 400) {
        Serial.println("Reverse");
      }
      //else {
      //Serial.print("Dir Error, ");
      //Serial.println(dirHallVal);
      //}
      dirTrigIndex = 0;
      dirSample = 0;
    }

    if ((rpm == 0.00) || (speedInMap == 0.00)) {
      Serial.println("No movement");
      i = 0;
      dirTrigIndex = 0;
      dirSample = 0;
    }
  }
}
void loop() {
  //currentMillis = millis();
  keyVal = digitalRead(key);
  if ((rpm == 0.00) && (speedInMap == 0.00) && (modeSafetyIndex == 0)) {
    manuAutoModeVal = digitalRead(manuAutoMode);
  }
  if (manuAutoModeVal == 0) {
    inputStr = Serial.readStringUntil('\n'); //serial input function
    store(inputStr); //serial input function
    readInput();
    speedSample();
    //printInput(); // prints all samples periodically
    writeOutput();
  }
  if (manuAutoModeVal == 1) {
    inputStr = Serial.readStringUntil('\n'); //serial input function
    store(inputStr); //serial input function
    speedSample();
  }
  //inByte = Serial3.read();
  //Serial.write(inByte);
}
