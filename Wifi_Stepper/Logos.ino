void bitMaps() {
  while (i < bitmapNum) {
    display.clearDisplay(); //for Clearing the display
    display.drawBitmap(0, 0, bitmap[i], 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.display();
    delay(2000);
    i++;
  }
}
