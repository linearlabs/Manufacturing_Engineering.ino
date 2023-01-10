//EndOfLineTesterV1.0_Teensy4.0_LLC2022

#define pwm_pin 15

void setup() {
  analogWriteFrequency(pwm_pin, 1000);  // https://www.pjrc.com/teensy/td_pulse.html
  analogWriteResolution(8);             // https://www.pjrc.com/teensy/td_pulse.html
  pinMode(pwm_pin, OUTPUT);
}
void loop() {
  analogWrite(pwm_pin, 0);
  delay(2000);

  analogWrite(pwm_pin, 127);
  delay(2000);
  analogWrite(pwm_pin, 255);
  delay(2000);
}