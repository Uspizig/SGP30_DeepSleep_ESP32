/*
  Library for the Sensirion SGP30 Indoor Air Quality Sensor
  By: Ciara Jekel
  SparkFun Electronics
  Date: June 28th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SGP30 Datasheet: https://cdn.sparkfun.com/assets/c/0/a/2/e/Sensirion_Gas_Sensors_SGP30_Datasheet.pdf

  Modified code by USpizig:
  - Replaced EEPROM Lib by RTC Memory
  - Added Deep Sleep and Wakeup

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/14813

  This example Stores the Baseline values in RTC Memory of ESP32 before sleeping
  After Wakeup of Deep Sleep it restores its baseline.
*/

#include "SparkFun_SGP30_Arduino_Library.h" // Click here to get the library: http://librarymanager/All#SparkFun_SGP30
#include <Wire.h>
#include <EEPROM.h>
long t1, t2;

RTC_DATA_ATTR int CO2BASE = 0;
RTC_DATA_ATTR int TVOCBASE = 0;
//Innen_CO2_baseline, Innen_TVOC_baseline

#define sda2 15
#define scl2 14
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5        /* Time ESP32 will go to sleep (in seconds) */

SGP30 mySensor; //create an object of the SGP30 class
byte baselineC02Addr = 0x00;
byte baselineTVOCAddr = 0x02;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by Deep Sleep Timer"); 
      
      mySensor.initAirQuality();//A new initAirQuality” command has to be sent after every power-up or soft reset.
      delay(16000);//Wait 20 sec before first messure
      //For the first 15s after the “sgp30_iaq_init” command the sensor is
      //in an initialization phase during which a “sgp30_measure_iaq” command returns fixed values of 400 ppm CO2eq and 0 ppb
      mySensor.measureAirQuality();
      delay(1000); //Wait 1 second
      mySensor.setBaseline(CO2BASE, TVOCBASE);         // set baseline from last cycle
      delay(1000); //Wait 1 second
      /*Serial.println("Schreibe Baseline Werte nach aufwachen in den CHIP: CO2Base " +String(CO2BASE) + " TVOC: " + String(TVOCBASE)+ " und warte 15sec");
      delay(15000); //Wait 15 sec between Set Baseline and first Measurement https://github.com/paulvha/svm30/blob/master/examples/example3_svm30_update_baseline/example3_svm30_update_baseline.ino#L21
      if(mySensor.getBaseline()==0){
        Serial.println("Baseline CO2 aus Chip rückgelesen nach DeepSleep Recovery:" +String(mySensor.baselineCO2) + " TVOC: " +String(mySensor.baselineTVOC));
      }*/
     break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason);  
      mySensor.initAirQuality(); 
      delay(20000); //Wait 20 sec before messure
      //For the first 15s after the “sgp30_iaq_init” command the sensor is
      //in an initialization phase during which a “sgp30_measure_iaq” command returns fixed values of 400 ppm CO2eq and 0 ppb
      Serial.println("Starte 30 CO2 Messungen für Baseline Kalibrierung "); //First fifteen readings will be bullshit
      for (byte count = 0; count < 30; count++)
      {
        delay(1000); //Wait 1 second
        mySensor.measureAirQuality();
      }
      //mySensor.getBaseline(); //get current baseline from SGP30 Chip
      //CO2BASE = mySensor.baselineCO2; TVOCBASE = mySensor.baselineTVOC;//store baselineTVOC in location baselineTVOCAddr
      //Serial.println("Initale Baseline Co2 in RTC Memory gespeichert:" + String(mySensor.baselineCO2) + " TVOC:" +String(mySensor.baselineTVOC));
    break;
  }
}


void print_wakeup_reason2(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by Deep Sleep Timer"); 
      
      mySensor.initAirQuality();//A new initAirQuality” command has to be sent after every power-up or soft reset.
      delay(1000); //Wait 1 second
      mySensor.setBaseline(CO2BASE, TVOCBASE);         // set baseline from last cycle
      delay(14000);
      //For the first 15s after the “sgp30_iaq_init” command the sensor is
      //in an initialization phase during which a “sgp30_measure_iaq” command returns fixed values of 400 ppm CO2eq and 0 ppb
      mySensor.measureAirQuality();
      //delay(1000); //Wait 1 second
      /*Serial.println("Schreibe Baseline Werte nach aufwachen in den CHIP: CO2Base " +String(CO2BASE) + " TVOC: " + String(TVOCBASE)+ " und warte 15sec");
      delay(15000); //Wait 15 sec between Set Baseline and first Measurement https://github.com/paulvha/svm30/blob/master/examples/example3_svm30_update_baseline/example3_svm30_update_baseline.ino#L21
      if(mySensor.getBaseline()==0){
        Serial.println("Baseline CO2 aus Chip rückgelesen nach DeepSleep Recovery:" +String(mySensor.baselineCO2) + " TVOC: " +String(mySensor.baselineTVOC));
      }*/
     break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason);  
      mySensor.initAirQuality(); 
      delay(20000); //Wait 20 sec before messure
      //For the first 15s after the “sgp30_iaq_init” command the sensor is
      //in an initialization phase during which a “sgp30_measure_iaq” command returns fixed values of 400 ppm CO2eq and 0 ppb
      Serial.println("Starts 30 CO2 Measurements for Baseline Calibration "); //First fifteen readings will be bullshit
      for (byte count = 0; count < 30; count++)
      {
        delay(1000); //Wait 1 second
        mySensor.measureAirQuality();
      }
      //mySensor.getBaseline(); //get current baseline from SGP30 Chip
      //CO2BASE = mySensor.baselineCO2; TVOCBASE = mySensor.baselineTVOC;//store baselineTVOC in location baselineTVOCAddr
      //Serial.println("Initale Baseline Co2 in RTC Memory gespeichert:" + String(mySensor.baselineCO2) + " TVOC:" +String(mySensor.baselineTVOC));
    break;
  }
}


void setup() {
  Serial.begin(115200);
  Wire.begin(sda2, scl2);
  //Wire.setClock(400000);
  
  sgp30_start();
  print_wakeup_reason2();      
  MeasureCo2();
  StoreNewBaseline();
  sleeping();
}

void loop() {
 
}

void StoreNewBaseline(void){
  mySensor.getBaseline(); 
  CO2BASE=mySensor.baselineCO2;  TVOCBASE=mySensor.baselineTVOC;
  Serial.println("Baseline Co2 gespeichert: CO2: "+ String(mySensor.baselineCO2) + " - TVOC: " +String(mySensor.baselineTVOC));  
  mySensor.generalCallReset(); //to lower operating current and enable sleep Mode   
  delay(200);
}

void sleeping(void){
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  esp_deep_sleep_start();
}
void MeasureCo2(void){
  for (byte Mess_count = 0; Mess_count < 30; Mess_count++)
  {
    delay(1000); //Wait 1 second
    mySensor.measureAirQuality();
    Serial.println("CO2 Messung #" + String(Mess_count) +":" + String(mySensor.CO2) + " ppm TVOC: " + String(mySensor.TVOC)+ " ppb");    
  }
}
int sgp30_start(void){
  if (mySensor.begin() == false) {
    Serial.println("No SGP30 Detected. Check connections.");
    return 1;
  }
  else
  { 
    return 0;
  }
}
