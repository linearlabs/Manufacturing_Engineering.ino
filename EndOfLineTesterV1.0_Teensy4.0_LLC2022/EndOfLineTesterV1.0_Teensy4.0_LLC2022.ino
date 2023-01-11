//EndOfLineTesterV1.0_Teensy4.0_LLC2022

#define CLK 0       // rotary encoder
#define DT 1        // rotary encoder
#define SW 2        // rotary encoder
#define rdy_pin 14  // 10Vin
#define pwm_pin 15  // speed wiper DAC

int counter = 255;  // pwm to speed potentiometer value 0-255 (0=fullspeed)
int currentStateCLK;
int lastStateCLK;
bool enable_speed = false;  // encoder knob enable state
bool btnState;
bool btnFlag = 0;

void setup() {
  analogWriteFrequency(pwm_pin, 1000);  // https://www.pjrc.com/teensy/td_pulse.html
  analogWriteResolution(8);             // https://www.pjrc.com/teensy/td_pulse.html
  pinMode(pwm_pin, OUTPUT);
  pinMode(rdy_pin, OUTPUT);     // X Driver enable and LED
  digitalWrite(rdy_pin, HIGH);  // enables speed
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(115200);
}
void buttonState() {
  btnState = digitalRead(SW);                 // Read the button state
  if ((btnState == LOW) && (btnFlag == 0)) {  //If we detect LOW signal, button is pressed
    btnFlag = 1;
  }
  if ((btnState == HIGH) && (btnFlag == 1)) {  //If we detect LOW signal, button is pressed
    btnFlag = 0;
    if (enable_speed == true) {
      enable_speed = false;
      counter = 255;
      Serial.println("Speed: Disengaged");  // button has been pressed
      digitalWrite(rdy_pin, HIGH);          // enables speed
      analogWrite(pwm_pin, counter);
    } else {
      enable_speed = true;
      counter = 255;
      Serial.println("Speed: Engaged");  // button has been pressed
      digitalWrite(rdy_pin, LOW);        // enables speed
      analogWrite(pwm_pin, counter);
    }
  }
}
void loop() {
  buttonState();
  while (enable_speed == true) {
    currentStateCLK = digitalRead(CLK);  // Read the current state of CLK
    buttonState();
    if (currentStateCLK != lastStateCLK) {       // If last and current state of CLK are different, then pulse occurred
      if (digitalRead(DT) != currentStateCLK) {  // If the DT state is different than the CLK state then
        if (counter > 1) {
          counter = counter - 5;
          analogWrite(pwm_pin, counter);
          Serial.print("Counter: ");
          Serial.println(counter);
        }
      } else {
        if (counter <= 255) {
          counter = counter + 5;
          analogWrite(pwm_pin, counter);
        }
      }
    }
    lastStateCLK = currentStateCLK;  // Remember last CLK state
  }
}
