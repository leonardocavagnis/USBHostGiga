#include "USBHost_H7.h"

//REDIRECT_STDOUT_TO(Serial)

USBHost_H7  usbhost;
HUBDevice   hub;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("USB HUB Test");

  usbhost.begin();
  hub.begin();
}

void loop() {
  int num_ports = hub.available();
  if (num_ports > 0) {
    Serial.print("Hub detected with ");
    Serial.print(num_ports);
    Serial.println(" ports.");

    while(hub.available() > 0);
    Serial.println("Hub disconnected.");
  }
}