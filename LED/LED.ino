
/* 
 * Example of using the ChainableRGB library for controlling a Grove RGB.
 * This code cycles through all the colors in an uniform way. This is accomplished using a HSB color space. 
 */


#include <ChainableLED.h>

#define NUM_LEDS  5

ChainableLED leds(7, 8, NUM_LEDS);

void setup()
{
  leds.init();
  Serial.begin(9600);// initialisation de la communication
  Serial.println("Communication initialisée");// envoi d'un message
}

float hue = 0.0;
boolean up = true;
int r = 255;
int g = 255;
int b = 255;

void loop()
{
  
  Serial.println("Choisissez r : ");//envoi d'un autre message
  
}

