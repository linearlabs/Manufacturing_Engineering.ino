#include <LiquidCrystal.h>
#include <SDConfigCommand.h>
#include <StringSplitter.h>
#include <AccelStepper.h>

#define chipSelect 53
#define leftDoor 2
#define rightDoor 3
#define keyMode1 4
#define keyMode2 5
#define liftLimit 10
#define up 48
#define dwn 47
#define ent 46
#define esc 45
#define light 22
#define fumeOut 29
#define fumeIn A0
#define ESTOP 30
#define Error 31
#define Warning 32
#define Ready 33
#define Marking 34
#define Complete 35
#define motorInterfaceType 1 // interface type for the accel stepper library, works with TB6600 stepper drivers​
#define stepPinZ 11 // Driver Z pusle pin
#define dirPinZ 12 // Driver Z direction pin
#define en_Z 13
int currentPositionZ = 0; // max 4800 only use in autoLift() and homeLift() for safty
int addSteps = 0;
int minusSteps = 0;
AccelStepper stepperZ = AccelStepper(motorInterfaceType, stepPinZ, dirPinZ);

LiquidCrystal lcd(36, 37, 41, 40, 39, 38);

bool upVal; // up
bool dwnVal; // down
bool entVal; // ent
bool escVal; // esc
bool RDoorVal;                                   //rightDoor
bool LDoorVal;                                   //leftDoor
bool keyModeVal1;                                   //keyMode1
bool keyModeVal2;                                   //keyMode2
bool ESTOPVal;
bool liftLimitVal;
bool ErrorVal; // error IO from laser
bool errorIndex = 0; // error for lazer range finder failure
bool WarningVal;
bool ReadyVal;
bool MarkingVal;
bool CompleteVal;
bool completeValIndex;
bool homeLiftIndex;
bool mainMenuIndex;
bool lightIndex;
bool liftIndex;
bool fumeIndex;
bool doorIndex;
bool keyModeIndex;
bool upIndex;
bool snMenuIndex;
bool entIndex;
bool barcodeIndex;
bool foundBarcodeIndex;
bool laserIndex;
bool writeIndex;
bool readIndex;
bool searchIndex;
bool barcodeFindIndex;
float zTrack = 0.0; // laser range find number
float liftHeight = 0.0; // predetermined lift hight in sd config
String barcode = "";
String laserMessage = "";
String barcodePartNum = "";
String barcodeSn = "";
String sdSn = ""; // sd sernum read as a string before converting to long int
String jobNumFile = "";
String jobNum = "";
int jobNumInt;
String partNum = "";
String sysFolderName = "SYSTEM~1"; // sdcard system folder name to ignore
unsigned long serNum; // max is 4,294,967,295
unsigned long barcodeSerialNum; // max is 4,294,967,295
int serLeng; // number of Serial characters, max is 10
int numTabs;
int fumeInVal; // determines if fume extractor is on
bool sdSearchTries;
unsigned long previousMillis = 0; // will store last time LCD was updated
const long interval = 500; // for bling without delay
int lcdState = LOW;
File root;
File entry;
SDConfigCommand sdcc;
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
String inputStr; // serial input string commands
//char startChar; // serial input Char commands

