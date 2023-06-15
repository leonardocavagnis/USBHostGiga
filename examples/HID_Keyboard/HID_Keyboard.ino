#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

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
    HIDKeyboard_evt keybd_evt = keybd.read();
    
    Serial.println(keybd.getAscii(keybd_evt));
  }
}