#include <LiquidCrystal.h>
#include <SDConfigCommand.h>
#include <StringSplitter.h>
#define  chipSelect 53
#define leftDoor 2
#define rightDoor 3
#define warpDrive1 4
#define warpDrive2 5
#define up 48
#define dwn 47
#define ent 46
#define esc 45
#define light 22
#define ESTOP 30
#define Error 31
#define Warning 32
#define Ready 33
#define Marking 34
#define Complete 35
LiquidCrystal lcd(36, 37, 41, 40, 39, 38);

bool upVal; // up
bool dwnVal; // down
bool entVal; // ent
bool escVal; // esc
bool RDoorVal;                                   //rightDoor
bool LDoorVal;                                   //leftDoor
bool warpVal1;                                   //warpDrive1
bool warpVal2;                                   //warpDrive2
bool ESTOPVal;
bool ErrorVal;
bool WarningVal;
bool ReadyVal;
bool MarkingVal;
bool CompleteVal;
bool completeValIndex;
bool mainMenuIndex;
bool doorIndex;
bool warpIndex;
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
String barcode = "";
String laserMessage = "";
String barcodePartNum = "";
String barcodeSn = "";
String sdSn = ""; // sd sernum read as a string before converting to long int
String jobNumFile = "";
String jobNum = "";
String partNum = "";
String sysFolderName = "SYSTEM~1"; // sdcard system folder name to ignore
unsigned long serNum; // max is 4,294,967,295
unsigned long barcodeSerialNum; // max is 4,294,967,295
int serLeng; // number of Serial characters, max is 10
int numTabs;
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
  lcd.begin(16, 2); // LCD one time set up
  lcd.createChar(0, Check); // lcd custom character
  pinMode (up, INPUT_PULLUP);
  pinMode (dwn, INPUT_PULLUP);
  pinMode (ent, INPUT_PULLUP);
  pinMode (esc, INPUT_PULLUP);
  pinMode(rightDoor, INPUT_PULLUP);
  pinMode(leftDoor, INPUT_PULLUP);
  pinMode(warpDrive1, INPUT_PULLUP);
  pinMode(warpDrive2, INPUT_PULLUP);
  pinMode(ESTOP, INPUT_PULLUP);
  pinMode(Error, INPUT_PULLUP);
  pinMode(Warning, INPUT_PULLUP);
  pinMode(Ready, INPUT_PULLUP);
  pinMode(Marking, INPUT_PULLUP);
  pinMode(Complete, INPUT_PULLUP);
  pinMode (light, OUTPUT);
  mainMenuIndex = 0;
  doorIndex = 0;
  warpIndex = 0;
  upIndex = 0;
  snMenuIndex = 0;
  entIndex = 0;
  barcodeIndex = 0;
  barcodeFindIndex = 0; // finding barcode with serial input command find:
  foundBarcodeIndex = 0; // finding barcode with barcode scanner
  writeIndex = 0;
  readIndex = 0; // index for serial input command read:
  searchIndex = 0;
  sdSearchTries = 0; // bool val to know when all configs have been read
  laserIndex = 0;
  completeValIndex = 0; // from laser indicating job done
  Serial.begin(115200); // Arduino USB UART
  Serial.setTimeout(50);
  Serial1.begin(115200); // barcode scanner USB UART
  Serial1.setTimeout(50);
  Serial2.begin(115200); // laser etcher USB UART
  Serial2.setTimeout(100);
  Serial.println("For help type help?");
  pinMode(chipSelect, OUTPUT);
  SD.begin(53);
  root = SD.open("/");
  sdcc.set(jobNumFile, chipSelect, processCmd);
  readyTrig();
}
void readInputs() {
  upVal = digitalRead(up);
  dwnVal = digitalRead(dwn);
  entVal = digitalRead(ent);
  escVal = digitalRead(esc);
  RDoorVal = digitalRead(rightDoor);
  LDoorVal = digitalRead(leftDoor);
  warpVal1 = digitalRead(warpDrive1);
  warpVal2 = digitalRead(warpDrive2);
  ESTOPVal = digitalRead(ESTOP);
  ErrorVal = digitalRead(Error);
  WarningVal = digitalRead(Warning);
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
  }
}

void pnMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(partNum);
  Serial.print(jobNumFile);
  Serial.print(" ,");
  Serial.print(partNum);
  Serial.print(" ,");
  Serial.print(serNum);
  Serial.print(" ,");
  Serial.print(jobNum);
  Serial.print(" ,");
  Serial.println(serLeng);
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
void formatSn() {
  if ((serLeng == 10) && (serNum <= 4294967296)) {       //9 digit serNum
    if (serNum >= 4294967296) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0000000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000000000" + String(serNum);
    }
  }
  if ((serLeng == 9) && (serNum <= 1000000000)) {       //9 digit serNum
    if (serNum == 1000000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00000000" + String(serNum);
    }
  }
  if ((serLeng == 8) && (serNum <= 100000000)) {       //8 digit serNum
    if (serNum == 100000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "000000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0000000" + String(serNum);
    }
  }
  if ((serLeng == 7) && (serNum <= 10000000)) {       //7 digit serNum
    if (serNum == 10000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000000" + String(serNum);
    }
  }
  if ((serLeng == 6) && (serNum <= 1000000)) {        //6 digit serNum
    if (serNum == 1000000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00000" + String(serNum);
    }
  }
  if ((serLeng == 5) && (serNum <= 100000)) {        //5 digit serNum
    if (serNum == 100000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "000" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0000" + String(serNum);
    }
  }
  if ((serLeng == 4) && (serNum <= 10000)) {        //4 digit serNum
    if (serNum == 10000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 1000) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "00" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "000" + String(serNum);
    }
  }
  if ((serLeng == 3) && (serNum <= 1000)) {        //3 digit serNum
    if (serNum == 1000) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 100) {
      barcodeSn = "0" + String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "00" + String(serNum);
    }
  }
  if ((serLeng == 2) && (serNum <= 100)) {            //2 digit serNum
    if (serNum == 100) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
    if (serNum < 10) {
      barcodeSn = "0" + String(serNum);
    }
  }
  if ((serLeng == 1) && (serNum <= 10)) {           //1 digit serNum
    if (serNum == 10) {
      serNum = 1;
      barcodeSn = String(serNum);
    }
  }
}

void snMenu() {
  barcodeSn = String(serNum);
  formatSn();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(partNum);
  lcd.setCursor(0, 1);
  lcd.print("SN:");
  lcd.setCursor(3, 1);
  lcd.print(barcodeSn);
  Serial.print("PN:");
  Serial.println(partNum);
  Serial.print("SN:");
  Serial.println(barcodeSn);
  Serial.print("JN:");
  Serial.println(jobNum);
}

void deleteConfigs() { // with serial input delete config
  Serial.println("Removing config file...");
  SD.remove(jobNumFile);
  if (SD.exists(jobNumFile)) {
    Serial.println("Config failed to delete.");
  } else {
    Serial.println("Config removed successfully.");
  }
}
void writeConfigs() { // with serial input write new config
  deleteConfigs();
  Serial.println("Creating config...");
  entry = SD.open(jobNumFile, FILE_WRITE); //must delete file first because there isnt an eddit function
  entry.print("partNum=");
  entry.println(partNum);
  entry.print("serNum=");
  entry.println(serNum);       //save the next serNum to use
  entry.print("serLeng=");
  entry.println(serLeng);
  entry.print("jobNum=");
  entry.println(jobNum);
  entry.close();
  if (SD.exists(jobNumFile)) {
    Serial.println("Created config.");
  } else {
    Serial.println("Failed to create config.");
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
  entry.close();
  if (SD.exists(jobNumFile)) {
    Serial.println("Created config.");
  } else {
    Serial.println("Failed to create config.");
  }
}

void marking() {
  while (MarkingVal == HIGH) {
    MarkingVal = digitalRead(Marking);
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      if (lcdState == LOW) {
        lcdState = HIGH;
        lcd.setCursor(13, 1);
        lcd.print("Run");
      } else {
        lcdState = LOW;
        lcd.setCursor(13, 1);
        lcd.print("   ");
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
    lcd.setCursor(13, 1);
    lcd.write(byte(0));
    lcd.setCursor(14, 1);
    lcd.write(byte(0));
    lcd.setCursor(15, 1);
    lcd.write(byte(0));
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
    }
  }
  else if (foundBarcodeIndex == 1) {
    snMenuIndex = 1;
    serNum = barcodeSerialNum;
    snMenu();
  }
  while (snMenuIndex == 1) {
    inputStr = Serial.readStringUntil('\n'); //serial input function
    store(inputStr); // serial input function
    readInputs();
    eStop();
    error();
    doors();
    warpDrive();
    escButton();
    marking();
    notReady();

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
        laserIndex = 1;
        sendCmd();
        deleteWriteConfig();
        mainMenuIndex = 0;
        serNum = serNum + 1;
        barcode = "";
        entIndex = 0;
        foundBarcodeIndex = 0;
        snMenu();
        delay(200); // button debouncing delay
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

void sendCmd() {
  while (laserIndex == 1) {
    Serial2.println("WX,JOB=" + jobNum + ",BLK=000,CharacterString=" + partNum + "-" + barcodeSn);
    while (laserMessage == "") {
      laserMessage = Serial2.readString();
    }
    Serial.println(laserMessage);
    laserMessage = "";
    delay(200);
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
      }
      doorIndex = 1;
      Serial.println("Doors Closed");
      //Serial2.println("WX,Lighting=0");
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
      Serial2.println("WX,Lighting=1");
      laserMessage = Serial2.readString();
      laserMessage = "";
      digitalWrite(light, HIGH);
      doorIndex = 1;
    }
  }
}

void warpDrive() {
  if ((warpVal1 == HIGH) && (warpVal2 == HIGH) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) {
    if (warpIndex == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Warp Drive");
      Serial.println("Warp Drive");
      warpIndex = 1;
      delay(2000);
      if (snMenuIndex == 0) {
        mainMenuIndex = 0;
      }
      if (snMenuIndex == 1) {
        snMenu();
      }
    }
  }
  if ((warpVal1 == LOW) && (warpVal2 == LOW) && (RDoorVal == HIGH) && (LDoorVal == HIGH)) {
    if (warpIndex == 1) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SubLight Engines");
      Serial.println("SubLight Engines");
      warpIndex = 0;
      delay(2000);
      if (snMenuIndex == 0) {
        mainMenuIndex = 0;
      }
      if (snMenuIndex == 1) {
        snMenu();
      }
    }
  }
}

