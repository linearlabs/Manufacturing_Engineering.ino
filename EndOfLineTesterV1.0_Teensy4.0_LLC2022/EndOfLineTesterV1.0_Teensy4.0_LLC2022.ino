//EndOfLineTesterV1.0_Teensy4.0_LLC2022

#include <Wire.h> // i2c
#include <Adafruit_GFX.h> //oled
#include <Adafruit_SSD1306.h> // oled
#include "LLC_FULL_LOGO.h"  // bitmap img
#define OLED_RESET -1 // oled
#define SCREEN_ADDRESS 0x3C // oled

#define EN_230V 23 // Enable on 230V VFD
#define EN_460V 22 // Enable on 460V VFD
#define M_Type1 21 // enables Delta Star Enclosure Opto Isolators
#define M_Type2 20 // enables Wye/Double Wye Enclosure Opto Isolators
#define pwm_pin 15  // speed wiper DAC
#define rdy_pin 14  // 10Vin enable for speed
#define relay2 12 // 230V vfd Relays & 230V motor voltage selector Relays
#define relay1 11 // 460V vfd Relays & 460V motor voltage selector Relays
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

int PWM = 256; // 255 = off
int displayPWM = 0;
bool UP_BTN;
bool STOP_BTN;
bool DN_BTN;
bool YY230V_BTN;
bool enable_YY230V = false;  // enable state
bool Y460V_BTN;
bool enable_Y460V = false;  // enable state
bool Y460V2_BTN;
bool enable_Y460V2 = false;  // enable state
bool Delta230V_BTN;
bool enable_Delta230V = false;  // enable state

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

void setup() {
  pinMode(EN_230V, OUTPUT); // Enable on 230V VFD
  pinMode(EN_460V, OUTPUT); // Enable on 460V VFD
  pinMode(M_Type1, OUTPUT); // enables Delta Star Enclosure Opto Isolators
  pinMode(M_Type2, OUTPUT); // enables Wye/Double Wye Enclosure Opto Isolators
  pinMode(relay2, OUTPUT); // 230V vfd Relays & 230V motor voltage selector Relays
  pinMode(relay1, OUTPUT); // 460V vfd Relays & 460V motor voltage selector Relays
  relaysOff();

  beginDisplay();

  analogWriteFrequency(pwm_pin, 1000);  // https://www.pjrc.com/teensy/td_pulse.html  (Freq)
  analogWriteResolution(8);             // https://www.pjrc.com/teensy/td_pulse.html  (Resolution)
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
