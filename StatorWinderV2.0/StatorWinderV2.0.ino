//Stator Winder, Arduino MEGA, V2.0
//Doug Gammill 12/29/22
//Linear Labs Inc.
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#define stepPinX 48 // Driver X pusle pin
#define dirPinX 47 // Driver X direction pin
#define en_X 46
#define footSwitch 50
#define motorInterfaceType 1 // interface type for the accel stepper library, works with TB6600 stepper drivers​
#define stepPinY 28 // Driver Y pusle pin
#define dirPinY 29 // Driver Y direction pin
#define en_Y 30
#define manual_SwY 51
#define DT 52
#define CLK 53
#define startA 8
#define startB 9
#define startC 10
#define up 12
#define dn 11
#define Estop 13

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
//const char* idn = "Stator Winder, Arduino MEGA, V2.0";
bool footSwitch_Val;
bool footSwitch_Index;
bool startAVal;
bool phaseIndex;
bool indexA;
bool startBVal;
bool indexB;
bool startCVal;
bool indexC;
bool upVal;
bool dnVal;
bool eStopVal;
bool eStopIndex;
//const long interval = 200; // interval at which to execute timing functions (milliseconds)
//unsigned long prevMillis = 0; // will store last time was updated
//unsigned long currentMillis = 0; // timing functions
//String inputStr; // serial input commands
int rotationY;  // encoder knop starting position
int valueY; // encoder knob value
int rotationX;  // encoder knop starting position
int valueX; // encoder knob value
bool LeftRight; // encoder knob CCW or CW
int currentPositionY; // whole number of steps to move 1 pole = 20
int positionX = 0;
int xHalfRotation = 260; // number of steps it takes for x to swing around 180 degrees
float carry1; // carry to add up whole number for y steps
int indexY;
int i;
int turns; // serial input variable total number of turns per pole
bool clockWise; // serial input variable
float firstPole; // serial output what step were on 0.5 increments
float secondPole; // serial output what step were on 0.5 increments
int poleNum; // example: a1, a2, a3, a4
int poleNumNot; // example: A1, A2, A3, A4
String startOK; // a, B, or c
String poleLetter; // a, B, or c
String poleLetterNot; // A, b, or C
bool manual_SwY_Val; // encoder knob value
bool en_manY_State; // encoder knob enable state
bool lastButtonStateY;
bool manual_SwY_State; // the current reading from the input pin
unsigned long lastDebounceTimeY = 0; // the last time the output pin was toggled
unsigned long debounceDelayY = 50; // the debounce time; increase if the output flickers
byte Check[] = {
  B00000,
  B00001,
  B00011,
  B10110,
  B11100,
  B01000,
  B00000,
  B00000
};

AccelStepper stepperX = AccelStepper(motorInterfaceType, stepPinX, dirPinX);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepPinY, dirPinY);

