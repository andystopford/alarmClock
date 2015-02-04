#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
RTC_DS1307 rtc;

//Variables:

int time_hour;
int time_mins; 
int time_day;
int time_month;
int time_year;  
int hourSet = 16;
int minSet = 40; 
int hrsToGo;
int minsToGo;         


void setup()   /*----( SETUP: RUNS ONCE )----*/
{

	rtc.begin();
	Serial.begin(9600);  // Used to type in characters
	lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight

	if (! rtc.isrunning()) 
	  {
	    Serial.println("RTC is NOT running!");
	    rtc.adjust(DateTime(__DATE__, __TIME__));
	  }
	rtc.adjust(DateTime(__DATE__, __TIME__));  //for setting if already running



}


void loop()   
{
	DateTime now = rtc.now();
	time_hour = (now.hour());
	time_mins = (now.minute());
	time_day = (now.day());
	time_month = (now.month());
	time_year = (now.year());


	//print current time
	lcd.setCursor(0,0);
	if (time_hour < 10)
	{
		lcd.print("0");
		lcd.print(time_hour);
	}
	else
	{
		lcd.print(time_hour);
	}

	lcd.print(":");
	if (time_mins < 10)
	{
		lcd.print("0");
		lcd.print(time_mins);
	}
	else
	{
		lcd.print(time_mins);
	}
	//print date
	lcd.setCursor(8,0);
	if (time_day < 10)
	{
		lcd.print("0");
		lcd.print(time_day);
	}
	else
	{
		lcd.print(time_day);
	}
	lcd.print(":");
	if (time_month < 10)
	{
		lcd.print("0");
		lcd.print(time_month);
	}
	else
	{
		lcd.print(time_month);
	}
	lcd.print(":");
	lcd.print(time_year - 2000);

	//print alarm set time
	lcd.setCursor(0, 1);
	if (hourSet < 10)
	{
		lcd.print("0");
		lcd.print(hourSet);
	}
	else
	{
		lcd.print(hourSet);
	}
	lcd.print(":");
	if (minSet < 10)
	{
		lcd.print("0");
		lcd.print(minSet);
	}
	else
	{
		lcd.print(minSet);
	}

	//print time before alarm
	hrsToGo = hourSet - time_hour;
	minsToGo = minSet - time_mins;
	Serial.print(hrsToGo);
	if (hrsToGo >= 0)
	{
		mins2go(minsToGo);
	}
	//print time to serial to debug
	/*
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
	*/
	delay(1000);
}/* --(end main loop )-- */

////////////////////////////////////////////////////////////////////////////////

int mins2go (int minsToGo)
{
	if (minsToGo > 0)
		{
			lcd.setCursor(7, 1);
			lcd.print("TTG");
			lcd.setCursor(11, 1);
			if (hrsToGo < 10)
			{
				lcd.print("0");
				lcd.print(hrsToGo);
			}
			else
			{
				lcd.print(hrsToGo);
			}
			lcd.print(":");
			if (minsToGo < 10)
			{
				lcd.print("0");
				lcd.print(minsToGo);
			}
			else
			{
			lcd.print(minsToGo);		
			}
		}
	else
		{
			lcd.setCursor(11, 1);
			//lcd.clear(); need an if loop and a dirty setting or stays cleared 
			lcd.print("Alarm");
			tone(6, 440, 200);
				delay(200);
				noTone(6);
		}
}