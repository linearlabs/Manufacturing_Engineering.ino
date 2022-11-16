// Peripheral_Bluart_DataLogger_nRF52840_V1.0_LLC_2022
#include <bluefruit.h>
#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Wire.h>
#include <Adafruit_I2CDevice.h>
#include <Adafruit_I2CRegister.h>
#include "Adafruit_MCP9600.h"
#include <avr/dtostrf.h> // used to convert float to char*

#define I2C_ADDRESS (0x67)
#define I2C_ADDRESS2 (0x60)
#define BAT A0
#define LED D1

char result[8]; // Buffer big enough for 7-character float #include <avr/dtostrf.h>
char result2[8]; // Buffer big enough for 7-character float #include <avr/dtostrf.h>
char sendInfo[25];
char batChar[5];
//char sen2[17] = "Sensor 2 deg C: ";
int BATvalue = 0;
int BATmap = 0;

Adafruit_MCP9600 mcp;
Adafruit_MCP9600 mcp2;
BLEDfu  bledfu;  // OTA DFU service
BLEDis  bledis;  // device information
BLEUart bleuart; // uart over ble

void setup() {
  pinMode (LED, OUTPUT);
  Serial.begin(115200);
  if (! mcp.begin(I2C_ADDRESS)) {
    Serial.println("Sensor1 not found. Check wiring!");
    while (1);
  }
  if (! mcp2.begin(I2C_ADDRESS2)) {
    Serial.println("Sensor2 not found. Check wiring!");
    while (1);
  }
  Serial.println("Found Sensor 1 and 2");
  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18);
  mcp2.setADCresolution(MCP9600_ADCRESOLUTION_18);
  Serial.print("Sensor1 resolution set to ");
  switch (mcp.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
    case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
    case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
    case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
  }
  Serial.println(" bits");
  Serial.print("Sensor2 resolution set to ");
  switch (mcp2.getADCresolution()) {
    case MCP9600_ADCRESOLUTION_18:   Serial.print("18"); break;
    case MCP9600_ADCRESOLUTION_16:   Serial.print("16"); break;
    case MCP9600_ADCRESOLUTION_14:   Serial.print("14"); break;
    case MCP9600_ADCRESOLUTION_12:   Serial.print("12"); break;
  }
  Serial.println(" bits");
  mcp.setThermocoupleType(MCP9600_TYPE_K);
  mcp2.setThermocoupleType(MCP9600_TYPE_K);
  Serial.print("Sensor1 Thermocouple type set to ");
  switch (mcp.getThermocoupleType()) {
    case MCP9600_TYPE_K:  Serial.print("K"); break;
    case MCP9600_TYPE_J:  Serial.print("J"); break;
    case MCP9600_TYPE_T:  Serial.print("T"); break;
    case MCP9600_TYPE_N:  Serial.print("N"); break;
    case MCP9600_TYPE_S:  Serial.print("S"); break;
    case MCP9600_TYPE_E:  Serial.print("E"); break;
    case MCP9600_TYPE_B:  Serial.print("B"); break;
    case MCP9600_TYPE_R:  Serial.print("R"); break;
  }
  Serial.println(" type");
  Serial.print("Sensor2 Thermocouple type set to ");
  switch (mcp2.getThermocoupleType()) {
    case MCP9600_TYPE_K:  Serial.print("K"); break;
    case MCP9600_TYPE_J:  Serial.print("J"); break;
    case MCP9600_TYPE_T:  Serial.print("T"); break;
    case MCP9600_TYPE_N:  Serial.print("N"); break;
    case MCP9600_TYPE_S:  Serial.print("S"); break;
    case MCP9600_TYPE_E:  Serial.print("E"); break;
    case MCP9600_TYPE_B:  Serial.print("B"); break;
    case MCP9600_TYPE_R:  Serial.print("R"); break;
  }
  Serial.println(" type");
  mcp.setFilterCoefficient(3);
  mcp2.setFilterCoefficient(3);
  Serial.print("Sensor1 Filter coefficient value set to: ");
  Serial.println(mcp.getFilterCoefficient());
  Serial.print("Sensor2 Filter coefficient value set to: ");
  Serial.println(mcp2.getFilterCoefficient());
  //  mcp.setAlertTemperature(1, 30);
  //  Serial.print("Alert #1 temperature set to ");
  //  Serial.println(mcp.getAlertTemperature(1));
  //  mcp.configureAlert(1, true, true);  // alert 1 enabled, rising temp
  mcp.enable(true);
  mcp2.enable(true);
  Serial.println(F("------------------------------"));
