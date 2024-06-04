/*****************************************************************
File:        BM92S2331-1.cpp
Author:      BEST MODULES CORP.
Description: BM92S2331-1.cpp is the library for controlling the BM92S2331_1 Fingerprint Sensor Module
Version:     V1.0.1   -- 2024-06-04
******************************************************************/
#include "BM92S2331-1.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/**********************************************************
Description: Constructor
Parameters:  *theSerial : Wire object if your board has more than one UART interface
Return:
Others:
**********************************************************/
BM92S2331_1::BM92S2331_1(uint8_t WakeupPin,HardwareSerial *theSerial)
{
  _softSerial = NULL;
  _serial = theSerial;
  _wakeupPin=WakeupPin;
}

/**********************************************************
Description: Constructor
Parameters:  rxPin : Receiver pin of the UART
             txPin : Send signal pin of UART
Return:
Others:
**********************************************************/
BM92S2331_1::BM92S2331_1(uint8_t WakeupPin,uint8_t rxPin, uint8_t txPin)
{
  _serial = NULL;
  _rxPin = rxPin;
  _txPin = txPin;
  _wakeupPin= WakeupPin;
  _softSerial = new SoftwareSerial(_rxPin, _txPin);
}

/**********************************************************
Description: initialize the module
Parameters:  If use SW Serial,baud : baud rate 9600, 19200, 38400, 57600, 115200(default)
             If use HW Serial,baud : baud rate 9600, 19200, 38400, 57600, 115200(default), 128000, 230400, 256000, 460800
Return:      -
Others:
**********************************************************/
void BM92S2331_1::begin(unsigned long baud)
{
  if (_softSerial != NULL)
  {
   _softSerial->begin(baud);
  }
   else
   {
    _serial->begin(baud);
   }   
   pinMode(_wakeupPin,OUTPUT);
}
/**********************************************************
Description: Get the number of enrolled fingerprints,check ID status
Parameters: ID(1~1000)
Return:     0:ID is within range and not used
            4:ID already in use
            8:ID out of range
			      0xff：error
Others:
**********************************************************/
int8_t BM92S2331_1::InputEnrollID(int16_t ID)
{
  uint8_t i,idl,idh,readstate;
  uint16_t sum = 0;
  uint8_t responseByte[255];
  idl=(byte)ID&0xFF;
  idh=(byte)(ID>>8)&0xFF;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,idl,idh,0,0,0x11,0x00,0x00,0x00};
  for (i=0; i<10; i++) 
  {
    sum += commandByte[i];
  }  
  commandByte[10] = (byte)sum&0xFF;
  commandByte[11] = (byte)(sum>>8)&0xFF;
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12);
  if (check(responseByte) == true&&readstate==0)
  {
    return responseByte[4];
  }
  else 
  {
    return 0xff;
  }
}


/**********************************************************
Description: Get enrollment status
Parameters:
Return:      false : Communication error
             responseByte[4] : Registration Status :
             1: Press finger again
             2: enroll success
             3: enroll fail
             5: Try again with other areas on the surface of your fingers
             6：Please move finger

Others:
**********************************************************/
uint8_t BM92S2331_1::enroll()
{
 uint8_t readstate;
 uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x03,0x00,0x02,0x01};
 uint8_t responseByte[255];
 writeBytes(commandByte,12);  
 readstate=readBytes(responseByte, 12);  
 if(readstate==1)
 {
    return false;
 }
 else
 {
   if (check(responseByte)==true)
  {
    return responseByte[4];
  }
  else 
  {
    return false;
  }
 }
}

/**********************************************************
Description: Stop enroll
Parameters:
Return:      true : successful
             false : failed
Others:
**********************************************************/
bool BM92S2331_1::stopEnroll()
{
 uint8_t readstate;
 uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x04,0x00,0x03,0x01};
 uint8_t responseByte[255];
 writeBytes(commandByte,12);
 readstate=readBytes(responseByte, 12); 
  if ((readstate==0)&&(check(responseByte)==true))
  {
    return true;
  }
  else 
  {
    return false;
  }
}

