void scanModeButtons() {
  YY230V_BTN = digitalRead(YY230V);
  Y460V_BTN = digitalRead(Y460V);
  Y460V2_BTN = digitalRead(Y460V2);
  Delta230V_BTN = digitalRead(Delta230V);


  // YY230V Button
  if (YY230V_BTN != last_YY230V_State) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (YY230V_BTN != YY230V_State) {
      YY230V_State = YY230V_BTN;
      if (YY230V_State == LOW) {

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
          PWM = 255;  // 255 = 0% speed
          analogWrite(pwm_pin, PWM);
          digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
          YY230V_Relays(); // possibly need to enable relays before enabling speed and/or write PWM after relay enable with delay (VFD starts funky)
        }
      }
    }
  }
  last_YY230V_State = YY230V_BTN;

  // Y460V Button
  if (Y460V_BTN != last_Y460V_State) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (Y460V_BTN != Y460V_State) {
      Y460V_State = Y460V_BTN;
      if (Y460V_State == LOW) {

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
          PWM = 255;  // 255 = 0% speed
          analogWrite(pwm_pin, PWM);
          digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
          Y460V_Relays();
        }
      }
    }
  }
  last_Y460V_State = Y460V_BTN;

  // Y460V2 Button
  if (Y460V2_BTN != last_Y460V2_State) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (Y460V2_BTN != Y460V2_State) {
      Y460V2_State = Y460V2_BTN;
      if (Y460V2_State == LOW) {
        if (enable_Y460V2 == false) {
          enable_Y460V2 = true;
          enable_YY230V = false;
          enable_Y460V = false;
          enable_Delta230V = false;
          display.clearDisplay();
          display.setCursor(0, 0);  // Start at top-left corner
          display.println("Y");
          display.print("460V");
          display.display();
          digitalWrite(Y460V2_LED, LOW);
          digitalWrite(YY230V_LED, HIGH);
          digitalWrite(Y460V_LED, HIGH);
          digitalWrite(Delta230V_LED, HIGH);
          PWM = 255;  // 255 = 0% speed
          analogWrite(pwm_pin, PWM);
          digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
          Y460V2_Relays();
        }
      }
    }
  }
  last_Y460V2_State = Y460V2_BTN;

  // Delta230V Button
  if (Delta230V_BTN != last_Delta230V_State) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (Delta230V_BTN != Delta230V_State) {
      Delta230V_State = Delta230V_BTN;
      if (Delta230V_State == LOW) {
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
          PWM = 255;  // 255 = 0% speed
          analogWrite(pwm_pin, PWM);
          digitalWrite(rdy_pin, LOW);  // LOW enables speed adjustment
          delta230V_Relays();
        }
      }
    }
  }
  last_Delta230V_State = Delta230V_BTN;
}
