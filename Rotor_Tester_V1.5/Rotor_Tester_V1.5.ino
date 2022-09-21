// Rotor Tester V1.5
// Added HiFi magnet spacing test
// Doug Gammill
// Linear Labs Inc
// 5/4/2022
#include <AccelStepper.h> // stepper library
#define stepPinX 2 // Driver X pusle pin
#define dirPinX 3 // Driver X direction pin
#define en_X 4 // Driver X enable pin
#define stepPinY 7 // Driver Y pusle pin
#define dirPinY 6 // Driver Y direction pin
#define en_Y 5 // Driver Y enable pin
#define stepPinZ 10 // Driver Z pusle pin
#define dirPinZ 9 // Driver Z direction pin
#define en_Z 8 // Driver Z enable pin
#define ManSW 11 // joystick manual mode start switch 
#define SwX A3 // X limit switch pin
#define SwY 12 // Y limit switch pin
#define SwZ 13 // Z limit switch pin
#define motorInterfaceType 1 // interface type for the accel stepper library, works with TB6600 stepper drivers

AccelStepper stepperX = AccelStepper(motorInterfaceType, stepPinX, dirPinX);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepPinY, dirPinY);
AccelStepper stepperZ = AccelStepper(motorInterfaceType, stepPinZ, dirPinZ);

int VRxA5; // manual mode joystick
int VRx; // manual mode joystick
int VRyA4; // manual mode joystick
int VRy; // manual mode joystick
int sensorA0; // CH1 hall sensor
int sensorA1; // CH2 hall sensor
int sensorA2; // CH3 hall sensor
int hallA; // CH1 hall sensor
int hallB; // CH2 hall sensor
int hallC; // CH3 hall sensor
int valSw; // variable for Manual mode switch
int buttonState; // the current reading from the manual switch (debouncing)
int ledState = HIGH; // the current state of the output pin
int lastButtonState = LOW; // the previous reading from the manual switch (debouncing
unsigned long lastDebounceTime = 0; // the last time the output pin was toggled (debouncing)
unsigned long debounceDelay = 50; // the debounce time; increase if the output flickers
int valSwX; // variable for X limit switch
int valSwY; // variable for y limit switch
int valSwZ; // variable for Z limit switch
int valX; // variable for X enable and LED
int valY; // variable for Y enable and LED
int valZ; // variable for Z enable and LED
int indexZ = 0; // flag for z axis
unsigned long i = 1; // sets magnet counter to start at 1
unsigned long millisK; // timer for 500ms sampling
unsigned long magNum = 0; // variable for number of magnets a rotor has
int xStep;
float xSteps; // variable for number of steps between magnets in that rotor
float carry; // for carry a fraction number of steps
float carry1; // for carry a fraction number of steps
int ySteps = 0; // variable for moving sensor along y axis, initial set so it can home on startup
int zSteps = 0; // variable for moving sensor along Z axis, initial set so it can home on startup
int dZ = 0; // the change in z
int dY = 0; // the change in y
int flagX1 = 0;
int flagX2 = 0;
char startCond;
String inputStr; // serial input commands
char startChar;
boolean syncDone; // flag for if sync is complete
boolean magNumCollected = false;  // flags for when all 3 variables are recieved from python program it initiates testing
boolean yStepsCollected = false;
boolean zStepsCollected = false;
boolean HiFiNumCollected = false; // used in place of magNumCollected to start HiFi/High resolution test for magnet spacing