/**********************************************************
Description: Checks the currently pressed finger against all enrolled fingerprints
Parameters:
Return:      userID :  Verified against the specified ID 1-500 (found, and here is the ID number)
Others:
**********************************************************/
int16_t BM92S2331_1::identify()
{
  uint8_t readState;
  int16_t userID;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x0A,0x00,0x09,0x01};
  uint8_t responseByte[255];
  writeBytes(commandByte,12);
  readState=readBytes(responseByte, 12 );
   if (check(responseByte)==true&&readState==0)
  {
    userID=responseByte[4]+(responseByte[5]<<8)+(responseByte[6]<<16)+(responseByte[7]<<24);
    return userID;
  }
  else 
  {
    return false;
  }
}
/**********************************************************
Description: Stop detection
Parameters:
Return:     true : successful
            false : failed
**********************************************************/
bool BM92S2331_1::stopIdentify()
{
  uint8_t readState;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x0B,0x00,0x0A,0x01};
  uint8_t responseByte[255];
  writeBytes(commandByte,12);
  readState=readBytes(responseByte, 12 );  
   if (check(responseByte)==true&&readState==0)
  {
    return true;
  }
  else 
  {
    return false;
  }
}
/**********************************************************
Description: Deletes the specified ID from the database
Parameters:  id : 1-500 (the ID number to be deleted)
Return:      false : failed
             responseByte[4]： 
             1：Successfully deleted
             2：Delete failed
             3：The deleted ID does not exist
Others:
**********************************************************/
uint8_t BM92S2331_1::deleteID(uint16_t id)
{
  uint8_t i,idl,idh,readstate;
  uint16_t sum = 0;
  uint8_t responseByte[255];
  idl=(byte)id&0xFF;
  idh=(byte)(id>>8)&0xFF;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,idl,idh,0,0,0x05,0x00};
  for (i=0; i<10; i++) 
  {
    sum += commandByte[i];
  }  
  commandByte[10] = (byte)sum&0xFF;
  commandByte[11] = (byte)(sum>>8)&0xFF;
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12);
  if (check(responseByte)==true&&readstate==0)
  {
    return responseByte[4];
  }
  else 
  {
    return false;
  }
}

/**********************************************************
Description: Deletes all IDs from the database
Parameters:
Return:      true : successful
             false : failed
Others:
**********************************************************/
bool BM92S2331_1::deleteAll()
{
  uint8_t readstate;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x06,0x00,0x05,0x01};
  uint8_t responseByte[255];

  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12 );
  if (check(responseByte)==true&&responseByte[4] == 1&&readstate==0) 
  {
    return true;
  }
  else 
  {
    return false;
  }
}
/**********************************************************
Description: write WakeUp pin status
Parameters:state pin status,HIGH or LOW
Return:      
Others:
**********************************************************/

void BM92S2331_1::wakeup()
{
    digitalWrite(_wakeupPin,LOW);
    delay(2);
    digitalWrite(_wakeupPin,HIGH);
}

/**********************************************************
Description: Module enters standby mode
Parameters:
Return:     true : successful
            false : failed
Others:
**********************************************************/
bool BM92S2331_1::StandbyMode()
{
  uint8_t readstate;
  uint8_t responseByte[255];
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x13,0x00,0x12,0x01};
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12); 
  if (check(responseByte) == true&&readstate==0)
  {
    return true;
  }
  else 
  {
    return false;
  }
}

