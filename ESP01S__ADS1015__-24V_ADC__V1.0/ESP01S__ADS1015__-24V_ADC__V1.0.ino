// ESP01S, ADS1015 +-24V ADC, V1.0
#include <SparkFun_ADS1015_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_ADS1015
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3c
//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password
const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
String d = "";
const char* idn = "ESP01S, ADS1015 +-24V ADC, V1.0";
const char* unitValue = " psi";
const char* maxValue = " max";
const char* subdirectory = "/webserial";
float psi = 0.0;
float psiMax = 0.0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; // will store last time LCD was updated
int interval = 200; // for blink without delay in ms

ADS1015 adcSensor; // default address is 0x48
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
AsyncWebServer server(80);
AsyncWebSocket ws("/python"); // needs to be something different than webserial

void connectToWiFi() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println("Connecting To");
  display.println(ssid);
  display.println("WiFi");
  display.display();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.println("Connecting to WiFi");
  }
}

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
  //Serial.println(d);
  ws.textAll(d);
  String cmd = d.substring(0, 5); // 5 character commands
  if (cmd == "*idn?") { // request identification
    WebSerial.println(idn);
    //Serial.println(idn);
    ws.textAll(idn);
  }
  d = "";
}
void sendInfo() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.setTextSize(2);
  display.print(int(psiMax)); display.println(maxValue);
  display.print(int(psi)); display.println(unitValue);
  display.setTextSize(1);
  display.println();
  display.println(ssid);
  display.println(WiFi.localIP());
  display.print(subdirectory);
  display.display();
  WebSerial.print(int(psiMax));
  WebSerial.println(unitValue);
  //ws.textAll(d);
}
void setup() {
  Wire.begin(2, 0); // needed for ESP8266-01 (not needed for D1)
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
  display.setTextColor(SSD1306_WHITE);
  // Serial.begin(115200); // serial isnt being used on the ESP01S I-IoT Module
  if (adcSensor.begin() == true)
  {
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println("Device found. I2C connections are good.");
    display.display();
    //Serial.println("Device found. I2C connections are good.");
  }
  else
  {
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println("Device not found. Check wiring.");
    display.display();
    //Serial.println("Device not found. Check wiring.");
    while (1); // stall out forever
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connectToWiFi();
  WebSerial.begin(&server); // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.msgCallback(recvMsg);
  ws.onEvent(onWsEvent); // for python webserial
  server.addHandler(&ws);  // for python webserial
  server.begin();
  sendInfo();
}

void loop() {
  currentMillis = millis();
  uint16_t channel_A0 = adcSensor.getSingleEnded(0);
  uint16_t channel_A1 = adcSensor.getSingleEnded(1);
  //uint16_t channel_A2 = adcSensor.getSingleEnded(2); // spare
  if (channel_A0 > 1200) { // reset
    psi = 0.0;
    psiMax = 0.0;
  }
  if (channel_A1 >= 1182) {
    channel_A1 = channel_A1 - 1182;
    psi = channel_A1 / 0.054;
  }
  else {
    psi = 0.0;
  }
  if (psi > psiMax) {
    psiMax = psi;
  }
  if (currentMillis - previousMillis >= interval) { // Timed Function
    previousMillis = currentMillis;
    sendInfo();
  }
}
