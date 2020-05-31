#include<Stdio.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <M5StickC.h>

float bat_volt = 0;
float bat_cur = 0;
float usb_volt = 0;
float usb_cur =0;
float external_volt =0;
float external_cur = 0;
float power =0;

char BatV[10];
char BatC[10];
char USBV[10];
char USBC[10];
char ExtV[10];
char ExtC[10];
char Pow[10];


// Update these with values suitable for your network.

const char* ssid = "setu";
const char* password = "12345678z";
const char* mqtt_server = "192.168.43.126";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
int value = 0;


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();



}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  M5.begin();
  M5.Axp.EnableCoulombcounter();
  M5.Lcd.setRotation(1);
  M5.Lcd.setCursor(45, 30);
  M5.Lcd.setTextSize(1);
  
 
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

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
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

void loop() {
  
  ArduinoOTA.handle();
   
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  

  bat_volt = M5.Axp.GetBatVoltage();
  bat_cur = M5.Axp.GetBatCurrent();
  usb_volt = M5.Axp.GetVBusVoltage();
  usb_cur = M5.Axp.GetVBusCurrent();
  external_volt = M5.Axp.GetVinVoltage();
  external_cur = M5.Axp.GetVinCurrent();
  power = bat_volt*bat_cur;

  gcvt(bat_volt, 6, BatV);
  gcvt(bat_cur, 6, BatC);
  gcvt(usb_volt, 6, USBV);
  gcvt(usb_cur, 6, USBC);
  gcvt(external_volt, 6, ExtV);
  gcvt(external_cur, 6, ExtC);
  gcvt(power, 6, Pow);
  

  long now = millis();
  if (now - lastMsg > 2000) {
    ArduinoOTA.handle();
    lastMsg = now;
    ++value;
    M5.Lcd.setCursor(30, 30);
    //M5.Lcd.print(external_volt);
    //M5.Lcd.setCursor(80, 30);
    M5.Lcd.print(WiFi.macAddress());
    client.publish("TopicBatteryVolt", BatV);
    client.publish("TopicBatteryCurrent", BatC);
    client.publish("TopicUSB_Volt", USBV);
    client.publish("TopicGUSB_CURRENT", USBC);
    client.publish("TopicGzExtVolt", ExtV);
    client.publish("TopicGzExtCurrent", ExtC);
    client.publish("TopicBatteryPower", Pow); 
  }
}
