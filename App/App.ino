
/* 
 * Example of using the ChainableRGB library for controlling a Grove RGB.
 * This code cycles through all the colors in an uniform way. This is accomplished using a HSB color space. 
 */

#include <SPI.h> //bibliothèqe pour SPI
#include <Ethernet.h> //bibliothèque pour Ethernet
#include <ChainableLED.h>
#include <string.h>

#define NUM_LEDS  5

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xA7, 0x6F}; // tableau pour l'adresse MAC de votre carte
byte ip[] = {192, 168, 1, 123}; //tableau pour l'adresse IP

EthernetServer serveur(80);
ChainableLED leds(7, 8, NUM_LEDS);

void setup()
{
  Serial.begin (9600); //initialisation de communication série
  
  Ethernet.begin (mac, ip); //initialisatio de la communication Ethernet
  
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP()); //on affiche l'adresse IP de la connexion
  
  serveur.begin();
  
  leds.init();
  //Serial.begin(9600);// initialisation de la communication
  Serial.println("Communication initialisée");// envoi d'un message
}

float hue = 0.0;
boolean up = true;
int r = 255;
int g = 255;
int b = 255;


const char* home = 
"<!DOCTYPE HTML>"
"<html>"
  "<head>"
    "<title>Relevés analogiques</title>"
  "</head>"
  
  "<body>"
    "<h1>Controle de la LED</h1>"
      
    "<hr>"
      
    "R :"
    "<input type=\"text\" />"
    "<br>"
      
    "G :"
    "<input type=\"text\" />"
    "<br>"
    
    "B :"
    "<input type=\"text\" />"
    "<br>"
      
    "<hr>"
  "</body>"
"</html>";

EthernetClient client = NULL;

void loop()
{
  if(client == NULL) {
    client = serveur.available();

    Serial.println("Client connected"); //on le dit...
  }
  
  if (client && client.connected()) { //si client connecté et si le client est en connecté
    while (client.available()) { // tant qu'il a des infos à transmettre
      char c = client.read(); // on lit le caractère 
       
      Serial.write(c);// on l'écrit sur le moniteur série
      
      delay(1); //delai de lecture
    }
    
    //réponse au client
    client.println(home);
    client.stop(); //on déconnecte le client
    
    Serial.println("Fin de communication avec le client");
  }
  
  Serial.println("Choisissez r : ");//envoi d'un autre message 
}

