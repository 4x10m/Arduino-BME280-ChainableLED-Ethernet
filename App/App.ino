#include <Regexp.h>

#include <SPI.h> //bibliothèqe pour SPI
#include <Ethernet.h> //bibliothèque pour Ethernet
#include <ChainableLED.h>
#include <string.h>

#include "SparkFunBME280.h"
//Library allows either I2C or SPI, so include both.
#include "Wire.h"
#include "SPI.h"

#define NUM_LEDS  5

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xA7, 0x6F}; // tableau pour l'adresse MAC de votre carte
byte ip[] = {192, 168, 1, 123}; //tableau pour l'adresse IP




const char* homeFirst = 
"HTTP/1.1 200 OK"
"Content-Type: text/html"
"Connection: close"
"Refresh: 5"
""
"<!DOCTYPE HTML>"
"<html>"
  "<head>"
    "<title>Relevés analogiques</title>"
  "</head>"
  
  "<body>"
    "<h1>Données</h1>";


const char* homeSecond = 
    "<hr>"
  
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


EthernetServer serveur(80);
ChainableLED leds(7, 8, NUM_LEDS);
BME280 capteur;

String readString;

float hue = 0.0;
boolean up = true;
int r = 255;
int g = 255;
int b = 255;

void printValue(const char* title, int value) {
  Serial.print(title);
  Serial.print(" <input type=\"text\" value=\"");
  Serial.print(value);
  Serial.print("\" disabled />");
}

void setup()
{
  Serial.begin (9600); //initialisation de communication série

  while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino LEONARDO
  }

  capteur.settings.commInterface = I2C_MODE; 
  capteur.settings.I2CAddress = 0x76;
  capteur.settings.runMode = 3; 
  capteur.settings.tStandby = 0;
  capteur.settings.filter = 0;
  capteur.settings.tempOverSample = 1 ;
  capteur.settings.pressOverSample = 1;
  capteur.settings.humidOverSample = 1;
  
  Ethernet.begin (mac, ip); //initialisatio de la communication Ethernet
  
  Serial.print("\nLe serveur est sur l'adresse : ");
  Serial.println(Ethernet.localIP()); //on affiche l'adresse IP de la connexion
  
  serveur.begin();
  
  leds.init();
  //Serial.begin(9600);// initialisation de la communication
  Serial.println("Communication initialisée");// envoi d'un message

  Serial.println("Starting BME280... ");
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  capteur.begin();
}

void loop()
{
  float temperature = capteur.readTempC();
  float pressure = capteur.readFloatPressure() / 100;
  float humidity = capteur.readFloatHumidity();
  
  EthernetClient client = serveur.available();
  
  if (client) { //si client connecté et si le client est en connecté
    Serial.println("Client connected"); //on le dit...

    boolean currentLineIsBlank = true;
    while(client.connected()) {
      if(client.available()) {
        char c = client.read(); // on lit le caractère 
       
        Serial.write(c);// on l'écrit sur le moniteur série
        readString += c;

        if (c == '\n' && currentLineIsBlank) {
          
          MatchState ms;
          ms.Target ((char*) readString.c_str());
          
          if (ms.Match ("/home") == REGEXP_MATCHED) {
            // matching offsets in ms.capture
            client.print(homeFirst);
            printValue("Température", temperature);
            printValue("Pression", pressure);
            printValue("Humidité", humidity);
            client.print(homeSecond);
          }
          
          if (ms.Match ("/color/(%d+)/(%d+)/(%d+)") == REGEXP_MATCHED) {
            // matching offsets in ms.capture
            
            Serial.print ("Captures: ");
            Serial.println (ms.level);

            int red, blue, green;

            for (int j = 0; j < ms.level; j++)
            {
              Serial.print ("Capture number: ");
              Serial.println (j, DEC);

              if (j == 0) {
                red = ms.GetCapture ((char*)readString.c_str(), j);
              } else if (j == 1) {
                blue = ms.GetCapture ((char*)readString.c_str(), j);
              } else if (j == 2) {
                green = ms.GetCapture ((char*)readString.c_str(), j);
              }            
            } // end of for each capture

            Serial.print ("Values");
            Serial.print (red, DEC);
            Serial.print (" ");
            Serial.print (blue, DEC);
            Serial.print (" ");
            Serial.print (green, DEC);
            Serial.print ("\n");

            leds.setColorRGB(0, red, blue, green);
          }
        }

        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  
  Serial.println("Choisissez r : ");//envoi d'un autre message 
}

