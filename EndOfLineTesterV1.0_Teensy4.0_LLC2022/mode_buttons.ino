void scanModeButtons() {
  YY230V_BTN = digitalRead(YY230V);
  Y460V_BTN = digitalRead(Y460V);
  Y460V2_BTN = digitalRead(Y460V2);
  Delta230V_BTN = digitalRead(Delta230V);

  if (YY230V_BTN == LOW) {
    if (enable_YY230V == false) {
      enable_YY230V = true;
      enable_Y460V = false;
      enable_Y460V2 = false;
      enable_Delta230V = false;
      display.clearDisplay();
      display.setCursor(0, 0);  // Start at top-left corner
      display.println("YY");
      display.print("230V");
      display.display();
      digitalWrite(YY230V_LED, LOW);
      digitalWrite(Y460V_LED, HIGH);
      digitalWrite(Y460V2_LED, HIGH);
      digitalWrite(Delta230V_LED, HIGH);
      PWM = 256;                   // 255 = 0% speed
      digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
      YY230V_Relays();             // possibly need to enable relays before enabling speed and/or write PWM after relay enable with delay (VFD starts funky)
      delay(200);
      analogWrite(pwm_pin, PWM);
    }
  }

  if (Y460V_BTN == LOW) {
    if (enable_Y460V == false) {
      enable_Y460V = true;
      enable_YY230V = false;
      enable_Y460V2 = false;
      enable_Delta230V = false;
      display.clearDisplay();
      display.setCursor(0, 0);  // Start at top-left corner
      display.println("Y");
      display.print("460V");
      display.display();
      digitalWrite(Y460V_LED, LOW);
      digitalWrite(YY230V_LED, HIGH);
      digitalWrite(Y460V2_LED, HIGH);
      digitalWrite(Delta230V_LED, HIGH);
      PWM = 256;                   // 255 = 0% speed
      digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
      Y460V_Relays();
      delay(200);
      analogWrite(pwm_pin, PWM);
    }
  }
  if (Y460V2_BTN == LOW) {
    if (enable_Y460V2 == false) {
      enable_Y460V2 = true;
      enable_YY230V = false;
      enable_Y460V = false;
      enable_Delta230V = false;
      display.clearDisplay();
      display.setCursor(0, 0);  // Start at top-left corner
      display.println("Star");
      display.print("460V");
      display.display();
      digitalWrite(Y460V2_LED, LOW);
      digitalWrite(YY230V_LED, HIGH);
      digitalWrite(Y460V_LED, HIGH);
      digitalWrite(Delta230V_LED, HIGH);
      PWM = 256;                   // 255 = 0% speed
      digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
      Y460V2_Relays();
      delay(200);
      analogWrite(pwm_pin, PWM);
    }
  }
  if (Delta230V_BTN == LOW) {
    if (enable_Delta230V == false) {
      enable_Delta230V = true;
      enable_YY230V = false;
      enable_Y460V = false;
      enable_Y460V2 = false;
      display.clearDisplay();
      display.setCursor(0, 0);  // Start at top-left corner
      display.print("Delta");
      display.print("230V");
      display.display();
      digitalWrite(Delta230V_LED, LOW);
      digitalWrite(YY230V_LED, HIGH);
      digitalWrite(Y460V_LED, HIGH);
      digitalWrite(Y460V2_LED, HIGH);
      PWM = 256;                   // 255 = 0% speed
      digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
      delta230V_Relays();
      delay(200);
      analogWrite(pwm_pin, PWM);
    }
  }
}
