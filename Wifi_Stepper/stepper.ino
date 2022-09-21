void runStepper() {
  stepper.run(); // non blocking
  
  if (stepper.isRunning() == false) {
    rawPosistion();
    sendPosition();
    digitalWrite(enPin, HIGH);
    message = "moving, done";
    sendToAll();
    lastStep = steps;
    stepperRun = 0;
  }
}
