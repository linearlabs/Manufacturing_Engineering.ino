void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "help?") { // HELP COMMAND
    Serial.println();
    Serial.println("*idn? returns PN SN and Software Ver of Arduino PLC");
    Serial.println("main: returns to main menu");
    Serial.println("pnum:xxx-xxxxxx-xxx writes partnumber");
    Serial.println("snum:xxxx writes serial number");
    Serial.println("leng:1 - 10 writes serial number length");
    Serial.println("jnum:xxxx writes jobNum");
    Serial.println("file:xxxx.TXT writes jobNumFile");
    Serial.println("high: lift table hieght");
    Serial.println("find:xxx-xxxxxx-xxx-xxxx finds partnumber config");
    Serial.println("print displays jobNumFile,jobNum,partNum,serNum,SerLeng");
    Serial.println("read:XXXX.TXT reads specified text file from SD config");
    Serial.println("load:XXXX.TXT loads config variables from SD card");
    Serial.println("dele:XXXX.TXT deletes text file from SD config");
    Serial.println("list: returns all config files stored on SD");
    Serial.println("rite: writes new text file to SD with loaded variables");
    Serial.println("fume:1/0 enable / disable fume extractor");
    Serial.println("lite:1/0 enable / disable light");
    Serial.println("lift:1/0 enable / disable lift stepper");
    Serial.println("home: lift moves to home");
    Serial.println("limi? is lift at home limit");
    Serial.println("rang: laser range finder");
    Serial.println("mvup: 0-4800 move lift up by this many steps");
    Serial.println("mvdn: 4800-0 move lift down by this many steps");
    Serial.println("step? queries curentPositionZ");
    Serial.println();
  }
  else if ((startStr == "main:") && (RDoorVal == HIGH) && (LDoorVal == HIGH)) { // goes back to main menu, the same as escape button
    mainMenuIndex = 0;
    snMenuIndex = 0;
    barcodeIndex = 0;
    foundBarcodeIndex = 0;
    barcode = "";
  }
  if (startStr == "jnum:") { // serial input
    jobNum = input.substring(5);
    Serial.print("jobNum, ");
    Serial.println(jobNum);
  }
  else if (startStr == "file:") { // serial input
    jobNumFile = input.substring(5);
    jobNumFile.trim();
    Serial.print("jobNumFile, ");
    Serial.println(jobNumFile);
  }
  else if (startStr == "snum:") { // serial input
    serNum = input.substring(5).toInt();
    Serial.print("serNum, ");
    Serial.println(serNum);
  }
  else if (startStr == "pnum:") { // serial input
    partNum = input.substring(5);
    Serial.print("partNum, ");
    Serial.println(partNum);
  }
  else if (startStr == "leng:") { // serial input
    serLeng = input.substring(5).toInt();
    Serial.print("serLeng, ");
    Serial.println(serLeng);
  }
  else if (startStr == "high:") { // serial input
    liftHeight = input.substring(5).toFloat();
    Serial.print("liftHeight, ");
    Serial.println(liftHeight);
  }
  else if (startStr == "print") { // serial input
    pnMenu();
  }
  else if (startStr == "list:") { // serial input
    listDirectory(root, 0);
  }
  else if (startStr == "rite:") { // serial input
    if (partNum != "") {
      if (serLeng != 0) {
        if (serNum > 0) {
          if (jobNumFile != "") {
            jobNumFile.trim();
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
    jobNumFile.trim();
    Serial.print("deleteFile, ");
    Serial.println(jobNumFile);
    deleteConfigs();
  }
  else if (startStr == "*idn?") {
    Serial.print("LASER ETCHER, Dr. Evil, V1.5\r\n");
  }
  else if (startStr == "read:") {
    jobNumFile = input.substring(5);
    jobNumFile.trim();
    //Serial.print("readFile, ");
    //Serial.println(jobNumFile);
    SD.open(jobNumFile);
    if (SD.exists(jobNumFile)) {
      //Serial.println("Config opened.");
      while ( !(sdcc.set(jobNumFile, chipSelect, processCmd)) ) {} // initiates sd
      sdcc.openInSerial(); //dumps contents of sd onto serial (read only)
      //sdcc.readConfig(); // reads contents and turns values into variables (used to load variables)
    }
    else {
      Serial.println("Config doesn't exist.");
    }
  }
  else if (startStr == "load:") { // loads variables in jobNumFile"
    jobNumFile = input.substring(5);
    jobNumFile.trim();
    //Serial.print("loadFile, ");
    //Serial.println(jobNumFile);
    SD.open(jobNumFile);
    if (SD.exists(jobNumFile)) {
      //Serial.println("Config opened.");
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
  else if (startStr == "fume:") { // move down
    fumeIndex = input.substring(5).toInt();
    if (fumeIndex == 1) {
      fumeEnable();
    }
    else {
      fumeDisable();
    }
  }

  else if (startStr == "lite:") { // move down
    lightIndex = input.substring(5).toInt();
    digitalWrite(light, lightIndex);
  }
  else if (startStr == "lift:") { // enable 1 or disable 0 lift
    liftIndex = input.substring(5).toInt();
    digitalWrite(en_Z, liftIndex);
    Serial.print("liftEnable=");
    Serial.println(liftIndex);
  }
  else if (startStr == "limi?") { // is lift at home?
    liftIndex = input.substring(5).toInt();
    liftLimitVal = digitalRead(liftLimit);
    Serial.print("liftLimit=");
    Serial.println(liftLimitVal);;
  }
  else if (startStr == "home:") { // go to home position
    homeLiftIndex = 1; // 1 = go home
  }
  else if (startStr == "rang:") { // use laser range finder
    rangeFind();
  }
  if (startStr == "mvup:") { // move up
    addSteps = input.substring(5).toInt();
    currentPositionZ = currentPositionZ + addSteps;
    if (currentPositionZ < 4800) {
      stepperZ.runToNewPosition(currentPositionZ);
      Serial.println(currentPositionZ);
    }
    else {
      Serial.print("out of bounds:");
      Serial.println(currentPositionZ);
      currentPositionZ = currentPositionZ - addSteps;
    }
  }
  else if (startStr == "mvdn:") { // move down
    minusSteps = input.substring(5).toInt();
    currentPositionZ = currentPositionZ - minusSteps;
    if (currentPositionZ >= 0) {
      stepperZ.runToNewPosition(currentPositionZ);
      Serial.println(currentPositionZ);
    }
    else {
      Serial.print("out of bounds:");
      Serial.println(currentPositionZ);
      currentPositionZ = currentPositionZ + minusSteps;
    }
  }
  else if (startStr == "step?") { // move down
    Serial.print("currentPositionZ:");
    Serial.println(currentPositionZ);
  }
}
