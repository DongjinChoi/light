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
short delayCnt = 0;
bool sunFlag = false; // true : sunrise, false sunset

long changeTime = 0;

const char *dayWeekData[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

#define RISE_HOUR 16
#define RISE_MIN 1
#define RISE_SEC 0

#define SET_HOUR 23
#define SET_MIN 1
#define SET_SEC 0

//#define DIM_STEP 1
//#define DIM_INTERVAL 3000

#define DIM_STEP 1
#define DIM_INTERVAL 10000

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

  if (dt.hour == RISE_HOUR && dt.minute == RISE_MIN && dt.second == RISE_SEC ) {
    Serial.println("SUNRISE");
    sunFlag = true;
    dimmingStep = DIM_STEP;
    byTimeFlag =  true;
  }

  if (dt.hour == SET_HOUR && dt.minute == SET_MIN && dt.second == SET_SEC ) {
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
