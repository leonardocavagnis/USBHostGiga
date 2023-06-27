#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

USBHost_H7 usbhost;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB HUB Test");

  usbhost.begin();
}

void loop() {

}