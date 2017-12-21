#include <stdio.h>
#include "ds3231.h"
#include <Wire.h>

DS3231 clock;
RTCDateTime dt;
//pin number
short swtPin = 2;
short ledPin = 13;
short dimmingPin = 5;

//Sunrise Time, 14:01:00
#define SUNRISE_HOUR 16
#define SUNRISE_MIN 1
#define SUNRISE_SEC 0

//Sunset Time , 23:01:1
#define SUNSET_HOUR 23
#define SUNSET_MIN 1
#define SUNSET_SEC 1

#define DIM_STEP 1
//#define SUN_SPEED 5 // 1 is fast, 5 is slow : if set 5, then dimming run 1 hour to sunrise or sunset.
#define MAX_LIGHT 255 // 63 ~ 255
#define DIMMING_TIME 1000 * 60 * 60 // 1 hour






#define DIM_INTERVAL_0 1000*12*SUN_SPEED*10 // 2 * SUN_SPEED minute, only use when dimming value is 1 10분
#define DIM_INTERVAL_1 1000*12*SUN_SPEED*3  // 0.6 * SUN_SPEED minute, dimming value is 2, 3 6분
#define DIM_INTERVAL_2 1000*10*SUN_SPEED*1  // 10 second * SUN_SPEED, dimming value is 4 ~ 9 5분
#define DIM_INTERVAL_3 1000*2*SUN_SPEED*1  // 2 second * SUN_SPEED, dimming value is 3% ~ 30% ( 50 ) , 10분
#define DIM_INTERVAL_4 1000*SUN_SPEED*1  // 1 second * SUN_SPEED, dimming value is 20% ~ 70% ( 51 ~ 178)

const char *dayWeekData[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };


short dimmingVal = 0;
short dimmingStep = 0;
short prevDimmingVal = 0;
bool isDemo = false;

bool byTimeFlag = false;

//short dimmingState = 0;
short delayCnt = 0;
bool sunFlag = false; // true : sunrise, false sunset

long changeTime = 0;





#define BUFF_MAX 16
#define DEBUG
//#define SET_RTC_TIME

void setup() {
  // put your setup code here, to run once:
  pinMode(swtPin, INPUT_PULLUP);
  pinMode(dimmingPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  analogWrite(dimmingPin, 0);
  changeTime = millis();
  
  clock.begin();
  Serial.begin(9600);
  #ifdef SET_RTC_TIME
  clock.setDateTime(2017, 12, 20, 1, 10, 0);  // Manual (YYYY, MM, DD, HH, II, SS)
  #endif
}

void loop() {
   dt = clock.getDateTime();
   if (delayCnt == 0) {
      digitalWrite(ledPin, LOW);
   } else {
      delayCnt --;
   }
   
  // put your main code here, to run repeatedly:  
  if ( LOW == digitalRead(swtPin)) { //switch push..
    Serial.println(clock.dateFormat("d-m-Y H:i:s", dt));
    digitalWrite(ledPin, HIGH);
    delayCnt = 3;  //led delayCnt
    sunFlag = sunFlag ? false : true;
    dimmingStep = 3;
    isDemo = true;
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

  if (dt.hour == SUNRISE_HOUR && dt.minute == SUNRISE_MIN && dt.second == SUNRISE_SEC ) {
    Serial.println("SUNRISE");
    sunFlag = true;
    dimmingStep = DIM_STEP;
    byTimeFlag =  true;
  }

  if (dt.hour == SUNSET_HOUR && dt.minute == SUNSET_MIN && dt.second == SUNSET_SEC ) {
    Serial.println("SUNSET");
    sunFlag = false;
    dimmingStep = DIM_STEP;
    byTimeFlag = true;
  }

  char tBuff2[BUFF_MAX] = {0,};
  if (isDemo || byTimeFlag ) {
    if (isDemo || millis() - changeTime > DIM_INTERVAL)
    {
        if (sunFlag == true) {
           if (dimmingVal < 255){
               dimmingVal += dimmingStep;
           } else {
              dimmingVal = 255;
              isDemo = false;
           }
        } else {
           if (dimmingVal > 0){
               dimmingVal -= dimmingStep;
           } else {
              dimmingVal = 0;
              isDemo = false;
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

  delay(500);
}
