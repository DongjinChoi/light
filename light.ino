#include <stdio.h>
#include "ds3231.h"
#include <Wire.h>

DS3231 clock;
RTCDateTime dt;
short swtPin = 2;
const char *dayWeekData[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

#define DEBUG
//#define SET_TIME

void setup() {
  // put your setup code here, to run once:
  pinMode(swtPin, INPUT_PULLUP);
  clock.begin();
  Serial.begin(9600);
  #ifdef SET_TIME
  // Manual (YYYY, MM, DD, HH, II, SS)
  clock.setDateTime(2017, 12, 20, 1, 10, 0);
  #endif
}

void loop() {
  // put your main code here, to run repeatedly:  
  if ( LOW == digitalRead(swtPin)) { //switch push..
    dt = clock.getDateTime();
    Serial.print("Long number format:          ");
    Serial.println(clock.dateFormat("d-m-Y H:i:s", dt));
  }

  delay(250);
}
