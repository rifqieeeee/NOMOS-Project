#include"TFT_eSPI.h"

TFT_eSPI tft;

void setup() {
    tft.begin();
    
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE); //Red background
    tft.fillRect(0,0,320,80,TFT_BLUE);
    tft.fillRect(159,80,2,160,TFT_BLACK);
    tft.fillRect(0,159,320,2,TFT_BLACK);
}
 
void loop() {  
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("Mosquito Grwoth", 70, 10);
  tft.drawString("Monitoring System", 60, 40);
  
  tft.setTextColor(TFT_BLACK);
  tft.drawString("Temperature", 15, 95);
  tft.drawString("Humidity", 195, 95);
  tft.drawString("Water Flow", 20, 175);
  tft.drawString("Status", 210, 175);

  tft.fillRect(50,125,60,30,TFT_WHITE);
  tft.fillRect(220,125,60,30,TFT_WHITE);
  tft.fillRect(50,205,70,30,TFT_WHITE);
  tft.fillRect(210,205,60,30,TFT_WHITE);
  
  tft.drawString("30", 50, 125);
  tft.drawString("50", 220, 125);
  tft.drawString("128", 50, 205);
  tft.drawString("Normal", 210, 205);
    
  
  delay(1000);
}