void setup() {
  pinMode (en_X, OUTPUT); // X Driver enable and LED
  digitalWrite(en_X, HIGH); // TB6600 driver enable needs pull up resistor and high on start up otherwise it draws 2Amps each fix by making it active low
  pinMode (en_Y, OUTPUT);
  digitalWrite(en_Y, HIGH);
  pinMode (en_Z, OUTPUT);
  digitalWrite(en_Z, HIGH);
  pinMode(ManSW, INPUT); // joystick momentary switch to enable manual mode
  pinMode(SwX, INPUT); // x limit switch
  pinMode(SwY, INPUT); // y limit switch
  pinMode(SwZ, INPUT); // z limit switch
  stepperX.setCurrentPosition(0);
  stepperX.setAcceleration(1000000);
  stepperX.setMaxSpeed(1000);
  stepperY.setCurrentPosition(0);
  stepperY.setAcceleration(1000000);
  stepperY.setMaxSpeed(800);
  stepperZ.setCurrentPosition(0);
  stepperZ.setAcceleration(1000000);
  stepperZ.setMaxSpeed(800);
  Serial.begin(115200);
  Serial.setTimeout(50); // set this low so manual switch doesnt have to be long pressed, can be changed if needed.
  Serial.println("<Rotor Arduino is ready>");
  delay(1000); // wait a sec to allow Python to verify connection
  Serial.println(": goes into standby (required to start collecting parameters)");
  Serial.println("m + number declares magnet number, ex. m40");
  Serial.println("y + number declares Y steps, ex. y4000 (limit is 8200)");
  Serial.println("z + number declares Z steps, ex. z8000 (limit is 14200)");
  Serial.println("steppers will move as soon as m, y, and z have been entered");
  Serial.println("<Waiting on Python to connect to rotor Arduino>");
  sync(); // waits for a ":" character to initiate
  millisK = millis() + 500; // millisK for timed if statements, can bug out after i loops
}

void debounceManSw() {
  if (valSw != lastButtonState) { // joystick manual mode switch, reset the debouncing timer
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (valSw != buttonState) {
      buttonState = valSw;
      if (buttonState == LOW) {
        ledState = !ledState;
      }
    }
  }
  digitalWrite(en_Y, ledState);
  digitalWrite(en_Z, ledState);
  lastButtonState = valSw;
}

void manualMode() {
  valY = digitalRead(en_Y); // variable for y enable and LED
  valZ = digitalRead(en_Z); // variable for z enable and LED

  while ((valY == LOW) && (valZ == LOW)) {
    VRxA5 = analogRead(A5);
    VRx = map(VRxA5, 0, 1023, -512, 512);
    VRyA4 = analogRead(A4);
    VRy = map(VRyA4, 0, 1023, -512, 512);

    valY = digitalRead(en_Y); // variable for y enable and LED
    valZ = digitalRead(en_Z);
    valSw = digitalRead(ManSW);

    if (millis() > millisK) { // timer. try to keep things in if statements to reduce processing
      millisK = millisK + 500; // samples every 360ms
      Serial.print("manual"); // ready to recieve serial commands
      CH1();
      CH2();
      CH3();
      Serial.print("dY= ");
      Serial.print(dY * 100); // prints how many steps Y has taken in manual mode
      Serial.print(", dZ= ");
      Serial.println(dZ * 100); // prints how many steps Z has taken in manual mode
    }

    if ((VRx > 100) && (dZ <= 141)) { // dZ here is a soft limit for how far it can go 140dZ = 14000steps plus 100 steps
      stepperZ.runToNewPosition(dZ * 100); // 14400steps/40magnets = 360steps per magnet(blocking function).
      dZ++;
    }
    valSwZ = digitalRead(SwZ);

    if (VRx < -100) {
      if (valSwZ == LOW) {
        stepperZ.runToNewPosition(dZ * 100); // blocking function
        dZ--;

      }
      if (valSwZ == HIGH) {
        stepperZ.setCurrentPosition(0);
        dZ = 0;
      }
    }
    if ((VRy > 100) && (dY <= 80)) { // dY here is a soft limit 80x100=8000
      stepperY.runToNewPosition(-dY * 100); // blocking function
      dY++;
    }
    valSwY = digitalRead(SwY);

    if (VRy < -100) {
      if (valSwY == LOW) {
        stepperY.runToNewPosition(-dY * 100); // blocking function
        dY--;

      }
      if (valSwY == HIGH) {
        stepperY.setCurrentPosition(0);
        dY = 0;
      }
    }
    debounceManSw(); // exits Manual Mode
    digitalWrite(en_Y, ledState);
    digitalWrite(en_Z, ledState);
  }
  indexZ = 0; // index for Z limit switch, gravity moves z down after limit is engaged causing it to unengage again
}

void Run() {
  runY();
  runZ();
  runX();
}

void RunHiFi() {
  runY();
  runZ();
  runXHiFi();
}

void Home() {
  homeZ();
  homeY();
}

void CH1() {
  sensorA0 = analogRead(A0);
  hallA = map(sensorA0, 0, 1023, -511, 513);
  Serial.print(", CH1, ");
  Serial.print(hallA);
}
void CH2() {
  sensorA1 = analogRead(A1);
  hallB = map(sensorA1, 0, 1023, -509, 515);
  Serial.print(", CH2, ");
  Serial.print(hallB);
}

