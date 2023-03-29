void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0);  // Start at top-left corner
  display.println("Connecting To");
  display.println(ssid);
  display.println("WiFi");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}
void recvMsg(uint8_t *data, size_t len) {  // web server to serial
  if (enableWebserial == 1) {
    for (int i = 0; i < len; i++) {
      d += char(data[i]);
    }
  }
}
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {  // for python webserial
  if (enableWebserial == 1) {
    if (type == WS_EVT_CONNECT) {       // client connect event
      message = "for help type help?";  // do not send to python only usb and webserial
      WebSerial.println(message);                                                                                                  // send to webserial API
    } else if (type == WS_EVT_DISCONNECT) {  // client disconnect event
      message = "Client disconnected";
      WebSerial.println(message);  // send to webserial API
    }
  }
  if (type == WS_EVT_DATA) {
    for (int i = 0; i < len; i++) {
      d += char(data[i]);
    }
  }
}
void sendToAll() {
  if (enableWebserial == 1) {
    WebSerial.println(message);  // send to webserial API
  }
  ws.textAll(message);  // broadcast to all websocket clients (python)
  message = "";
}
