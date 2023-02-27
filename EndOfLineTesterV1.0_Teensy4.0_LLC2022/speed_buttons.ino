
void scanSpeedButtons() {

  UP_BTN = digitalRead(UP);
  STOP_BTN = digitalRead(STOP);
  DN_BTN = digitalRead(DN);

  // UP Button
  if ((UP_BTN == LOW) && (PWM >= 1)) {
    PWM--;
    displayPWM = map(PWM, 255, 1, 0, 100);
    display.clearDisplay();
    display.setCursor(0, 0);  // Start at top-left corner
    display.println("Speed");
    display.print(displayPWM);
    display.print("%");
    display.display();
    analogWrite(pwm_pin, PWM);
    Serial.println(PWM);
  }

  if ((DN_BTN == LOW) && (PWM <= 255)) {
    PWM++;
    displayPWM = map(PWM, 255, 1, 0, 100);
    display.clearDisplay();
    display.setCursor(0, 0);  // Start at top-left corner
    display.println("Speed");
    display.print(displayPWM);
    display.print("%");
    display.display();
    analogWrite(pwm_pin, PWM);
    Serial.println(PWM);
  }

  // STOP Button
  if (STOP_BTN == LOW) {

    enable_YY230V = false;
    enable_Y460V = false;
    enable_Y460V2 = false;
    enable_Delta230V = false;
    PWM = 255;  // 255 = 0% speed
    displayPWM = 0;
    display.clearDisplay();                                   //for Clearing the display
    display.drawBitmap(0, 0, LLC_FULL_LOGO, 128, 64, WHITE);  // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.display();
    digitalWrite(Y460V_LED, HIGH);
    digitalWrite(YY230V_LED, HIGH);
    digitalWrite(Y460V2_LED, HIGH);
    digitalWrite(Delta230V_LED, HIGH);
    analogWrite(pwm_pin, PWM);
    digitalWrite(rdy_pin, HIGH);  // LOW enables speed adjustment
    // possible need a delay here for motor stoping time
    relaysOff();
  }
}