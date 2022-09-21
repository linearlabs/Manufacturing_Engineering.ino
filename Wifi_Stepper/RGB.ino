void cycleLEDs() {
  i = 0;
  while (true) {
    if (j == 3) {
      break;
    }
    strip.clear();  //   Set all pixels in RAM to 0 (off)
    if (j == 0) {
      strip.setPixelColor(i, strip.Color(brightness, 0, 0)); // Red, 255 max
    }
    else if (j == 1) {
      strip.setPixelColor(i, strip.Color(0, brightness, 0)); // Green, 255 max
    }
    else if (j == 2) {
      strip.setPixelColor(i, strip.Color(0, 0, brightness)); // Blue, 255 max
    }
    strip.show(); // Update strip with new contents
    delay(100);  // Pause for a moment
    if (i == 7) {
      i = 0;
      j++;
    }
    else {
      i++;
    }
  }
}
void showStepperPosition() {
  if (rotations == 0) {
    R = 0;
    G = 0;
    B = brightness;
  }
  if (abs(rotations) == 1) {
    R = 0;
    G = brightness;
    B = 0;
  }
  if (abs(rotations) == 2) {
    R = brightness;
    G = 0;
    B = 0;
  }
  if (abs(rotations) == 3) {
    R = brightness;
    G = brightness;
    B = 0;
  }
  if (abs(rotations) == 4) {
    R = 0;
    G = brightness;
    B = brightness;
  }
  if (abs(rotations) == 5) {
    R = brightness;
    G = brightness;
    B = brightness;
  }

  if (offsetPos >= 0 && offsetPos < 45) {
    strip.setPixelColor(0, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 45 && offsetPos < 90) {
    strip.setPixelColor(1, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 90 && offsetPos < 135) {
    strip.setPixelColor(2, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 135 && offsetPos < 180) {
    strip.setPixelColor(3, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 180 && offsetPos < 225) {
    strip.setPixelColor(4, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 225 && offsetPos < 270) {
    strip.setPixelColor(5, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 270 && offsetPos < 315) {
    strip.setPixelColor(6, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
  else if (offsetPos >= 315 && offsetPos < 360) {
    strip.setPixelColor(7, strip.Color(R,G,B)); // Blue, 255 max
    strip.show();
  }
}
