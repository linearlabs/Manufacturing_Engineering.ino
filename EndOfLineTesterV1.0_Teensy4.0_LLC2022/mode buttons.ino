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

          digitalWrite(EN_230V, HIGH);  // Enable on 230V VFD
          digitalWrite(EN_460V, LOW);   // Enable on 460V VFD
          digitalWrite(M_Type1, LOW);  // enables Delta Star Enclosure Opto Isolators
          digitalWrite(M_Type2, HIGH);   // enables Wye/Double Wye Enclosure Opto Isolators
          digitalWrite(relay2, HIGH);   // 230V vfd Relays & 230V motor voltage selector Relays
          digitalWrite(relay1, LOW);    // 460V vfd Relays & 460V motor voltage selector Relays
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

          digitalWrite(EN_230V, LOW);   // Enable on 230V VFD
          digitalWrite(EN_460V, HIGH);  // Enable on 460V VFD
          digitalWrite(M_Type1, LOW);  // enables Delta Star Enclosure Opto Isolators
          digitalWrite(M_Type2, HIGH);   // enables Wye/Double Wye Enclosure Opto Isolators
          digitalWrite(relay2, LOW);    // 230V vfd Relays & 230V motor voltage selector Relays
          digitalWrite(relay1, HIGH);   // 460V vfd Relays & 460V motor voltage selector Relays
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

          digitalWrite(EN_230V, LOW);   // Enable on 230V VFD
          digitalWrite(EN_460V, HIGH);  // Enable on 460V VFD
          digitalWrite(M_Type1, HIGH);   // enables Delta Star Enclosure Opto Isolators
          digitalWrite(M_Type2, LOW);  // enables Wye/Double Wye Enclosure Opto Isolators
          digitalWrite(relay2, LOW);    // 230V vfd Relays & 230V motor voltage selector Relays
          digitalWrite(relay1, HIGH);   // 460V vfd Relays & 460V motor voltage selector Relays
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

          digitalWrite(EN_230V, HIGH);   // Enable on 230V VFD
          digitalWrite(EN_460V, LOW);  // Enable on 460V VFD
          digitalWrite(M_Type1, HIGH);   // enables Delta Star Enclosure Opto Isolators
          digitalWrite(M_Type2, LOW);  // enables Wye/Double Wye Enclosure Opto Isolators
          digitalWrite(relay2, HIGH);    // 230V vfd Relays & 230V motor voltage selector Relays
          digitalWrite(relay1, LOW);   // 460V vfd Relays & 460V motor voltage selector Relays
        }
      }
    }
  }
  last_Delta230V_State = Delta230V_BTN;
}
