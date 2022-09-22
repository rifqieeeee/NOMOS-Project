#include"TFT_eSPI.h"
TFT_eSPI tft;
 
void setup() {
  tft.begin();
  tft.setRotation(3);

  pinMode(WIO_5S_UP, INPUT_PULLUP);
  pinMode(WIO_5S_DOWN, INPUT_PULLUP);
  pinMode(WIO_5S_LEFT, INPUT_PULLUP);
  pinMode(WIO_5S_RIGHT, INPUT_PULLUP);
  pinMode(WIO_5S_PRESS, INPUT_PULLUP);

  pinMode(WIO_BUZZER, OUTPUT);
}
 
void loop() {
  tft.fillScreen(TFT_WHITE); 
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(3);
  if (digitalRead(WIO_5S_UP) == LOW) {
    Serial.println("5 Way Up");
    tft.drawString("UP", 0, 50);
   }
   else if (digitalRead(WIO_5S_DOWN) == LOW) {
    Serial.println("5 Way Down");
    tft.drawString("DOWN", 0, 50);
   }
   else if (digitalRead(WIO_5S_LEFT) == LOW) {
    Serial.println("5 Way Left");
    tft.drawString("LEFT", 0, 50);
   }
   else if (digitalRead(WIO_5S_RIGHT) == LOW) {
    Serial.println("5 Way Right");
    tft.drawString("RIGHT", 0, 50);
    analogWrite(WIO_BUZZER, 0);
   }
   else if (digitalRead(WIO_5S_PRESS) == LOW) {
    Serial.println("5 Way Press");
    tft.drawString("PRESS", 0, 50);
    analogWrite(WIO_BUZZER, 128);
   }
}
