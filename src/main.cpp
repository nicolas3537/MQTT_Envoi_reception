#include <Arduino.h>
#include <Ethernet.h>
#include <PubSubClient.h>



// L'adresse MAC pour le shield
byte mac[] = { 0xAE, 0xAD, 0xAE, 0xAE, 0xAF, 0xAD };

//Infos MQTT a adapter
const char* mqttServer = "192.168.0.23";  //Adresse IP du Broker MQTT
const int mqttPort = 1883;               //Port utilisé par le Broker MQTT
int cons_vent_bas =0;
int cons_vent_haut=0;


// On créé l'objet client pour la connexion au serveur
EthernetClient client;
// Creation d un objet client publication MQTT s appuyant sur le client WIFI
PubSubClient monClientMqtt(client);


//Connexion
IPAddress IParduino(192, 168, 0, 159);
// IP de la freebox (va servir pour : dns, passerelle)
IPAddress IPDNS(192,168,0,1);


// POUR DEBUG
// Variable pour compter le nombre de connexion échouée de client.connect
boolean etat_ventil_1 = 0;
boolean etat_ventil_2 = 0;
boolean etat_ventil_1_old = 0;
boolean etat_ventil_2_old = 0;

// Variable de Tempo pour déclenchement de lecture
unsigned long previousMillis = 0;

//MQTT définition
#define Clientarduino "Test"
#define MQTT_USERNAME "homeassistant"
#define MQTT_KEY "ohs8Phookeod6chae0ENg5aingeite8Jaebooziheevug0huinei8Ood9iePoh9l"

void callback(char* topic, byte *payload, unsigned int length) {
   Serial.println("-------Nouveau message du broker mqtt-----");
   Serial.print("Canal:");
   Serial.println(topic);
   Serial.print("donnee:");
   Serial.write(payload, length);
   Serial.println();
  cons_vent_bas = (int)payload[0];
 }


void setup() {

  // put your setup code here, to run once:
   
  // On ouvre le port série pour DEBUG
  Serial.begin(9600);
  
  // On initialise le shield avec IP fixe
  Ethernet.begin(mac, IParduino , IPDNS , IPDNS);

    // On attend que le shield s'initialise
  delay(500);

    // Init client MQTT pour la publication
  monClientMqtt.setServer(mqttServer, mqttPort);

      // On attend que le shield s'initialise
  delay(500);

  Serial.println(F("*** Fin de la configuration ***"));
 
   //Pour DEBUG
  //Obtenir l'adresse IP de l'arduino
  IPAddress IP_Arduino = Ethernet.localIP();
  Serial.println(IP_Arduino);

  //Connection MQTT
  if (monClientMqtt.connect("Clientarduino", "homeassistant", "ohs8Phookeod6chae0ENg5aingeite8Jaebooziheevug0huinei8Ood9iePoh9l")) {
    monClientMqtt.publish("outTopic","hello world");
    monClientMqtt.subscribe("inTopic");
  }
  else {
      Serial.print("echec, code erreur= ");
      Serial.println(monClientMqtt.state());
  }
monClientMqtt.setCallback(callback);
monClientMqtt.publish("Test/vent/Temp_cons_0", "0");
monClientMqtt.publish("Test/vent/Temp_cons_1", "0");
}

void loop() {
  // put your main code here, to run repeatedly:
  //Mesure du temps
  // Publication MQTT
  monClientMqtt.loop();
      if(etat_ventil_1 != etat_ventil_1_old){
        monClientMqtt.publish("Baie/vent/etat_vent_1", String(etat_ventil_1).c_str());
        etat_ventil_1_old = etat_ventil_1;
        Serial.println("Trame ventilateur 1 envoyé");                
      }
      if(etat_ventil_2 != etat_ventil_2_old){
        monClientMqtt.publish("Baie/vent/etat_vent_2", String(etat_ventil_2).c_str());
        etat_ventil_2_old = etat_ventil_2;
        Serial.println("Trame ventilateur 2 envoyé");                  
      }
//Récupération de valeur sur MQTT
cons_vent_bas = monClientMqtt.subscribe("Test/vent/Temp_cons_0");
Serial.print("consigne bas :");
Serial.println(cons_vent_bas);
cons_vent_haut = int(monClientMqtt.subscribe("Test/vent/Temp_cons_1"));
Serial.print("consigne haute :");
Serial.println(cons_vent_haut);

//Débug
Serial.print(etat_ventil_1);
Serial.print(etat_ventil_1_old);
Serial.print(etat_ventil_2);
Serial.print(etat_ventil_2_old);
delay(5000);
}