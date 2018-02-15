//#include "SparkFunBME280.h"
//Library allows either I2C or SPI, so include both.
#include "Wire.h"
#include "SPI.h"

#define I2C_MODE 0
#define SPI_MODE 1
#define BME280_TEMPERATURE_MSB_REG 0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG    0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG    0xFC //Temperature XLSB
#define BME280_DIG_T1_LSB_REG      0x88
#define BME280_DIG_T1_MSB_REG     0x89
#define BME280_DIG_T2_LSB_REG      0x8A
#define BME280_DIG_T2_MSB_REG     0x8B
#define BME280_DIG_T3_LSB_REG      0x8C
#define BME280_DIG_T3_MSB_REG     0x8D

//BME280 capteur;
uint16_t dig_T1 ;
int16_t dig_T2 ;
int16_t dig_T3 ;

void setup() {

   Serial.begin(9600);
  while (!Serial) {
    // Attente de l'ouverture du port série pour Arduino
  }
  //configuration du capteur
  //capteur.settings.commInterface = I2C_MODE; 
  //capteur.settings.I2CAddress = 0x76;
  //capteur.settings.runMode = 3;
  //capteur.settings.tStandby = 0;
  //capteur.settings.filter = 0;
  //capteur.settings.tempOverSample = 1 ;
  //capteur.settings.pressOverSample = 1;
  //capteur.settings.humidOverSample = 1;
  uint8_t commInterface = I2C_MODE;
  uint8_t I2CAddress = 0x76;
  uint8_t runMode = 3; 
  uint8_t tStandby = 0;
  uint8_t filter = 0;
  uint8_t tempOverSample = 1;
  uint8_t pressOverSample = 1;
  uint8_t humidOverSample = 1;
  
  dig_T1 = ((uint16_t)((readRegister(BME280_DIG_T1_MSB_REG) << 8) + readRegister(BME280_DIG_T1_LSB_REG)));
  dig_T2 = ((int16_t)((readRegister(BME280_DIG_T2_MSB_REG) << 8) + readRegister(BME280_DIG_T2_LSB_REG)));
  dig_T3 = ((int16_t)((readRegister(BME280_DIG_T3_MSB_REG) << 8) + readRegister(BME280_DIG_T3_LSB_REG)));; 
  
  Serial.println("Starting BME280... ");
  delay(10);  // attente de la mise en route du capteur. 2 ms minimum
  // chargement de la configuration du capteur
  //capteur.begin();
  Wire.begin();
}
 
void loop() {
  Serial.print("Température: ");
  //Serial.print(capteur.readTempC(), 2);
  Serial.print(readTempC(), 2);
  Serial.print(" °C");
  Serial.print("\t Pression: ");
  //Serial.print(capteur.readFloatPressure()/100, 2);
  Serial.print(readFloatPressure()/100, 2);
  Serial.print(" hPa");
  Serial.print("\t humidité relative : ");
  //Serial.print(capteur.readFloatHumidity(), 2);
  Serial.print(readFloatHumidity(), 2);
  Serial.println(" %");
  delay(1000);
}

float readTempC(){

  // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
  // t_fine carries fine temperature as global value

  //get the reading (adc_T);
  int32_t adc_T = ((uint32_t)readRegister(BME280_TEMPERATURE_MSB_REG) << 12) | ((uint32_t)readRegister(BME280_TEMPERATURE_LSB_REG) << 4) | ((readRegister(BME280_TEMPERATURE_XLSB_REG) >> 4) & 0x0F);

  //By datasheet, calibrate
  int64_t var1, var2;

  int32_t t_fine;

  var1 = ((((adc_T>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T>>4) - ((int32_t)dig_T1)) * ((adc_T>>4) - ((int32_t)dig_T1))) >> 12) *
  ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  float output = (t_fine * 5 + 128) >> 8;

  output = output / 100;
  
  return output;
  
}

float readFloatPressure(){
  
}

float readFloatHumidity(){
  
}

uint8_t readRegister(uint8_t offset){
  //Return value
  uint8_t result;
  uint8_t numBytes = 1;
  
  Wire.beginTransmission(I2CAddress);
  Wire.write(offset);
  Wire.endTransmission();

  Wire.requestFrom(I2CAddress, numBytes);
  while ( Wire.available() ) // slave may send less than requested
  {
    result = Wire.read(); // receive a byte as a proper uint8_t
  }
  
  return result;
}
