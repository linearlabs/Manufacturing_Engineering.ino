void printInfo() {
  display.clearDisplay();
  display.setCursor(0, 0);  // Start at top-left corner
  display.print("IP: ");
  display.println(WiFi.localIP());
  display.print("MAC:");
  display.println(WiFi.macAddress());
  display.print("SSID:");
  display.println(ssid);
  display.print("Port = ");
  display.println(port);
  if (enableWebserial == 1) {
    display.print(WiFi.localIP());
    display.println("/webserial");
  }
  display.display();
  if (enableWebserial == 1) {
    WebSerial.print("MAC:");
    WebSerial.println(WiFi.macAddress());
    WebSerial.print("SSID:");
    WebSerial.println(ssid);
    WebSerial.print("Port = ");
    WebSerial.println(port);
  }
  delay(100);
}
