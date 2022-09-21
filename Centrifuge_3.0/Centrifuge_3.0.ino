//Frequency and RPM tachometer

#define pwmBuzzer 9                       //alarm
#define timeSwitch 2                      //45 mins or 90 minute count down timer
#define hallSensorA 8                     //this has to be pin 8 ICP1 (timer capture pin)
#define pwmOutB 10                        //motor
#define pwmOutA 3 
#define StartSwitchA 4                    //Start A
#define StartSwitchB 5
#define StopSwitchA 12                    //Stop A
#define StopSwitchA 13
#define enLedA 11
#define enLedA 7

unsigned long millisK;
unsigned int pulseHighA;                                   // Integer variable to capture High time of the incoming pulse
unsigned int pulseLowA;                                    // Integer variable to capture Low time of the incoming pulse
unsigned int pulseTotalA;                                  // Float variable to capture Total time of the incoming pulse
unsigned int frequencyA;                                   // Calculated Frequency
int dutyCycleA;
unsigned int rpmA;
int secondsA;
int minutesA;
int eRPMA;
int dCycleA;
int errorA;
int val1;
int val2;
int val3;
int val4;



void setup() {

  //TCCR1B = TCCR1B & B11111000 | B00000010;                // for PWM frequency of 3921.16 Hz
  TCCR2B = TCCR2B & B11111000 | B00000010;                //for PWM frequency of 3921.16 Hz
  pinMode(hallSensorA, INPUT);                             //(increaing pwm frequency helped with accuracy) added 0.1uF to smooth out ripple
  pinMode(pwmOutA, OUTPUT);
  pinMode(pwmBuzzer, OUTPUT);
  pinMode(StartSwitchA, INPUT);
  pinMode(StopSwitchA, INPUT);
  pinMode(timeSwitch, INPUT);
  pinMode(enLedA, OUTPUT);
  digitalWrite (enLedA, HIGH);
  millisK = millis() + 1000;
  Serial.begin(57600);                                    //Intiates Serial communications

}



void loop() {


  val1 = digitalRead(StartSwitchA);
  val2 = digitalRead(timeSwitch);
  val4 = digitalRead(StopSwitchA);

  if (val1 == LOW) {                                          //running, Val1 = Start Switch A
    digitalWrite (enLedA, LOW);
  }
  if (val4 == LOW) {                                          //running, Val1 = Start Switch A
    digitalWrite (enLedA, HIGH);
  }
  val3 = digitalRead(enLedA);                               //val3 = enLedA



  if (val3 == LOW) {                                        //val3 = LED1

    dutyCycleA = 115;                                        //automatic control
    analogWrite(pwmOutA, dutyCycleA);

    pulseHighA = pulseIn(hallSensorA, HIGH);                  //measures high period
    pulseLowA = pulseIn(hallSensorA, LOW);                    //measures low period
    pulseTotalA = pulseHighA + pulseLowA;                      //Time period of the pulse in microsecondsAAA
    frequencyA = 1000000 / pulseTotalA;                       //Frequency in Hertz (Hz)
    rpmA = 750000 / pulseTotalA;                              //frequency converted to RPM //rpm = frequency x 60;
    dCycleA = dutyCycleA / 2.25;                              //duty cycle as a percent
    eRPMA = dCycleA * 41.2;                                   //expect RPM for error detection = duty cycle percent * ratio... DutyCycle% : RPM = 1:44
    errorA = rpmA * 12;                                       //error triggers when RPM is 20% less than expected RPM

    if (errorA < eRPMA) {
      analogWrite(pwmBuzzer, 112);
      Serial.println("error!!!!");
    }

    if (errorA > eRPMA) {
      analogWrite(pwmBuzzer, 000);
    }

    if (millis() > millisK) {                               //countdown timer. try to keep things in if statements to reduce processing power
      secondsA -= 1;
      millisK = millisK + 1000;

      Serial.print(frequencyA);                              //range seems to be anything above 15Hz (940RPM)
      Serial.println(" Hz");
      Serial.print(dCycleA);
      Serial.println("% Duty Cycle");
      Serial.print(errorA);
      Serial.println(" Error");
      Serial.print(rpmA);
      Serial.print("0");
      Serial.println(" RPM");
      Serial.print(eRPMA);
      Serial.println(" Expected RPM");
      Serial.print(minutesA);                              //range seems to be anything above 15Hz (940RPM)
      Serial.print(":");
      Serial.println(secondsA);

      if ((minutesA == 0) && (secondsA == 0)) {
        digitalWrite (enLedA, HIGH);
      }
      if (val1 == LOW) {
        digitalWrite (enLedA, HIGH);
      }
    }

    if (secondsA < 1) {
      secondsA = 60;
      minutesA -= 1;
    }
  }


  if ((val1 == HIGH) && (val3 == HIGH)) {                     //standby state       val1=sw1   val3=LED1

    if (millis() > millisK) {                               //countdown timer. try to keep things in if statements to reduce processing power
      secondsA += 1;
      millisK = millisK + 1000;
      Serial.println("off");
      Serial.print(minutesA);                                //range seems to be anything above 15Hz (940RPM)
      Serial.println(":00");
      analogWrite(pwmOutA, 0);
    }

    if (val2 == LOW) {
      minutesA = 2;
    }
    if (val2 == HIGH) {
      minutesA = 1;
    }
    secondsA = 0;
  }
}
