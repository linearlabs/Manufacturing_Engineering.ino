// wire length encoder pully V1.1
#include <Wire.h>
#include "AS5600.h" // magnetic encoder library
// #include <Adafruit_GFX.h> // OLED draw library
#include <Adafruit_SSD1306.h> // OLED library
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino D1 mini: D2(SDA), D1(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // Use I2C port scanner sketch to determine addresses
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
String inputStr; // serial input commands
String idn = "Wire Length Encoder, V1.1";
float inch;
float degree;
float offsetPos;
float lastPos;
float firstPos;
float fullTurn = 8.8125; // 360 degrees = wirelength in inches
float rotationIndex;
float delta;
float rotations;
bool cw;
bool movement;

unsigned long previousMillis = 0; // will store last time LCD was updated
int interval = 1000; // for blink without delay
AS5600 as5600; // use default Wire

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(20); // set to 50 because it adds too much delay to relay
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  Wire.begin();
  as5600.setDirection(AS5600_COUNTERCLOCK_WISE);  // default, just be explicit.
  resetData();
  Serial.println("For help type help?");
}

void resetData() {
  inch = 0.00;
  offsetPos = 0.00;
  lastPos = 0.00;
  rotations = 0;
  rotationIndex = 0.00;
  delta = 0.00;
  degree = as5600.rawAngle() * AS5600_RAW_TO_DEGREES; // read sensor
  firstPos = degree;
}

void currentPosistion() {
  degree = as5600.rawAngle() * AS5600_RAW_TO_DEGREES; // read sensor
  if ((degree >= firstPos) && (degree <= 360)) {
    offsetPos = degree - firstPos;
  }
  else if ((degree <= firstPos) && (degree >= 0)) {
    offsetPos = 360 - firstPos + degree;
  }
}

void countRotations() {
  delta = offsetPos - lastPos;
  //Serial.println(delta);
  if (delta <= -270) {
    rotations++;
    cw = true;
    movement = true;
  }
  else if (delta >= 270) {
    rotations--;
    cw = false;
    movement = true;
  }
  else if ((delta < 270) && (delta > 0)) {
    cw = true;
    movement = true;
  }
  else if ((delta > -270) && (delta < 0)) {
    cw = false;
    movement = true;
  }
  else if (delta == 0) {
    movement = false;
  }

  rotationIndex = fullTurn / 360;  // = 0.00716 in/deg
  rotationIndex = rotationIndex * offsetPos;
  inch = fullTurn * rotations + rotationIndex;
  delta = 0;
  if (offsetPos != lastPos) {
    lastPos = offsetPos;
  }
}

void printInfo(void) {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0); // Start at top-left corner
    display.print(inch, 0); display.println("in"); // (in, 0) removes decimal

    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.print(inch * .0833); display.println("ft");

    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.print(inch * 2.54, 0); display.println("cm"); // (cm, 0) removes decimal

    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.print(inch * .0254); display.println("m");
    display.display();
  }
}
void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") {
    Serial.println(idn);
  }
  else if (startStr == "help?") { // serial input help variable
    Serial.println("*idn? for unit ID");
    Serial.println("reset for data reset");
    Serial.println("inch? returns current wire length in inches");
    Serial.println("dire? returns current wire length in inches");

  }
  else if (startStr == "reset") { // serial input help variable
    resetData();
    Serial.println("reset, 1");
  }
  else if (startStr == "inch?") { // serial input help variable
    Serial.print("inch,");
    Serial.println(inch);
  }
  else if (startStr == "dire?") { // serial input help variable
    if (movement == true) {
      if (cw == true) {
        Serial.println("direction, CW");
      }
      if (cw == false) {
        Serial.println("direction, CCW");
      }
    }
    else {
      Serial.println("direction, No Movement");
    }
  }
}
void loop() {
  inputStr = Serial.readStringUntil('\n'); // serial input function
  store(inputStr); // serial input function
  currentPosistion();
  countRotations();
  printInfo(); // Print data to OLED and Serial
}
