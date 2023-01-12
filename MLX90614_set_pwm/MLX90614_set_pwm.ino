//set PWM mode on MLX90614

#include <Adafruit_MLX90614.h>

//== CHANGE THIS ============
// double new_emissivity = 0.95;
//===========================
double I2C_MODE = 0x201;  // 513 in dec (temporarily short SCL to ground to get out of PWM mode and back into I2C mode for EEPROM Programming)
double PWM_MODE = 0x207;   // 519 in dec

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;

  Serial.println("Adafruit MLX90614 Setter.\n");

  // init sensor
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1)
      ;
  };

  // read current PWM register
  Serial.print("Current PWM register = ");
  Serial.println(mlx.readPWMCTRLReg());

  //   // set new emissivity
  //   Serial.print("Setting emissivity = "); Serial.println(new_emissivity);
  //   mlx.writeEmissivity(new_emissivity); // this does the 0x0000 erase write

  //set PWM_MODE
  Serial.print("Setting PWM_MODE = ");
  Serial.println(PWM_MODE);
  mlx.writePWMCTRL(PWM_MODE);  // this does the 0x0000 erase write

  //  // read back
  Serial.print("Reading New PWM register = ");
  Serial.println(mlx.readPWMCTRL());

  //   // done
  //   Serial.print("DONE. Restart the module.");
}

void loop() {
}