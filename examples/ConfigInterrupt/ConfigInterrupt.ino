/*
  LPS22HB - Pressure interrupt
  This example config the INTERRUPT pin of the sensor
  that allows to use interrupts attached to that pin
  and do something, for example light up a LED

  This example code is in the public domain.
*/

#include <Arduino_LPS22HB.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  if (!BARO.begin()) {
    Serial.println("Failed BAro");
    while (1);
  }

  // Config for the Interrupts and the Interrupt pin
  BARO.enableInterruptPin();              // Enables ic's pin
  BARO.setOpenDrain();                    // Config ic's pin to be in a open drain
  BARO.setActiveHigh();                   // High means interrupt
  BARO.enableHighPressureInterrupt();     // We set to interrupt when the pressure is higher than the Threshold setThreshold()
}

void loop() {
  // put your main code here, to run repeatedly:
  BARO.setThreshold(16);                   // +-Interrupt threshold = value /16 , in this case 16/16 = 1 hPa
  Serial.println(BARO.readPressure());    // The interrupt is updated when it has been read
  delay(1500);

  BARO.setThreshold(20000);               // +-Interrupt threshold = value /16 , in this case 20000/16 = 125 hPa
  Serial.println(BARO.readPressure());    // The interrupt is updated when it has been read
  delay(1500);
}
