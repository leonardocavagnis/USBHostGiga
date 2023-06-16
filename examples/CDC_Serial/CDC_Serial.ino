#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

USBHost_H7 usbhost;
CDCSerial host;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB CDC Serial");

  usbhost.begin();
  host.begin();
}

void loop() {
  while (host.available()) {
    auto _char = host.read();
    Serial.write(_char);
  }
}