//Rotor TESTER V2.0, Teensy 4.1, LLC 2022

//#include <AccelStepper.h>                         //stepper library that is good at controlling accelleration and runs a bit smoother than others
//#define stepPinV 2                                //Driver X pusle pin
//#define dirPinV 3                                 //Driver X direction pin
//#define en_V 4                                    //Driver X enable pin
//#define stepPinX 5                                //Driver X pusle pin
//#define dirPinX 6                                 //Driver X direction pin
//#define en_X 7                                    //Driver X enable pin
//#define motorInterfaceType 1                      //interface type for the accel stepper library, works with TB6600 stepper drivers​
//AccelStepper stepperV = AccelStepper(motorInterfaceType, stepPinV, dirPinV);
//AccelStepper stepperX = AccelStepper(motorInterfaceType, stepPinX, dirPinX);

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
  //pinMode (en_V, OUTPUT);
  //pinMode (en_X, OUTPUT);                   //X Driver enable and LED
  //stepperV.setAcceleration(1200);
  //stepperV.setMaxSpeed(1500);
  //stepperV.setCurrentPosition(0);
  //digitalWrite(en_X, HIGH);
  //digitalWrite(en_X, LOW);
  //digitalWrite(en_V, HIGH);
  while (!Serial1 && millis() < 4000);
  //Serial.begin(115200);
  //Serial.setTimeout(20);
  Serial1.begin(115200);
  //Serial1.setTimeout(20);
}

void loop() {
  //stepperV.runToNewPosition(100000);
  //delay(200);
  //stepperV.runToNewPosition(0);
  //delay(200);
  //stepperX.runToNewPosition(-4000);
  //delay(200);
  if (ReadSerial1PortData()) {
    Serial.println(inputStr);
    store(inputStr);
    inputStr = "";
  }
}
