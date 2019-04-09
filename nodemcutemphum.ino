#include <ThingerESP8266.h>
#include <SimpleDHT.h>
#include <SPI.h>
#include <DHT.h>

#define USERNAME "Hamide"
#define DEVICE_ID "ham"
#define DEVICE_CREDENTIAL "&KU1dcuwi&wq"
//::::::::::::::::::::::SHT10:::::::::::::::::::::::://
#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


#define SSID "PC-Faster-3196"
#define SSID_PASSWORD "1234567890"

ThingerESP8266 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

void setup() {
   dht.begin();
   pinMode(DHTPIN,INPUT);
  
 
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  

  thing.add_wifi(SSID, SSID_PASSWORD);

  // digital pin control example (i.e. turning on/off a light, a relay, configuring a parameter, etc)
  thing["led"] << digitalPin(LED_BUILTIN);

  thing["millis"] >> outputValue(millis());
  
   thing["dht22"] >> [](pson& out){
    out["humidity"] = dht.readHumidity();
    out["celsius"] = dht.readTemperature();
   };
  



}

void loop() {
  thing.handle();
  Serial.print(dht.readTemperature());
}
