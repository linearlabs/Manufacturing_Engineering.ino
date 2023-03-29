void relaysOff() {              // active LOW
  digitalWrite(EN_230V, HIGH);  // Enable on 230V VFD
  digitalWrite(EN_460V, HIGH);  // Enable on 460V VFD
  digitalWrite(M_Type1, HIGH);  // enables Delta Star Enclosure Opto Isolators
  digitalWrite(M_Type2, HIGH);  // enables Wye/Double Wye Enclosure Opto Isolators
  digitalWrite(relay2, HIGH);   // 460V vfd Relays & 460V motor voltage selector Relays
  digitalWrite(relay1, HIGH);   // 230V vfd Relays & 230V motor voltage selector Relays
}

// YY230V Relays
void YY230V_Relays() {
  digitalWrite(M_Type1, HIGH);  // enables Delta Star Enclosure Opto Isolators
  digitalWrite(M_Type2, LOW);   // enables Wye/Double Wye Enclosure Opto Isolators
  digitalWrite(relay1, LOW);    // 230V vfd Relays & 230V motor voltage selector Relays
  digitalWrite(relay2, HIGH);   // 460V vfd Relays & 460V motor voltage selector Relays
  delay(200);
  // enable VFDs last
  digitalWrite(EN_230V, LOW);   // Enable on 230V VFD
  digitalWrite(EN_460V, HIGH);  // Enable on 460V VFD
}

// Y460V Relays
void Y460V_Relays() {
  digitalWrite(M_Type1, HIGH);  // enables Delta Star Enclosure Opto Isolators
  digitalWrite(M_Type2, LOW);   // enables Wye/Double Wye Enclosure Opto Isolators
  digitalWrite(relay1, HIGH);   // 230V vfd Relays & 230V motor voltage selector Relays
  digitalWrite(relay2, LOW);    // 460V vfd Relays & 460V motor voltage selector Relays
  // enable VFDs last
  delay(200);
  digitalWrite(EN_230V, HIGH);  // Enable on 230V VFD
  digitalWrite(EN_460V, LOW);   // Enable on 460V VFD
}

// Y460V2 Relays
void Y460V2_Relays() {
  digitalWrite(M_Type1, LOW);   // enables Delta Star Enclosure Opto Isolators
  digitalWrite(M_Type2, HIGH);  // enables Wye/Double Wye Enclosure Opto Isolators
  digitalWrite(relay1, HIGH);   // 230V vfd Relays & 230V motor voltage selector Relays
  digitalWrite(relay2, LOW);    // 460V vfd Relays & 460V motor voltage selector Relays
  // enable VFDs last
  delay(200);
  digitalWrite(EN_230V, HIGH);  // Enable on 230V VFD
  digitalWrite(EN_460V, LOW);   // Enable on 460V VFD
}

// Delta230V Relays
void delta230V_Relays() {
  digitalWrite(M_Type1, LOW);   // enables Delta Star Enclosure Opto Isolators
  digitalWrite(M_Type2, HIGH);  // enables Wye/Double Wye Enclosure Opto Isolators
  digitalWrite(relay1, LOW);   // 230V vfd Relays & 230V motor voltage selector Relays
  digitalWrite(relay2, HIGH);  // 460V vfd Relays & 460V motor voltage selector Relays
  // enable VFDs last
  delay(200);
  digitalWrite(EN_230V, LOW);   // Enable on 230V VFD
  digitalWrite(EN_460V, HIGH);  // Enable on 460V VFD
}
