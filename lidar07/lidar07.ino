#include"DFRobot_LIDAR07.h"

DFRobot_LIDAR07_IIC  LIDAR07;

void setup() {
  // put your setup code here, to run once:
  uint32_t version;
  Serial.begin(115200);
  while(!LIDAR07.begin()){
    Serial.println("The sensor returned data validation error");
    delay(1000);
  }

  version = LIDAR07.getVersion();
  Serial.print("VERSION: ");
  Serial.print((version>>24)&0xFF,HEX);
  Serial.print(".");Serial.print((version>>16)&0xFF,HEX);
  Serial.print(".");Serial.print((version>>8)&0xFF,HEX);
  Serial.print(".");Serial.println((version)&0xFF,HEX);

  //After enabling the filter, it can be stopped by calling LIDAR07.stopFilter()
  LIDAR07.startFilter();

  while(!LIDAR07.setMeasureMode(LIDAR07.eLidar07Single)){
      Serial.println("set measure mode err");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  //Open measurement (in single measurement mode, it will automatically close after sampling),To stop collection, use stopMeasure()
  LIDAR07.startMeasure();

  //Get the collected data
  if(LIDAR07.getValue()){
    Serial.print("Distance:");Serial.print(LIDAR07.getDistanceMM());Serial.println(" mm");
    Serial.print("Amplitude:");Serial.println(LIDAR07.getSignalAmplitude());
  }
  delay(1000);

}
