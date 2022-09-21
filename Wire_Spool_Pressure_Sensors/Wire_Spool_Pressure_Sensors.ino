 // Wire Spool Pressure Sensors, Teensy 4.1, LLC 2022 V1.0
const int analog_pins[] = {A17, A16, A15, A14, A0, A1, A2, A3, A4, A5, A8, A13, A12, A11, A10};
String idn = "Wire Spool Pressure Sensors, Teensy 4.1, LLC 2022 V1.0";
String inputStr = ""; // command input string
int val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14;
int valMap0, valMap1, valMap2, valMap3, valMap4, valMap5, valMap6, valMap7, valMap8, valMap9, valMap10, valMap11, valMap12, valMap13, valMap14;
int sensorValues[] = {val0, val1, val2, val3, val4, val5, val6, val7, val8, val9, val10, val11, val12, val13, val14};
int sensorValueMaps[] = {valMap0, valMap1, valMap2, valMap3, valMap4, valMap5, valMap6, valMap7, valMap8, valMap9, valMap10, valMap11, valMap12, valMap13, valMap14};
//  ReadSerial5PortData keeps reading the port until it sees a ~ character, appends what it reads to the command
// buffer.
bool continuousReadIndex = false;
int analogPin = 0; // for read: command
boolean ReadSerial5PortData()
{
  boolean returnCode = false;
  int Serial5BytesRecieved = 0;
  Serial5BytesRecieved = Serial5.available();
  if (Serial5BytesRecieved > 0)
  {
    returnCode = true;
    //Serial5.println("ACK>");
    // we got a valid buffer but it's likely fragmented  Keep reading until we see the ~ character.
    inputStr += Serial5.readStringUntil('\n');
  }
  return (returnCode);
}

void setup() {
  while (!Serial5 && millis() < 4000) ;
  Serial.begin(115200);
  Serial.println("For help type help?");
  Serial5.begin(115200);
  Serial5.setTimeout(20);
  Serial5.println("For help type help?");
}


void loop() {
  if (ReadSerial5PortData()) {
    Serial.println(inputStr);
    store(inputStr);
    inputStr = "";
  }
  continuousRead();
}
