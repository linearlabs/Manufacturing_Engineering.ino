//Rotor TESTER V2.0, Teensy 4.1, LLC 2022

#include <AccelStepper.h>                         //stepper library that is good at controlling accelleration and runs a bit smoother than others
#define en_X 2                                    //Driver 1 enable pin
#define dirPinX 3                                 //Driver 1 direction pin
#define stepPinX 4                                //Driver 1 pusle pin

#define en_Y 5                                    //Driver 2 enable pin
#define dirPinY 6                                 //Driver 2 direction pin
#define stepPinY 7                                //Driver 2 pusle pin

#define en_Z 8                                    //Driver 3 enable pin
#define dirPinZ 9                                 //Driver 3 direction pin
#define stepPinZ 10                                //Driver 3 pusle pin

#define en_W 11                                    //Driver 7 enable pin
#define dirPinW 12                                 //Driver 7 direction pin
#define stepPinW 24                                //Driver 7 pusle pin

#define en_V 25                                    //Driver 6 enable pin
#define dirPinV 26                                 //Driver 6 direction pin
#define stepPinV 27                                //Driver 6 pusle pin

#define en_U 28                                    //Driver 4 & 5 enable pin
#define dirPinU 29                                 //Driver 4 & 5 direction pin
#define stepPinU 30                                //Driver 4 & 5 pusle pin



#define motorInterfaceType 1                      //interface type for the accel stepper library, works with TB6600 stepper drivers​
AccelStepper stepperZ = AccelStepper(motorInterfaceType, stepPinZ, dirPinZ);
AccelStepper stepperY = AccelStepper(motorInterfaceType, stepPinY, dirPinY);

AccelStepper stepperX = AccelStepper(motorInterfaceType, stepPinX, dirPinX);
AccelStepper stepperU = AccelStepper(motorInterfaceType, stepPinU, dirPinU);
AccelStepper stepperV = AccelStepper(motorInterfaceType, stepPinV, dirPinV);
AccelStepper stepperW = AccelStepper(motorInterfaceType, stepPinW, dirPinW);

const char* idn = "Rotor TESTER V2.0, Teensy 4.1, LLC 2022";
String inputStr = ""; // command input string
boolean ReadSerial1PortData()
{
  boolean returnCode = false;
  int Serial1BytesRecieved = 0;
  Serial1BytesRecieved = Serial1.available();
  if (Serial1BytesRecieved > 0)
  {
    returnCode = true;
    //Serial1.println("ACK>");
    inputStr += Serial1.readStringUntil('\n');
  }
  return (returnCode);
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") {
    //Serial.println(idn);
    delay(200);
    Serial1.println(idn);
  }
}

void setup() {
  pinMode (en_Z, OUTPUT);
  pinMode (en_Y, OUTPUT);
  pinMode (en_X, OUTPUT);
  pinMode (en_U, OUTPUT);
  pinMode (en_V, OUTPUT);
  pinMode (en_W, OUTPUT);

  stepperZ.setAcceleration(500);
  stepperZ.setMaxSpeed(1000);
  stepperZ.setCurrentPosition(0);
  stepperY.setAcceleration(500);
  stepperY.setMaxSpeed(1000);
  stepperY.setCurrentPosition(0);
  stepperX.setAcceleration(500);
  stepperX.setMaxSpeed(1000);
  stepperX.setCurrentPosition(0);
  stepperU.setAcceleration(500);
  stepperU.setMaxSpeed(1000);
  stepperU.setCurrentPosition(0);
  stepperV.setAcceleration(500);
  stepperV.setMaxSpeed(1000);
  stepperV.setCurrentPosition(0);
  stepperW.setAcceleration(500);
  stepperW.setMaxSpeed(1000);
  stepperW.setCurrentPosition(0);

  digitalWrite(en_Z, LOW);
  digitalWrite(en_Y, HIGH);
  digitalWrite(en_X, LOW);
  digitalWrite(en_W, LOW);
  digitalWrite(en_V, LOW);
  digitalWrite(en_U, LOW);

  while (!Serial1 && millis() < 4000);
  //Serial.begin(115200);
  //Serial.setTimeout(20);
  Serial1.begin(115200);
  //Serial1.setTimeout(20);
}

void loop() {
  stepperY.runToNewPosition(2000);
  delay(1000);
  stepperY.runToNewPosition(-2000);
  delay(1000);

  if (ReadSerial1PortData()) {
    Serial.println(inputStr);
    store(inputStr);
    inputStr = "";
  }
}
