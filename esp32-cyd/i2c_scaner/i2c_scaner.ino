#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);  // optional on ESP32
  delay(2000);

  Serial.println("=== I2C Scanner Start ===");

  Wire.begin(22, 27);  // SDA, SCL from CN1


}

void loop() {

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at 0x");
      Serial.println(address, HEX);
      delay(2000);
    }
  }

  Serial.println("=== Scan Complete ===");



}
