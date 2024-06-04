/*****************************************************************
File:             BM92S2331-1.h
Author:           BEST MODULES CORP.
Description:      Define classes and required variables
Version:          V1.0.1   -- 2024-06-04
******************************************************************/
#ifndef BM92S2331_1_H
#define BM92S2331_1_H

#include "Arduino.h"
#include "SoftwareSerial.h"


class BM92S2331_1
{
  public:
    
    BM92S2331_1(uint8_t WakeupPin=2,HardwareSerial *theSerial = &Serial);
    BM92S2331_1(uint8_t WakeupPin,uint8_t rxPin, uint8_t txPin);
    void begin(unsigned long baud = 115200);
    int8_t InputEnrollID(int16_t ID);
    uint8_t enroll();
    bool stopEnroll();

    int16_t identify();
    bool stopIdentify();

    uint8_t deleteID(uint16_t id);
    bool deleteAll();

    void wakeup();
    bool StandbyMode();
    bool changeBaud(unsigned long baud);     
    uint8_t getDeviceInformation();
    bool getModuleSettingsInformation(uint8_t inforArray[]);
    bool getImageSettingInformation(uint8_t inforArray[]);
    
    bool userSet(uint16_t score,uint8_t checkAngle,uint8_t numberTemplates );
    bool imageSet(uint16_t imageThreshold,uint16_t imagePercentage);
   

  private:
    uint8_t _rxPin;
    uint8_t _txPin;
    uint8_t _wakeupPin;
    HardwareSerial *_serial = NULL;
    SoftwareSerial *_softSerial = NULL;
    void writeBytes(uint8_t wbuf[] ,uint8_t wlen);
    uint8_t readBytes(uint8_t response[], uint8_t length,uint16_t timeout = 500);
    bool check(uint8_t receivebuf[]);
    bool checkData(uint8_t receivebuf[],uint16_t receivenum);
    bool firmwareUpdate();
};
#endif
