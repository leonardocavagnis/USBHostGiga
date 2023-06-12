#include "USBHost_H7.h"

USBHost_H7 usbhost;
HIDKeyboard keybd;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB HID Keyboard");

  usbhost.begin();
}

void loop() {
  if (keybd.available()) {
    auto _key = keybd.read();
    Serial.println(keybd.getAscii(_key));
  }
}