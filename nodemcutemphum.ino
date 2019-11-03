#include <ThingerESP8266.h>
#include <ESP8266WiFi.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <SPI.h>
// declaration du nome d'utilisateur et code pin reservé a notre appareil
#define N_Utilisateur "Hamide"
#define ID_Appareil "Superv"
#define pin_Apareil "2znv8XI%c%5Z"
// configuration du wifi " nom et code "
#define WIFI "PC-Faster-3196"
#define WIFI_Code "1234567890"

//declaration du pin et Type de dht 
#define DHTPIN 13
#define DHTTYPE DHT22

// communication avec thinger.io
ThingerESP8266 thing(N_Utilisateur, ID_Appareil, pin_Apareil);
//configuration du dht22
DHT dht(DHTPIN, DHTTYPE);

unsigned long DerniereVer = 0;
unsigned long DerniereVer1= 0;

int Etat_Act ;
int Compresseur = 5 ;
int porte = 0 ;
int porteLED = 4 ; 


void setup() {
    //configuration du dht22
     dht.begin();
     pinMode(DHTPIN,INPUT);
     // indicateur de demarage de compresseur frigo
     pinMode(Compresseur, OUTPUT);
     // le bouton qui indique l'ouverture de la porte frigo 
     pinMode(porte,INPUT);
     // la led d'indication d' de la porte frigo 
     pinMode(porteLED,OUTPUT);
     
     // configurer la led d'indication de dépassement de temperature 
     pinMode(LED_BUILTIN,OUTPUT);
    //connexion au point d'accès wifi 
     thing.add_wifi(WIFI, WIFI_Code);
  
    //acquisition de la temperature et l'humidité
    thing["DHT22"] >> [](pson& out){
    out["Humidité"] = dht.readHumidity();
    out["Temperature"] = dht.readTemperature();
   };

   // le curseur de chiox de temperature désiré dans le frigo
   thing ["Curseur"] << [](pson& in){
      
       if(in.is_empty()){
         in = Etat_Act;
      }
        else{
          Etat_Act = in;

     }
      };


}

void loop() {
  //la cummunication avec la plateforme thinger.io
  thing.handle();
  // enregistrement des données dans la base de données 
  thing.write_bucket("data", "DHT22");
  
  unsigned long tempsAct = millis();
  
   if(dht.readTemperature()> 28){//le test de la condition est fait chaque 15 min
    digitalWrite(LED_BUILTIN,HIGH);
    thing["ExceeTemp"] >> outputValue(1);//allumer l'indicateur rouge dans la plateforme
    if(tempsAct-DerniereVer>=900000){
      DerniereVer=tempsAct;
       thing.call_endpoint("email",thing["Temperature"]);// envoyer l'email d'alerte chaque 15 min
      } 
   }else {
        digitalWrite(LED_BUILTIN,LOW);
        thing["ExceeTemp"] >> outputValue(0);//eteindre  l'indicateur rouge dans la plateforme
   }

   // compresseur 
   
    if (dht.readTemperature() > Etat_Act){
    // Démarrer le compresseur 
         digitalWrite(Compresseur, HIGH);
         thing["Comp_ind"] >> outputValue(1);
       }else {
        // Arreter le compresseur 
         digitalWrite(Compresseur, LOW);
         thing["Comp_ind"] >> outputValue(0);
       }
       int etatPorte = digitalRead(porte);
       // detection d'ouverture de porte et envoi d'alerte
  if( etatPorte == HIGH ){
     digitalWrite(porteLED,HIGH);
     thing["PorteO"] >> outputValue(1);
    if(tempsAct-DerniereVer1>=900000){
       DerniereVer1=tempsAct;
       thing.call_endpoint("porte",thing["PorteO"]);
      } 
        }else {
      digitalWrite(porteLED,LOW);
       thing["PorteO"] >> outputValue(0);
      }  

}