/**********************************************************
Description: Change baud rate
Parameters:  If use SW Serial,baud : baud rate 9600, 19200, 38400, 57600, 115200(default)
             If use HW Serial,baud : baud rate 9600, 19200, 38400, 57600, 115200(default), 128000, 230400, 256000, 460800
Return:      true : Successfully changed baud rate.
             false : Failed to change baud rate.
Others:      9600   baud command bytes = {0x55,0xAA,0x00,0x00,0x80,0x25,0x00,0x00,0x02,0x00,0xA6,0x01};
             19200  baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0x4B,0x00,0x00,0x02,0x00,0x4C,0x01};
             38400  baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0x96,0x00,0x00,0x02,0x00,0x97,0x01};
             57600  baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0xE1,0x00,0x00,0x02,0x00,0xE2,0x01};
             115200 baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0xC2,0x01,0x00,0x02,0x00,0xC4,0x01};
             128000 baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0xF4,0x01,0x00,0x02,0x00,0xF6,0x01};
             230400 baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0x84,0x03,0x00,0x02,0x00,0x88,0x01};
             256000 baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0xE8,0x03,0x00,0x02,0x00,0xEC,0x01};
             460800 baud command bytes = {0x55,0xAA,0x00,0x00,0x00,0x08,0x07,0x00,0x02,0x00,0x10,0x01};
**********************************************************/
bool BM92S2331_1::changeBaud(unsigned long baud)
{
  uint8_t i,readstate;
  uint8_t responseByte[255];
  uint16_t sum = 0;
  if ((baud == 9600) || (baud == 19200) || (baud == 38400) || (baud == 57600) || (baud == 115200)|| (baud == 128000) || (baud == 230400)|| (baud == 256000) || (baud == 460800))
  {
    uint8_t commandByte[12]={0x55,0xAA,0x00,0x00};
    commandByte[4] = (baud & 0x000000ff);
    commandByte[5] = (baud & 0x0000ff00)>>8;
    commandByte[6] = (baud & 0x00ff0000)>>16;
    commandByte[7] = (baud & 0xff000000)>>24;
    commandByte[8] = 0x02;
    commandByte[9] = 0;
    for (i=0; i<10; i++)
    {
      sum += commandByte[i];
    }  
    commandByte[10] = (byte)sum&0xFF;
    commandByte[11] = (byte)(sum>>8)&0xFF;

    writeBytes(commandByte,12);
    readstate=readBytes(responseByte, 12);

     if (check(responseByte)==true&&readstate==0)
    {
      return true;
    }
    else 
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}


/**********************************************************
Description: Get module device information
Parameters:
Return:      0:error
             1: MODULE_BM92S2131_1 
             2: MODULE_BM92S2231_1 
             3：MODULE_BM92S2331_1
Others:
**********************************************************/
uint8_t BM92S2331_1::getDeviceInformation()
{
  uint16_t i;
  uint8_t readstate;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x0D,0x00,0x0C,0x01};
  uint8_t responseByte[255]; 
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12 ); 
  if (check(responseByte)==true&&readstate==0) 
  {
     readstate=readBytes(responseByte, 16 ); 
    if(checkData(responseByte,16)==true&&readstate==0) 
    {
    i=responseByte[4]+(responseByte[5]<<8)+(responseByte[6]<<16)+(responseByte[7]<<24);
    if(i==21311)
    {
      return 1;    
    }
    if(i==22311)
    {
      return 2;    
    }
    if(i==23311)
    {
      return 3;    
    }
    else
    {
      return 0;
    }
    }
    else
    return 0;
  }
  else 
  {
    return 0;
  }
}


/**********************************************************
Description: Get the score threshold, detection angle, and template quantity information of the device
Parameters:
Return:      true : successful
             false : failed
Others:
**********************************************************/
bool BM92S2331_1::getModuleSettingsInformation(uint8_t inforArray[])
{
  uint8_t readstate;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x14,0x00,0x13,0x01};
  uint8_t responseByte[255];
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12 ); 
  if (check(responseByte)==true&&readstate==0) 
  {

    inforArray[0]=responseByte[4];//
    inforArray[1]=responseByte[5];//
    inforArray[2]=responseByte[6];//
    inforArray[3]=responseByte[7];//
    //  scoreThreshold=responseByte[4]+(responseByte[5]<<8);
    // Serial.print(F("Score threshold : "));
    // Serial.println(scoreThreshold);
    // Serial.print(F("Detection angle : "));
    // Serial.println(responseByte[6]);
    // Serial.print(F("Number of templates : "));
    // Serial.println(responseByte[7]);   
    return true;
  }
  else 
  {
    return false;
  }
}


