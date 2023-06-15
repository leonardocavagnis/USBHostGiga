#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

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
    HIDMouse_evt mou_evt = mouse.read();
    
    Serial.print("Buttons: ");
    Serial.print(mou_evt.buttons[0]);
    Serial.print(",");
    Serial.print(mou_evt.buttons[1]);
    Serial.print(",");
    Serial.println(mou_evt.buttons[2]);

    Serial.print("X,Y: ");
    Serial.print(mou_evt.x);
    Serial.print(",");
    Serial.println(mou_evt.y);
  }
}