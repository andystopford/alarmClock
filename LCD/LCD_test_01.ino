#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include "RTClib.h"
#include <DS3232RTC.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
RTC_DS1307 rtc;
/*/////////////////////////////////////////////////////////////////////////////////////////
TO DO: 
LED indicator for alarm AND snooze - three colour LED
AM/PM indicator - set LCD display to 24hr clock for alarm time set.

*//////////////////////////////////////////////////////////////////////////////////////////
//Variables:

int time_hour;
int time_mins; 
int time_day;
int time_month;
int time_year;  
int hourSet = 19;	//hour alarm is set to
int minSet = 5;	//minute alarm is set to (will be 0, 15, 30, 45 when switched) 
int hrsToGo;
int minsToGo; 
//int alarmOn;	//Variable to store alarm switch position on/off
//int alarmSnooze;	//Variable to store alarm switch position snooze/off 
int snoozeMins = 5;	//number of minutes added when snooze is enabled
alarmOn = digitalRead(2);
alarmSnooze = digitalRead(3);

void setup()   /*----( SETUP: RUNS ONCE )----*/
{

	rtc.begin();
	Serial.begin(9600);  
	lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
	pinMode(1, INPUT);	//alarm on/off
	pinMode(2, INPUT);	//snooze on/off

	//if (! rtc.isrunning()) 
	//  {
	//    Serial.println("RTC is NOT running!");
	//    rtc.adjust(DateTime(__DATE__, __TIME__));
	//  }
	//rtc.adjust(DateTime(__DATE__, __TIME__));  //for setting if already running
	rtc.adjust(DateTime(2015, 2, 10, 19, 15, 0));	//set time explicitly

}


void loop()   
{
	DateTime now = rtc.now();
	time_hour = (now.hour());
	time_mins = (now.minute());
	time_day = (now.day());
	time_month = (now.month());
	time_year = (now.year());
	//alarmOn = digitalRead(2);
	//alarmSnooze = digitalRead(3);


	//calc time before alarm
	hrsToGo = hourSet - time_hour;
	if (minSet == 0)
	{
		minsToGo = 60 - time_mins; //deal with condition where the alarm time is 00 mins
		hrsToGo -= 1;
	}
	else
	{
		minsToGo = minSet - time_mins;
	}
	

	if (alarmOn == LOW)
	{
		if (hrsToGo >= 0)
		{
			mins2go(minsToGo);
			//Serial.print(minsToGo);
		}
	}
	
	if (alarmSnooze == LOW)	//shifts alarm time forward by number of snooze minutes
	{
		if (hrsToGo >= 0)
		{
			minsToGo += snoozeMins;
			mins2go(minsToGo); 
		}
	}
	
	LCDisplay();
	delay(1000);
}/* --(end main loop )-- */

////////////////////////////////////////////////////////////////////////////////

int mins2go(int minsToGo)
//prints time before alarm sounds
{
	if (minsToGo > 0)		
		{
			lcd.setCursor(6, 1);
			lcd.print(" TTG");	//leading space for 16x2 display
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
			alarm();
		}
}


int alarm()
//sounds alarm
{
	lcd.setCursor(11, 1);
	lcd.print("Alarm");
	lcd.setCursor(7, 1);
	lcd.print("   ");
	tone(4, 262, 300);
	delay(200);
	tone(4, 330, 250);
	delay(200);
	tone(4, 294, 500);
	delay(800);
}


int LCDisplay()
//prints time and alarm set time to lcd
{
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


	
	if (alarmSnooze == LOW)	//snooze on	
	{
		lcd.setCursor(0, 1);
		lcd.print("Snooze ");
	}

	if (alarmOn == LOW)  //alarm on. print alarm set time
	{
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
	}
	else if (alarmSnooze == HIGH)	//neither alarm nor snooze on
	{
		lcd.setCursor(0, 1);
		lcd.print("Alarm Off       ");
	}
}