// menu false = dateTime main menu
// menu true = edit date or Time selection

void Edit_Time_Buttons() {
  scanButtons();
  if ((enter > 500) && (enterIndex == 0)) {
    enterIndex = 1;
    menu = 1;
  }
  while (menu == true) { // edit date time menu
    scanButtons();
    DateTime now = rtc.now();
    if (selection != 0) {
      yearNow = now.year(), DEC;
      yearNow = yearNow - 2000;
    }
    if (selection != 1) {
      monthNow = now.month(), DEC;
    }
    if (selection != 2) {
      dayNow = now.day(), DEC;
    }
    if (selection != 3) {
      hourNow = now.hour(), DEC;
    }
    if (selection != 4) {
      minNow = now.minute(), DEC;
    }
    if (selection != 5) {
      secNow = now.second(), DEC;
    }
    if (selection == 6) {
      selection = 0;
      menu = false;
    }
    display.clearDisplay();
    display.setCursor(0, 0); // Start at top-left corner
    display.println(LLC);
    display.println();
    if (selection == 0) {
      yearNow = yearNow + add_subtract;
      add_subtract = 0;
      if (yearNow > 99) {
        yearNow = 22;
      }
      if (yearNow < 22) {
        yearNow = 99;
      }
      display.setTextColor(BLACK, WHITE);
      display.print(yearNow); // selected
      display.setTextColor(WHITE);
      display.print("/"); display.print(monthNow); display.print("/"); display.print(dayNow); display.print(" "); display.print(hourNow); display.print(":"); display.print(minNow); display.print(":"); display.println(secNow);
    }
    if (selection == 1) {
      monthNow = monthNow + add_subtract;
      add_subtract = 0;
      if (monthNow  > 12) {
        monthNow  = 1;
      }
      if (monthNow  < 1) {
        monthNow  = 12;
      }
      display.print(yearNow); display.print("/");
      display.setTextColor(BLACK, WHITE);
      display.print(monthNow); // selected
      display.setTextColor(WHITE);
      display.print("/"); display.print(dayNow); display.print(" "); display.print(hourNow); display.print(":"); display.print(minNow); display.print(":"); display.println(secNow);
    }
    if (selection == 2) {
      dayNow = dayNow + add_subtract;
      add_subtract = 0;
      if (dayNow  > 31) {
        dayNow  = 1;
      }
      if (dayNow  < 1) {
        dayNow  = 31;
      }
      display.print(yearNow); display.print("/"); display.print(monthNow); display.print("/");
      display.setTextColor(BLACK, WHITE);
      display.print(dayNow);
      display.setTextColor(WHITE);
      display.print(" "); display.print(hourNow); display.print(":"); display.print(minNow); display.print(":"); display.println(secNow);
    }
    if (selection == 3) {
      hourNow = hourNow + add_subtract;
      add_subtract = 0;
      if (hourNow  > 23) {
        hourNow  = 0;
      }
      if (hourNow  < 0) {
        hourNow  = 23;
      }
      display.print(yearNow); display.print("/"); display.print(monthNow); display.print("/"); display.print(dayNow); display.print(" ");
      display.setTextColor(BLACK, WHITE);
      display.print(hourNow);
      display.setTextColor(WHITE);
      display.print(":"); display.print(minNow); display.print(":"); display.println(secNow);
    }
    if (selection == 4) {
      minNow = minNow + add_subtract;
      add_subtract = 0;
      if (minNow  > 59) {
        minNow  = 0;
      }
      if (minNow  < 0) {
        minNow  = 59;
      }
      display.print(yearNow); display.print("/"); display.print(monthNow); display.print("/"); display.print(dayNow); display.print(" "); display.print(hourNow); display.print(":");
      display.setTextColor(BLACK, WHITE);
      display.print(minNow);
      display.setTextColor(WHITE);
      display.print(":"); display.println(secNow);
    }
    if (selection == 5) {
      secNow = secNow + add_subtract;
      add_subtract = 0;
      if (secNow  > 59) {
        secNow  = 0;
      }
      if (secNow  < 0) {
        secNow  = 59;
      }
      display.print(yearNow); display.print("/"); display.print(monthNow); display.print("/"); display.print(dayNow); display.print(" "); display.print(hourNow); display.print(":"); display.print(minNow); display.print(":");
      display.setTextColor(BLACK, WHITE);
      display.println(secNow);
      display.setTextColor(WHITE);
    }
    display.println("yr/mo/dy hr:mn:ss");
    display.display();
    if ((enter > 500) && (enterIndex == 0)) {
      enterIndex = 1;
      selection = selection + 1;
      rtc.adjust(DateTime(yearNow, monthNow, dayNow, hourNow, minNow, secNow));
    }
  }
}

void scanButtons() {
  enter = ads1115.readADC_SingleEnded(0);
  up = ads1115.readADC_SingleEnded(1);
  down = ads1115.readADC_SingleEnded(2);
  escape = ads1115.readADC_SingleEnded(3);
  if ((up > 500) && (upIndex == 0)) {
    upIndex = true;
    add_subtract = add_subtract + 1;
  }
  if ((down > 500) && (downIndex == 0)) {
    downIndex = true;
    add_subtract = add_subtract - 1;
  }

  if (escape > 500) {
    menu = 0;
    selection = 0;
    add_subtract = 0; // reused variable to add or subtract values
  }
  if ((down < 500) && (downIndex == 1)) {
    downIndex = 0;
  }
  if ((up < 500) && (upIndex == 1)) {
    upIndex = 0;
  }
  if ((enter < 500) && (enterIndex == 1)) {
    enterIndex = 0;
  }
}
