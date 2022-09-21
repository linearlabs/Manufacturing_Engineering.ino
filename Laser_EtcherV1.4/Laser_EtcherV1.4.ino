// Laser Etcher V1.4
// adding lift axis
#include <AccelStepper.h>
#define motorInterfaceType 1 // interface type for the accel stepper library, works with TB6600 stepper drivers​
#define stepPinY 2 // Driver Y pusle pin
#define dirPinY 3 // Driver Y direction pin
#define en_Y 4
#define manual_SwY 5
#define DT 6
#define CLK 7

const long interval = 200; // interval at which to execute timing functions (milliseconds)
unsigned long prevMillis = 0; // will store last time was updated
unsigned long currentMillis = 0; // timing functions
String inputStr; // serial input commands
int rotation;  // encoder knop starting position
int value; // encoder knob value
boolean LeftRight; // encoder knob cw or ccw
int currentPositionY; // whole number of steps to move 1 pole = 20
bool manual_SwY_Val;
bool en_manY_State = false;
bool lastButtonStateY = LOW;
bool manual_SwY_State; // the current reading from the input pin
unsigned long lastDebounceTimeY = 0; // the last time the output pin was toggled
unsigned long debounceDelayY = 50; // the debounce time; increase if the output flickers
int addSteps = 0;
int minusSteps = 0;
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepPinY, dirPinY);

void setup() {
  pinMode(manual_SwY, INPUT_PULLUP);
  stepperY.setAcceleration(3000);
  stepperY.setMaxSpeed(500);
  pinMode (en_Y, OUTPUT); // Y Driver enable and LED
  digitalWrite(en_Y, HIGH); // en_Y_State is variable high or low
  pinMode (CLK, INPUT);
  pinMode (DT, INPUT);
  rotation = digitalRead(CLK); // for encoder knob
  Serial.begin(57600);
  Serial.setTimeout(50); // set to 50 because it adds too much delay to relay
}

void potentiometerValue() {
  while (en_manY_State == true) {
    debounceManSw();
    value = digitalRead(CLK);
    if (value != rotation) { // Use the DT pin to find out which way were turning.
      if (digitalRead(DT) != value) {  // Clockwise
        currentPositionY = currentPositionY + 16;
        LeftRight = true;
      } else { //Counterclockwise
        LeftRight = false;
        currentPositionY = currentPositionY - 16;
      }
      if (LeftRight) {
        Serial.println ("clockwise");
      } else {
        Serial.println("counterclockwise");
      }
      Serial.print("Encoder currentPositionY: ");
      Serial.println(currentPositionY);
    }
    rotation = value;
    stepperY.moveTo(currentPositionY);
    stepperY.run();
  }
}
void debounceManSw() {
  manual_SwY_Val = digitalRead(manual_SwY);
  if (manual_SwY_Val != lastButtonStateY) {
    // reset the debouncing timer
    lastDebounceTimeY = millis();
  }
  if ((millis() - lastDebounceTimeY) > debounceDelayY) {
    if (manual_SwY_Val != manual_SwY_State) {
      manual_SwY_State = manual_SwY_Val;
      if (manual_SwY_State == LOW) {
        en_manY_State = !en_manY_State;
        //potInVal = analogRead(potIn);
        //potInMap = map(potInVal, 0, 1023, 0, 255);
        //digitalWrite(en_Y, LOW); // en_Y_State is variable high or low
        Serial.print("enYstate=");
        Serial.println(en_manY_State);
      }
    }
  }
  lastButtonStateY = manual_SwY_Val;
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "mvup:") { // move up
    addSteps = input.substring(5).toInt();
    currentPositionY = currentPositionY + addSteps;
    stepperY.runToNewPosition(currentPositionY);
  }
  else if (startStr == "mvdn:") { // move down
    minusSteps = input.substring(5).toInt();
    currentPositionY = currentPositionY - minusSteps;
    stepperY.runToNewPosition(currentPositionY);
  }
}
void loop() {
  currentMillis = millis(); // for debouncing function
  inputStr = Serial.readStringUntil('\n'); // serial input function
  store(inputStr); // serial input function
  debounceManSw();
  potentiometerValue();
  stepperY.moveTo(currentPositionY);
  stepperY.run();

}
