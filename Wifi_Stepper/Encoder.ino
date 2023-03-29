void sendPosition(){
  String degreeS = String(offsetPos);
  message = "Encoder Pos, " + degreeS;
  sendToAll();
}
void rawPosistion() {
  degree = as5600.rawAngle() * AS5600_RAW_TO_DEGREES; // read sensor
}
void currentPosistion() {
  degree = as5600.rawAngle() * AS5600_RAW_TO_DEGREES; // read sensor
  if ((degree >= firstPos) && (degree <= 360)) {
    offsetPos = degree - firstPos;
  }
  else if ((degree <= firstPos) && (degree >= 0)) {
    offsetPos = 360 - firstPos + degree;
  }
}
void countRotations() {
  currentPosistion();
  delta = offsetPos - lastPos;
  if (delta <= -270) {
    rotations++;
    //cw = true;
    //movement = true;
  }
  else if (delta >= 270) {
    rotations--;
    //cw = false;
    //movement = true;
  }
//  else if ((delta < 270) && (delta > 0)) {
//    cw = true;
//    movement = true;
//  }
//  else if ((delta > -270) && (delta < 0)) {
//    cw = false;
//    movement = true;
//  }
//  else if (delta == 0) {
//    movement = false;
//  }
  //rotationIndex = fullTurn / 360;  // = 0.00716 in/deg
  //rotationIndex = rotationIndex * offsetPos;
  Serial.print("Rotations=");
  Serial.println(rotations);
//  Serial.print("CW=");
//  Serial.println(cw);
//  Serial.print("Movement=");
//  Serial.println(movement);
  delta = 0;
  if (offsetPos != lastPos) {
    lastPos = offsetPos;
  }
}
void resetEncoder() {
  stepper.setCurrentPosition(0);
  offsetPos = 0.00;
  lastPos = 0.00;
  rotations = 0;
  rotationIndex = 0.00;
  delta = 0.00;
  degree = as5600.rawAngle() * AS5600_RAW_TO_DEGREES; // read sensor
  firstPos = degree;
}
void encoderDirection() {
  if (movement == true) {
    if (cw == true) {
      Serial.println("direction, CW");
    }
    if (cw == false) {
      Serial.println("direction, CCW");
    }
  }
  else {
    Serial.println("direction, No Movement");
  }
}
