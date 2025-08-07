#include <Wire.h>
#include <Adafruit_PCF8591.h>

#define ADC_REF_VOLTAGE 3.3  // Same as your ESP32 VCC
#include <Adafruit_PCF8591.h>
Adafruit_PCF8591 pcf = Adafruit_PCF8591();


void setup() {
  Serial.begin(115200);
  Wire.begin(22, 27);  // SDA=GPIO22, SCL=GPIO27 on CYD
  if (!pcf.begin()) {
    Serial.println("PCF8591 not found!");
    while (1);
  }
  Serial.println("PCF8591 detected at 0x48");
}

void loop() {
  uint8_t val = pcf.analogRead(0); // Read AIN0
  float volts = (val / 255.0) * ADC_REF_VOLTAGE *4.3;
//Vin = Vnode * ((33.0 + 10.0) / 10.0) // i.e., *4.3
//So you can just do: Vin ≈ adc * (3.3 / 255.0) * 4.3

  Serial.print("A0: ");
  Serial.print(val);
  Serial.print(" → ");
  Serial.print(volts, 3);
  Serial.println(" V");
  delay(500);
}
