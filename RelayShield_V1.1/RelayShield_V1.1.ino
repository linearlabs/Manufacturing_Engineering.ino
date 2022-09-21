//Relay_Shield V1.1

#include <AccelStepper.h> // stepper library that is good at controlling accelleration and runs a bit smoother than others
#include <StringSplitter.h>
#define relay1 7 // pins to relays, active low turns them on
#define relay2 6
#define relay3 5
#define relay4 4
String idn = "Arduino, Relay_Shield, V1.1";
String Relays = ""; // bool variable used to turn on A
String inputStr;
int relayA = 0;
int relayB = 0;
int relayC = 0;
int relayD = 0;
bool relayIndex = 0; // turns relays on and off only once so commands what execute over and over

void setup() {
  pinMode (relay1, OUTPUT); // outputs to relays, active low turns them on, initially set LOW
  pinMode (relay2, OUTPUT);
  pinMode (relay3, OUTPUT);
  pinMode (relay4, OUTPUT);
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  digitalWrite(relay3, LOW);
  digitalWrite(relay4, LOW);
  Serial.begin(115200);
  Serial.setTimeout(50); // set to 50 because it adds too much delay to relay responce
}

void store(String input) {
  String startStr = input.substring(0, 5);
  if (startStr == "*idn?") { //serial input relay 1
    Serial.println(idn);
  }
  else if (startStr == "rela:") { //serial input relay 1
    Relays = input.substring(5);
    StringSplitter *splitter = new StringSplitter(Relays, ',', 4); // new StringSplitter(string_to_split, delimiter, limit)
    int itemCount = splitter->getItemCount();
    if (itemCount == 4) {
      String item1 = splitter->getItemAtIndex(0); // item index is the part in the string to display
      String item2 = splitter->getItemAtIndex(1);
      String item3 = splitter->getItemAtIndex(2);
      String item4 = splitter->getItemAtIndex(3);
      relayA = item1.toInt(); // getValueInt converts the string value into int
      relayB = item2.toInt(); // getValueInt converts the string value into int
      relayC = item3.toInt(); // getValueInt converts the string value into int
      relayD = item4.toInt(); // getValueInt converts the string value into int
      Serial.print("rela:");
      Serial.println(Relays);
      Relays = "";
      relayIndex = 1;
    }
    else {
      Serial.print("error:");
      Serial.println(Relays);
      
      Relays = "";
    }
  }
}

void relayStates() {
  if (relayIndex == 1) {
    if (relayA == 1){
      digitalWrite(relay1, HIGH);
      delay(20);
    }
    if (relayA == 0){
      digitalWrite(relay1, LOW);
      delay(20);
    }
    if (relayB == 1){
      digitalWrite(relay2, HIGH);
      delay(20);
    }
    if (relayB == 0){
      digitalWrite(relay2, LOW);
      delay(20);
    }
    if (relayC == 1){
      digitalWrite(relay3, HIGH);
      delay(20);
    }
    if (relayC == 0){
      digitalWrite(relay3, LOW);
      delay(20);
    }
    if (relayD == 1){
      digitalWrite(relay4, HIGH);
      delay(20);
    }
    if (relayD == 0){
      digitalWrite(relay4, LOW);
      delay(20);
    }
    relayIndex = 0;
  }
}

void loop() {
  inputStr = Serial.readStringUntil('\n'); // serial input function
  store(inputStr); // serial input function
  relayStates();

}
