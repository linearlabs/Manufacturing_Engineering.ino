void checkComms() {
  if (d != "") {  // run serial commands if present
    //WebSerial.println(d);  // display message on web API (ip/webserial
    ws.textAll(d);  // send message to all clients
    display.clearDisplay();
    display.setCursor(0, 0);  // Start at top-left corner
    display.println(d);       //
    display.display();
    store(d);  // serial input function
    d = "";
  } else if (WiFi.status() != WL_CONNECTED) {  // reconnect to Wifi if signal is lost
    connectToWiFi();
  }
}
void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") {
    message = idn;
    sendToAll();
  } else if (startStr == "print") {  // serial input help variable
    printInfo();
  } else if (startStr == "help?") {  // serial input help variable
    message = "*idn? for unit ID";
    message = message + "\nprint displays info";
    message = message + "\nmove: blocking func to move stepper";
    message = message + "\ngogo: non-block/non-accel move forever until stop:";
    message = message + "\ngoto: non-block/non-accel move to position";
    message = message + "\nrunx: non-block with accel move stepper";
    message = message + "\nmaxs: set stepper MAX speed (800 max at 12V)";
    message = message + "\nspee: set stepper RUN speed";
    message = message + "\nacce: set stepper acceleration (500 default)";
    message = message + "\nreset resets encoder position";
    message = message + "\nhalf: 1=halfstep ; 0=fullstep";
    message = message + "\nzero:0 sets current stepper possition to zero";
    message = message + "\nlogo: displays logo";
    message = message + "\nenen: enable encoder, nonblocking movement only";
    message = message + "\nwser: enable webserial";
    message = message + "\nprim: 1=primary stepper || 0=secondary";
    message = message + "\nenst: enable stepper 1 or 0";
    WebSerial.println(message);
    message = "";
  } else if (startStr == "manu:") {  // manual run, runs forever (one stepper only) like gogo:
    manualRun = input.substring(5).toInt();
    digitalWrite(enPin, LOW);
    stepper.setMaxSpeed(stepperMaxSpeed);
    stepper.setSpeed(stepperSpeed);
  } else if (startStr == "prim:") {  // primary stepper sync output || secondary stepper sync input
    primaryStepper = input.substring(5).toInt();
    if (primaryStepper == 1) {
      pinMode(stepper_sync, OUTPUT);
      digitalWrite(stepper_sync, HIGH);  // goes low when stepper.isRunning()
    } else {
      pinMode(stepper_sync, INPUT);
    }
  } else if (startStr == "prsp:") {  // priming speed
    primeSpeed = input.substring(5).toInt();
  } else if (startStr == "prst:") {  // priming steps
    primeSteps = input.substring(5).toInt();
  } else if (startStr == "desp:") {  // depriming speed
    deprimeSpeed = input.substring(5).toInt();
  } else if (startStr == "dest:") {  // depriming steps
    deprimeSteps = input.substring(5).toInt();
  } else if (startStr == "enen:") {
    enableEncoder = input.substring(5).toInt();
  } else if (startStr == "wser:") {
    enableWebserial = input.substring(5).toInt();
    if ((enableWebserial == 1) && (enableWebserialIndex == 0)) {
      WebSerial.begin(&server);
      WebSerial.msgCallback(recvMsg);
      enableWebserialIndex = 1;
    }
  } else if (startStr == "zero:") {
    currentPosition = input.substring(5).toInt();
    stepper.setCurrentPosition(currentPosition);
  } else if (startStr == "logo:") {
    bitMap();
  } else if (startStr == "acce:") {
    stepperAcceleration = input.substring(5).toInt();
    stepper.setAcceleration(stepperAcceleration);
  } else if (startStr == "spee:") {
    stepperSpeed = input.substring(5).toInt();
    stepper.setSpeed(stepperSpeed);
  } else if (startStr == "enst:") {
    int enStepper = input.substring(5).toInt();
    if (enStepper == 1){
      digitalWrite(enPin, LOW);
    }
    else{
      digitalWrite(enPin, HIGH);
    }
  }else if (startStr == "maxs:") {
    stepperMaxSpeed = input.substring(5).toInt();
    stepper.setMaxSpeed(stepperMaxSpeed);
  } else if (startStr == "move:") {
    steps = input.substring(5).toInt();
    digitalWrite(enPin, LOW);
    stepper.setAcceleration(stepperAcceleration);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.runToNewPosition(steps);  // blocking
    digitalWrite(enPin, HIGH);
  } else if (startStr == "runx:") {
    steps = input.substring(5).toInt();
    digitalWrite(enPin, LOW);
    stepperRun = 1;
    stepper.setAcceleration(stepperAcceleration);
    stepper.setMaxSpeed(stepperSpeed);
    stepper.setSpeed(stepperSpeed);
    stepper.moveTo(steps);  // non blocking
  } else if (startStr == "goto:") {
    steps = input.substring(5).toInt();
    digitalWrite(enPin, LOW);
    runWithoutAcceleration = 1;
    stepper.setMaxSpeed(stepperMaxSpeed);
    stepper.moveTo(steps);  // non blocking
    stepper.setSpeed(stepperSpeed);
  }

  else if (startStr == "stpr:") {  // stepper prime, prime goop line (move without acceleration)
    primeIndex = 1;
    digitalWrite(enPin, LOW);
    stepper.setCurrentPosition(0);
    stepper.setMaxSpeed(stepperMaxSpeed);
    stepper.moveTo(primeSteps);  // non blocking
    stepper.setSpeed(primeSpeed);
  } else if (startStr == "stde:") {
    deprimeIndex = 1;
    stepper.setCurrentPosition(0);
    digitalWrite(enPin, LOW);
    stepper.setMaxSpeed(stepperMaxSpeed);
    stepper.moveTo(deprimeSteps);  // non blocking
    stepper.setSpeed(deprimeSpeed);
  }

  else if (startStr == "gogo:") {  // runs forever
    digitalWrite(enPin, LOW);
    runSpd = 1;
    stepper.setMaxSpeed(stepperMaxSpeed);
    stepper.setSpeed(stepperSpeed);
  } else if (startStr == "reset") {  // encoder direction
    resetEncoder();
  } else if (startStr == "half:") {  // half step mode
    halfStep_Val = input.substring(5).toInt();
    if (halfStep_Val == 1) {
      digitalWrite(halfStep, HIGH);
    } else {
      digitalWrite(halfStep, LOW);
    }
  }
}
