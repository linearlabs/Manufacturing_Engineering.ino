// ESP8266-01S, Websockets, V1.0

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password
const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
String idn = "ESP8266-01S, LLC Websockets, V1.0";
String d = "";
String message = "";
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; // will store last time LCD was updated
int interval = 1000; // for blink without delay

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
AsyncWebServer server(80);
AsyncWebSocket ws("/python"); // needs to be something different than webserial
void recvMsg(uint8_t *data, size_t len) { // web server to serial
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  cmdIn();
}
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
  //if (type == WS_EVT_CONNECT) {
  //Serial.println("Websocket client connection received");
  //} else if (type == WS_EVT_DISCONNECT) {
  //Serial.println("Client disconnected");
  //} else if (type == WS_EVT_DATA) {
  if (type == WS_EVT_DATA) {
    for (int i = 0; i < len; i++) {
      //Serial.print(data[i]);
      //Serial.print("|");
      d += char(data[i]);
    }
    cmdIn();
  }
}
void cmdIn() {
  WebSerial.println(d);
  Serial.println(d);
  ws.textAll(d);
  String cmd = d.substring(0, 5); // 5 character commands
  if (cmd == "*idn?") { // request identification
    WebSerial.println(idn);
    Serial.println(idn);
    ws.textAll(idn);
  }
  d="";
}
void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println("Connecting To");
  display.println(ssid);
  display.println("WiFi");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi");
  }
}
void updateTerms() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Timed Function
    previousMillis = currentMillis;
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    //display.setTextSize(2);
    //display.println(message);
    display.setTextSize(1);
    display.println("");
    display.println("");
    display.println(ssid);
    display.println("");
    display.println(WiFi.localIP());
    display.print("/webserial");
    display.display();
    //WebSerial.println(message);
    //ws.textAll(message); // broadcast to all websocket clients
  }
}
void sendToAll() {
  Serial.println(message); // send over usb
  WebSerial.println(message); // send to webserial API
  ws.textAll(message); // broadcast to all websocket clients
}

void setup() {
  Wire.begin(2,0); // needed for ESP8266-01 (not needed for D1)
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
  display.setTextColor(SSD1306_WHITE);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connectToWiFi();
  WebSerial.begin(&server); // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.msgCallback(recvMsg);
  ws.onEvent(onWsEvent); // for python webserial
  server.addHandler(&ws);  // for python webserial
  server.begin();
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  updateTerms();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) { // reconnect to Wifi if signal is lost
    connectToWiFi();
  }
}
