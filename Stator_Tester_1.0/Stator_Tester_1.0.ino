//STATOR TESTER

#define ledN 3
#define ledS 4
//#define sensorA0 A0

int hallA;
int sensorA0;

void setup() {
  pinMode (ledN, OUTPUT);                         //X Driver enable and LED
  digitalWrite(ledN, LOW);                       //TB6600 driver enable needs pull up resistor and high on start up
  pinMode (ledS, OUTPUT);
  digitalWrite(ledS, LOW);
  Serial.begin(57600);

}

void loop() {
  sensorA0 = analogRead(A0);
  hallA = map(sensorA0, 0, 1023, -507, 517);
  Serial.println(hallA);

  if (hallA > 3) {
    digitalWrite(ledS, HIGH);
  }
  else {
    digitalWrite(ledS, LOW);
  }

  if (hallA < -3) {
    digitalWrite(ledN, HIGH);
  }
  else {
    digitalWrite(ledN, LOW);
  }
}
