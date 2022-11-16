// open the file. note that only one file can be open at a time,
// so you have to close this one before opening another.
void writeSD() {
  strcpy(fileLog, dateNow); // dateNow.txt
  strcat(fileLog, fileType);
  myFile = SD.open(fileLog, FILE_WRITE);
  if (myFile) { // if the file opened okay, write to it:
    myFile.print(dateNow); myFile.print(","); myFile.print(timeNow); myFile.print(","); myFile.print(tempIn); myFile.println(", degC");
    myFile.close(); // close the file:
  }
  else {
    while (!SD.begin(ChipSelect)) {
      Serial.println("SD initialization failed!");
      display.clearDisplay();
      display.setCursor(0, 0); // Start at top-left corner
      display.println("SD Not Installed");
      display.display();
      delay(1000);
    }
  }
}
void readSD() {
  myFile = SD.open("test.txt"); // re-open the file for reading:
  if (myFile) {
    Serial.println("test.txt:");
    while (myFile.available()) { // read from the file until there's nothing else in it:
      Serial.write(myFile.read());
    }
    myFile.close(); // close the file:
  }
  else {
    Serial.println("error opening test.txt"); // if the file didn't open, print an error:
  }
}
