void countRotations() {
  //  update every 100 ms
  //  should be enough up to ~200 RPM
  if (millis() - lastTime >= 100) {
    lastTime = millis();
    offsetPos = as5600.getCumulativePosition();
    rotations = as5600.getRevolutions();
  }
}

void resetEncoder() {
  //  set initial position
  as5600.resetPosition();
  offsetPos = as5600.getCumulativePosition();
  rotations = as5600.getRevolutions();
}
