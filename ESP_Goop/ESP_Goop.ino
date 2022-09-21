#include <AccelStepper.h>

#define fullstep 4 // fullstep=4 halfstep=8
#define IN1 5  // D1
#define IN2 4  // D2
#define IN3 14 // D5
#define IN4 12 // D6
#define potIn A0
#define startSw 16
#define runSw 2

int potInVal; // speed in potentiometer adc value
int potInMap;
int stepsPerRevolution = 0; // Define number of steps per rotation:
int steps;
bool startSwVal;
bool startSwIndex = 0;
bool runSwVal;
bool runSwIndex = 0;
unsigned long previousMillis = 0;
const long interval = 1000;
String inputStr;

AccelStepper myStepper(fullstep, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(57600);
  Serial.setTimeout(20);
  myStepper.setMaxSpeed(200.0);
  myStepper.setAcceleration(200.0);
  myStepper.setSpeed(100);
  pinMode(startSw, INPUT);
  pinMode(runSw, INPUT);
  //pinMode(IN1, OUTPUT);
  //pinMode(IN2, OUTPUT);
  //pinMode(IN3, OUTPUT);
  //pinMode(IN4, OUTPUT);
}
void potentiometerValue() {
  potInVal = analogRead(potIn);
  potInMap = map(potInVal, 0, 1023, 0, 200);
  Serial.print("potVal=");
  Serial.println(potInMap);
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "step:") {                      //serial input variable, phase a to b flux test
    stepsPerRevolution = input.substring(5).toInt();
    Serial.print("step=");
    Serial.println(stepsPerRevolution);
    myStepper.enableOutputs();
    myStepper.setCurrentPosition(0);
    myStepper.runToNewPosition(stepsPerRevolution);
    myStepper.disableOutputs();

  }
}

void loop() {
  inputStr = Serial.readStringUntil('\n');            //serial input function
  store(inputStr);                                    //serial input function
  startSwVal = digitalRead(startSw);
  runSwVal = digitalRead(runSw);
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    potentiometerValue();
    Serial.print("swIndex=");
    Serial.println(runSwVal);
  }
  if ((startSwVal == 0) && (startSwIndex == 0)) {
    stepsPerRevolution = potInMap;
    Serial.print("step=");
    Serial.println(stepsPerRevolution);
    startSwIndex = 1;
    myStepper.enableOutputs();
    myStepper.setCurrentPosition(0);
    myStepper.runToNewPosition(stepsPerRevolution);
    myStepper.disableOutputs();

  }
  if ((startSwVal == 1) && (startSwIndex == 1)) {
    startSwIndex = 0;
  }

  if ((runSwVal == 0) && (runSwIndex == 0)) {
    runSwIndex = 1;
    myStepper.enableOutputs();
    myStepper.setAcceleration(5000.0);
  }
  while (runSwIndex == 1) {
    runSwVal = digitalRead(runSw);
    myStepper.setCurrentPosition(0);
    myStepper.runToNewPosition(400);

    if ((runSwVal == 1) && (runSwIndex == 1)) {
      myStepper.disableOutputs();
      runSwIndex = 0;
    }
  }
}
