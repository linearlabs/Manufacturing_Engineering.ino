void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") {
    Serial5.println(idn);
  }

  if (startStr == "read?") {
    for (int i = 0; i < 15; i++) { // or i <= 14
      Serial5.print("Sensor ");
      Serial5.print(i);
      Serial5.print(", ");
      sensorValues[i] = analogRead(analog_pins[i]);
      sensorValueMaps[i] = map(sensorValues[i], 965, 620, 0, 100); // full weight about equals 11lbs
      if (sensorValueMaps[i] >= 95) {
        sensorValueMaps[i] = 100;
      }
      else if (sensorValueMaps[i] <= 5) {
        Serial5.print("Less than ");
        sensorValueMaps[i] = 5;
      }
      Serial5.print(sensorValueMaps[i]);
      Serial5.println("% left");
      delay(100);
    }
  }
  if (startStr == "read:") {
    analogPin = input.substring(5).toInt();
    if (continuousReadIndex == false) {
      continuousReadIndex = true;
    }
    else {
      continuousReadIndex = false;
    }
  }
  if (startStr == "help?") {
    Serial5.println("");
    Serial5.println("-*idn? for identification and version");
    Serial5.println("-read? to read all sensors once");
    Serial5.println("-read: to read all sensors continuously");
    Serial5.println("");
  }
  if (startStr == "stri?") {
    Serial5.println(continuousReadIndex);

  }
}