void setup() {
  pinMode (en_X, OUTPUT); // X Driver enable and LED
  digitalWrite(en_X, HIGH); // en_X_State is variable high or low
  pinMode(footSwitch, INPUT);
  pinMode(startA, INPUT_PULLUP);
  pinMode(startB, INPUT_PULLUP);
  pinMode(startC, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(dn, INPUT_PULLUP);
  pinMode(Estop, INPUT_PULLUP);
  pinMode(manual_SwY, INPUT_PULLUP);
  stepperX.setAcceleration(500);
  stepperX.setMaxSpeed(250);
  stepperY.setAcceleration(2000);
  stepperY.setMaxSpeed(2000);
  pinMode (en_Y, OUTPUT); // Y Driver enable and LED
  digitalWrite(en_Y, HIGH); // en_Y_State is variable high or low
  pinMode (CLK, INPUT);
  pinMode (DT, INPUT);
  Serial.begin(115200);
  Serial.setTimeout(20); // set to 50 because it adds too much delay to relay
  lcd.begin(16, 2); // LCD one time setup
  lcd.createChar(0, Check);
  Serial.println("Enter help? for command list");
  resetTurns();
}

void resetTurns() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Linear Labs Inc.");
  Serial.println("reset, 1");
  footSwitch_Index = 0;
  phaseIndex = 0;
  indexA = 0; // a start
  indexB = 0; // B start
  indexC = 0; // c start
  eStopIndex = 0;
  currentPositionY = 0; // whole number of steps to move 1 pole = 20
  carry1 = 0.00; // carry to add up whole number for y steps
  turns = 0; // serial input variable total number of turns per pole
  clockWise = true; // start clockWise
  firstPole = 0.5; // serial output what step were on 0.5 increments
  secondPole = 0.5; // serial output what step were on 0.5 increments
  poleNum = 1; // example: a1, a2, a3, a4
  poleNumNot = 1; // example: A1, A2, A3, A4
  startOK = "";
  poleLetter = "";
  poleLetterNot = "";
  en_manY_State = false;
  lastButtonStateY = LOW;
  stepperX.setCurrentPosition(0);
  stepperY.setCurrentPosition(0);
  currentPositionY = 0;
  carry1 = 0.0;
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Pole: ");
  lcd.setCursor(8, 0);
  lcd.print("Total: ");
  lcd.setCursor(14, 0);
  lcd.print(turns);
  lcd.setCursor(0, 1);
  lcd.print("Turn: ");
  if (clockWise == true) {
    lcd.setCursor(5, 0);
    lcd.print(poleLetter);
    lcd.setCursor(6, 0);
    lcd.print(poleNum);
    lcd.setCursor(5, 1);
    lcd.print(firstPole);
  }
  else {
    lcd.setCursor(5, 0);
    lcd.print(poleLetterNot);
    lcd.setCursor(6, 0);
    if (poleNumNot < 9) {
      lcd.print(poleNumNot);
      lcd.setCursor(5, 1);
      lcd.print(secondPole);
    }
    else {
      lcd.print("");
      lcd.setCursor(5, 1);
      lcd.print("");
    }
  }
}
void potentiometerValue() {
  if (en_manY_State == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Offset: Engaged!");
    stepperX.setCurrentPosition(0);
    stepperY.setCurrentPosition(0);
    currentPositionY = 0;
    positionX = 0;
    carry1 = 0.0;
  }
  while (en_manY_State == true) {
    //checkEstop();
    upVal = digitalRead(up);
    if (upVal == 1) {
      debounceManSw();
      valueY = digitalRead(CLK);
      if (valueY != rotationY) { // Use the DT pin to find out which way were turning.
        if (digitalRead(DT) != valueY) {  // Clockwise
          currentPositionY = currentPositionY + 4;
          LeftRight = true;
        } else { //Counterclockwise
          LeftRight = false;
          currentPositionY = currentPositionY - 4;
        }
      }
      rotationY = valueY;
      stepperY.moveTo(currentPositionY);
      stepperY.run();
      if (en_manY_State == false) {
        stepperX.setCurrentPosition(0);
        stepperY.setCurrentPosition(0);
        currentPositionY = 0;
        positionX = 0;
        updateLCD();
      }
    }
    if (upVal == 0) {
      valueX = digitalRead(CLK);
      if (valueX != rotationX) { // Use the DT pin to find out which way were turning.
        if (digitalRead(DT) != valueX) {  // Clockwise
          positionX = positionX + 4;
          LeftRight = true;
        } else { //Counterclockwise
          LeftRight = false;
          positionX =  positionX - 4;
        }
      }
      rotationX = valueX;
      stepperX.moveTo(positionX);
      stepperX.run();
    }
  }
}
void checkEstop() {
  eStopVal = digitalRead(Estop);
  if (eStopVal == 0) {
    eStopIndex = 1;
    Serial.println("ESTOP, 1");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Estop: Engaged!!");
    en_manY_State = false;
  }
  while (eStopIndex == 1) {
    eStopVal = digitalRead(Estop);
    digitalWrite(en_X, LOW);
    digitalWrite(en_Y, LOW);
    if (eStopVal == 1) {
      Serial.println("ESTOP, 0");
      updateLCD();
      digitalWrite(en_X, HIGH);
      digitalWrite(en_Y, HIGH);
      eStopIndex = 0;
    }
  }
}
void debounceManSw() {
  manual_SwY_Val = digitalRead(manual_SwY);
  if (manual_SwY_Val != lastButtonStateY) {
    lastDebounceTimeY = millis(); // reset the debouncing timer
  }
  if ((millis() - lastDebounceTimeY) > debounceDelayY) {
    if (manual_SwY_Val != manual_SwY_State) {
      manual_SwY_State = manual_SwY_Val;
      if (manual_SwY_State == LOW) {
        en_manY_State = !en_manY_State;
        Serial.print("enYstate, ");
        Serial.println(en_manY_State);
      }
    }
  }
  lastButtonStateY = manual_SwY_Val;
}
void upDnButton() {
  upVal = digitalRead(up);
  dnVal = digitalRead(dn);
  if (startOK == "") {
    if ((upVal == 0) && (turns <= 40)) {
      turns++;
      if (turns > 40) {
        turns = 1;
      }
      Serial.print("Turn, ");
      Serial.println(turns);
      updateLCD();
      delay(100);
    }

    if ((dnVal == 0) && (turns >= 1)) {
      turns--;
      if (turns < 1) {
        turns = 40;
      }
      Serial.print("Turn, ");
      Serial.println(turns);
      updateLCD();
      delay(100);
    }
  }
  while ((upVal == 0) && (dnVal == 0)) {
    resetTurns();
    delay(50);
    upVal = digitalRead(up);
    dnVal = digitalRead(dn);
    if ((upVal == 1) && (dnVal == 1)) {
      break;
    }
  }
}
void startButton() {
  if (startOK == "") {
    startAVal = digitalRead(startA);
    startBVal = digitalRead(startB);
    startCVal = digitalRead(startC);
    if ((startAVal == 0) && (indexA == 0)) {
      indexA = 1;
      startOK = "a";
      startWinding();
    }
    else if ((startAVal == 1) && (indexA == 1)) {
      indexA = 0;
    }
    else if ((startBVal == 0) && (indexB == 0)) {
      indexB = 1;
      startOK = "B";
      startWinding();
    }
    else if ((startBVal == 1) && (indexB == 1)) {
      indexB = 0;
    }
    else if ((startCVal == 0) && (indexC == 0)) {
      indexC = 1;
      startOK = "c";
      startWinding();
    }
    else if ((startCVal == 1) && (indexC == 1)) {
      indexC = 0;
    }
  }
}
//void store(String input) {
//  String startStr = input.substring(0, 5);
//  if (startStr == "turn:") {
//    if (startOK == "") {
//      turns = input.substring(5).toInt();
//    }
//    Serial.print("Turn, ");
//    Serial.println(turns);
//    updateLCD();
//  }
//  else if (startStr == "strt:") { // serial input start variable
//    if (startOK == "") {
//      startOK = input.substring(5);
//      startOK.remove(1, 2); // remove "\r"
//      startWinding();
//    }
//    else  {
//      Serial.println("reset, 0");
//    }
//  }
//  else if (startStr == "reset") { // reset everything
//    resetTurns();
//  }
//  else if (startStr == "help?") { // serial input help variable
//    Serial.println("turn: enter this plus number of turns per pole");
//    Serial.println("strt: enter this plus letter of the phase you are winding");
//    Serial.println("reset to reset all values");
//    Serial.println("*idn? for unit ID");
//  }
//  else if (startStr == "*idn?") { // serial input ID variable
//    Serial.println(idn);
//  }
//}
void startWinding() { // run cycle
  firstPole = 0.5;
  secondPole = 0.0;
  clockWise = true;
  carry1 = 0; // on a new job set carry to 0
  currentPositionY = 0;
  poleNum = 1; // example: a1, a2, a3, a4
  poleNumNot = 1; // example: A1, A2, A3, A4
  if ((startOK == "a") || (startOK == "A")) {
    poleLetter = "a";
    poleLetterNot = "A";
  }
  else if ((startOK == "b") || (startOK == "B")) {
    poleLetter = "B";
    poleLetterNot = "b";
  }
  else if ((startOK == "c") || (startOK == "C")) {
    poleLetter = "c";
    poleLetterNot = "C";
  }
  else {
    Serial.println("Pole, wrong pole");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pole:");
    lcd.setCursor(5, 0);
    lcd.print("wrong pole");
    poleLetter = "";
    poleLetterNot = "";
    startOK = "";
  }
  if (startOK != "") {
    if ((turns <= 0) || (turns >= 41)) {
      Serial.println("Turn, 0");
      poleLetter = "";
      poleLetterNot = "";
      startOK = "";
      turns = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pole: ");
      lcd.setCursor(5, 0);
      lcd.print(poleLetterNot);
      lcd.setCursor(6, 0);
      lcd.print(poleNumNot);
      lcd.setCursor(8, 0);
      lcd.print("Total:");
      lcd.setCursor(14, 0);
      lcd.print(turns);
      lcd.setCursor(0, 1);
      lcd.print("Turn:");
      lcd.setCursor(5, 1);
      lcd.print("Set Turns!");
    }
  }
}
void moveY() { // can only move whole number of steps need to use carry; 1 pole = 20.83 steps
  i = 0;
  while (i != indexY) {
    checkEstop();
//    carry1 = carry1 + 0.83; // number of carried steps = 0.83
//    if (carry1 >= 1) {
//      currentPositionY = currentPositionY + 1;
//      carry1 = carry1 - 1;
//    }
    currentPositionY = currentPositionY + 125; // 125 steps = 1 pole
    stepperY.runToNewPosition(currentPositionY);
    i++;
  }
  lcd.setCursor(10, 1);
  lcd.write("     ");
}
void printPole() {
  Serial.print(poleLetter);
  Serial.print(poleNum);
  Serial.print(", ");
  Serial.println(firstPole);
}
void printPoleNot() {
  Serial.print(poleLetterNot);
  Serial.print(poleNumNot);
  Serial.print(", ");
  Serial.println(secondPole);
}
void FootSwac() { // A or C phase
  footSwitch_Val =   digitalRead(footSwitch);
  // start off a, or c with a half turn clockWise
  if ((footSwitch_Val == 0) && (footSwitch_Index == 0) && (clockWise == true) && (firstPole == 0.5) && (poleNum == 1)) { // do not move y
    firstPole = firstPole + 0.5;
    printPole();
    stepperX.setCurrentPosition(0);
    stepperX.runToNewPosition(+xHalfRotation);
    updateLCD();
  }
  // only Foot switch activated
  else if ((footSwitch_Val == 1) && (footSwitch_Index == 0)) {
    footSwitch_Index = 1; // foot switch will only engage once when pressed down
    // a1, c1 turns clockwise
    if ((clockWise == true) && (firstPole != turns) && (poleNum == 1)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A1, or C1 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 1)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A1, or C1 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 1)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a2, or c2 turns single CCW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 1)) { // move y
      poleNumNot = 2;
      poleNum = 2;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a2, or c2 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 2)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A2, or C2 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 2)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A2, or C2 turns
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 2)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start A3, or C3 with a single CCW turn
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 2)) { // move y long
      poleNumNot = 3;
      poleNum = 3;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A3, or C3 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 3)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a3,or c3 turns with single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 3)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a3, or c3 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 3)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A4,or C4 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 3)) { // move y
      poleNumNot = 4;
      poleNum = 4;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A4, or C4 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 4)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a4, or c4 with single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 4)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a4, or c4 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 4)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start a5, or c5 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 4)) { // move y long
      poleNumNot = 5;
      poleNum = 5;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a5, c5 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 5)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A5, or C5 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 5)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A5, or C5 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 5)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a6, or c6 turns with single CCW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 5)) { // move y
      poleNumNot = 6;
      poleNum = 6;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a6, or c6 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 6)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A6, or C6 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 6)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A6, or C6 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 6)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start A7, or C7 with a single CCW turn
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 6)) { // move y long
      poleNumNot = 7;
      poleNum = 7;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A7, or C7 turns CCW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 7)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a7, or c7 turns single CCW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 7)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a7, or c7 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 7)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start A8, or C8 with a single clock wise turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 7)) { // move y
      poleNumNot = 8;
      poleNum = 8;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // A8, or C8 turns
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 8)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start a8, or c8 turns single CCW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 8)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // a8, or c8 turns CW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 8)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    //Finish step,  a8 move to B phase winding or c8 move back to begining
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 8)) { // move y to B to finish
      Serial.println("Pass, 1");
      poleNum = 9;
      poleNumNot = 9;
      if (startOK == "a") {
        indexY = 13; // moveY 13 poles to start phaseB
      }
      else if (startOK == "c") {
        indexY = 1; // moveY 1 pole to move back to phaseA
      }
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      lcd.setCursor(13, 1);
      lcd.write(byte(0));
      lcd.write(byte(0));
      lcd.write(byte(0));
    }
  }
  else if ((footSwitch_Val == 0) && (footSwitch_Index == 1)) {
    footSwitch_Index = 0; // foot switch will only engage once when pressed down
  }
}