void CH3() {
  sensorA2 = analogRead(A2);
  hallC = map(sensorA2, 0, 1023, -507, 517);
  Serial.print(", CH3, ");
  Serial.println(hallC);
}

void runY() {
  digitalWrite(en_Y, LOW); // always move steppers to were hall sensor arm wont get broken off
  valY = digitalRead(en_Y); // enable for stepper Y
  while (valY == LOW) { // while stepper Y is enabled
    stepperY.moveTo(-ySteps); // make steps positive if you want to go opposite way
    stepperY.run(); // non-blocking function, allows for other stuff to be done in loop at the same time but takes alot of processing
    if (stepperY.isRunning()) { // executes ony if stepper Y is Running
      if (millis() > millisK) { // timer. try to keep things in if statements to reduce processing power
        millisK = millisK + 500;
        digitalWrite(en_Y, LOW);
        Serial.println("setup Y");
      }
    }
    else { // once stepper is done running this will execute
      digitalWrite(en_Z, LOW);
      stepperY.setCurrentPosition(0);
      digitalWrite(en_Y, HIGH);
      valY = digitalRead(en_Y);
    }
  }
}

void runZ() {
  valZ = digitalRead(en_Z);
  while (valZ == LOW) {
    stepperZ.moveTo(zSteps); // make steps negative if you want to go opposite way
    stepperZ.run();
    if (stepperZ.isRunning()) {
      if (millis() > millisK) { // try to keep things in if statements to reduce processing power
        millisK = millisK + 500;
        digitalWrite(en_Z, LOW);
        Serial.println("setup Z");
      }
    }
    else {
      stepperX.setCurrentPosition(0);
      digitalWrite(en_Z, HIGH);
      valZ = digitalRead(en_Z);
    }
  }
}

void runX() {
  homeX();
  xSteps = 14400 / magNum;
  carry1 = 0;
  xStep = xSteps; // changes Xsteps to integer
  carry = xSteps - xStep;
  while (i <= magNum) { // X circular magnet testing with hall sensor
    Serial.print("running");
    xStep = xSteps; // changes Xsteps to integer
    carry1 = carry1 + carry;
    if (carry1 >= 1) {
      xStep = xStep + 1;
      carry1 = carry1 - 1;
    }
    CH1();
    CH2();
    CH3();
    stepperX.setCurrentPosition(0); // not using run() funtion because hall sampling sometimes drops out
    stepperX.runToNewPosition(xStep); // 14400steps/40magnets = 360steps per magnet(blocking function).
    i++;
  }

  digitalWrite(en_X, HIGH);
  indexZ = 0; // set Z flag to 0 so it will home
  i = 1; // resets i loop to 1
  millisK = millis() + 500; // a timing bug with millisK happens without this line of code
}

void runXHiFi() { // function to run magnet spacing test
  homeX();
  offsetHiFi();
  while (i <= magNum) {
    Serial.print("running");
    CH1();
    Serial.println(", CH2, 0, CH3, 0");
    delay(30);
    stepperX.setCurrentPosition(0); // using blocking funtion because hall sampling sometimes drops out
    stepperX.runToNewPosition(1); // move one step inbetween sample
    i++;
    delay(30);
  }
  digitalWrite(en_X, HIGH);
  indexZ = 0; // set Z flag to 0 so it will home
  i = 1; // resets i loop to 1
  millisK = millis() + 500; // a timing bug with millisK happens without this line of code
}

