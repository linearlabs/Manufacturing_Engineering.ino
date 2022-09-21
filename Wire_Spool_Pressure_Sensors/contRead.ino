void continuousRead() { // full roll = 656 (pot wiper = 2.1V at full roll load.
  if (continuousReadIndex == true) {
    sensorValues[analogPin] = analogRead(analog_pins[analogPin]);
    Serial5.print("Sensor ");
    Serial5.print(analogPin);
    Serial5.print(", ");
    Serial5.print(sensorValues[analogPin]);
    delay(500);
  }
}
