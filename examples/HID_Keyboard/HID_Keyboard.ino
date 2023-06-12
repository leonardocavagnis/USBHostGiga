#include "USBHost_H7.h"

Keyboard keybd;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB HID Keyboard");

  keybd.begin();
}

void loop() {
  if (keybd.available()) {
    auto _key = keybd.read();
    Serial.println(keybd.getAscii(_key));
  }
}