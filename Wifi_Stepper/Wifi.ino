void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println("Connecting To");
  display.println(ssid);
  display.println("WiFi");
  display.println("For help type help?");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}
void recvMsg(uint8_t *data, size_t len) { // web server to serial
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  dIndex = 1;
}
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) { // for python webserial
  if (type == WS_EVT_CONNECT) { // client connect event
    message = "for help type help?";
    sendToAll();
  } else if (type == WS_EVT_DISCONNECT) { // client disconnect event
    Serial.println("Client disconnected");
  }
  else if (type == WS_EVT_DATA) {
    for (int i = 0; i < len; i++) {
      d += char(data[i]);
    }
    dIndex = 1;
  }
}
void sendToAll(){
  Serial.println(message); // send over usb
  WebSerial.println(message); // send to webserial API
  ws.textAll(message); // broadcast to all websocket clients
  message="";
}
