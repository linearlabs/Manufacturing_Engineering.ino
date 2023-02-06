//EndOfLineTesterV1.0_Teensy4.0_LLC2022

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LLC_FULL_LOGO.h"  // img
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

#define pwm_pin 15  // speed wiper DAC
#define rdy_pin 14  // 10Vin
#define YY230V_LED 10
#define Y460V_LED 9
#define Y460V2_LED 8
#define Delta230V_LED 7
#define YY230V 6
#define Y460V 5
#define Y460V2 4
#define Delta230V 3
#define UP 2
#define STOP 1
#define DN 0


unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
int BTN_Debounce = 50;
int PWM = 0;
int displayPWM = 0;

bool UP_BTN;
bool STOP_BTN;
bool DN_BTN;

bool YY230V_State;              // the current reading from the input pin
bool last_YY230V_State = HIGH;  // the previous reading from the input pin
bool YY230V_BTN;
bool enable_YY230V = false;  // enable state

bool Y460V_State;              // the current reading from the input pin
bool last_Y460V_State = HIGH;  // the previous reading from the input pin
bool Y460V_BTN;
bool enable_Y460V = false;  // enable state

bool Y460V2_State;              // the current reading from the input pin
bool last_Y460V2_State = HIGH;  // the previous reading from the input pin
bool Y460V2_BTN;
bool enable_Y460V2 = false;  // enable state

bool Delta230V_State;              // the current reading from the input pin
bool last_Delta230V_State = HIGH;  // the previous reading from the input pin
bool Delta230V_BTN;
bool enable_Delta230V = false;  // enable state


Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void setup() {

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);  //or 0x3C
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(4);

  display.setCursor(0, 0);                                  // Start at top-left corner
  display.clearDisplay();                                   //for Clearing the display
  display.drawBitmap(0, 0, LLC_FULL_LOGO, 128, 64, WHITE);  // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.display();

  analogWriteFrequency(pwm_pin, 1000);  // https://www.pjrc.com/teensy/td_pulse.html
  analogWriteResolution(8);             // https://www.pjrc.com/teensy/td_pulse.html
  pinMode(pwm_pin, OUTPUT);
  pinMode(rdy_pin, OUTPUT);
  digitalWrite(rdy_pin, HIGH);  // LOW enables speed adjustment

  pinMode(UP, INPUT_PULLUP);
  pinMode(STOP, INPUT_PULLUP);
  pinMode(DN, INPUT_PULLUP);

  pinMode(YY230V_LED, OUTPUT);
  pinMode(Y460V_LED, OUTPUT);
  pinMode(Y460V2_LED, OUTPUT);
  pinMode(Delta230V_LED, OUTPUT);
  digitalWrite(YY230V_LED, HIGH);
  digitalWrite(Y460V_LED, HIGH);
  digitalWrite(Y460V2_LED, HIGH);
  digitalWrite(Delta230V_LED, HIGH);
  pinMode(YY230V, INPUT_PULLUP);
  pinMode(Y460V, INPUT_PULLUP);
  pinMode(Y460V2, INPUT_PULLUP);
  pinMode(Delta230V, INPUT_PULLUP);
  delay(200);
  Serial.begin(115200);
}

void loop() {
  if ((enable_Delta230V == false) && (enable_YY230V == false) && (enable_Y460V == false) && (enable_Y460V2 == false)) {  // only enable speed controll if motor has been selected
  scanModeButtons();
  }
  if ((enable_Delta230V == true) || (enable_YY230V == true) || (enable_Y460V == true) || (enable_Y460V2 == true)) {  // only enable speed controll if motor has been selected
    scanSpeedButtons();
  }
}
