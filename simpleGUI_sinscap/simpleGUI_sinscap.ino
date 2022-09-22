#include"TFT_eSPI.h"
#include <SensirionI2CSht4x.h>

TFT_eSPI tft;
SensirionI2CSht4x sht4x;

void setup() {
    tft.begin();
    Wire.begin();
    
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE); //Red background
    tft.fillRect(0,0,320,80,TFT_GREEN);
    tft.fillRect(159,80,2,160,TFT_BLACK);
    //tft.fillRect(0,159,320,2,TFT_BLACK);

    uint16_t error;
    char errorMessage[256];
 
    sht4x.begin(Wire);
 
    uint32_t serialNumber;
    error = sht4x.serialNumber(serialNumber);
    if (error) {
        Serial.print("Error trying to execute serialNumber(): ");
        errorToString(error, errorMessage, 256);
        Serial.println(errorMessage);
    } else {
        Serial.print("Serial Number: ");
        Serial.println(serialNumber);
    }
}
 
void loop() {  
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("S I N S C A P", 80, 10);
  tft.drawString("Egg Incubator", 80, 40);
  
  tft.setTextColor(TFT_BLACK);
  tft.drawString("Temperature", 15, 95);
  tft.drawString("Humidity", 195, 95);


  tft.fillRect(50,150,60,30,TFT_WHITE);
  tft.fillRect(220,150,60,30,TFT_WHITE);


  float temperature;
  float humidity;
  uint16_t error;
  char errorMessage[256];
  error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    tft.drawString("error", 50, 150);
    tft.drawString("error", 220, 150);
    
  } else {
      static char ctemp[7];dtostrf(temperature, 2, 2, ctemp);
      static char chumd[7];dtostrf(humidity, 2, 2, chumd);
      
      tft.drawString(ctemp, 50, 150);
      tft.drawString(chumd, 220, 150);
  }
  
  delay(1000);
}
