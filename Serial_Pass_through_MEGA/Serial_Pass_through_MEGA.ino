

void setup() {
  Serial.begin(115200); // controller serial communicates at 57600
  Serial1.begin(115200); // controller serial communicates at 57600
}

void loop() {
  if (Serial1.available()) {

    int inByte = Serial1.read();

    Serial.write(inByte);

  }

  // read from port 0, send to port 1:

  if (Serial.available()) {

    int inByte = Serial.read();

    Serial1.write(inByte);

  }
}
