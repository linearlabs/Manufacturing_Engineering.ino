// Tutorial for creating bitmaps:
// https://diyusthad.com/2019/02/display-images-in-oled-display.html
// Converting images to bitmaps:
// https://diyusthad.com/image2cpp

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LLC_LOGO.h" // img
#include "snail.h" // img
#include "LLC_FULL_LOGO.h" // img
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3c
#define secs() (millis()/1000)
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

unsigned long previousMillis = 0; // will store last time LCD was updated
int interval = 1000; // for blink without delay
unsigned long previousSeconds = 0; // will store last time LCD was updated
int sleepInterval = 1; // for blink without delay
int i = 0;
const int bitmapNum = 3; // # of pictures to display
const unsigned char* bitmap[3] = { // pictures
  LLC_LOGO, snail, LLC_FULL_LOGO
};

void setup() {
  Wire.begin(); // needed for ESP8266-D1
  //Wire.begin(2,0); // needed for ESP8266-01S
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
 //Serial.begin(115200);
  //Serial.setTimeout(20);
}

void sleepMode() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (i >= bitmapNum){
      i=0;
    }
    display.clearDisplay(); //for Clearing the display
    display.drawBitmap(0, 0, bitmap[i], 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.display();
    //Serial.println("print");
    i++;
  }
}

void loop() {
  //unsigned long currentSeconds = secs();
  //if (currentSeconds - previousSeconds >= sleepInterval) {
  sleepMode();
  //previousSeconds = currentSeconds; // removes sleep mode
  //}

}
