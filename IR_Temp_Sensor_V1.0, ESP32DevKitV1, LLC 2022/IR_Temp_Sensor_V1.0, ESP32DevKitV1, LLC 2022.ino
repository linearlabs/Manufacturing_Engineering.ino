//IR_Temp_Sensor_V1.0, ESP32DevKitV1, LLC 2022

#include <Adafruit_MLX90614.h>
#define PWM1_GPIO 32 // pwm1 gpio
#define PWM1_Ch 0 // PWM1 channel 0-15
#define PWM1_Res 16 // pwm1 resolution 1-16 bits
#define PWM1_Freq 1000 // pwm1 freq 1kHz
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
unsigned int PWM1_DutyCycle = 65535; //  pwm1 duty cycle 0-65535
float temp1;
float temp1_map;
void setup() {
  Serial.begin(115200);
  while (!Serial);

  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };
  ledcAttachPin(PWM1_GPIO, PWM1_Ch);
  ledcSetup(PWM1_Ch, PWM1_Freq, PWM1_Res);
  Serial.print("Emissivity = "); Serial.println(mlx.readEmissivity());
  Serial.println("================================================");
}

void loop() {
  temp1 = mlx.readObjectTempC();
  temp1_map = map(temp1,0,150,0,65535);
  ledcWrite(PWM1_Ch, temp1_map);
  //Serial.println("Ambient = "); Serial.print(mlx.readAmbientTempC());
  Serial.print("PWM1 = "); Serial.print(temp1); Serial.println("*C");
  Serial.println(temp1_map*0.002288);
  // Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF());
  // Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
  
  Serial.println();
  delay(500);
}
