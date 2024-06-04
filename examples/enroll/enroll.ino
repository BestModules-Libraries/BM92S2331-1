/*****************************************************************
File:         enroll.ino
Description:   Fingerprint Enrollment 
******************************************************************/
#include "BM92S2331-1.h"

BM92S2331_1 fps(2,4,5);        //rxPin:4, txPin:5, Please comment this line of code if don't use SW Serial
//BM92S2331_1 fps(22,&Serial1);  // Please uncomment this line of code if you use HW Serial1 on BMduino
//BM92S2331_1 fps(25,&Serial2);  // Please uncomment this line of code if you use HW Serial2 on BMduino

uint16_t id =1;
uint8_t idCheck = 1,enrollEnable=0;
uint16_t enrollState;
uint8_t idState=0xff;
uint16_t timeCnt=0;
void setup() {
  Serial.begin(115200);
  delay(100); // wait for the module stable
  fps.begin();
}
void loop() {

     if(idCheck==1)  
    {       
      idState=fps.InputEnrollID(id);   
      if(idState==0)
      {
        idCheck=0;
        enrollEnable=1;
        timeCnt=0;
        Serial.print(F("Enroll ID : ")); 
        Serial.println(id);     
        Serial.println(F("Please press finger to Enroll"));     
      }
      else if(idState==4)
      {
       id++;
       //Serial.println(F("ID already used "));
      }
      else if(idState==8)
      {
        idCheck=0;
       Serial.println(F("ID out of range "));
      }
      else
      {
        idCheck=0;
       Serial.println(F("Communication error "));
      }
    }
    if(enrollEnable==1)
    {
       
        enrollState=fps.enroll();   
        if(enrollState!=0) 
          timeCnt=0;  
           
        if(enrollState==1)        
         Serial.println(F("Press finger again")); 
         
        else if(enrollState==2)
        {
         enrollEnable=0;
         Serial.println(F("Enroll success"));
         
         if(fps.stopEnroll()==false)
         Serial.println(F("Communication error ")); 
         else
         idCheck=1;
         Serial.println(F(""));
          
        }
        
        else if(enrollState==3)
        {
          enrollEnable=0;
          Serial.println(F("Enroll fail"));
          
          if(fps.stopEnroll()==false)
          Serial.println(F("Communication error ")); 
          else
          idCheck=1;
          
          Serial.println(F(""));      
        }
        
        else if(enrollState==5)
        Serial.println(F("Try again with other areas on the surface of your fingers"));        

        else if(enrollState==6)        
         Serial.println(F("Please move finger"));  
             
        else if(enrollState==0)
        {
          timeCnt++; 
          if(timeCnt>60)
          {
            Serial.println(F("No fingerprint detected "));
            enrollEnable=0;
            
            if(fps.stopEnroll()==false)
            Serial.println(F("Communication error "));  
          }
       }
       
    }

}
