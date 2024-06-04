/*****************************************************************
File:         identify.ino
Description:   Fingerprint Enrollment 
******************************************************************/
#include "BM92S2331-1.h"

BM92S2331_1 fps(2,4,5);        //rxPin:4, txPin:5, Please comment this line of code if don't use SW Serial
//BM92S2331_1 fps(22,&Serial1);  // Please uncomment this line of code if you use HW Serial1 on BMduino
//BM92S2331_1 fps(25,&Serial2);  // Please uncomment this line of code if you use HW Serial2 on BMduino

uint16_t result;
void setup() {
  Serial.begin(115200);
  delay(100); // wait for the module stable
  fps.begin();
  Serial.println(F("Please press your finger to start detection"));
}
void loop() {

    result=fps.identify();
    if(result>0)
    {
      if(result==0xffff)
      {
      Serial.println(F("Detection failed"));
      }
      else
      {
      Serial.print(F("Verified ID: "));
      Serial.println(result);
      }
    }
}