void moveToZero() {
  while (hallA <= 0) { // north is a negative number (should start on north)
    sensorA0 = analogRead(A0);
    hallA = map(sensorA0, 0, 1023, -511, 513);
    stepperX.setCurrentPosition(0);
    stepperX.runToNewPosition(-1); // moves back for offset
    delay(30);
  }
}
void offsetHiFi() { // moves rotor back half a magnet to where sensor is position over a through rather than a crest
  while (true) {
    delay(1000); // let vibrations settle for acurate reading
    sensorA0 = analogRead(A0);
    hallA = map(sensorA0, 0, 1023, -511, 513);
    if (hallA < 0) {
      moveToZero();
    }
    else {
      break;
    }
  }
}
void homeX() {
  flagX1 = 1; // Flags for homing X on the leading edge of X limit switch
  flagX2 = 1; // Flags for homing X on the leading edge of X limit switch
  digitalWrite(en_X, LOW);
  while ((flagX1 == 1) || (flagX2 == 1)) {
    valSwX = digitalRead(SwX); // read X limit switch
    valX = digitalRead(en_X); // read X driver enable
    if ((valSwX == HIGH) && (flagX1 == 1) && (flagX2 == 1)) {
      stepperX.setCurrentPosition(0); // this if function moves x out of limit switch actuation to make sure it starts on leading edge
      Serial.println("homingX,");
      stepperX.runToNewPosition(-440); // 360 is about how many steps X limit switch is actuated for
      stepperX.setCurrentPosition(0);
      valSwX = digitalRead(SwX); // read X limit switch
      while ((valSwX == HIGH) && (flagX1 == 1) && (flagX2 == 1)) {  // fails if stepper doesnt kick back all the way out of limit switch, reset required
        if (millis() > millisK) { // countdown timer. try to keep things in if statements to reduce processing power
          millisK = millis() + 500;
          Serial.println("failedHomingX,");
        }
      }
    }

    if ((valSwX == LOW) && (flagX1 == 1) && (flagX2 == 1)) { // if limit switch is acuated set X1 flag false to move out of limit
      flagX1 = 0;
    }
    if ((valSwX == LOW) && (flagX1 == 0) && (flagX2 == 1)) { // when or if limit switch is not actuated find its leading edge
      stepperX.moveTo(14400); // make steps positive if you want to go opposite way
      stepperX.run();
      if (millis() > millisK) { // countdown timer. try to keep things in if statements to reduce processing power
        millisK = millis() + 500;
        if (stepperX.isRunning()) { // non blocking function, checks to see if arduino is still pulsing steppers
          Serial.println("homingX,");
        }
        else {
          Serial.println("failedHomingX,"); // if arduino is done pulsing stepper but hasnt reached limit switch then failure happens
        }
      }
    }
    if ((valSwX == HIGH) && (flagX1 == 0) && (flagX2 == 1)) { // if X limit switch is reach and FlagX1 is already set to off (ensures leading edge of limit switch is found.)
      flagX2 = 0; // exit while loop by turning final X limit flag off
    }
  }
}

void homeZ() {
  if ((valSwZ == LOW) && (indexZ == 0)) {
    digitalWrite(en_Z, LOW);
    valZ = digitalRead(en_Z);
    while (valZ == LOW) {
      stepperZ.moveTo(-14200); // negative makes it go toward limit switch (11.6k steps is max limit)
      stepperZ.run();
      if (millis() > millisK) { // 500ms timer. try to keep things in if statements to reduce processing power
        millisK = millisK + 500;
        if (stepperZ.isRunning()) {
          Serial.println("homing Z");
        }
        else {
          Serial.println("x Homing Z Failed");
        }
      }
      valSwZ = digitalRead(SwZ); // Exits homeZ
      if (valSwZ == HIGH) {
        digitalWrite(en_Z, HIGH);
      }
      valZ = digitalRead(en_Z);
    }
    digitalWrite(en_Z, HIGH);
    stepperZ.setCurrentPosition(0);
    indexZ = 1;
  }
}

void homeY() {
  if (valSwY == LOW) {
    digitalWrite(en_Y, LOW);
    valY = digitalRead(en_Y);
    while (valY == LOW) {
      stepperY.moveTo(8200); // positive makes it go toward limit switch
      stepperY.run();
      if (millis() > millisK) { // timer. try to keep things in if statements to reduce processing power
        millisK = millisK + 500;
        if (stepperY.isRunning()) {
          Serial.println("homing Y");
        }
        else {
          Serial.println("x Homing Y Failed");
        }
      }
      valSwY = digitalRead(SwY);
      if (valSwY == HIGH) {
        digitalWrite(en_Y, HIGH);
      }

      valY = digitalRead(en_Y);
    }
    stepperY.setCurrentPosition(0);
    digitalWrite(en_Y, HIGH);
  }
}

void sync() {
  syncDone = false;
  while (!syncDone) {
    startCond = Serial.read();
    if ( startCond == ':') {
      Serial.println("Transferring parameters from Python to Arduino");
      syncDone = true;
    }
  }
}

void displayData() {
  Serial.print(magNumCollected);
  Serial.print(" magnet number: ");
  Serial.println(magNum);

  Serial.print(yStepsCollected);
  Serial.print(" y: ");
  Serial.println(ySteps);

  Serial.print(zStepsCollected);
  Serial.print(" z: ");
  Serial.println(zSteps);
}