void setup() {
  stepperZ.setAcceleration(3000);
  stepperZ.setMaxSpeed(500);
  stepperZ.setCurrentPosition(0);
  pinMode (en_Z, OUTPUT); // Y Driver enable and LED
  digitalWrite(en_Z, LOW); // en_Z_State is variable high or low
  lcd.begin(16, 2); // LCD one time set up
  lcd.createChar(0, Check); // lcd custom character
  pinMode (up, INPUT_PULLUP);
  pinMode (dwn, INPUT_PULLUP);
  pinMode (ent, INPUT_PULLUP);
  pinMode (esc, INPUT_PULLUP);
  pinMode(rightDoor, INPUT_PULLUP);
  pinMode(leftDoor, INPUT_PULLUP);
  pinMode(keyMode1, INPUT_PULLUP);
  pinMode(keyMode2, INPUT_PULLUP);
  pinMode(ESTOP, INPUT_PULLUP);
  pinMode(liftLimit, INPUT_PULLUP);
  pinMode(Error, INPUT_PULLUP);
  pinMode(Warning, INPUT_PULLUP);
  pinMode(Ready, INPUT_PULLUP);
  pinMode(Marking, INPUT_PULLUP);
  pinMode(Complete, INPUT_PULLUP);
  pinMode (light, OUTPUT);
  pinMode (fumeOut, OUTPUT);
  mainMenuIndex = 0;
  doorIndex = 0;
  lightIndex = 0;
  keyModeIndex = 0;
  upIndex = 0;
  snMenuIndex = 0;
  entIndex = 0;
  barcodeIndex = 0;
  liftIndex = 0;
  barcodeFindIndex = 0; // finding barcode with serial input command find:
  foundBarcodeIndex = 0; // finding barcode with barcode scanner
  writeIndex = 0;
  readIndex = 0; // index for serial input command read:
  searchIndex = 0;
  sdSearchTries = 0; // bool val to know when all configs have been read
  laserIndex = 0;
  completeValIndex = 0; // from laser indicating job done
  homeLiftIndex = 1; // 1= go home
  Serial.begin(115200); // Arduino USB UART
  Serial.setTimeout(50);
  Serial1.begin(115200); // barcode scanner RS232
  Serial1.setTimeout(50);
  Serial2.begin(115200); // laser etcher RS232
  Serial2.setTimeout(100);
  Serial.println("For help type help?");
  pinMode(chipSelect, OUTPUT);
  SD.begin(53);
  root = SD.open("/");
  sdcc.set(jobNumFile, chipSelect, processCmd);
  readyTrig();
  homeLift();
}
void readInputs() {
  upVal = digitalRead(up);
  dwnVal = digitalRead(dwn);
  entVal = digitalRead(ent);
  escVal = digitalRead(esc);
  RDoorVal = digitalRead(rightDoor);
  LDoorVal = digitalRead(leftDoor);
  keyModeVal1 = digitalRead(keyMode1);
  keyModeVal2 = digitalRead(keyMode2);
  ReadyVal = digitalRead(Ready);
  MarkingVal = digitalRead(Marking);
  CompleteVal = digitalRead(Complete);
}
void returnToMain() {
  if (mainMenuIndex == 0) { // main startup
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Linear Labs Inc.");
    lcd.setCursor(0, 1);
    lcd.print("Laser Etcher");
    Serial.println("mainMenu");
    mainMenuIndex = 1;
    upIndex = 0;
    entIndex = 0;
    barcodeIndex = 0;
    foundBarcodeIndex = 0;
    homeLiftIndex = 1; // go home
    errorIndex = 0; // reset laser range finder error condition
  }
}

void pnMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(partNum);
  Serial.print("partNum, ");
  Serial.println(partNum);
  Serial.print("serNum, ");
  Serial.println(serNum);
  Serial.print("serLeng, ");
  Serial.println(serLeng);
  Serial.print("jobNum, ");
  Serial.println(jobNum);
  Serial.print("jobNumFile, ");
  Serial.println(jobNumFile);
  Serial.print("liftHeight, ");
  Serial.println(liftHeight);
}

