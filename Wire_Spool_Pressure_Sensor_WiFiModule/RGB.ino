#define REDPIN 3
#define GREENPIN 4
#define BLUEPIN 5
#define COLD_WHITE 18
#define WARM_WHITE 19
#define FADESPEED 5     // make this higher to slow down

void setupRGB() {
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(BLUEPIN, OUTPUT);
  pinMode(COLD_WHITE, OUTPUT);
  pinMode(WARM_WHITE, OUTPUT);
}

void RGB() {
  int r, g, b, cw, ww;
  // fade from blue to violet
  for (r = 0; r < 100; r++) {
    analogWrite(REDPIN, r);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from violet to red
  for (b = 100; b > 0; b--) {
    analogWrite(BLUEPIN, b);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from red to yellow
  for (g = 0; g < 100; g++) {
    analogWrite(GREENPIN, g);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from yellow to green
  for (r = 100; r > 0; r--) {
    analogWrite(REDPIN, r);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from green to teal
  for (b = 0; b < 100; b++) {
    analogWrite(BLUEPIN, b);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from teal to blue
  for (g = 100; g > 0; g--) {
    analogWrite(GREENPIN, g);
    analogWrite(COLD_WHITE, 0);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from off to cold white
  for (cw = 0; cw < 100; cw++) {
    analogWrite(REDPIN, 0);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
    analogWrite(COLD_WHITE, cw);
    analogWrite(WARM_WHITE, 0);
    delay(FADESPEED);
  }
  // fade from off to warm white
  for (ww = 0; ww < 100; ww++) {
    analogWrite(REDPIN, 0);
    analogWrite(GREENPIN, 0);
    analogWrite(BLUEPIN, 0);
    analogWrite(WARM_WHITE, ww);
    analogWrite(COLD_WHITE, 0);
    delay(FADESPEED);
  }
  analogWrite(WARM_WHITE, 0);
  analogWrite(GREENPIN, 40);
}