#if CFG_DEBUG
  // Blocking wait for connection when debug mode is enabled via IDE
  while ( !Serial ) yield();
#endif

  Serial.println("Bluefruit52 BLEUART Example");
  Serial.println("---------------------------\n");

  // Setup the BLE LED to be enabled on CONNECT
  // Note: This is actually the default behavior, but provided
  // here in case you want to control this LED manually via PIN 19
  Bluefruit.autoConnLed(true);

  // Config the peripheral connection with maximum bandwidth
  // more SRAM required by SoftDevice
  // Note: All config***() function must be called before begin()
  Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
  //Bluefruit.setName(getMcuUniqueID()); // useful testing with multiple central connections
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // To be consistent OTA DFU should be added first if it exists
  bledfu.begin();

  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Bluefruit Feather52");
  bledis.begin();

  // Configure and Start BLE Uart Service
  bleuart.begin();


  // Set up and start advertising
  startAdv();

  Serial.println("Please use Adafruit's Bluefruit LE app to connect in UART mode");
  Serial.println("Once connected, enter character(s) that you wish to send");
}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();

  // Include bleuart 128-bit uuid
  Bluefruit.Advertising.addService(bleuart);

  // Secondary Scan Response packet (optional)
  // Since there is no room for 'Name' in Advertising packet
  Bluefruit.ScanResponse.addName();

  /* Start Advertising
     - Enable auto advertising if disconnected
     - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     - Timeout for fast mode is 30 seconds
     - Start(timeout) with timeout = 0 will advertise forever (until connected)

     For recommended advertising interval
     https://developer.apple.com/library/content/qa/qa1931/_index.html
  */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle) {
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

/**
   Callback invoked when a connection is dropped
   @param conn_handle connection where this event happens
   @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
*/
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.println();
  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
}

void loop() {
  BATvalue = analogRead(BAT);
  BATmap = map(BATvalue, 510, 570, 0, 100);
  BATmap = map(BATvalue, 430, 610, 0, 100);
  if (BATmap > 100) {
    BATmap = 100;
  }
  if (BATmap <= 0) {
    BATmap = 0;
    digitalWrite(LED, HIGH); // turn off if battery needs charging
  }
  else {
    digitalWrite(LED, LOW); // turn on if battery does not need charging
  }
  // Forward data from HW Serial to BLEUART
  while (Serial.available()) {
    // Delay to wait for enough input, since we have a limited transmission buffer
    delay(2);

    uint8_t buf[64];
    int count = Serial.readBytes(buf, sizeof(buf));
    bleuart.write( buf, count );
  }

  // Forward from BLEUART to HW Serial
  while (bleuart.available()) {
    uint8_t ch;
    ch = (uint8_t) bleuart.read();
    Serial.write(ch);
  }
  dtostrf(mcp.readThermocouple(), 6, 2, result); // #include <avr/dtostrf.h> !!!Float to char* conversion
  dtostrf(mcp2.readThermocouple(), 6, 2, result2); // #include <avr/dtostrf.h> !!!Float to char* conversion
  strcpy(sendInfo, result); // copy result char to sendinfo
  strcat(sendInfo, ",");
  strcat(sendInfo, result2); // concat result2 to the end of sendInfo
  strcat(sendInfo, ":");
  strcat(sendInfo, itoa(BATmap, batChar, 10)); // concat battery mapped value to the end of sendInfo
  Serial.println(sendInfo);
  bleuart.write(sendInfo);
  //Serial.print("Cold Junction: "); Serial.println(mcp.readAmbient());
  //Serial.print("ADC: "); Serial.print(mcp.readADC() * 2); Serial.println(" uV");
  delay(500);
}
