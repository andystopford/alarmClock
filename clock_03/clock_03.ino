//****************************************************************
//  Name    : alarmClock                               
//  Author  : A.S. (from Carlyn Maw)                                        
//  Date    : 2 Jan, 2015                                      
//  Version : 1.0                                               
//  Notes   : Code for using a CD4021B Shift Register
//          :trying to use just 3 SRs            

//****************************************************************

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

//define where your pins are
const int switch_PM = 5;
const int switch_BST = 6;
const int clockPin = 7;
const int latchPin = 8;
const int dataPin = 9;
int testA = 0;
int testB = 0;

//variables to hold shift register data
byte alarmVar1 = 256;  //100000000
byte alarmVar2 = 159; //10011111


//define arrays that corresponds to values for each 
//of the shift register's pins

int arrayHrs[] =  
{9, 10 , 11, 12};

int arrayMins[] =
{0, 5, 10, 15};

int arrayLen[] =
{0, 1, 2, 3, 4, 5, 6, 7};

//Initialize time values
int hourSet = 0;
int minSet = 0;
int time_hour;

int index = 0;
int arrayFlag = 0;
int alarmHour = 0;
int alarmMin = 0;


RTC_DS1307 rtc;

int alarmTime = 30;

////////////////////////////////////////
void setup () 
{
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();

    //define pin modes
  pinMode(switch_PM, INPUT);
  pinMode(switch_BST, INPUT);
  pinMode(clockPin, OUTPUT); 
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, INPUT);

  

  if (! rtc.isrunning()) 
  {
    Serial.println("RTC is NOT running!");
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
}

////////////////////////////////
void loop () 
{
  DateTime now = rtc.now(); 
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

  time_hour = (now.hour());

  digitalWrite(latchPin,1);
  delayMicroseconds(20);
  digitalWrite(latchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  alarmVar1 = shiftIn(dataPin, clockPin);
  alarmVar2 = shiftIn(dataPin, clockPin);

  //Serial.println(alarmVar1, BIN);
  //Serial.println(testA, BIN);
  //Serial.println(testB, BIN);

  for (int n=0; n <= 7; n++)
  {
    if (testA & (1 << n))
    {
      index = arrayLen[n] + 1;
    }
  }

  for (int n=0; n <= index; n++)
    {
      if (testB & (1 << n) )
      {
        //Serial.println("a");
        hourSet = arrayHrs[n];
        minSet = arrayMins[n];   // 0 here
        //Serial.println(hourSet);  //for debug         
        if (arrayFlag == 0)
        {
          //Serial.println("first");  //for debug
          alarmHour = hourSet;
          arrayFlag = 1;
        } 
        else
        {
          //Serial.println("second"); //for debug
          alarmMin = hourSet;          
          arrayFlag =0;
        }      
      }

    }
    Serial.println("start");
    Serial.println(alarmHour);
    Serial.println(alarmMin);
    Serial.println("end");
  //white space
  Serial.println("-------------------");
  //Serial.println("-------------------");
  //delay so all these print satements can keep up. 
  delay(2000);
}

  /////////////////////////////////////////////////////////////////////////

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
  int hrs = 0;
  int mins = 0;

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
        testA = myDataIn;
        switchState = 1;
        }
      else
        {
        pinState = 1;
        //set the bit to 0 no matter what
        myDataIn = myDataIn | (1 << i);
        //Serial.println("State 1 =");
        //Serial.println(myDataIn, BIN);
        testB = myDataIn;
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
  //debuging print statements whitespace
  //Serial.println();
  //Serial.println("output");
  //Serial.println(myDataIn, BIN);
  //return myDataIn;
  return testA;
  return testB;
}

////////////////////////////////////
/*To Do:
send both switchStates to loop
*/
