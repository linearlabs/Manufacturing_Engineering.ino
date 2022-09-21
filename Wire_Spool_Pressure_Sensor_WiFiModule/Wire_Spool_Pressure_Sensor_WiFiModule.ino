// wirespool pressure sensors serial to wifi
#include <WiFi.h> // ESP32 wifi
#include <Arduino.h> // for websockets library
#include <AsyncTCP.h> // websockets for ESP32
#include <ESPAsyncWebServer.h> // websockets
#include <WebSerial.h> // serial API

const char* ssid = "Production Line";          // Your WiFi SSID
const char* password = "t1me33Pus$movinG";  // Your WiFi Password
//const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
//const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
int port = 80;
String inputStr = ""; // command input string
String d = "";
String message = ""; // variable message to send to all destinations
bool dIndex = 0;
AsyncWebServer server(port);
AsyncWebSocket ws("/python"); // for python webserial

void connectToWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi");
  }
}
void recvMsg(uint8_t *data, size_t len) { // web server to serial
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  dIndex = 1;
}
void sendToAll(){
  Serial.println(message); // send over usb
  WebSerial.println(message); // send to webserial API
  ws.textAll(message); // broadcast to all websocket clients
  message="";
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

void checkComms() {
  if (dIndex == 1) { // if serial over wifi
    inputStr = d;
    dIndex = 0;
  }
  else if (inputStr == "") { // read serial over usb
    inputStr = Serial.readStringUntil('\n'); //serial to web server
  }
  else if (inputStr != "") { // run serial commands if present
    Serial.println(inputStr); // display message on serial
    WebSerial.println(inputStr); // display message on web API (ip/webserial
    ws.textAll(inputStr); // send message to all clients
    //store(inputStr); // serial input function
    inputStr = "";
    d = "";
  }
  else if (WiFi.status() != WL_CONNECTED) { // reconnect to Wifi if signal is lost
    connectToWiFi();
  }
}
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connectToWiFi();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //WebSerial is accessible at "<IP Address>/webserial" in browser
  ws.onEvent(onWsEvent); // for python webserial
  server.addHandler(&ws);  // for python webserial
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  Serial.println(WiFi.macAddress());
  server.begin();
  setupRGB();
  RGB();
}

void loop() {
  checkComms();
}
