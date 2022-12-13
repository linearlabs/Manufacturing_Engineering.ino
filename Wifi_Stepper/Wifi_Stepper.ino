// Tutorial for creating bitmaps:
// https://diyusthad.com/2019/02/display-images-in-oled-display.html
// Converting images to bitmaps:
// https://diyusthad.com/image2cpp
#include "AS5600.h" // magnetic encoder library
#include <AccelStepper.h> // stepper 
#include <ESP8266WiFi.h> // d1 mini wifi
#include <Arduino.h> // for websockets library
#include <ESPAsyncTCP.h> // websockets
#include <ESPAsyncWebServer.h> // bebsockets
#include <WebSerial.h> // serial API
#include <Wire.h> // I2C library
#include <Adafruit_GFX.h> // OLED library
#include <Adafruit_SSD1306.h> // OLED library
#include "LLC_LOGO.h" // img
#include "snail.h" // img
#include "LLC_FULL_LOGO.h" // img
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN 13
#define LED_COUNT 3
#define OLED_RESET -1 // -1 = no reset pin on OLED
#define SCREEN_ADDRESS 0x3C
#define halfStep 15 // high = halfstep / Low = FUllstep
#define stepPin 16 // Driver X pusle pin
#define dirPin 12 // Driver X direction pin
#define enPin 14
#define motorInterfaceType 1

//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password
const char* ssid = "Linear Labs Corp";          // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";  // Your WiFi Password
int port = 80;
int i = 0;
int j = 0;
int R = 0; // Red
int G = 0; // Green
int B = 0; // Blue
const int brightness = 50; // brightness of LEDs
String inputStr = ""; // command input string
String idn = "ESP8266, Wifi Stepper, V1.0"; // device ID
String d = "";
String message = ""; // variable message to send to all destinations
float degree;
float rotationIndex;
float delta;
float rotations;
float offsetPos;
float lastPos;
float firstPos;
bool cw;
bool movement;
bool dIndex = 0;
bool stepperRun = false;
const int bitmapNum = 3; // # of pictures to display
const unsigned char* bitmap[3] = { // pictures
  LLC_LOGO, snail, LLC_FULL_LOGO
};
long steps = 0;
long lastStep = 0;
AS5600 as5600; // use default Wire
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
AsyncWebServer server(port);
AsyncWebSocket ws("/webserial"); // for python webserial
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB); // Declare our NeoPixel strip object:

void setup() {
  pinMode (enPin, OUTPUT); // X Driver enable and LED
  digitalWrite(enPin, HIGH); // en_X_State is variable high or low
  pinMode (halfStep, OUTPUT); // X Driver enable and LED
  digitalWrite(halfStep, LOW); 
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  //strip.show(); // Turn OFF all pixels ASAP
  //strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
  stepper.setAcceleration(500);
  stepper.setMaxSpeed(50);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  Serial.begin(115200);
  Serial.setTimeout(20);
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
  as5600.setDirection(AS5600_COUNTERCLOCK_WISE);  // default, just be explicit.
  resetEncoder();
  bitMaps();
  printInfo();
  cycleLEDs();
}
void loop() {
  if (stepperRun == false) { // if stepper is not running
    checkComms();
  }
  else {
    runStepper();
    countRotations();
    showStepperPosition();
  }
}