/**********************************************************
Description: Get image threshold and percentage information
Parameters:
Return:      true : successful
             false : fail
Others:
**********************************************************/
bool BM92S2331_1::getImageSettingInformation(uint8_t inforArray[])
{
  uint8_t readstate;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x15,0x00,0x14,0x01};
  uint8_t responseByte[255];
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12 ); 
  if (check(responseByte)==true&&readstate==0) 
  {
     inforArray[0]=responseByte[4];//
    inforArray[1]=responseByte[5];//
    inforArray[2]=responseByte[6];//
    inforArray[3]=responseByte[7];//
    // imageThreshold=responseByte[4]+(responseByte[5]<<8);
    // imagePercentage=responseByte[6]+(responseByte[7]<<8);
    // Serial.print(F("Image threshold : "));
    // Serial.println(imageThreshold);
    // Serial.print(F("Image percentage : "));
    // Serial.print(imagePercentage);
    // Serial.println(F("%"));
    return true;
  }
  else 
  {
    return false;
  }
}
/**********************************************************
Description: Set the score threshold, detection angle, and number of templates for the module
Parameters:  scoreThreshold:1-100
             checkAngle:0,1,3,4
             numberTemplates:1-3
Return:      true : successful
             false : failed
Others:
**********************************************************/
bool BM92S2331_1::userSet(uint16_t scoreThreshold,uint8_t checkAngle,uint8_t numberTemplates )
{
  uint8_t i,readstate,scorel,scoreh;
  uint16_t sum = 0;
  uint8_t responseByte[255];
  scorel=(byte)scoreThreshold&0xFF;
  scoreh=(byte)(scoreThreshold>>8)&0xFF;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,scorel,scoreh,checkAngle,numberTemplates,0x0f,0x00,0x00,0x00};
  for (i=0; i<10; i++) 
  {
    sum += commandByte[i];
  }  
  commandByte[10] = (byte)sum&0xFF;
  commandByte[11] = (byte)(sum>>8)&0xFF;
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12);
  if (check(responseByte)==true&&readstate==0)
  {
   // Serial.println(F("Set success"));
    return true;
  }
  else 
  {
   // Serial.println(F("Set fail"));
    return false;
  }
}
/**********************************************************
Description: Set image threshold and percentage
Parameters:  imageThreshold:1-1000
             imagePercentage:1-100
Return:      true : successful
             false : fail
Others:
**********************************************************/
bool BM92S2331_1::imageSet(uint16_t imageThreshold,uint16_t imagePercentage)
{
  uint8_t i,readstate,imageThresholdl,imageThresholdh,imagePercentagel,imagePercentageh;
  uint16_t sum = 0;
  uint8_t responseByte[255];
  imageThresholdl=(byte)imageThreshold&0xFF;
  imageThresholdh=(byte)(imageThreshold>>8)&0xFF;
  imagePercentagel=(byte)imagePercentage&0xFF;
  imagePercentageh=(byte)(imagePercentage>>8)&0xFF;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,imageThresholdl,imageThresholdh,imagePercentagel,imagePercentageh,0x10,0x00,0x00,0x00};
  for (i=0; i<10; i++) 
  {
    sum += commandByte[i];
  }  
  commandByte[10] = (byte)sum&0xFF;
  commandByte[11] = (byte)(sum>>8)&0xFF;
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12);
  if (check(responseByte)==true&&readstate==0)
  {
  //  Serial.println(F("Set success"));
    return true;
  }
  else 
  {
 //   Serial.println(F("Set fail"));
    return false;
  }
}

