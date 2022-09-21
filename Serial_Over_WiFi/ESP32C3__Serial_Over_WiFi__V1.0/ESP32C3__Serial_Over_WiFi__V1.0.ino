/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-webserial-library/
  This sketch is based on the WebSerial library example: ESP32_Demo
  https://github.com/ayushsharma82/WebSerial
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#define LED_BRIGHTNESS 255 // Change white brightness (max 255)

const char* ssid = "Production Line";          // Your WiFi SSID
const char* password = "t1me33Pus$movinG";  // Your WiFi Password

//const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
//const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
String inputStr=""; // command input string
String idn = "ESP32C3, Serial Over WiFi, V1.0";

AsyncWebServer server(80);

void recvMsg(uint8_t *data, size_t len) { // web server to serial
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  //WebSerial.println(d);
  Serial.println(d);
}

void RGB(){
  #ifdef BOARD_HAS_NEOPIXEL
  //digitalWrite(LED_BUILTIN, HIGH);   // Turn the RGB LED white
  //digitalWrite(LED_BUILTIN, LOW);    // Turn the RGB LED off
  neopixelWrite(LED_BUILTIN,LED_BRIGHTNESS,0,0); // Red
  delay(1000);
  neopixelWrite(LED_BUILTIN,0,0,LED_BRIGHTNESS); // Blue
  delay(1000);
  //neopixelWrite(LED_BUILTIN,0,0,0); // Off / black
  neopixelWrite(LED_BUILTIN,0,LED_BRIGHTNESS,0); // Green
  delay(1000);
  #endif
}
void setup() {
  //#ifdef BOARD_HAS_NEOPIXEL
  neopixelWrite(LED_BUILTIN,LED_BRIGHTNESS,0,0); // Red
  //#endif
  Serial.begin(115200);
  Serial.setTimeout(20);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi");
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  //WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  Serial.println(WiFi.macAddress());
  server.begin();
  RGB();
}

void loop() {
  inputStr = Serial.readStringUntil('\n'); //serial to web server
  if (inputStr != ""){
    WebSerial.println(inputStr);
    inputStr="";
  }
}
