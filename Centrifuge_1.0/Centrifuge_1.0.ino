
#include <AccelStepper.h>
#include <LiquidCrystal.h>
// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:

#define dirPin 3
#define stepPin 2
#define motorInterfaceType 1
#define but_1 4
#define en_1 24


// Create a new instance of the AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

LiquidCrystal lcd(43, 42, 41, 40, 39, 38);

void setup() {
  pinMode (but_1, INPUT);
  pinMode (en_1, OUTPUT);
  digitalWrite(en_1, HIGH);
  lcd.begin(16, 2);
  stepper.setCurrentPosition(0);


}

void loop() {
  int sensorReading1 = analogRead(A1);
  float f = map(sensorReading1, 0, 1023, 0, 8685);
  int sensorReading = analogRead(A0);
  float motorSpeed = map(sensorReading, 0, 1023, 0, 1002);
  float steps = motorSpeed / 3.34 * f;                   //193f about equals 1 minute so 8685 lasts 45 minutes
  float t = steps / motorSpeed / 3.34 / 17.29;
  int sensorReading2 = analogRead(A2);
  int a = map(sensorReading2, 0, 1023, 0, 99);
  stepper.setAcceleration(a);

  lcd.setCursor(0, 0);
  lcd.print("RPM:");
  lcd.setCursor(4, 0);
  lcd.print(motorSpeed / 3.34 * 1.74);
  lcd.setCursor(11, 0);
  lcd.print("A:");
  lcd.setCursor(13, 0);
  lcd.print(a);

  lcd.setCursor(0, 1);
  lcd.print("RunTime:");
  lcd.setCursor(8, 1);
  lcd.print(f / 193);
  lcd.setCursor(13, 1);
  lcd.print("min");

  int val1 = digitalRead(but_1);

  if (val1 == HIGH) {
    digitalWrite(en_1, LOW);
    stepper.setMaxSpeed(motorSpeed);
    stepper.setCurrentPosition(0);
    stepper.runToNewPosition(steps);
    digitalWrite(en_1, HIGH);
    stepper.setCurrentPosition(0);
  }

  if (val1 == LOW) {
    stepper.setCurrentPosition(0);
  }
}