/**********************************************************
Description: Firmware Update
Parameters:
Return:      true : successful
             false : fail
Others:
**********************************************************/
bool BM92S2331_1::firmwareUpdate()
{
  uint8_t readstate;
  uint8_t commandByte[12]={0x55,0xAA,0x00,0x00,0,0,0,0,0x16,0x00,0x15,0x01};
  uint8_t responseByte[255];
  writeBytes(commandByte,12);
  readstate=readBytes(responseByte, 12 ); 
  if (check(responseByte)==true&&readstate==0) 
  {
    //Serial.print(F("Please open HT32_UART_IAP_ Tool to update code"));
    return true;
  }
  else 
  {
    return false;
  }
}
/* Private functions ---------------------------------------------------------------------------------------*/
/**********************************************************
Description: Get the response date from the BM92S2131-1 module
Parameters:  response[] : the pointer of the array for storing response date
             length : the response date length for READ commands.
             timeout:Time out for storing input
Return:       1: Timeout error
              0:read success
Others:
**********************************************************/
uint8_t BM92S2331_1::readBytes(uint8_t response[] ,uint8_t length,uint16_t timeout)
{
  uint8_t i ;
  uint16_t delayCnt = 0;
  if (_softSerial != NULL)
  {
    for (i=0; i < length; i++)
    {     
        delayCnt = 0;
        while (_softSerial->available() == 0)
        {
          if (delayCnt > timeout)
          {
            return 1;
          }          
          delay(1);
          delayCnt++;
        }        
        response[i] = (byte) _softSerial->read();
    }
  }
  else
  {
    for (i=0; i < length; i++)
    {
      while (_serial->available() == 0)
      {
          delay(1);
          delayCnt++;
         if (delayCnt > timeout)
          {
            return 1;
          }
      }
      *(response+i) = (byte) _serial->read();
    }
  }
  return 0;
}




/**********************************************************
Description: writeBytes
Parameters:  wbuf : Data to be written
             wlen : Length of data to be written
Return:
Others:
**********************************************************/
void BM92S2331_1::writeBytes(uint8_t wbuf[] ,uint8_t wlen)
{
   if (_softSerial != NULL)
  {
    while (_softSerial->available() > 0)
    {
      _softSerial->read();
    }
    _softSerial->write(wbuf, wlen);
  }
  /* Select HardwareSerial Interface */
  else
  {
    while (_serial->available() > 0)
    {
      _serial->read();
    }
    _serial->write(wbuf, wlen);
  }
}

/**********************************************************
Description: Check the correctness of the module's response data
Parameters:  module's response data
Return:      true : successful
             false : fail
Others:
**********************************************************/
bool BM92S2331_1::check(uint8_t receivebuf[])
{
  uint16_t checksum=0;
  uint8_t checksum_l,checksum_h,i;
  for(i=0;i<10;i++) {
    checksum=checksum+receivebuf[i];
    }
  checksum_l= (byte)checksum&0xFF;
  checksum_h=(byte)(checksum>>8)&0xFF;
 if (receivebuf[0] == 0x55&&receivebuf[1] == 0xAA&&receivebuf[8] == 0x30&&receivebuf[10] == checksum_l&&receivebuf[11] == checksum_h) 
  {
    return true;
  }
  else 
  {
    return false;
  }
}
/**********************************************************
Description: Check the correctness of the feedback information from the module
Parameters:  feedback information from the module
Return:      true : successful
             false : fail
Others:
**********************************************************/
bool BM92S2331_1::checkData(uint8_t receivebuf[],uint16_t receivenum)
{
  uint16_t checksum=0,i;
  uint8_t checksum_1,checksum_2,checksum_3,checksum_4;
  for(i=0;i<receivenum-4;i++) {
    checksum=checksum+receivebuf[i];    
    }
  checksum_1= (byte)checksum&0xFF;
  checksum_2= (byte)(checksum>>8)&0xFF;
  checksum_3= (byte)(checksum>>16)&0xFF;
  checksum_4= (byte)(checksum>>24)&0xFF;

 if (receivebuf[0] == 0x5A&&receivebuf[1] == 0xA5&&receivebuf[receivenum-4] == checksum_1&&receivebuf[receivenum-3] == checksum_2&&receivebuf[receivenum-2] == checksum_3&&receivebuf[receivenum-1] == checksum_4) 
  {
    return true;
  }
  else 
  {
    return false;
  }
}
