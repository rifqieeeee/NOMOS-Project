#include <Arduino.h>
#include "disk91_LoRaE5.h"
 
Disk91_LoRaE5 lorae5(&Serial); // Where the AT command and debut traces are printed
 
#define Frequency DSKLORAE5_ZONE_AS923_2
/*
Select your frequency band here.
DSKLORAE5_ZONE_EU868
DSKLORAE5_ZONE_US915
DSKLORAE5_ZONE_AS923_1
DSKLORAE5_ZONE_AS923_2
DSKLORAE5_ZONE_AS923_3
DSKLORAE5_ZONE_AS923_4
DSKLORAE5_ZONE_KR920
DSKLORAE5_ZONE_IN865
DSKLORAE5_ZONE_AU915
 */
 
char deveui[] = "6081F942139B6800";
char appeui[] = "6081F98776E6BDE5";
char appkey[] = "0A7F64EE1FCC978A3A768EED28DF72C7";
 
void setup() {
 
  Serial.begin(9600);
  uint32_t start = millis();
  while ( !Serial && (millis() - start) < 1500 );  // Open the Serial Monitor to get started or wait for 1.5"
 
  // init the library, search the LORAE5 over the different WIO port available
  if ( ! lorae5.begin(DSKLORAE5_SEARCH_WIO) ) {
    Serial.println("LoRa E5 Init Failed");
    while(1); 
  }
 
  // Setup the LoRaWan Credentials
  if ( ! lorae5.setup(
        Frequency,     // LoRaWan Radio Zone EU868 here
        deveui,
        appeui,
        appkey
     ) ){
    Serial.println("LoRa E5 Setup Failed");
    while(1);         
  }
}
 
void loop() {
 
}