void upButton() {
  if ((upVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { // partnumber up
    entry.close();
    printDirectory(root, 0);
    pnMenu();
    sdSearchTries = 0;
    upIndex = 1;
    delay(200);
  }
}

void searchSD() {
  if (barcodeIndex == 1) {
    if (sdSearchTries == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Failed Barcode");
      lcd.setCursor(0, 1);
      lcd.print("Update Config SD");
      Serial.println("Failed Barcode,Update Config SD");
      delay(5000);
      mainMenuIndex = 0;
      barcodeIndex = 0;
      sdSearchTries = 0;
      barcodeFindIndex = 0;
      jobNumFile = "";
      partNum = "";
      serNum = 0;
      jobNum = "";
      serLeng = 0;
    }
    if ((barcodePartNum != partNum) && (barcodeIndex == 1)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Searching");
      Serial.println("Searching SD");
      printDirectory(root, 0);
      //pnMenu();
    }
    if (barcodePartNum == partNum) {
      Serial.println("Found part number");
      barcodeIndex = 0;
      sdSearchTries = 0;
      foundBarcodeIndex = 1;
      pnMenu();
      if (barcodeFindIndex == 1) {
        barcodeFindIndex = 0;
        mainMenuIndex = 0;
      }
    }
  }
}
void escButton() {
  if ((escVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) {
    mainMenuIndex = 0;
    snMenuIndex = 0;
    barcodeIndex = 0;
    foundBarcodeIndex = 0;
    barcode = "";
  }
}

void snMenu() {
  barcodeSn = String(serNum);
  formatSn();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(partNum);
  lcd.setCursor(0, 1);
  if (jobNumInt < 1900) { // for config items that dont use SNs
    lcd.print("SN:");
    lcd.setCursor(3, 1);
    lcd.print(barcodeSn);
  }
}

void deleteConfigs() { // with serial input delete config
  Serial.println("Removing config file...");

  //Serial.println(jobNumFile.length());
  SD.remove(jobNumFile);
  if (SD.exists(jobNumFile)) {
    Serial.println("Config failed to delete.");
  } else {
    Serial.println("Config removed successfully.");
  }
}
void writeConfigs() { // with serial input write new config
  //Serial.println(jobNumFile.length());
  if (!entry) {
    Serial.println("Creating config...");
    entry = SD.open(jobNumFile, FILE_WRITE);
    entry.print("partNum=");
    entry.println(partNum);
    entry.print("serNum=");
    entry.println(serNum);       //save the next serNum to use
    entry.print("serLeng=");
    entry.println(serLeng);
    entry.print("jobNum=");
    entry.println(jobNum);
    entry.print("liftHeight=");
    entry.println(liftHeight);
    entry.close();
    Serial.println("Done");
  }
  else {
    Serial.println("must dele: file first");
  }
}

void deleteWriteConfig() { // used to update serial number in config after job is started
  deleteConfigs();
  Serial.println("Creating config...");
  entry = SD.open(jobNumFile, FILE_WRITE); //must delete file first because there isnt an eddit function
  entry.print("partNum=");
  entry.println(partNum);
  entry.print("serNum=");
  entry.println(serNum + 1);       //save the next serNum to use
  entry.print("serLeng=");
  entry.println(serLeng);
  entry.print("jobNum=");
  entry.println(jobNum);
  entry.print("liftHeight=");
  entry.println(liftHeight);
  entry.close();
  if (SD.exists(jobNumFile)) {
    Serial.println("Created config.");
  } else {
    Serial.println("Failed to create config.");
  }
}

void marking() {
  while (MarkingVal == HIGH) {
    fumeEnable(); // start fume extractor fan
    MarkingVal = digitalRead(Marking);
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (lcdState == LOW) {
        lcdState = HIGH;
        lcd.setCursor(13, 1);
        lcd.print("On!");
        if (jobNumInt >= 1900) { // for config items that dont use SNs
          lcd.setCursor(0, 1);
          lcd.print("Marking");
        }
      } else {
        lcdState = LOW;
        lcd.setCursor(13, 1);
        lcd.print("   ");
        if (jobNumInt >= 1900) { // for config items that dont use SNs
          lcd.setCursor(0, 1);
          lcd.print("         ");
        }
      }
    }
    CompleteVal = digitalRead(Complete);
    if (CompleteVal == HIGH) {   //Prints check marks when done
      delay(50);
      completeValIndex = 1;
      laserMessage = Serial2.readString(); //when start marking command is sent, it doesnt send ok msg till done.
      Serial.println(laserMessage);
      laserMessage = "";
    }
  }
}

void complete() {
  if (completeValIndex == 1) {
    if (jobNumInt >= 1900) { // for config items that dont use SNs
        lcd.setCursor(0, 1);
        lcd.print("Defuming!");
      }
    delay(5000); // delay 5 seconds for fumes to be extracted
    fumeDisable(); // stop fume extractor fan
    lcd.setCursor(13, 1);
    lcd.write(byte(0));
    lcd.setCursor(14, 1);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(0));
    if (jobNumInt >= 1900) { // for config items that dont use SNs
      lcd.setCursor(0, 1);
      lcd.print("         ");
    }
    completeValIndex = 0;
  }
}
void entSnMenu() {
  if ((entVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { // Enter
    delay(100);
    if (upIndex == 0) { // when upIndex=0 returnToMain will reset everything
      mainMenuIndex = 0;
    }
    else {
      snMenu();
      snMenuIndex = 1;
      if (jobNumInt >= 1900) { // for config items that dont use SNs
        lcd.setCursor(0, 1);
        lcd.print("Press Ent");
      }
    }
  }
  else if (foundBarcodeIndex == 1) {
    snMenuIndex = 1;
    serNum = barcodeSerialNum;
    snMenu();
    if (jobNumInt >= 1900) { // for config items that dont use SNs
      lcd.setCursor(0, 1);
      lcd.print("Press Ent");
    }
  }
  while (snMenuIndex == 1) {
    inputStr = Serial.readStringUntil('\n'); //serial input function
    store(inputStr); // serial input function
    readInputs();
    eStop();
    error();
    escButton();
    marking();
    notReady();
    doors();
    if ((upVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { //increase serNum
      serNum++;
      snMenu();
      delay(100);
      entIndex = 1;
    }
    else if ((dwnVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { //partnumber down
      if (serNum > 1) { //sn cant go below +1
        serNum--;
        snMenu();
        delay(100);
        upIndex = 1;
        entIndex = 1;
      }
    }
    else if ((entVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { // start marking
      if (entIndex == 1) {
        if (jobNumInt >= 1900) { // for config items that dont use SNs
          lcd.setCursor(0, 1);
          lcd.print("         ");
        }
        autoLift();
        if (errorIndex == 0) {
          laserIndex = 1;
          sendCmd();
          if (jobNumInt >= 1900) { // for config items that dont use SNs
            lcd.setCursor(0, 1);
            lcd.print("         ");
          }
          deleteWriteConfig();
          if (jobNumInt >= 1900) { // for config items that dont use SNs
            lcd.setCursor(0, 1);
            lcd.print("         ");
          }

          serNum = serNum + 1;
          barcode = "";
          entIndex = 0;
          foundBarcodeIndex = 0;
          snMenu();
          delay(200); // button debouncing delay
        }
        else {
          mainMenuIndex = 0; // return to main
          snMenuIndex = 0; // exit
          barcodeIndex = 0;
          foundBarcodeIndex = 0;
          barcode = "";
        }
      }
    }
    else if ((entVal == HIGH) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) {
      if (entIndex == 0) { // enter can only be hit a second time after it goes high first
        entIndex = 1;
        delay(200); // button debouncing delay
      }
    }
    complete(); // sets checkmarks on lcd after marking is complete
  }
}

void rangeFind() {
  Serial2.println("WX,ZtrackingPointCheck=0,0,0,1,0");
  while (laserMessage == "") {
    laserMessage = Serial2.readString();
  }
  if (laserMessage != "") {
    StringSplitter *splitter = new StringSplitter(laserMessage, ',', 4); // new StringSplitter(string_to_split, delimiter, limit)
    String item3 = splitter->getItemAtIndex(2); // item index is the part in the string to display
    if (item3 == "S102") {
      zTrack = -50.00; // if out of range
      Serial.println("less than -50.00");
    }
    else {
      zTrack = item3.toFloat();
      if (zTrack == 0.00) { // use this for debugging
        Serial.println(laserMessage); // there is a posibilty for an error message
      }
    }
  }
  laserMessage = "";
}
void sendCmd() {
  while (laserIndex == 1) {
    if (jobNumInt < 1900) { // for config items that dont use SNs
      Serial2.println("WX,JOB=" + jobNum + ",BLK=000,CharacterString=" + partNum + "-" + barcodeSn);
      while (laserMessage == "") {
        laserMessage = Serial2.readString();
      }
      Serial.println(laserMessage);
      laserMessage = "";
      delay(200);
    }
    Serial2.println("WX,JobNo=" + jobNum);
    while (laserMessage == "") {
      laserMessage = Serial2.readString();
    }
    Serial.println(laserMessage);
    laserMessage = "";
    Serial2.println("WX,StartMarking");
    laserIndex = 0;
    barcodeIndex = 0;
  }
}

void doors() {
  if ((RDoorVal == HIGH) && (LDoorVal == HIGH)) {
    if (doorIndex == 0) {
      if (snMenuIndex == 0) {
        mainMenuIndex = 0;
      }
      if (snMenuIndex == 1) {
        snMenu();
        if (jobNumInt >= 1900) { // for config items that dont use SNs
          lcd.setCursor(0, 1);
          lcd.print("Press Ent");
        }
      }
      doorIndex = 1;
      Serial.println("Doors Closed");
      //Serial2.println("WX,Lighting=0");
      //laserMessage = Serial2.readString();
      //laserMessage = "";
      digitalWrite(light, LOW);
      delay(300);
    }
  }
  if ((RDoorVal == LOW) && (LDoorVal == HIGH)) {
    if (doorIndex == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Right Door Open");
      Serial.println("Right Door Open");
      Serial2.println("WX,Lighting=1");
      laserMessage = Serial2.readString();
      laserMessage = "";
      digitalWrite(light, HIGH);
      doorIndex = 0;
    }
  }
  if ((RDoorVal == HIGH) && (LDoorVal == LOW)) {
    if (doorIndex == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Left Door Open");
      Serial.println("Left Door Open");
      Serial2.println("WX,Lighting=1");
      laserMessage = Serial2.readString();
      laserMessage = "";
      digitalWrite(light, HIGH);
      doorIndex = 0;
    }
  }
  if ((RDoorVal == LOW) && (LDoorVal == LOW)) {
    if (doorIndex == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Both Doors Open");
      Serial.println("Both Doors Open");
      //Serial2.println("WX,Lighting=1");
      //laserMessage = Serial2.readString();
      //laserMessage = "";
      digitalWrite(light, HIGH);
      doorIndex = 1;
    }
  }
}
void autoLift() {
  digitalWrite(en_Z, HIGH); // en_Z_State is variable high or low
  rangeFind();
  Serial.print("zTrack=");
  Serial.println(zTrack);
  // move +dir or -dir until zTrack is within +-.5mm of liftHeight
  // zTrack = laser range finder
  // liftHeight = config file lift height
  while ((zTrack <= liftHeight - 1.0) || (zTrack >= liftHeight + 1.0)) {
    eStop();
    if (homeLiftIndex == 1) {
      break; // exit while loop and rehome after eStop
    }
    else if (zTrack == -50.00) { // move in + dir if outof range by 1250 steps(50mm)
      currentPositionZ = zTrack * -25.4 + currentPositionZ; // 25.4 steps = 1mm
    }
    //move in +dir or -dir if in range
    else if (zTrack > -50.00) { // when in range move to liftHeight
      zTrack = zTrack - liftHeight;
      currentPositionZ = zTrack * -25.4 + currentPositionZ;
    }
    // soft limit, cant move more than 4800 steps
    if ((currentPositionZ <= 4800) && (currentPositionZ >= 0)) {
      stepperZ.runToNewPosition(currentPositionZ); // blocking function
      rangeFind();
      Serial.print("zTrack=");
      Serial.println(zTrack);
      Serial.print("currentPositionZ=");
      Serial.println(currentPositionZ);
    }
    else {
      entVal = digitalRead(ent);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Err, RangeFinder");
      lcd.setCursor(0, 1);
      lcd.print("Press Ent To Clr");
      Serial.print("zTrack=");
      Serial.println(zTrack);
      Serial.print("currentPositionZ=");
      Serial.println(currentPositionZ);
      Serial.println("Error, coudn't find range:");
      errorIndex = 1;
      while (entVal == HIGH) {
        entVal = digitalRead(ent);
      }
      break;
    }
  }
}
void productionMode() {
  if (keyModeIndex == 1) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Production Mode");
    Serial.println("Production Mode");
    keyModeIndex = 0;
    delay(2000);
    if (snMenuIndex == 0) {
      mainMenuIndex = 0;
    }
    if (snMenuIndex == 1) {
      snMenu();
    }
  }
}
void toolingMode() {

  if (keyModeIndex == 0) {
    digitalWrite(light, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Move Lift To:");
    lcd.setCursor(0, 1);
    lcd.print(liftHeight);
    lcd.print("mm");
    Serial.println("Move Lift");
    keyModeIndex = 1;
  }

  if ((upVal == LOW) && (liftHeight <= 21.00)) {
    liftHeight = liftHeight + .1;
    lcd.setCursor(0, 1);
    lcd.print("        ");
    lcd.setCursor(0, 1);
    lcd.print(liftHeight);
    lcd.print("mm");
  }

  if ((dwnVal == LOW) && (liftHeight >= -21.00)) {
    liftHeight = liftHeight - .1;
    lcd.setCursor(0, 1);
    lcd.print("        ");
    lcd.setCursor(0, 1);
    lcd.print(liftHeight);
    lcd.print("mm");
  }

  if (entVal == LOW) {
    autoLift();
  }

  else if (escVal == LOW) {
    homeLiftIndex = 1; // move to home
    keyModeIndex = 0;
  }
}

void processCmd() {
  // This function will run every time there is a command
  // You can then check the command and value and dictate the next action
  if (sdcc.getCmdS() == "partNum") { // getCmdS() returns Arduino String(), you can use == to compare
    partNum = sdcc.getValueS();//.toInt(); // getValueS() returns Arduino String(), you can use String.toInt() function
  }
  if ((barcodePartNum == partNum) || (barcodeIndex == 0)){ // only load if manual search or partNum is correct from scanner
  if (sdcc.getCmdS() == "serNum") { // getCmd() returns c-string, thus use strcmp
    sdSn = sdcc.getValueS(); // getValueS() returns Arduino String(), you can use String.toInt() function
    serNum = sdSn.toInt();
  }
  if (strcmp(sdcc.getCmd(), "serLeng") == 0) { // getCmd() returns c-string, thus use strcmp
    serLeng = sdcc.getValueInt(); // getValueInt converts the string value into int
  }

  if (sdcc.getCmdS() == "jobNum") { // getCmdS() returns Arduino String(), you can use == to compare
    jobNum = sdcc.getValueS();//.toInt(); // getValueS() returns Arduino String(), you can use String.toInt() function
    jobNumInt = jobNum.toInt();
  }
  if (sdcc.getCmdS() == "liftHeight") { // getCmd() returns c-string, thus use strcmp
    String liftHeightString = sdcc.getValueS(); // getValueS() returns Arduino String(), you can use String.toInt() function
    liftHeight = liftHeightString.toFloat();
  }
  }
}
void printDirectory(File dir, int numTabs) {
  if (searchIndex == 1) {
    dir.rewindDirectory();
    searchIndex = 0;
  }
  entry =  dir.openNextFile();
  if (!entry) { // manual search allows infinite looping
    dir.rewindDirectory();
    sdSearchTries = 1; // scaning auto search tries time out
    printDirectory(root, 0);
  }

  if (entry) {
    jobNumFile = entry.name();
    if (jobNumFile.equalsIgnoreCase(sysFolderName)) { // if statement skips system folder "SYSTEM~1"
      entry.close();
      printDirectory(root, 0);
    } else {
      while ( !(sdcc.set(jobNumFile, chipSelect, processCmd)) ) {} // initiates sd
      //sdcc.openInSerial(); //dumps contents of sd onto serial
      sdcc.readConfig(); // reads contents and turns values into variables
      entry.close();
    }
    entry.close();
  }
  entry.close();
}
void readBarcode() {
  if (barcodeFindIndex == 0) {
    barcode = Serial1.readStringUntil('\n');
  }
  if (barcode != "") {
    if (barcodeIndex == 0) {
      StringSplitter *splitter = new StringSplitter(barcode, '-', 4); // new StringSplitter(string_to_split, delimiter, limit)
      int itemCount = splitter->getItemCount();
      if (itemCount == 4) {
        String item1 = splitter->getItemAtIndex(0); // item index is the part in the string to display
        String item2 = splitter->getItemAtIndex(1);
        String item3 = splitter->getItemAtIndex(2);
        String item4 = splitter->getItemAtIndex(3);
        barcodePartNum = String(item1) + "-" + String(item2) + "-" + String(item3);
        barcodeSn = String(item4);
        barcodeSerialNum = barcodeSn.toInt(); // getValueInt converts the string value into int
        barcode = "";
        barcodeIndex = 1;
        searchIndex = 1;
      }
      else {
        Serial.println("Barcode needs to follow format xxx-xxxxxx-xxx-xxxx");
        barcode = "";
        barcodeIndex = 0;
        barcodeFindIndex = 0;

      }
    }
  }
}

void listDirectory(File dir, int numTabs) {
  dir.rewindDirectory();
  while (true) {
    entry =  dir.openNextFile();
    jobNumFile = entry.name();
    if (! entry) {
      // no more files
      break;
    }

    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }

    if (jobNumFile.equalsIgnoreCase(sysFolderName)) { // if statement skips system folder "SYSTEM~1"
      entry.close();
      listDirectory(entry, numTabs + 1);
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      listDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  //Serial.println("done");
  dir.rewindDirectory();
}
void homeLift() {
  if (homeLiftIndex == 1) {
    homeLiftIndex = 0; // reset value
    liftLimitVal = digitalRead(liftLimit);
    if (liftLimitVal == 0) {
      digitalWrite(en_Z, HIGH);
      currentPositionZ = 0;
      currentPositionZ = currentPositionZ - 4880; // 4880 is worst case
      stepperZ.setCurrentPosition(0);
      while (liftLimitVal == 0) {
        liftLimitVal = digitalRead(liftLimit);
        eStop();
        if (homeLiftIndex == 1) { // if estop was engage retry home
          break;
        }
        stepperZ.moveTo(currentPositionZ); // non blocking function 4800 steps is worst case
        stepperZ.run();
      }
      stepperZ.setCurrentPosition(0);
      stepperZ.runToNewPosition(-25); // blocking function
      digitalWrite(en_Z, LOW);
      stepperZ.setCurrentPosition(0);
      currentPositionZ = 0;
      keyModeIndex = 0; // display toolingMode on LCD
    }
    //    Serial.print("liftLimitVal,");
    //    Serial.println(liftLimitVal);
    //    Serial.print("homeLiftIndex,");
    //    Serial.println(homeLiftIndex);
  }
}

void fumeEnable() {
  fumeInVal = analogRead(fumeIn);
  if ((fumeInVal > 15) && (fumeInVal < 55)) {
    digitalWrite(fumeOut, HIGH);
    delay(200);
    digitalWrite(fumeOut, LOW);
  }
}

void fumeDisable() {
  fumeInVal = analogRead(fumeIn);
  if ((fumeInVal > 200)) {
    digitalWrite(fumeOut, HIGH);
    delay(200);
    digitalWrite(fumeOut, LOW);
  }
}

void eStop() {
  ESTOPVal = digitalRead(ESTOP);
  if (ESTOPVal == HIGH) {
    digitalWrite(en_Z, LOW);
    fumeDisable();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESTOP Engaged");
    Serial.println("ESTOP Engaged");
    Serial2.println("WX,Lighting=1");
    laserMessage = Serial2.readString();
    laserMessage = "";
    digitalWrite(light, HIGH);
    while (ESTOPVal == HIGH) {
      ESTOPVal = digitalRead(ESTOP);
    }
    error();
  }
}
void error() {
  ErrorVal = digitalRead(Error);
  if ((ErrorVal == HIGH) || (WarningVal == HIGH)) {
    Serial2.println("RX,Error");
    laserMessage = Serial2.readString();
    StringSplitter *splitter = new StringSplitter(laserMessage, ',', 4 );  // new StringSplitter(string_to_split, delimiter, limit)
    int itemCount = splitter->getItemCount();
    String item1 = splitter->getItemAtIndex(3);   //item index is the part in the string to display
    item1.trim();
    Serial.println(item1);
    Serial.println(laserMessage);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Press ENT to CLR");
    lcd.setCursor(0, 1);
    lcd.print("Error: ");
    lcd.setCursor(7, 1);
    lcd.print(item1);
    laserMessage = "";
  }
  while (ErrorVal == HIGH) {
    readInputs();
    if (entVal == LOW) {
      Serial2.println("WX,ErrorClear");
      laserMessage = Serial2.readString();
      laserMessage = "";
      mainMenuIndex = 0;
      entIndex = 0;
      snMenuIndex = 0;
      barcodeIndex = 0;
      foundBarcodeIndex = 0;
      homeLiftIndex = 1;
      liftLimitVal = digitalRead(liftLimit);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Error Cleared");
      Serial2.println("WX,Lighting=0");
      laserMessage = Serial2.readString();
      laserMessage = "";
      digitalWrite(light, LOW);
      break;
    }
  }
}
void readyTrig() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Warming up");
  while (ReadyVal == LOW) {
    readInputs();
    eStop();
    error();
  }
}
void notReady() {
  if ((ReadyVal == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH) && (MarkingVal == LOW)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Not Ready");
    while ((ReadyVal == LOW) && (MarkingVal == LOW)) {
      readInputs();
      eStop();
      error();
    }
    if (snMenuIndex == 1) {  //if in snMenu display sn Menu on lcd
      snMenu();
//      if (jobNumInt >= 1900) { // for config items that dont use SNs
//        lcd.setCursor(0, 1);
//        lcd.print("Press Ent");
//      }
    }
    else {
      mainMenuIndex = 0;    //else reset to main menu
    }
  }
}

void loop() {
  inputStr = Serial.readStringUntil('\n'); // serial input function
  store(inputStr); // serial input function
  readInputs();
  marking();
  complete();
  homeLift();
  eStop();
  error();
  if ((keyModeVal1 == HIGH) && (keyModeVal2 == HIGH)) { // productionMode
    notReady();
    readBarcode();
    searchSD();
    doors();
    returnToMain();
    upButton();
    entSnMenu();
    escButton();
    productionMode();
  }
  if ((keyModeVal1 == LOW) && (keyModeVal2 == LOW)) { // toolingMode
    toolingMode();
  }
}
