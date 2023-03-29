void runStepper() {
  if (stepperSetup == false) {  // setup to sync steppers start and stop
    if (primaryStepper == 1) {
      digitalWrite(stepper_sync, LOW);  // goes low when stepper.isRunning()
      stepperSetup = true;
    } else {
      sync = digitalRead(stepper_sync);
      while (sync == HIGH) {  // wait for primary to send start (LOW GPIO)
        sync = digitalRead(stepper_sync);
      }
      stepperSetup = true;
    }
  }

  else {                                 // if stepperSetup = true then run steppers
    if (primaryStepper == 0) {           // if secondary
      sync = digitalRead(stepper_sync);  // secondary reads gpio from primary for stop
    }
    if (enableEncoder == 1) {
      countRotations();
    }
    if (stepperRun == true) {
      stepper.run();  // non blocking
    } else if (runSpd == true) {
      stepper.runSpeed();
    } else if (runWithoutAcceleration == true) {
      stepper.runSpeedToPosition();
    }

    if ((stepper.isRunning() == false) || (stepper.distanceToGo() == 0) || (sync == HIGH)) {  // logic to stop
      stopStepper();
    }
    commsWhileRunning();
  }
}
void runManual() {
  stepper.runSpeed();   // running forever
  commsWhileRunning();  // looking for stop: command
}
void runPrime() {
  stepper.runSpeedToPosition();
  if (stepper.distanceToGo() == 0) {
    stepper.stop();
    digitalWrite(enPin, HIGH);
    message = "priming done";
    sendToAll();
    primeIndex = 0;
  }
}
void runDeprime() {
  stepper.runSpeedToPosition();
  if (stepper.distanceToGo() == 0) {
    stepper.stop();
    digitalWrite(enPin, HIGH);
    message = "depriming done";
    sendToAll();
    deprimeIndex = 0;
  }
}

void commsWhileRunning() {
  if (d != "") {  // run serial commands if present
    if (enableWebserial == 1) {
      WebSerial.println(d);  // display message on web API (ip/webserial
    }
    ws.textAll(d);  // send message to all clients
    display.clearDisplay();
    display.setCursor(0, 0);  // Start at top-left corner
    display.println(d);
    display.display();
    store2(d);  // serial input function
    d = "";
  }
}
void store2(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "stop:") {
    stopStepper();
  }
}

void stopStepper() {
  if (primaryStepper == 1) {
    digitalWrite(stepper_sync, HIGH);  // goes low when stepper.isRunning()
  }
  stepper.stop();
  digitalWrite(enPin, HIGH);
  message = String("moving done, ");
  if ((enableEncoder == 1) && (manualRun == 0)) {  // manual run doesnt need encoder
    delay(1000);                                   // let stepper settle before measuring last possition
    positionCalculation = offsetPos / 4096;        // remove full rotations
    positionCalculation = positionCalculation - rotations;
    positionCalculation = positionCalculation * 360;  // convert to degrees
    message = message + String(rotations) + "rot + " + String(positionCalculation) + "deg";
  }
  sendToAll();
  runSpd = 0;
  stepperRun = 0;
  runWithoutAcceleration = 0;
  stepperSetup = false;
  manualRun = 0;
}