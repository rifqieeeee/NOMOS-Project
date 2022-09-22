#include"TFT_eSPI.h"
#include <Arduino.h>
#include <SensirionI2CSht4x.h>
#include <Wire.h>
#include "rpcWiFi.h"
#include <nomos_inferencing.h>
#include "rpcWiFi.h"

TFT_eSPI tft;
SensirionI2CSht4x sht4x;
float tempValue, humiValue, watfValue;
// sensor buffer
static char ctemp[7];
static char chumd[7];
static char cflow[7];

const int Output_Pin = D0;
volatile int  Pulse_Count;
unsigned int  Liter_per_hour;
unsigned long Current_Time, Loop_Time;

float temperature;
float humidity;
uint16_t error;
char errorMessage[256];

static const float features[15] = {};
int flag_status;

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 30000L; // delay between updates, in milliseconds
 
const char* ssid = "Sekolah Robot Indonesia"; //Wi-Fi name    "2nd_Floor"
const char* password =  "sadapsadap"; //Wi-Fi password    "Lakpass321."

const char* serverName = "api.thingspeak.com"; //Domain Name for HTTP POST Request
String apiKey = "DSH9X12M2I3HRQTI"; //"Write API Key" from ThingSpeak

// Use WiFiClient class to create TCP connections
WiFiClient client;

void Detect_Rising_Edge ()
{ 
   Pulse_Count++;
}

void setup() {
  tft.begin();
  Wire.begin();
  sht4x.begin(Wire);    
  Serial.begin(115200);
  while (!Serial); //wait for Serial to be ready
  delay(100);

  WiFi.mode(WIFI_STA); //set WiFi to station mode 
  WiFi.disconnect(); //disconnect from an AP if it was previously connected
  
  Serial.println("Connecting to WiFi.."); //print string 
  WiFi.begin(ssid, password); //connect to Wi-Fi network

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
    
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE); //Red background
  tft.fillRect(0,0,320,80,TFT_BLUE);
  tft.fillRect(159,80,2,160,TFT_BLACK);
  tft.fillRect(0,159,320,2,TFT_BLACK);

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

void gui(){
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
  tft.fillRect(210,205,70,30,TFT_WHITE);

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
      
      dtostrf(temperature, 2, 2, ctemp);
      dtostrf(humidity, 2, 2, chumd);
      dtostrf(Liter_per_hour, 2, 2, cflow);
      
      tft.drawString(ctemp, 50, 125);
      tft.drawString(chumd, 220, 125);
      tft.drawString(cflow, 50, 205);
      if (flag_status<1){
        tft.drawString("Normal", 210, 205);
      } else tft.drawString("Danger", 210, 205);
        
    }
  }
}

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
    client.print(ctemp);
    client.print(F("&field2="));
    client.print(chumd);
    client.print(F("&field3="));
    client.print(cflow);
    client.println();
    // note the time that the connection was made
    lastConnectionTime = millis();
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}

int raw_feature_get_data(size_t offset, size_t length, float *out_ptr) {
  float features[15];
  for (byte i = 0; i < 15; i = i + 3)
  {
    features[i]=temperature;
    features[i+1]=humidity;
    features[i+2]=Liter_per_hour;
    delay(200);
  }
  memcpy(out_ptr, features + offset, length * sizeof(float));
  return 0;
}

void prediksi(){
    ei_printf("Edge Impulse standalone inferencing (Arduino)\n");

    if (sizeof(features) / sizeof(float) != EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
        ei_printf("The size of your 'features' array is not correct. Expected %lu items, but had %lu\n",
            EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, sizeof(features) / sizeof(float));
        delay(1000);
        return;
    }

    ei_impulse_result_t result = { 0 };

    // the features are stored into flash, and we don't want to load everything into RAM
    signal_t features_signal;
    features_signal.total_length = sizeof(features) / sizeof(features[0]);
    features_signal.get_data = &raw_feature_get_data;

    // invoke the impulse
    EI_IMPULSE_ERROR res = run_classifier(&features_signal, &result, false /* debug */);
    ei_printf("run_classifier returned: %d\n", res);

    if (res != 0) return;

    // print the predictions
    ei_printf("Predictions ");
    ei_printf("(DSP: %d ms., Classification: %d ms., Anomaly: %d ms.)",
        result.timing.dsp, result.timing.classification, result.timing.anomaly);
    ei_printf(": \n");
    ei_printf("[");
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("%.5f", result.classification[ix].value);
  #if EI_CLASSIFIER_HAS_ANOMALY == 1
          ei_printf(", ");
  #else
          if (ix != EI_CLASSIFIER_LABEL_COUNT - 1) {
              ei_printf(", ");
          }
  #endif
      }
  #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("%.3f", result.anomaly);
  #endif
      ei_printf("]\n");

      // human-readable predictions
      for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
          ei_printf("    %s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
      }

      if (result.classification[0].value > result.classification[1].value){
        flag_status = 1;
      } else flag_status = 0;

  #if EI_CLASSIFIER_HAS_ANOMALY == 1
      ei_printf("    anomaly score: %.3f\n", result.anomaly);
  #endif

    delay(1000);
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
  
  gui();
  prediksi();
  if (millis() - lastConnectionTime > postingInterval) {
    thingspeakTrans();
  }
}