void processCmd() {
  // This function will run every time there is a command
  // You can then check the command and value and dictate the next action

  if (sdcc.getCmdS() == "serNum") { // getCmd() returns c-string, thus use strcmp
    sdSn = sdcc.getValueS(); // getValueS() returns Arduino String(), you can use String.toInt() function
    serNum = sdSn.toInt();
  }
  if (strcmp(sdcc.getCmd(), "serLeng") == 0) { // getCmd() returns c-string, thus use strcmp
    serLeng = sdcc.getValueInt(); // getValueInt converts the string value into int
  }
  if (sdcc.getCmdS() == "partNum") { // getCmdS() returns Arduino String(), you can use == to compare
    partNum = sdcc.getValueS();//.toInt(); // getValueS() returns Arduino String(), you can use String.toInt() function
  }
  if (sdcc.getCmdS() == "jobNum") { // getCmdS() returns Arduino String(), you can use == to compare
    jobNum = sdcc.getValueS();//.toInt(); // getValueS() returns Arduino String(), you can use String.toInt() function
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
    File entry =  dir.openNextFile();
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
  Serial.println("done");
  dir.rewindDirectory();
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "help?") { // HELP COMMAND
    Serial.println();
    Serial.println("*idn? returns PN SN and Software Ver of Arduino PLC");
    Serial.println("jnum:xxxx writes jobNum and jobNumFile");
    Serial.println("snum:xxxx writes serial number");
    Serial.println("pnum:xxx-xxxxxx-xxx writes partnumber");
    Serial.println("find:xxx-xxxxxx-xxx-xxxx finds config from partnumber entered");
    Serial.println("leng:1 - 10 writes length of serial number digits");
    Serial.println("snum? reads serial number");
    Serial.println("jnum? reads job number");
    Serial.println("pnum? reads partnumber");
    Serial.println("leng? reads length of serial number digits");
    Serial.println("file? reads jobNumFile");
    Serial.println("print displays jobNumFile,partNum,serNum,jobNum,SerLeng");
    Serial.println("read:XXXX.TXT reads specified text file from SD config");
    Serial.println("load:XXXX.TXT reads and loads config variables from SD card");
    Serial.println("dele:XXXX.TXT deletes text file from SD config");
    Serial.println("list: returns all config files stored on SD");
    Serial.println("rite: writes text file to SD config with current variables please verify variables first with print");
    Serial.println();
  }
  else if ((startStr == "main:") && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { // goes back to main menu, the same as escape button
    mainMenuIndex = 0;
    snMenuIndex = 0;
    barcodeIndex = 0;
    foundBarcodeIndex = 0;
    barcode = "";
  }
  if (snMenuIndex == 0) {
    if (startStr == "jnum:") { // serial input
      jobNum = input.substring(5);
      Serial.print("jobNum, ");
      Serial.println(jobNum);
      jobNumFile = jobNum + ".TXT";
      Serial.print("jobNumFile, ");
      Serial.println(jobNumFile);
    }
    else if (startStr == "snum:") { // serial input
      serNum = input.substring(5).toInt();
      Serial.print("serNum, ");
      Serial.println(serNum);
    }
    else if (startStr == "print") { // serial input
      pnMenu();
    }
    else if (startStr == "pnum:") { // serial input
      partNum = input.substring(5);
      Serial.print("partNum, ");
      Serial.println(partNum);
    }
    else if (startStr == "list:") { // serial input
      listDirectory(root, 0);
    }
    else if (startStr == "leng:") { // serial input
      serLeng = input.substring(5).toInt();
      Serial.print("serLeng, ");
      Serial.println(serLeng);
    }
    else if (startStr == "jnum?") { // serial input
      Serial.print("jobNum, ");
      Serial.println(jobNum);
    }
    else if (startStr == "snum?") { // serial input
      Serial.print("serNum, ");
      Serial.println(serNum);
    }
    else if (startStr == "pnum?") { // serial input
      Serial.print("partNum, ");
      Serial.println(partNum);
    }
    else if (startStr == "leng?") { // serial input
      Serial.print("serLeng, ");
      Serial.println(serLeng);
    }
    else if (startStr == "file?") { // serial input
      Serial.print("jobNumFile, ");
      Serial.println(jobNumFile);
    }

    else if (startStr == "pnum?") { // serial input
      Serial.print("partNum, ");
      Serial.println(partNum);
    }

    else if (startStr == "rite:") { // serial input
      if (partNum != "") {
        if (serLeng != 0) {
          if (serNum > 0) {
            if (jobNumFile != "") {
              writeConfigs();
            }

            else {
              Serial.println("No jobNumFile entered");
            }
          }
          else {
            Serial.println("No serial number entered");
          }
        }

        else {
          Serial.println("No serial length entered");
        }
      }
      else {
        Serial.println("No partNumber entered");
      }
    }

    else if (startStr == "dele:") { // deletes specified jobNumFile "dele,0020.TXT"
      jobNumFile = input.substring(5);
      Serial.print("deleteFile, ");
      Serial.println(jobNumFile);
      deleteConfigs();
    }
    else if (startStr == "*idn?") {
      Serial.print("LASER ETCHER, Dr. Evil, V1.3\r\n");
    }
    else if (startStr == "read:") {
      jobNumFile = input.substring(5);
      Serial.print("readFile, ");
      Serial.println(jobNumFile);
      SD.open(jobNumFile);
      if (SD.exists(jobNumFile)) {
        Serial.println("Config opened.");
        while ( !(sdcc.set(jobNumFile, chipSelect, processCmd)) ) {} // initiates sd
        sdcc.openInSerial(); //dumps contents of sd onto serial
        //sdcc.readConfig(); // reads contents and turns values into variables
      }
      else {
        Serial.println("Config doesn't exist.");
      }
    }

    else if (startStr == "load:") { // deletes specified jobNumFile "dele,0020.TXT"
      jobNumFile = input.substring(5);
      Serial.print("loadFile, ");
      Serial.println(jobNumFile);
      SD.open(jobNumFile);
      if (SD.exists(jobNumFile)) {
        Serial.println("Config opened.");
        while ( !(sdcc.set(jobNumFile, chipSelect, processCmd)) ) {} // initiates sd
        sdcc.openInSerial(); //dumps contents of sd onto serial
        sdcc.readConfig(); // reads contents and turns values into variables
        pnMenu();
      }
      else {
        Serial.println("Config doesn't exist.");
      }
    }
    else if (startStr == "find:") { // deletes specified jobNumFile "dele,0020.TXT"
      barcode = input.substring(5);
      Serial.println(barcode);
      barcodeFindIndex = 1;
      readBarcode();
    }
  }
}

void eStop() {
  if (ESTOPVal == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("ESTOP Engaged");
    Serial.println("ESTOP Engaged");
    Serial2.println("WX,Lighting=1");
    laserMessage = Serial2.readString();
    laserMessage = "";
    digitalWrite(light, HIGH);

  }
  while (ESTOPVal == HIGH) {
    readInputs();
  }
}
void error() {
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
  eStop();
  error();
  readBarcode();
  searchSD();
  doors();
  marking();
  complete();
  returnToMain();
  upButton();
  entSnMenu();
  escButton();
  warpDrive();
  notReady();
}
