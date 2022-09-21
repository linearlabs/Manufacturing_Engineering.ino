#define sensor1 A0
int sensor1Val; // adc value
int sensor1Map; // weight lbs

void setup() {
  Serial.begin(57600); // controller serial communicates at 57600
  Serial.setTimeout(20); // set to 20 (acts like delay)
}

void loop() {
  sensor1Val = analogRead(sensor1);
  sensor1Map = map(sensor1Val, 0, 1023, 0, 100); // full weight about equals 11lbs
  Serial.println(sensor1Map);
  if (sensor1Map <= 50){
    Serial.println("No wire Installed");
  }
  else if ((sensor1Map > 50) && (sensor1Map <= 75)){
    Serial.println("Not enough wire");
  }
  else if ((sensor1Map > 75) && (sensor1Map <= 80)){
    Serial.println("25%");
  }
  else if ((sensor1Map > 80) && (sensor1Map <= 85)){
    Serial.println("50%");
  }
  else if ((sensor1Map > 85) && (sensor1Map <= 90)){
    Serial.println("75%");
  }
  else if (sensor1Map > 90){
    Serial.println("100%");
  }
  delay(500);
}
