#include <M5StickC.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "AXP192.h"

float bat_volt = 0;
float bat_cur = 0;
float usb_volt = 0;
float usb_cur =0;
float external_volt =0;
float external_cur = 0;
float power =0;

//#define pin 10
# define Mpin 36
const char* ssid = "setu";
const char* password = "12345678z";

void setup() {
  M5.begin();
  M5.Axp.EnableCoulombcounter();
  pinMode(Mpin, INPUT);
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(25, 30);
  M5.Lcd.setTextSize(2);
  Serial.begin(115200);
  Serial.println("Booting");
  //pinMode(pin,OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }


  //Set Host Name
  ArduinoOTA.setHostname("M5StickC_Setu");

 


  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();
  //M5.Lcd.print("SETU");
  M5.Lcd.setCursor(45, 30);
  //M5.Lcd.print(WiFi.localIP());
  
  bat_volt = M5.Axp.GetBatVoltage();
  bat_cur = M5.Axp.GetBatCurrent();
  usb_volt = M5.Axp.GetVBusVoltage();
  usb_cur = M5.Axp.GetVBusCurrent();
  external_volt = M5.Axp.GetVinVoltage();
  external_cur = M5.Axp.GetVinCurrent();
  power = bat_volt*bat_cur;
  
  M5.Lcd.print(power);
  delay(500);
  
  digitalWrite(pin,LOW);
  delay(50);
  */
}