void store(String input) { // serial command inputs
  startChar = input.charAt(0);
  switch (startChar) {
    case 'm': // collects magnet number
      magNum = input.substring(1).toInt();
      if (magNum < 14400) {
        magNumCollected = true;
        displayData();
      }
      else {
        HiFiNumCollected = true;
        Serial.println("HiFi Test");
      }
      break;
    case 'y': // collects y steps
      ySteps = input.substring(1).toInt();
      yStepsCollected = true;
      displayData();
      break;
    case 'z': // collects z steps
      zSteps = input.substring(1).toInt();
      //Serial.write("z.");
      zStepsCollected = true;
      displayData();
      break;
  }
}

void loop() {
  inputStr = Serial.readStringUntil('\n');
  store(inputStr);

  if (millis() > millisK) { // countdown timer. try to keep things in if statements to reduce processing power
    millisK = millisK + 500;
    Serial.print("standby"); // ready to recieve serial commands
    CH1();
    CH2();
    CH3();
  }
  valSw = digitalRead(ManSW); // manual mode switch
  valX = digitalRead(en_X); // variable for X enable and LED
  valY = digitalRead(en_Y); // variable for y enable and LED
  valZ = digitalRead(en_Z); // variable for z enable and LED
  debounceManSw();
  if ((valY == LOW) && (valZ == LOW)) {
    manualMode();
  }

  if (ySteps > 8200) { // limits the max number you can tell Y to travel
    Serial.println("x, Y value too high");
    magNum = 0; // variable for number of magnets a rotor has
    xSteps = 0; // variable for number of steps between magnets in that rotor
    ySteps = 0; // variable for moving sensor along y axis, initial set so it can home on startup
    zSteps = 0; // variable for moving sensor along Z axis, initial set so it can home on startup
    magNumCollected = false;
    yStepsCollected = false;
    zStepsCollected = false;
  }
  if (zSteps > 14200) { // limits the max number you can tell Z to travel
    Serial.println("x, Z value too high");
    magNum = 0; // variable for number of magnets a rotor has
    xSteps = 0; // variable for number of steps between magnets in that rotor
    ySteps = 0; // variable for moving sensor along y axis, initial set so it can home on startup
    zSteps = 0; // variable for moving sensor along Z axis, initial set so it can home on startup
    magNumCollected = false;
    yStepsCollected = false;
    zStepsCollected = false;
  }

  if ((magNumCollected && yStepsCollected && zStepsCollected)) {
    valSwX = digitalRead(SwX); // X limit switch variable
    valSwY = digitalRead(SwY); // Y limit switch variable
    valSwZ = digitalRead(SwZ); // Z limit switch variable
    Home();
    Run();
    valSwX = digitalRead(SwX); // X limit switch variable
    valSwY = digitalRead(SwY); // Y limit switch variable
    valSwZ = digitalRead(SwZ); // Z limit switch variable
    Home();
    magNum = 0; // variable for number of magnets a rotor has
    xSteps = 0; // variable for number of steps between magnets in that rotor
    ySteps = 0; // variable for moving sensor along y axis, initial set so it can home on startup
    zSteps = 0; // variable for moving sensor along Z axis, initial set so it can home on startup
    magNumCollected = false;
    yStepsCollected = false;
    zStepsCollected = false;
  }
  if ((HiFiNumCollected && yStepsCollected && zStepsCollected)) {
    valSwX = digitalRead(SwX); // X limit switch variable
    valSwY = digitalRead(SwY); // Y limit switch variable
    valSwZ = digitalRead(SwZ); // Z limit switch variable
    Home();
    RunHiFi();
    valSwX = digitalRead(SwX); // X limit switch variable
    valSwY = digitalRead(SwY); // Y limit switch variable
    valSwZ = digitalRead(SwZ); // Z limit switch variable
    Home();
    magNum = 0; // variable for number of magnets a rotor has
    xSteps = 0; // variable for number of steps between magnets in that rotor
    ySteps = 0; // variable for moving sensor along y axis, initial set so it can home on startup
    zSteps = 0; // variable for moving sensor along Z axis, initial set so it can home on startup
    HiFiNumCollected = false;
    yStepsCollected = false;
    zStepsCollected = false;
  }
}
