#include <stdio.h>
#include "ds3231.h"
#include <Wire.h>

DS3231 clock;
RTCDateTime dt;
//pin number
short swtPin = 2;
short ledPin = 13;
short dimmingPin = 5;
short dimmingVal = 0;
short dimmingStep = 0;
short prevDimmingVal = 0;
bool isDemo = false;

bool byTimeFlag = false;

//short dimmingState = 0;
bool sunFlag = false; // true : sunrise, false sunset

unsigned long changeTime = 0;

unsigned long pushSwtichTime = 0;
unsigned long prevGetDTTime = 0;

unsigned short g_interval = 0;

const char *dayWeekData[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

#define MAX_LIGHT 220 // max is 255

#define RISE_HOUR 16
#define RISE_MIN 1
#define RISE_SEC 0

#define SET_HOUR 22
#define SET_MIN 30
#define SET_SEC 0

//#define DIM_STEP 1
//#define DIM_INTERVAL 3000

#define DIM_STEP 1
#define DIM_INTERVAL 15000

#define DEMO_DIM_INTERVAL 50

#define BUFF_MAX 16
#define DEBUG
//#define SET_TIME

void setup() {
  // put your setup code here, to run once:
  pinMode(swtPin, INPUT_PULLUP);
  pinMode(dimmingPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  analogWrite(dimmingPin, 0);
  changeTime = millis();
  
  clock.begin();
  Serial.begin(9600);
  #ifdef SET_TIME
  // Manual (YYYY, MM, DD, HH, II, SS)
  clock.setDateTime(2017, 12, 20, 1, 10, 0);
  #endif
}

void loop() {
  if ( millis() - prevGetDTTime > 10000) {
    Serial.println("get Time");
    dt = clock.getDateTime();
    Serial.println(clock.dateFormat("d-m-Y H:i:s", dt));
    prevGetDTTime = millis();
  }

  // put your main code here, to run repeatedly:  
  if ( LOW == digitalRead(swtPin)) { //switch push..
    digitalWrite(ledPin, HIGH);
    if (pushSwtichTime == 0) {
       pushSwtichTime = millis();
    }
  } else {
    digitalWrite(ledPin, LOW);
    if (pushSwtichTime > 0) {
      Serial.println(millis() - pushSwtichTime);
      if (millis() - pushSwtichTime > 500) {
        //start dimming.
        if (byTimeFlag) {
          g_interval = DEMO_DIM_INTERVAL;
        } else {
          sunFlag = !sunFlag;
          g_interval = DEMO_DIM_INTERVAL;
        }

        isDemo = true;
        byTimeFlag =  false;
      }
      pushSwtichTime = 0;
    }
      /*
      dimmingState++;
      if (dimmingState % 5 == 0)
      {
        Serial.println("PWM : 0");
        analogWrite(dimmingPin, 0);
      } else if (dimmingState % 5 == 1) {
        Serial.println("PWM : 25%");
        analogWrite(dimmingPin, 63);
      } else if (dimmingState % 5 == 2) {
        Serial.println("PWM : 50%");
        analogWrite(dimmingPin, 127);
      } else if (dimmingState % 5 == 3) {
        Serial.println("PWM : 75%");
        analogWrite(dimmingPin, 190);
      } else if (dimmingState % 5 == 4) {
        Serial.println("PWM : 100%");
        analogWrite(dimmingPin, 255);
      }
      */
    
  }

  if (!isDemo) {
     if (!sunFlag && dt.hour == RISE_HOUR && dt.minute == RISE_MIN && dt.second > RISE_SEC ) {
        Serial.println("SUNRISE");
        sunFlag = true;
        byTimeFlag =  true;
        g_interval = DIM_INTERVAL;
     }
  
     if (sunFlag && dt.hour == SET_HOUR && dt.minute == SET_MIN && dt.second > SET_SEC ) {
       Serial.println("SUNSET");
       sunFlag = false;
       byTimeFlag = true;
       g_interval = DIM_INTERVAL;
     }
  }
  
  char tBuff2[BUFF_MAX] = {0,};
  if (isDemo || byTimeFlag ) {
    if (millis() - changeTime > g_interval)
    {
        if (sunFlag == true) {
           if (dimmingVal < MAX_LIGHT){
               dimmingVal += DIM_STEP;
           } else {
              dimmingVal = MAX_LIGHT;
              isDemo = false;
              byTimeFlag = false;
           }
        } else {
           if (dimmingVal > 0){
               dimmingVal -= DIM_STEP;
               if (dimmingVal < 0 ) {
                  dimmingVal = 0;
               }
           } else {
              dimmingVal = 0;
              isDemo = false;
              byTimeFlag = false;
           }
        }
    }
  }

  if (prevDimmingVal != dimmingVal) {
     prevDimmingVal = dimmingVal;
     char tBuff[BUFF_MAX] = {0,};
     snprintf(tBuff, BUFF_MAX, "val:%d", dimmingVal);
     Serial.println(tBuff);
     analogWrite(dimmingPin, dimmingVal);
     changeTime = millis();
  }
  
  if (isDemo) {
    delay(50);
  } else {
    delay(250);  
  }
}
