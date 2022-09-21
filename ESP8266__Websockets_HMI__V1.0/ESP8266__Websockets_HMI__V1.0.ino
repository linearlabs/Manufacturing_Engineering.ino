// ESP8266, Websockets HMI, V1.0

#include <ESP8266WiFi.h>
#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <LiquidCrystal_I2C.h>

#define up A0 // up button is on ADC pin
#define dn 13 // dn button pin D7
#define ent 14 // up button pin D5
#define esc 12 // dn button pin D6

//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password
const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
const char* subdirectory = "/webserial";
String d = "";
String message = "";
int upVal;
bool dnVal;
bool entVal;
bool escVal;
bool upIndex = 0;
bool dnIndex = 0;
bool entIndex = 0;
bool escIndex = 0;
unsigned long currentMillis = 0; // current update
unsigned long previousMillis = 0; // lastupdate
int interval = 50; // for blink without delay in ms

AsyncWebServer server(80);
AsyncWebSocket ws("/python"); // needs to be something different than webserial
LiquidCrystal_I2C lcd(0x27, 16, 2);

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
  if (cmd == "lcdx:") { // lcd clear
    lcd.clear();
  }
  else if (cmd == "lcd0:") { // lcd print 1st line
    String subCmd = d.substring(5); //add .toInt() to convert string to int
    lcd.setCursor(0, 0);
    lcd.print(subCmd);
  }
  else if (cmd == "lcd1:") { // lcd print 2nd line
    String subCmd = d.substring(5); //add .toInt() to convert string to int
    lcd.setCursor(0, 1);
    lcd.print(subCmd);
  }
  else if (cmd == "ipcf?") { // print ip
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
    lcd.setCursor(0, 1);
    lcd.print(subdirectory);
  }
  else if (cmd == "addr?") { // print mac
    WebSerial.println(WiFi.macAddress()); // send to webserial API
    ws.textAll(WiFi.macAddress()); // broadcast to all websocket clients
  }
  d = "";
}
void connectToWiFi() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(50);
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connectToWiFi();
  WebSerial.begin(&server); // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.msgCallback(recvMsg);
  ws.onEvent(onWsEvent); // for python webserial
  server.addHandler(&ws);  // for python webserial
  server.begin();
  Serial.println(WiFi.localIP());
  lcd.setCursor(0, 0);
  lcd.print(WiFi.localIP());
  lcd.setCursor(0, 1);
  lcd.print(subdirectory);
  pinMode(dn, INPUT_PULLUP); // dn
  pinMode(ent, INPUT_PULLUP); // ent
  pinMode(esc, INPUT_PULLUP); // esc
}

void scanButtons() {
  currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Timed Function, scan and print buttons timed
    previousMillis = currentMillis;
    upVal = analogRead(up); // (buttonOff=653 ; buttonOn=2
    dnVal = digitalRead(dn);
    entVal = digitalRead(ent);
    escVal = digitalRead(esc);
    if ((upVal < 300) && (upIndex == 0)) {
      Serial.println("upbt:1");
      upIndex = 1;
    }
    if ((upVal >= 300) && (upIndex == 1)) {
      Serial.println("upbt:0");
      upIndex = 0;
    }
    if ((dnVal == LOW) && (dnIndex == 0)) {
      Serial.println("dnbt:1");
      dnIndex = 1;
    }
    if ((dnVal == HIGH) && (dnIndex == 1)) {
      Serial.println("dnbt:0");
      dnIndex = 0;
    }
    if ((entVal == LOW) && (entIndex == 0)) {
      Serial.println("enbt:1");
      entIndex = 1;
    }
    if ((entVal == HIGH) && (entIndex == 1)) {
      Serial.println("enbt:0");
      entIndex = 0;
    }
    if ((escVal == LOW) && (escIndex == 0)) {
      Serial.println("esbt:1");
      escIndex = 1;
    }
    if ((escVal == HIGH) && (escIndex == 1)) {
      Serial.println("esbt:0");
      escIndex = 0;
    }
  }
}
void loop() {
  message = Serial.readStringUntil('\n'); // if serial from PLC send over wifi
  if (message != "") {
    WebSerial.println(message);
    ws.textAll(message);
    message = "";
  }
  if (WiFi.status() != WL_CONNECTED) { // reconnect to Wifi if signal is lost
    connectToWiFi();
  }
  scanButtons();
}
