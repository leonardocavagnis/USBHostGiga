#include "USBHost_H7.h"

USBHost_H7 usbhost;
HIDMouse mouse;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB HID Mouse");

  usbhost.begin();
}

void loop() {
  if (mouse.available()) {
    auto _key = mouse.read();
    
    Serial.println(_key.x);
    Serial.println(_key.y);
  }
}