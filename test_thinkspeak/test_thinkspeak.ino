//include libraries
#include "rpcWiFi.h" //Wi-Fi library 
#include "TFT_eSPI.h" //TFT LCD library 
#include "Free_Fonts.h" //free fonts library 
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>

TFT_eSPI tft; //initialize TFT LCD
TFT_eSprite spr = TFT_eSprite(&tft); //initialize sprite buffer

SensirionI2CSht4x sht4x;
float tempValue, humiValue, hicValue;
// sensor buffer
char temp_buf[10];
char humi_buf[10];
char hic_buf[10];

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 30000L; // delay between updates, in milliseconds
 
const char* ssid = "Sekolah Robot Indonesia"; //Wi-Fi name    "2nd_Floor"
const char* password =  "sadapsadap"; //Wi-Fi password    "Lakpass321."

const char* serverName = "api.thingspeak.com"; //Domain Name for HTTP POST Request
String apiKey = "7HYKF84XX6QS021W"; //"Write API Key" from ThingSpeak

// Use WiFiClient class to create TCP connections
WiFiClient client;
 
void setup() {
  Serial.begin(115200); //start serial communication
  while (!Serial); //wait for Serial to be ready
  delay(100);
 
  WiFi.mode(WIFI_STA); //set WiFi to station mode 
  WiFi.disconnect(); //disconnect from an AP if it was previously connected
 
  Serial.println("Connecting to WiFi.."); //print string 
  WiFi.begin(ssid, password); //connect to Wi-Fi network
 
  // attempt to connect to Wi-Fi network:
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      // wait 1 second for re-trying
      delay(1000);
  }
    
  Serial.print("Connected to ");
  Serial.println(ssid); //print Wi-Fi name 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); //print Wio Terminal's IP address
  Serial.println(""); //print empty line

  Wire.begin();
 
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

//  ----------------LCD------------------------

  //LCD setup
  tft.begin(); //start TFT LCD
  tft.setRotation(3); //set TFT LCD rotation
  tft.fillScreen(TFT_BLACK); //fill background

  //header title 
  tft.fillRect(0,0,320,50,TFT_YELLOW); //fill rectangle 
  tft.setFreeFont(FSB12); //set font type 
  tft.setTextColor(TFT_BLACK); //set text color
  tft.drawString("WEATHER STATION", 40, 15); //draw string 
    
  //text strings 
  tft.setTextColor(TFT_WHITE); 
  tft.setFreeFont(FS18); 
  tft.drawString("Temperature:", 10,75);
  tft.drawString("Humidity:", 10,175);
  
  tft.setTextColor(TFT_YELLOW); 
  tft.setFreeFont(FMB24);  
  tft.drawString("C",260,70);
  tft.drawString("%RH", 215,170);
  
  tft.drawFastHLine(0,140,320,TFT_YELLOW); //draw horizontal line

}

// Read sendsor value
void sensorRead(){
  uint16_t error;
  char errorMessage[256];
  float temperature;
  float humidity;
  error = sht4x.measureHighPrecision(temperature, humidity);
  if (error) {
    Serial.print("Error trying to execute measureHighPrecision(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
  } else {
    tempValue = temperature;
    humiValue = humidity;
    hicValue = (0.8 * temperature) + (0.2 * humidity);
    Serial.print("Temperature:");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print("Humidity:");
    Serial.println(humidity);
    }
  
  String strTemp = dtostrf(tempValue, 4, 1, temp_buf);
  String strHumi = dtostrf(humiValue, 4, 1, humi_buf);
  String strHic = dtostrf(hicValue, 4, 1, hic_buf);
  
  Serial.print("Temperature: ");
  Serial.println(tempValue);
  Serial.print("Humidity: ");
  Serial.println(humiValue);
  Serial.print("Cds: ");
  Serial.println(hicValue);
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
  Serial.print("Connecting to ");
  Serial.println(serverName); //print server IP address

  while (client.available()) {
    char c = client.read();
    Serial.print("recv data: ");
    Serial.write(c);
    Serial.println();
  }
  
  // if 30 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    sensorRead();
    thingspeakTrans();
  }
      
//sprite buffer for temperature
  spr.createSprite(55, 40); //create buffer
  spr.fillSprite(TFT_BLACK); //fill background color of buffer
  spr.setFreeFont(FMB24); //set font type 
  spr.setTextColor(TFT_YELLOW); //set text color
  spr.drawNumber(tempValue, 0, 0); //display number 
  spr.pushSprite(200, 70); //push to LCD 
  spr.deleteSprite(); //clear buffer

  //sprite buffer for humidity 
  spr.createSprite(55, 40);
  spr.fillSprite(TFT_BLACK);
  spr.setFreeFont(FMB24);
  spr.setTextColor(TFT_YELLOW);
  spr.drawNumber(humiValue, 0, 0); 
  spr.pushSprite(155, 170);
  spr.deleteSprite();
  
  //wait 10 seconds to send the next POST request
  Serial.println("Waiting...");
  delay(10000);
}
