
// nRF52840, Bluart DataLogger Central, V1.0
#include <bluefruit.h>
#include "RTClib.h"
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <SD.h>
#include "LLC_LOGO.h" // img
#include "snail.h" // img
#include "LLC_FULL_LOGO.h" // img
#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1 // -1 = no reset pin on OLED
#define SCREEN_ADDRESS 0x3C
#define BAT A0
#define ChipSelect D3

//Adafruit_ADS1015 ads1015;    // Construct an ads1015
Adafruit_ADS1115 ads1115; // Construct an ads1115
RTC_DS3231 rtc;
Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);
BLEClientDis  clientDis;  // device information client
BLEClientUart clientUart; // bleuart client
File myFile;

int16_t up = 0;
int16_t down = 0;
int16_t enter = 0;
int16_t escape = 0;
bool upIndex = false;
bool downIndex = false;
bool enterIndex = false;
bool escapeIndex = false;
const char* LLC = "Linear Labs Inc.";
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; // will store last time LCD was updated
int interval = 1000; // for blink without delay
int yearNow = 0;
int monthNow = 0;
int dayNow = 0;
int hourNow = 0;
int minNow = 0;
int secNow = 0;
char fiveChar[5]; // reused variable for int to char conversions
char dateNow[9];
char fileType[5] = ".txt";
char fileLog[13];
char timeNow[9];
char dataIn[20];
char* tempIn;
int wheelBattery = 0;
bool menu = 0;
int selection = 0;
int add_subtract = 0;
int Battery_value = 0;
int Battery_map = 0;
int i;
const int bitmapNum = 3; // # of pictures to display
const unsigned char* bitmap[3] = { // pictures
  LLC_LOGO, snail, LLC_FULL_LOGO,
};

void peripheral_disconnected() {
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println(LLC);
  display.print("Local Battery ");
  display.print(Battery_map);
  display.println("%");
  display.print("Date: ");
  display.println(dateNow);
  display.print("Time: ");
  display.println(timeNow);
  display.println("BlueTooth Peripheral Disconnected");
  display.display();
}

