#include "OTA.h"
unsigned long entry;
void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
/*
  ArduinoOTA.setHostname("M5StickC");
  setupOTA();

  // your code
  pinMode(pin,OUTPUT);
*/
}

void loop() {
  ArduinoOTA.handle();
  /*
  // your code here
  digitalWrite(pin,LOW);
  delay(100);
  digitalWrite(pin,HIGH);
  delay(100);
*/
}
