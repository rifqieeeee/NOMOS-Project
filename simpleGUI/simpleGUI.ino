#include"TFT_eSPI.h"
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>
#include "rpcWiFi.h"

TFT_eSPI tft;
SensirionI2CSht4x sht4x;

const int Output_Pin = D0;
volatile int  Pulse_Count;
unsigned int  Liter_per_hour;
unsigned long Current_Time, Loop_Time;

#define FREQUENCY_HZ 40
#define INTERVAL_MS (1000 / (FREQUENCY_HZ))

void Detect_Rising_Edge ()
{ 
   Pulse_Count++;
}

void setup() {
    tft.begin();
    Wire.begin();
    
    tft.setRotation(3);
    tft.fillScreen(TFT_WHITE); //Red background
    tft.fillRect(0,0,320,80,TFT_BLUE);
    tft.fillRect(159,80,2,160,TFT_BLACK);
    tft.fillRect(0,159,320,2,TFT_BLACK);

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

    pinMode(Output_Pin, INPUT);
    attachInterrupt(0, Detect_Rising_Edge, RISING);

    Current_Time = millis();
    Loop_Time = Current_Time;
}

//Transmitting sensor value to thingspeak
void thingspeakTrans()
{
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connect(serverName, 80)) {
    Serial.println("Connecting...");
    
    // send the Get request
    client.print(F("GET /update?api_key="));
    client.print(apiKey);
    client.print(F("&field1="));
    client.print(temp_buf);
    client.print(F("&field2="));
    client.print(humi_buf);
    client.print(F("&field3="));
    client.print(hic_buf);
    client.println();
    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
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

  float temperature;
  float humidity;
  uint16_t error;
  char errorMessage[256];
  error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    tft.drawString("error", 50, 125);
    tft.drawString("error", 220, 125);
    tft.drawString("error", 50, 205);
    tft.drawString("error", 210, 205);
    
  } else {
    Current_Time = millis();
    if(Current_Time >= (Loop_Time + 1000)){
      Loop_Time = Current_Time;
      Liter_per_hour = (Pulse_Count * 60 / 7.5);
      Pulse_Count = 0;
      
      static char ctemp[7];dtostrf(temperature, 2, 2, ctemp);
      static char chumd[7];dtostrf(humidity, 2, 2, chumd);
      static char cflow[7];dtostrf(Liter_per_hour, 2, 2, cflow);
      
      tft.drawString(ctemp, 50, 125);
      tft.drawString(chumd, 220, 125);
      tft.drawString(cflow, 50, 205);
      tft.drawString("Normal", 210, 205);
    }
  }
  
  delay(1000);
}
