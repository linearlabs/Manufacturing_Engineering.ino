#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#define LED 2
const char* ssid = "Production Line";          // Your WiFi SSID
const char* password = "t1me33Pus$movinG";  // Your WiFi Password

//const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
//const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
String d = "";

AsyncWebServer server(80);
AsyncWebSocket ws("/webserial");

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20);
  pinMode(LED, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP());
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();
}

void recvMsg(uint8_t *data, size_t len) {
  for (int i = 0; i < len; i++) {
    //Serial.print(data[i]);
    //Serial.print("|");
    d += char(data[i]);
  }
  WebSerial.println(d);
  Serial.println(d);
  ws.textAll(d);
  if (d == "ON") {
    digitalWrite(LED, HIGH);
  }
  if (d == "OFF") {
    digitalWrite(LED, LOW);
  }
  d = "";
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
    WebSerial.println(d);
    Serial.println(d);
    ws.textAll(d);
    if (d == "ON") {
      digitalWrite(LED, HIGH);
    }
    if (d == "OFF") {
      digitalWrite(LED, LOW);
    }
    d = "";
  }
}
void loop() {
  d = Serial.readStringUntil('\n');            //serial input function
  if (d != "") {
    //for (int i = 0; i < d.length(); i++) {
      //Serial.print(data[i]);
      //Serial.print("|");
      //d += char(data[i]);
    //}
    WebSerial.println(d);
    Serial.println(d);
    //ws.textAll((char*)text);
    ws.textAll(d);
    if (d == "ON") {
      digitalWrite(LED, HIGH);
    }
    if (d == "OFF") {
      digitalWrite(LED, LOW);
    }
    d = "";
  }
}
