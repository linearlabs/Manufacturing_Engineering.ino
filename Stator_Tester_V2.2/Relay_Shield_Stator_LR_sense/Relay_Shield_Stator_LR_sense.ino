//stator tester Relay sheild for LRC since leads


#define relayOut1 7                            //pins to relays, active low turns them on
#define relayOut2 6
#define relayOut3 5
#define relayOut4 4

#define relayIn2 A0                           //digital inputs
#define relayIn3 A1
#define relayIn4 A2
#define relayIn5 A3
#define relayIn6 A4
#define relayIn7 A5

int in2;
int in3;
int in4;
int in5;
int in6;
int in7;

void setup() {
  pinMode (relayIn2, INPUT);                  //Input From main relays, active low turns them on, initially set high. Opposite of what the other relay Board is
  pinMode (relayIn3, INPUT);
  pinMode (relayIn4, INPUT);
  pinMode (relayIn5, INPUT);
  pinMode (relayIn6, INPUT);
  pinMode (relayIn7, INPUT);

  pinMode (relayOut1, OUTPUT);                //output to relays, active high turns them on, initially set low
  pinMode (relayOut2, OUTPUT);
  pinMode (relayOut3, OUTPUT);
  pinMode (relayOut4, OUTPUT);
  digitalWrite(relayOut1, LOW);               //Relays are active high, Opposite of what the other relay Board is
  digitalWrite(relayOut2, LOW);
  digitalWrite(relayOut3, LOW);
  digitalWrite(relayOut4, LOW);
}

void loop() {
  in2 = digitalRead(relayIn2);
  in3 = digitalRead(relayIn3);
  in4 = digitalRead(relayIn4);
  in5 = digitalRead(relayIn5);
  in6 = digitalRead(relayIn6);
  in7 = digitalRead(relayIn7);

  //A to B LRC test
  if ((in2 == LOW) && (in3 == HIGH) && (in4 == HIGH) && (in5 == LOW) && (in6 == HIGH) && (in7 == HIGH)) {
    digitalWrite(relayOut1, HIGH);
    digitalWrite(relayOut2, LOW);
    digitalWrite(relayOut3, HIGH);
    digitalWrite(relayOut4, LOW);
  }

  //A to C LRC test
  else if ((in2 == LOW) && (in3 == HIGH) && (in4 == HIGH) && (in5 == HIGH) && (in6 == HIGH) && (in7 == LOW)) {
    digitalWrite(relayOut1, HIGH);
    digitalWrite(relayOut2, LOW);
    digitalWrite(relayOut3, HIGH);
    digitalWrite(relayOut4, HIGH);
  }

  //B to C LRC test
  else if ((in2 == HIGH) && (in3 == HIGH) && (in4 == LOW) && (in5 == LOW) && (in6 == HIGH) && (in7 == LOW)) {
    digitalWrite(relayOut1, HIGH);
    digitalWrite(relayOut2, HIGH);
    digitalWrite(relayOut3, HIGH);
    digitalWrite(relayOut4, HIGH);
  }
  else {
    digitalWrite(relayOut4, LOW);
    digitalWrite(relayOut3, LOW);
    digitalWrite(relayOut2, LOW);
    digitalWrite(relayOut1, LOW);
  }
}
