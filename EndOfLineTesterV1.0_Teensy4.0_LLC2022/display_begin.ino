void beginDisplay() {
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);  //or 0x3C
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(4);
  display.setCursor(0, 0);                                  // Start at top-left corner
  display.clearDisplay();                                   //for Clearing the display
  display.drawBitmap(0, 0, LLC_FULL_LOGO, 128, 64, WHITE);  // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.display();
}