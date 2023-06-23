#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

USBHost_H7 usbhost;
MSCDrive   pendrive;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("MSC Pendrive");

  usbhost.begin();
  pendrive.begin();
}

void loop() { }