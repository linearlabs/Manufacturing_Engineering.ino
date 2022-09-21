void checkComms() {
  if (dIndex == 1) { // if serial over wifi
    inputStr = d;
    dIndex = 0;
  }
  else if (inputStr == "") { // read serial over usb
    inputStr = Serial.readStringUntil('\n'); //serial to web server
  }
  else if (inputStr != "") { // run serial commands if present
    Serial.println(inputStr); // display message on serial
    WebSerial.println(inputStr); // display message on web API (ip/webserial
    ws.textAll(inputStr); // send message to all clients
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println(inputStr); //
    display.display();
    store(inputStr); // serial input function
    inputStr = "";
    d = "";
  }
  else if (WiFi.status() != WL_CONNECTED) { // reconnect to Wifi if signal is lost
    connectToWiFi();
  }
}
void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") {
    Serial.println(idn);
    WebSerial.println(idn);
    ws.textAll(idn);
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println(idn);
    display.display();
  }
  else if (startStr == "print") { // serial input help variable
    printInfo();
  }
  else if (startStr == "help?") { // serial input help variable
    message = "*idn? for unit ID";
    sendToAll();
    message = "print displays info";
    sendToAll();
    message = "move: blocking func to move stepper";
    sendToAll();
    message = "dire? returns direction or no movement";
    sendToAll();
  }
  else if (startStr == "move:") {
    String stepS = input.substring(5);
    steps = input.substring(5).toInt();
    message = "moving, " + stepS;
    sendToAll();
    //Serial.print("moving, ");
    //Serial.println(steps);
    digitalWrite(enPin, LOW);
    stepper.runToNewPosition(steps); // blocking
    //stepper.moveTo(steps); // non blocking
    //stepper.run(); // non blocking
    digitalWrite(enPin, HIGH);
    message = "moving, done";
    sendToAll();
  }
  else if (startStr == "runx:") {
    String stepS = input.substring(5);
    steps = input.substring(5).toInt();
    strip.clear();
    strip.show();
    message = "Moving to, " + stepS;
    sendToAll();
    rawPosistion();
    sendPosition();
    digitalWrite(enPin, LOW);
    stepperRun = 1;
    stepper.moveTo(steps); // non blocking
  }
  else if (startStr == "epos?") { // encoder position
    rawPosistion();
    sendPosition();
  }
  else if (startStr == "reset") { // encoder direction
    resetEncoder();
  }
  else if (startStr == "half:") { // half step mode
    bool halfSteps = input.substring(5).toInt();
    if (halfSteps == 1) {
      digitalWrite(halfStep, HIGH); // en_X_State is variable high or low
      message = "steps, halfStep ";
    }
    else if (halfSteps == 0) {
      digitalWrite(halfStep, LOW); // en_X_State is variable high or low
      message = "steps, fullStep ";
    }
    sendToAll();
  }
}
