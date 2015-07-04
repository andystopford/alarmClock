///////////////////////////////////////////////////////////////////////////////////
//  Name    : alarmClock                               
//  Author  : Andy Stopford                                        
//  Date    : 2 Jan, 2015                                      
//  Version : 1.0                                               
//  Notes   : Using four CD4021B Shift Registers
//          : Date and time using a DS3232 RTC connected via I2C and Wire lib
//          :     
//          : ShiftIn from Carlyn Maw, http://www.arduino.cc/en/Tutorial/ShiftIn
///////////////////////////////////////////////////////////////////////////////////

#include <DS3232RTC.h>        //http://github.com/JChristensen/DS3232RTC
#include <Streaming.h>        //http://arduiniana.org/libraries/streaming/
#include <Time.h>             //http://playground.arduino.cc/Code/Time
#include <Wire.h>             //http://arduino.cc/en/Reference/Wire
#include <LiquidCrystal_I2C.h>

//define pins 

const int clockPin = 9;
const int latchPin = 10;
const int dataPin = 11;

//variables to hold shift register data
byte alarmVar1;
byte alarmVar2;
byte alarmVar3;
byte alarmVar4;

//define arrays that corresponds to values for each 
//of the shift register's pins
int hrs_array1[] = {
  1, 2, 3, 4, 5, 6, '@', '@'}; 
int hrs_array2[] = {
  7, 8, 9, 10, 11, 12, '@', '@'};  //@ char returns int 64
int mins_array1[] = {
  5, 10, 15, 20, 25, 30, '@', '@'}; 
int mins_array2[] = {
  35, 40, 45, 50, 55, 0, '@', '@'};

//Initialize time values
int time_hour;
int time_mins;    
int hourSet;
int minSet;
int hrsToGo;
int minsToGo; 
int currTime;
int alarmTime;
int timeToGo;
int alarmOn;  //Variable to store alarm switch position on/off
int alarmSnooze;  //Variable to store alarm switch position snooze/off 
int flag_GMT;
int flag_AM;

int alarmState = LOW;
int alarmSound = LOW;
long previousMillis = 0;
long interval = 1000;
int snoozeTime = 15;   //Mins for snooze
int melody[]= {1900,2000,2100,200,1900};


// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void setup () 
{
  Serial.begin(9600); //For debugging
  Wire.begin();
    
  //setSyncProvider() causes the Time library to synchronize with the
  //external RTC by calling RTC.get() every five minutes by default.
  //It'll just give 1 Jan 1970 if this is omitted
  setSyncProvider(RTC.get);
  Serial << F("RTC Sync");
  if (timeStatus() != timeSet) Serial << F(" FAIL!");
  Serial << endl;

  pinMode(clockPin, OUTPUT); 
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, INPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(12, OUTPUT); //speaker on pin 12

  lcd.begin(20,4);   // initialize the lcd for 20 chars 4 lines, turn on backlight
}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void loop () 
{
  alarmOn = digitalRead(2); 
  alarmSnooze = digitalRead(3);
  flag_GMT = digitalRead(4);
  flag_AM = digitalRead(5);
  //lcd.setBacklight(0);  //Turn backlight off

  //Read RTC----------------------------------------------------
  time_t t;
  t = now();
  time_hour = hour(t);
  time_mins = minute(t);

  //GMT/BST------------------------------------------------------
  if (flag_GMT == LOW)  //i.e. it is BST
    {
      time_hour = time_hour + 1;
    }


  //Alarm status-------------------------------------------------
  
  if (alarmOn == HIGH)
    {
      digitalWrite(6, HIGH);   
      digitalWrite(7, LOW);
    }
  else
    {
      digitalWrite(7, HIGH);
      digitalWrite(6, LOW);
    }

  if (alarmSnooze == HIGH)
    {
      digitalWrite(6, HIGH);
      digitalWrite(7, HIGH);
    }


  //Read Shift Registers-----------------------------------------
  digitalWrite(latchPin,1);
  delayMicroseconds(20);
  digitalWrite(latchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  alarmVar1 = shiftIn(dataPin, clockPin);
  alarmVar2 = shiftIn(dataPin, clockPin);
  alarmVar3 = shiftIn(dataPin, clockPin);
  alarmVar4 = shiftIn(dataPin, clockPin);

  //Match bytes to array locations
  for (int n=0; n<=7; n++)
  {
    if (alarmVar1 & (1 << n) )    
      {
      hourSet = hrs_array1[n];   
      }

    if (alarmVar2 & (1 << n) )
      {
      hourSet = hrs_array2[n];
      }

    if (alarmVar3 & (1 << n) )
      {
      minSet = mins_array1[n];
      }

    if (alarmVar4 & (1 << n) )
      {
      minSet = mins_array2[n];
      }
  }


  //Morning or afternoon alarm------------------------------------
  if (flag_AM == HIGH) //i.e. we want to set to AM
  {
    hourSet = hourSet + 12;
  }


  //Calculate time to go before alarm-----------------------------
  currTime = (time_hour * 60) + time_mins;  //convert times into minutes
  alarmTime = (hourSet * 60) + minSet;
  timeToGo = alarmTime -currTime;

  if (alarmSnooze == HIGH)
    {
      timeToGo += snoozeTime;
    }
  if (timeToGo < 0)
    {
      timeToGo = (1440 - currTime) + alarmTime; //go forward to next day
    }

  minsToGo = timeToGo % 60;   //For LCD display
  hrsToGo = timeToGo / 60;


  //Alarm-----------------------------------------------------------
  if (timeToGo == 0)
    {  
      if (alarmOn == HIGH || alarmSnooze == HIGH) 
        {
          alarmActuate();
        } 
      else
        {
          alarmOff();
        }     
    }
  else
    {
      alarmOff();
    }

  //Print fixed text-----------------------------------------------
  lcd.setCursor(6, 0);
  if (flag_GMT == HIGH)
    {
      lcd.print("GMT");
    }
  else
    {
      lcd.print("BST");
    }

  lcd.setCursor(0, 3);
  lcd.print("Time To Go");


  lcd.setCursor(0, 2);
  if (alarmOn == HIGH)
    {
      lcd.print("Alarm Set For");
    }
  if (alarmSnooze == HIGH)
    {
      lcd.print("Alarm Snooze  ");
    }
  if (alarmOn != HIGH && alarmSnooze != HIGH)
    {
      lcd.print("Alarm Not Set");
    }


  //Print current time and date------------------------------------
  lcd.setCursor(0, 0);
  if (time_hour < 10)
    {
      lcd.print(0);
    }
  lcd.print(time_hour);
  lcd.print(":");
  if (minute(t) < 10)
    {
      lcd.print(0);
    }
  lcd.print(minute(t));

  lcd.setCursor(10, 0);
    if (day(t) < 10)
    {
      lcd.print(0);
    }
  lcd.print(day(t));
  lcd.print(":");
    if (month(t) < 10)
    {
      lcd.print(0);
    }
  lcd.print(month(t));
  lcd.print(":");
  lcd.print(year(t));


  //Print alarm set time---------------------------------------
  lcd.setCursor(15, 2);
  if (hourSet < 10)
    {
      lcd.print(0);
    }
  lcd.print(hourSet);
  lcd.print(":");
  if (minSet < 10)
    {
      lcd.print(0);
    }
  lcd.print(minSet);


  //Print time-to-go--------------------------------------------
  lcd.setCursor(15, 3);
  if (hrsToGo < 10)
    {
      lcd.print(" ");
    }
  
  lcd.print(hrsToGo);
  lcd.print(":");
  if (minsToGo < 10)
    {
      lcd.print(0);
    }
  lcd.print(minsToGo);

}

////////////////////////////////////////////////////////////////////////////

//shiftIn function just needs the location of the data pin and 
//the clock pin it returns a byte with each bit in the byte corresponding
//to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0

byte shiftIn(int myDataPin, int myClockPin) 
{ 
  int i;
  int temp = 0;
  int pinState;
  int myDataIn = 0;
  int switchState = 0;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, INPUT);
  //we will be holding the clock pin high 8 times (0,..,7) at the
  //end of each time through the for loop  
  //at the begining of each loop when we set the clock low, it will
  //be doing the necessary low to high drop to cause the shift
  //register's DataPin to change state based on the value
  //of the next bit in its serial information flow.
  //The register transmits the information about the pins from pin 7 to pin 0
  //so that is why our function counts down
  for (i=7; i>=0; i--)
  {
    digitalWrite(myClockPin, 0);
    delayMicroseconds(100);
    temp = digitalRead(myDataPin);
    if (temp) 
    {
      if (switchState == 0)    
        {
        pinState = 1;
        //set the bit to 0 no matter what
        myDataIn = myDataIn | (1 << i);
        //Serial.println("State 0 =");
        //Serial.println(myDataIn, BIN);
        switchState = 1;
        }
      else
        {
        pinState = 1;
        //set the bit to 0 no matter what
        myDataIn = myDataIn | (1 << i);
        //Serial.println("State 1 =");
        //Serial.println(myDataIn, BIN);
        switchState = 0;
        }
    }
    else 
    {
      //turn it off -- only necessary for debuging
     //print statement since myDataIn starts as 0
      pinState = 0;
    }
    digitalWrite(myClockPin, 1);    
  }
  return myDataIn;
}

////////////////////////////////////


/////////////////////////////////////////////////////////////////


void alarmActuate()
  {
    unsigned long currentMillis = millis();

    if (currentMillis - previousMillis > interval)
      {
        previousMillis = currentMillis;
        if (alarmSound == LOW)
          {
            alarmSound = HIGH;
          }
        else
          {
            alarmSound = LOW;
          }
        alarm(alarmSound);
      }
  }


void alarm(int test)
  {
    //temp led 
    digitalWrite(12, test);
  }


void alarmOff()
  {
    digitalWrite(12, LOW);
  }
