void bitMap() {
    display.clearDisplay(); //for Clearing the display
    display.drawBitmap(0, 0, LLC_LOGO, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.display();
    delay(2000);
}
