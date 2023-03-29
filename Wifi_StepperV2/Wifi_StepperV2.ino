// Tutorial for creating bitmaps:
// https://diyusthad.com/2019/02/display-images-in-oled-display.html
// Converting images to bitmaps:
// https://diyusthad.com/image2cpp
#include "AS5600.h"             // magnetic encoder library
#include <AccelStepper.h>       // stepper
#include <ESP8266WiFi.h>        // d1 mini wifi
#include <Arduino.h>            // for websockets library
#include <ESPAsyncTCP.h>        // websockets
#include <ESPAsyncWebServer.h>  // websockets
#include <WebSerial.h>          // serial API
#include <Wire.h>               // I2C library
#include <Adafruit_GFX.h>       // OLED library
#include <Adafruit_SSD1306.h>   // OLED library
#include "LLC_LOGO.h"           // img
// #include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>  // Required for 16 MHz Adafruit Trinket
#endif
#define stepper_sync 13  // pin to start steppers at same time
#define OLED_RESET -1    // -1 = no reset pin on OLED
#define SCREEN_ADDRESS 0x3C
#define halfStep 15  // high = halfstep / Low = FUllstep
#define stepPin 16   // Driver X pusle pin
#define dirPin 12    // Driver X direction pin
#define enPin 14
#define motorInterfaceType 1
// #define LED_PIN 13 // neopixel LED pin
// #define LED_COUNT 1 // neopixel LED count
// int i = 0;   // for rgb functions
// int j = 0;   // for rgb functions
// int R = 0;   // Red
// int G = 0;   // Green
// int B = 0;   // Blue
// const int brightness = 50; // brightness of LEDs

//const char* ssid = "Production Line";          // Your WiFi SSID
//const char* password = "t1me33Pus$movinG";  // Your WiFi Password
const char* ssid = "Linear Labs Corp";            // Your WiFi SSID
const char* password = "3n3rgy ^ of humanity!";   // Your WiFi Password
const char* idn = "ESP8266, Wifi Stepper, V1.1";  // device ID
int port = 80;
float stepperMaxSpeed = 800;
float stepperSpeed = 800;  // 800 is max at 12V
float stepperAcceleration = 500;
long currentPosition = 0;
float primeSpeed = 0;
long primeSteps = 0;
float deprimeSpeed = 0;
long deprimeSteps = 0;
bool primeIndex = 0;
bool deprimeIndex = 0;
String inputStr = "";  // command input string
String d = "";         // variable string input from clients
String message = "";   // variable message to send to all clients
long rotations = 0;
float offsetPos = 0.0;
float positionCalculation = 0.0;
bool manualRun = false;
bool halfStep_Val = false;
bool stepperRun = false;
bool directionIndex = false;
bool runSpd = false;
bool runWithoutAcceleration = false;
bool enableEncoder = false;
bool enableWebserial = false;
bool enableWebserialIndex = false;  // only allows webserial to be started once
bool primaryStepper = false;        // sets stepper_sync pin either input or output depepending which is primary
bool stepperSetup = false;          // stepper run primary / secondary run setup
bool sync = false;                  // stepper secondary sync index for sync pin
long steps = 0;
long lastStep = 0;
static uint32_t lastTime = 0;
AS5600 as5600;  // use default Wire
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
AsyncWebServer server(port);
AsyncWebSocket ws("/python");  // for python webserial
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB);  // Declare our NeoPixel strip object:

void setup() {
  // strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  as5600.begin();
  as5600.setDirection(AS5600_COUNTERCLOCK_WISE);
  pinMode(enPin, OUTPUT);     // X Driver enable and LED
  digitalWrite(enPin, HIGH);  // en_X_State is variable high or low
  pinMode(halfStep, OUTPUT);  // X Driver enable and LED
  digitalWrite(halfStep, LOW);
  stepper.setCurrentPosition(currentPosition);
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);  //or 0x3C
  display.setTextSize(1);                               // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  connectToWiFi();
  ws.onEvent(onWsEvent);   // for python webserial
  server.addHandler(&ws);  // for python webserial
  server.begin();
  bitMap();
  printInfo();
}
void loop() {
  if ((stepperRun == false) && (runSpd == false) && (runWithoutAcceleration == false) && (manualRun == false) && (primeIndex == false) && (deprimeIndex == false)) {  // if stepper is not run or runSpd
    checkComms();
  } else if ((stepperRun == true) || (runSpd == true) || (runWithoutAcceleration == true)) {
    if (manualRun == false) {
      runStepper();
    }
  } else if (manualRun == true) {
    if ((stepperRun == false) && (runSpd == false) && (runWithoutAcceleration == false)) {
      runManual();
    }
  } else if (primeIndex == true) {
    if ((stepperRun == false) && (runSpd == false) && (runWithoutAcceleration == false) && (manualRun == false) && (deprimeIndex == false)) {
      runPrime();
    }
  }else if (deprimeIndex == true) {
    if ((stepperRun == false) && (runSpd == false) && (runWithoutAcceleration == false) && (manualRun == false) && (primeIndex == false)) {
      runDeprime();
    }
  }
}
