#include <Regexp.h>

#include <SPI.h> //bibliothèqe pour SPI
#include <Ethernet.h> //bibliothèque pour Ethernet
#include "ChainableLED.h"

#include "SparkFunBME280.h"
//Library allows either I2C or SPI, so include both.
#include "Wire.h"
#include "SPI.h"

#include <avr/pgmspace.h>

#define NUM_LEDS  5

byte mac[] = {0x90, 0xA2, 0xDA, 0x0E, 0xA7, 0x6F}; // tableau pour l'adresse MAC de votre carte
byte ip[] = {192, 168, 1, 123}; //tableau pour l'adresse IP



const char homeFirst[] PROGMEM = \
"<!DOCTYPE HTML>" \
"<html>" \
  "<!--<meta http-equiv=\"refresh\" content=\"2\" >-->" \
  "<head>" \
    "<meta charset=\"utf-8\" />" \
    "<title>Relevés analogiques</title>" \
  "</head>" \
  
  "<body>" \
    "<h1>Données</h1>\0";


const char homeSecond[] PROGMEM = \
    "<hr>" \
    "<h1>Controle de la LED</h1>" \
    "<hr>" \
    "R :" \
    "<input id=\"red\" type=\"text\" />" \
    "<br>" \
    "G :" \
    "<input id=\"green\" type=\"text\" />" \
    "<br>" \
    "B :" \
    "<input id=\"blue\" type=\"text\" />" \
    "<br>" \
    "<button onclick=\"go()\">Ok</button>" \
    "<hr>" \

    "<SCRIPT LANGUAGE=\"JavaScript\">" \
     "  function go(){" \
      "  var red = document.getElementById(\"red\").value;" \
      "  var green = document.getElementById(\"green\").value;" \
      "  var blue = document.getElementById(\"blue\").value;" \

     "   var url = \"/color/\" + red + \"/\" + green + \"/\" + blue;" \
        
      "  document.location.href=url;" \
      " }" \
    "  " \
    "</SCRIPT>" \
  "</body>" \
"</html>\0";


//String readString = String();
EthernetServer serveur(80);
ChainableLED leds(7, 8, NUM_LEDS);
BME280 capteur;
MatchState ms;
char buff[500];
int i = 0;

float temperature;
float pressure;
float humidity;
char c;
EthernetClient client;



int red, blue, green;

boolean currentLineIsBlank = true;

char* printValue(const char* title, int value) {
  

  sprintf(buff, "%s <input type=\"text\" value=\"%d\" disabled />", title, value);


//Serial.println(" <input type=\"text\" value=\"");
//Serial.print(result);
  
  return buff;
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
  
  serveur.begin();
  
  leds.init();
  //Serial.begin(9600);// initialisation de la communication

  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  capteur.begin();
  
  Serial.println(F("Inited... "));
}



void loop()
{
   temperature = capteur.readTempC();
   pressure = capteur.readFloatPressure() / 100;
   humidity = capteur.readFloatHumidity();
  
  client = serveur.available();

  
  
  if (client) { //si client connecté et si le client est en connecté
    Serial.println(F("Client connected")); //on le dit...

    
    while(client.connected()) {
      if(client.available()) {
        c = client.read(); // on lit le caractère 
       
        Serial.write(c);// on l'écrit sur le moniteur série
        buff[i] = c;

        i++;
        
        //readString += c;
        
        if (c == '\n' && currentLineIsBlank) {
          
          //strcpy_P(buff,readString.c_str());
          //readString = "";
          ms.Target (buff);
          
          if (ms.Match ("/home") == REGEXP_MATCHED) {
            buff[0] = '\0';
            
            // matching offsets in ms.capture
               // read back a char
               
            strcpy_P(buff,homeFirst);
            client.print(buff);
            
            client.print(printValue("Température", temperature));
            client.print(printValue("Pression", pressure));
            client.print(printValue("Humidité", humidity));

            strcpy_P(buff,homeSecond);
            client.print(buff);

            buff[0] = '\0';

            client.stop();

          }
          
          if (ms.Match ("/color/(%d+)/(%d+)/(%d+)") == REGEXP_MATCHED) {
            // matching offsets in ms.capture
            
            Serial.print (F("Captures: "));
            Serial.println (ms.level);

            

            for (int j = 0; j < ms.level; j++)
            {
              Serial.print ("Capture number: ");
              Serial.println (j, DEC);

              if (j == 0) {
                
                red = String(ms.GetCapture (buff, j)).toInt();
              } else if (j == 1) {
                blue = String(ms.GetCapture (buff, j)).toInt();
              } else if (j == 2) {
                green = String(ms.GetCapture (buff, j)).toInt();
              }            
            } // end of for each capture

            Serial.print (F("Values"));
            Serial.print (red);
            Serial.print (" ");
            Serial.print (blue);
            Serial.print (" ");
            Serial.print (green);
            Serial.print ("\n");

            leds.setColorRGB(0, red, blue, green);
            client.stop();
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
    i = 0;
    // close the connection:
    //client.stop();
  }
  
  //Serial.println("Choisissez r : ");//envoi d'un autre message 
}

