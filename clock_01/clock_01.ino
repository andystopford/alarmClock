// Date and time functions using a DS1307 RTC connected via I2C and Wire lib

#include <Wire.h>
#include "RTClib.h"

RTC_DS1307 rtc;

int alarmTime = 30;
const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

////////////////////////////////////////
void setup () 
{
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  Serial.begin(57600);
  Wire.begin();
  rtc.begin();
  
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

////////////////////////////////
void loop () {
    DateTime now = rtc.now();    
    buttonState = digitalRead(buttonPin);     // read the state of the pushbutton value:

    if (buttonState == HIGH) 
    {
      Serial.print("ON");
      Serial.println();   
      Serial.print(now.year(), DEC);
      Serial.print('/');
      Serial.print(now.month(), DEC);
      Serial.print('/');
      Serial.print(now.day(), DEC);
      Serial.print(' ');
      Serial.print(now.hour(), DEC);
      Serial.print(':');
      Serial.print(now.minute(), DEC);
      Serial.print(':');
      Serial.print(now.second(), DEC);
      Serial.println();
      
      if (now.second() > alarmTime)
       {
        Serial.print("ALARM");
       } 
    }
    else
    {
      Serial.print("OFF");
    }

    Serial.println();
    delay(1000);
}
