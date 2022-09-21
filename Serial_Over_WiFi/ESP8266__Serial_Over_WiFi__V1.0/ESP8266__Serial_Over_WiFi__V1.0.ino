#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password

const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
String inputStr=""; // command input string
String idn = "ESP8266, Serial Over WiFi, V1.0";

AsyncWebServer server(80);

void recvMsg(uint8_t *data, size_t len) { // web server to serial
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  //WebSerial.println(d);
  Serial.println(d);
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  Serial.println(WiFi.macAddress());
  server.begin();
}

void loop() {
  inputStr = Serial.readStringUntil('\n'); //serial to web server
  if (inputStr != ""){
    WebSerial.println(inputStr);
    inputStr="";
  }
}