void scan_callback(ble_gap_evt_adv_report_t* report) {
  // Check if advertising contain BleUart service
  if ( Bluefruit.Scanner.checkReportForService(report, clientUart)) {
    Serial.print("BLE UART service detected. Connecting ... ");
    Bluefruit.Central.connect(report); // Connect to device with bleuart service in advertising
  } else {
    // For Softdevice v6: after received a report, scanner will be paused
    // We need to call Scanner resume() to continue scanning
    Bluefruit.Scanner.resume();
  }
}
void connect_callback(uint16_t conn_handle) {
  Serial.println("Connected");
  Serial.print("Dicovering Device Information ... ");
  if ( clientDis.discover(conn_handle) ) {
    Serial.println("Found it");
    char buffer[32 + 1];
    memset(buffer, 0, sizeof(buffer)); // read and print out Manufacturer
    if ( clientDis.getManufacturer(buffer, sizeof(buffer))) {
      Serial.print("Manufacturer: ");
      Serial.println(buffer);
    }
    memset(buffer, 0, sizeof(buffer)); // read and print out Model Number
    if ( clientDis.getModel(buffer, sizeof(buffer))) {
      Serial.print("Model: ");
      Serial.println(buffer);
    }
    Serial.println();
  } else {
    Serial.println("Found NONE");
  }
  Serial.print("Discovering BLE Uart Service ... ");
  if ( clientUart.discover(conn_handle) ) {
    Serial.println("Found it");
    Serial.println("Enable TXD's notify");
    clientUart.enableTXD();
    Serial.println("Ready to receive from peripheral");
  } else {
    Serial.println("Found NONE");
    // disconnect since we couldn't find bleuart service
    Bluefruit.disconnect(conn_handle);
  }
}
/**
   Callback invoked when a connection is dropped
   @param conn_handle
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void) conn_handle;
  (void) reason;
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}
/**
   Callback invoked when uart received data
   @param uart_svc Reference object to the service where the data
   arrived. In this example it is clientUart
*/
void bleuart_rx_callback(BLEClientUart& uart_svc) {
  //Serial.print("[RX]: ");
  i = 0;
  while (uart_svc.available()) {
    // Serial.print((char)uart_svc.read());
    dataIn[i] = (char)uart_svc.read();
    i++;
  }
  //Serial.println(dataIn);
  tempIn = strtok(dataIn, ":"); // split string and save first variable
  char* wheelBatteryChar = strtok(NULL, ":"); // save second variable (repeat for more variables)
  wheelBattery = atoi(wheelBatteryChar); // convert char* to int
  wheelBatteryChar[0] = '\0'; // reset all possible pointers for battery% char* (not sure if there is a better way to do this)
  wheelBatteryChar[1] = '\0';
  wheelBatteryChar[2] = '\0';
  //Serial.println(wheelBattery);
  //Serial.println(tempIn);

}
void setup () {
  Serial.begin(115200);
  Serial.print("Initializing ADC1115...");
  ads1115.begin(0x49);  // Initialize ads1115 at address 0x49
  Serial.println("ADC1115 initialized");
  Serial.print("Initializing RTC...");
  while (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    delay(1000);
  }
  Serial.println("RTC initialized");
  Serial.print("Initializing SD card...");
  while (!SD.begin(ChipSelect)) {
    Serial.println("SD initialization failed!");
    delay(1000);
  }
  Serial.println("SD card initialized.");
  Serial.print("Initializing OLED...");
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); //or 0x3C
  Serial.println("OLED initialized");
  display.setTextSize(1); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.clearDisplay();
  display.setCursor(0, 0); // Start at top-left corner
  display.println(LLC);
  display.display();
  // Initialize Bluefruit with maximum connections as Peripheral = 0, Central = 1
  // SRAM usage required by SoftDevice will increase dramatically with number of connections
  Bluefruit.begin(0, 1);
  Bluefruit.setName("Bluefruit52 Central");
  clientDis.begin(); // Configure DIS client
  clientUart.begin(); // Init BLE Central Uart Serivce
  clientUart.setRxCallback(bleuart_rx_callback);
  Bluefruit.setConnLedInterval(250); // Increase Blink rate to different from PrPh advertising mode
  Bluefruit.Central.setConnectCallback(connect_callback); // Callbacks for Central
  Bluefruit.Central.setDisconnectCallback(disconnect_callback);
  /* Start Central Scanning
     - Enable auto scan if disconnected
     - Interval = 100 ms, window = 80 ms
     - Don't use active scan
     - Start(timeout) with timeout = 0 will scan forever (until connected)
  */
  Bluefruit.Scanner.setRxCallback(scan_callback);
  Bluefruit.Scanner.restartOnDisconnect(true);
  Bluefruit.Scanner.setInterval(160, 80); // in unit of 0.625 ms
  Bluefruit.Scanner.useActiveScan(false);
  Bluefruit.Scanner.start(0); // 0 = Don't stop scanning after n seconds
}
void Local_Battery() {
  Battery_value = analogRead(BAT);
  //Serial.println(Battery_value);
  Battery_map = map(Battery_value, 430, 610, 0, 100);
  if (Battery_map < 0) {
    Battery_map = 0;
  }
  if (Battery_map > 100) {
    Battery_map = 100;
  }
}
void Get_DateTime() {
  DateTime now = rtc.now();
  yearNow = now.year(), DEC;
  yearNow = yearNow - 2000;
  monthNow = now.month(), DEC;
  dayNow = now.day(), DEC;
  hourNow = now.hour(), DEC;
  minNow = now.minute(), DEC;
  secNow = now.second(), DEC;
  strcpy(dateNow, itoa(yearNow, fiveChar, 10)); // copy yearNow char to dateNow
  strcat(dateNow, "-");
  strcat(dateNow, itoa(monthNow, fiveChar, 10)); // concat monthNow to the end of dateNow
  strcat(dateNow, "-");
  strcat(dateNow, itoa(dayNow, fiveChar, 10)); // concat dayNow to the end of dateNow
  strcpy(timeNow, itoa(hourNow, fiveChar, 10)); // copy hourNow to timeNow
  strcat(timeNow, ":");
  strcat(timeNow, itoa(minNow, fiveChar, 10)); // concat minNow to the end of timeNow
  strcat(timeNow, ":");
  strcat(timeNow, itoa(secNow, fiveChar, 10)); // concat secNow to the end of timeNow
}
void loop () {
  currentMillis = millis();
  if (Bluefruit.Central.connected()) { // Not discovered yet
    if (clientUart.discovered()) { // Discovered means in working state
      if (Serial.available()) { // Get Serial input and send to Peripheral
        delay(2); // delay a bit for all characters to arrive
        char str[20 + 1] = {0};
        Serial.readBytes(str, 20);
        clientUart.print(str);
      }
      if (currentMillis - previousMillis >= interval) { // Timed Function
        previousMillis = currentMillis;
        Local_Battery();
        Get_DateTime();
        display.clearDisplay();
        display.setCursor(0, 0); // Start at top-left corner
        display.println(LLC);
        display.print("Local Battery ");
        display.print(Battery_map);
        display.println("%");
        display.print("Wheel Battery ");
        display.print(wheelBattery);
        display.println("%");
        display.print("Date: ");
        display.println(dateNow);
        display.print("Time: ");
        display.println(timeNow);
        display.print("Temps ");
        display.println(tempIn);
        display.display();
        writeSD();
      }
    }
  }
  else {
    if (currentMillis - previousMillis >= interval * 4) {
      previousMillis = currentMillis;
      Local_Battery();
      Get_DateTime();
      if (i == bitmapNum) {
        peripheral_disconnected();
      }
      if (i >= bitmapNum + 1) {
        i = 0;
      }
      if (i < bitmapNum) {
        display.clearDisplay(); //for Clearing the display
        display.drawBitmap(0, 0, bitmap[i], 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
        display.display();
      }
      i++;
    }
  }
  Edit_Time_Buttons();
}
