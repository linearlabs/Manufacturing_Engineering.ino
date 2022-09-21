void printInfo() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.print("IP: "); display.println(WiFi.localIP());
  display.print("MAC:"); display.println(WiFi.macAddress());
  display.print("SSID:"); display.println(ssid);
  display.print(WiFi.localIP()); display.println("/webserial");
  display.print("Port = "); display.println(port);
  display.display();
  //WebSerial.print("IP: ");
  //WebSerial.println(WiFi.localIP());
  WebSerial.print("MAC:");
  WebSerial.println(WiFi.macAddress());
  WebSerial.print("SSID:");
  WebSerial.println(ssid);
  WebSerial.print("Port = ");
  WebSerial.println(port);
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC:");
  Serial.println(WiFi.macAddress());
  Serial.print("SSID:");
  Serial.println(ssid);
  Serial.print("Port = ");
  Serial.println(port);
  // const char* ipadd1 = WiFi.localIP().toString().c_str(); // came close to converting IP to const char*
  // ws.textAll(IpAddress2String); // broken
  ws.textAll(WiFi.macAddress());
  ws.textAll(ssid);
  //ws.textAll(port); // broken
}
