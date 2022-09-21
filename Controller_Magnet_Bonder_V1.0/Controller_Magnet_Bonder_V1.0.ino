// Controller Magnet Bonder Test Rig V1.0
// Margaret, Brandon, Tania, Chris, Doug 7/29
// Linear Labs Inc.
#include <AccelStepper.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define PulMinus A0 //Pulse Pin for Driver
#define DirMinus A1 //Direction Pin for Driver
#define EnMinus A2 //Enable Minus for Driver 
#define speedPin 8
#define motorInterfaceType 1 //interface type for accel stepper lib, works with TB6600 stepper driver
#define SWTWTY 5 //Moves every 20 magnets
#define SWONE 6 //Moves every 1 magnet
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // -1 = no reset pin on OLED
#define SCREEN_ADDRESS 0x3C

float speedMeasure;
float rotaryStepFloat;
float carry; //for carrying a fraction a number of steps
float carry1; //for carrying a fraction a number of steps
unsigned long totTime;
unsigned long startTime;
unsigned long endTime;
bool startIndex;
bool endIndex;
bool directionUpIndex = false;
bool inputstate;
bool togglestate;
bool valSWTWTY; //variable for 20-turn switch
bool valSWONE; //variable for 1-turn switch
int rotarySteps;
int carry1in; //integer of carry1
AccelStepper rotaryTable = AccelStepper(motorInterfaceType, PulMinus, DirMinus);
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void reset() {
  startTime = 0;
  endTime = 0;
  totTime = 0;
  startIndex = false;
  endIndex = false;
  speedMeasure = 0.0;
  togglestate = false;
}

void measureSpeed() {
  inputstate = digitalRead(speedPin);
  if ((inputstate == HIGH) && (startIndex == false) && (togglestate == false)) {
    startTime = micros();
    Serial.println(startTime);
    togglestate = true;
  }
  else if ((inputstate == LOW) && (startIndex == false) && (togglestate == true)) {
    startIndex = true;
    togglestate = false;
  }
  else if ((inputstate == HIGH) && (startIndex == true) && (togglestate == false)) {
    endTime = micros();
    Serial.println(endTime);
    togglestate = true;
  }
  else if ((inputstate == LOW) && (startIndex == true) && (togglestate == true)) {
    endIndex = true;
    togglestate = false;
    Serial.println(endIndex);
  }
  else if ((startIndex == true) && (endIndex == true) && (directionUpIndex == false)) { // print down speed
    totTime = endTime - startTime;
    speedMeasure = totTime * 0.000001;
    speedMeasure = 42 / speedMeasure;
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.print("Down: ");
    display.print(speedMeasure);
    display.println("mm/sec");
    display.display();
    directionUpIndex = true;
    reset();
  }
  else if ((startIndex == true) && (endIndex == true) && (directionUpIndex == true)) { // print up speed
    totTime = endTime - startTime;
    speedMeasure = totTime * 0.000001;
    speedMeasure = 42 / speedMeasure;
    display.print("Up: ");
    display.print(speedMeasure);
    display.println("mm/sec");
    display.display();
    directionUpIndex = false;
    reset();
  }
}
// Run 20 magnet to fill in every other north first. After 13 north magnets placed, move over 1 magnet, then move every 4 from then on for remaining north
void runTwentyMagnets() {
  digitalWrite(EnMinus, HIGH);
  rotarySteps = rotaryStepFloat * 20.0;
  carry1 = carry1 + carry * 20.0;
  if (carry1 >= 1) {
    carry1in = carry1;
    rotarySteps = rotarySteps + carry1in;
    carry1 = carry1 - carry1in;
  }
  rotaryTable.setCurrentPosition(0);
  rotaryTable.runToNewPosition(rotarySteps); //14400 steps/52 magnets =  steps per magnet
  digitalWrite(EnMinus, LOW);
}
void displayLLC() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println("Linear Labs Inc.");
  display.display();
}
void runRotaryTable() {
  valSWTWTY = digitalRead(SWTWTY);
  valSWONE = digitalRead(SWONE);
  if (valSWTWTY == LOW) {
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println("Moving 20 Poles");
    display.display();
    reset();
    directionUpIndex = false;
    runTwentyMagnets();
    displayLLC();
  }
  if (valSWONE == LOW) {
    rotarySteps = rotaryStepFloat;
    carry1 = carry1 + carry;
    if (carry1 >= 1) {
      rotarySteps = rotarySteps + 1;
      carry1 = carry1 - 1;
    }
    digitalWrite(EnMinus, HIGH);
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println("Moving 1 Pole");
    display.display();
    reset();
    directionUpIndex = false;
    rotaryTable.setCurrentPosition(0);
    rotaryTable.runToNewPosition(rotarySteps); //14400 steps/52 magnets =  steps per magnet
    digitalWrite(EnMinus, LOW);
    displayLLC();
  }
}
void setup() {
  pinMode(SWTWTY, INPUT_PULLUP);
  pinMode(SWONE, INPUT_PULLUP);
  pinMode(EnMinus, OUTPUT); //Rotary Table enable
  pinMode(speedPin, INPUT);
  digitalWrite(EnMinus, LOW); // TB6600 enable needs low on start up
  rotaryTable.setCurrentPosition(0);
  rotaryTable.setAcceleration(4000); //Steps per sec per sec - get rid of accel
  rotaryTable.setMaxSpeed(10000);
  Serial.begin(115200); //USB communication (115200 - speed in kiloments or kilobyt)
  rotaryStepFloat = 553.461538; //Total steps/num of magnets
  carry1 = 0;
  rotarySteps = rotaryStepFloat;
  carry = rotaryStepFloat - rotarySteps;
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println("Linear Labs Inc.");
  display.display();
  reset();
}
void loop() {
  runRotaryTable();
  measureSpeed();
}