void FootSwB() { // phase B
  footSwitch_Val =   digitalRead(footSwitch);
  // start off B with a half turn CCW
  if ((footSwitch_Val == 0) && (footSwitch_Index == 0) && (clockWise == true) && (firstPole == 0.5) && (poleNum == 1)) { // do not move y
    firstPole = firstPole + 0.5;
    printPole();
    stepperX.setCurrentPosition(0);
    stepperX.runToNewPosition(-xHalfRotation);
    updateLCD();
  }
  // only Foot switch activated
  else if ((footSwitch_Val == 1) && (footSwitch_Index == 0)) {
    footSwitch_Index = 1; // foot switch will only engage once when pressed down
    // B1 turns CCW
    if ((clockWise == true) && (firstPole != turns) && (poleNum == 1)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b1 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 1)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b1 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 1)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start B2 turns single CCW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 1)) { // move y
      poleNumNot = 2;
      poleNum = 2;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B2 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 2)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b2 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 2)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b2 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 2)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start b3 with a single CW turn
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 2)) { // move y long
      poleNumNot = 3;
      poleNum = 3;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b3 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 3)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start B3 turns with single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 3)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B3 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 3)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b4 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 3)) { // move y
      poleNumNot = 4;
      poleNum = 4;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b4 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 4)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start B4 with single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 4)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B4 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 4)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start B5 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 4)) { // move y long
      poleNumNot = 5;
      poleNum = 5;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B5 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 5)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b5 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 5)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b5 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 5)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start B6 turns with single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 5)) { // move y
      poleNumNot = 6;
      poleNum = 6;
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B6 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 6)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();;
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b6 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 6)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b6 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 6)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start b7 with a single CW turn
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 6)) { // move y long
      poleNumNot = 7;
      poleNum = 7;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 9; // moveY 9 poles
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b7 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 7)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // B7 turn single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 7)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B7 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 7)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // start b8 with a single CW turn
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 7)) { // move y
      poleNumNot = 8;
      poleNum = 8;
      firstPole = 0.5;
      secondPole = 0.5;
      printPoleNot();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = false; // change to clockwise false so update LCD knows which phase were on
      updateLCD();
    }
    // b8 turns CW
    else if ((clockWise == false) && (secondPole != turns) && (poleNumNot == 8)) { // do not move y
      secondPole = secondPole + 0.5;
      printPoleNot();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      updateLCD();
    }
    // start B8 turns single CW
    else if ((clockWise == false) && (secondPole == turns) && (poleNumNot == 8)) { // move y
      firstPole = 0.5;
      secondPole = 0.5;
      printPole();
      indexY = 1; // moveY 1 pole
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      clockWise = true; // change to clockwise so update LCD knows which phase were on
      updateLCD();
    }
    // B8 turns CCW
    else if ((clockWise == true) && (firstPole != turns) && (poleNum == 8)) { // do not move y
      firstPole = firstPole + 0.5;
      printPole();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(-xHalfRotation);
      updateLCD();
    }
    // B8 Finish step, move to C phase winding
    else if ((clockWise == true) && (firstPole == turns) && (poleNum == 8)) { // move y to B to finish
      Serial.println("Pass, 1");
      poleNum = 9;
      poleNumNot = 9;
      indexY = 13; // moveY 13 poles to start phaseB
      moveY();
      stepperX.setCurrentPosition(0);
      stepperX.runToNewPosition(+xHalfRotation);
      lcd.setCursor(13, 1);
      lcd.write(byte(0));
      lcd.write(byte(0));
      lcd.write(byte(0));
    }
  }
  else if ((footSwitch_Val == 0) && (footSwitch_Index == 1)) { // reset foot switch index
    footSwitch_Index = 0; // foot switch will only engage once when pressed down
  }
}
void FootSw() {
  if ((startOK == "a") || (startOK == "c")) {
    FootSwac();
  }
  else if (startOK == "B") {
    FootSwB();
  }
}
void loop() {
  //currentMillis = millis();
  //inputStr = Serial.readStringUntil('\n'); // serial input function
  //store(inputStr); // serial input function
  checkEstop();
  startButton();
  debounceManSw();
  potentiometerValue();
  upDnButton();
  FootSw();
}
