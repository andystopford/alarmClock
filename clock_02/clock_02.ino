//****************************************************************
//  Name    : alarmClock                               
//  Author  : A.S. (from Carlyn Maw)                                        
//  Date    : 2 Jan, 2015                                      
//  Version : 1.0                                               
//  Notes   : Code for using a CD4021B Shift Register            
//          : This works, identifies each pin correctly
//          : and would be OK with 4 shift regs but is waste-
//          : ful leaving 4 pins of each sr unused

//Currently connected for hours only, press button to set to PM
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

//variables to hold shift register data
byte hoursVar1 = 72;  //01001000
byte hoursVar2 = 159; //10011111
byte minsVar1 = 72;  //01001000
byte minsVar2 = 159; //10011111

//define arrays that corresponds to values for each 
//of the shift register's pins
int hrs_array1[] = {
  1, 2, 3, 4, 5, 6, 7, 8}; 
  int hrs_array2[] = {
  9, 10, 11, 12, '@', '@', '@', '@'};  //@ char returns int 64
  int mins_array1[] = {
    0, 5, 10, 15, 20, 25, 30, 35}; 
  int mins_array2[] = {
    40, 45, 50, 55, '@', '@', '@', '@'};

//Initialize time values
int hourSet = 0;
int minSet = 0;
int time_hour;


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

  //Pulse the latch pin:
  //set it to 1 to collect parallel data
  digitalWrite(latchPin,1);
  //set it to 1 to collect parallel data, wait
  delayMicroseconds(20);
  //set it to 0 to transmit data serially  
  digitalWrite(latchPin,0);

  //while the shift register is in serial mode
  //collect each shift register into a byte
  //the register attached to the chip comes in first 
  hoursVar1 = shiftIn(dataPin, clockPin);
  hoursVar2 = shiftIn(dataPin, clockPin);
  

  //This for-loop steps through the byte
  //bit by bit which holds the shift register data 
  //and if it was high (1) then it prints
  //the corresponding location in the array
  for (int n=0; n<=7; n++)
  {
    //so, when n is 3, it compares the bits
    //in switchVar1 and the binary number 00001000
    //which will only return true if there is a 
    //1 in that bit (ie that pin) from the shift
    //register.
    if (hoursVar1 & (1 << n) )
    {
      hourSet = hrs_array1[n]; 
      Serial.println(hourSet);  
    }
    if (hoursVar2 & (1 << n) )
    {
      hourSet = hrs_array2[n];
      Serial.println(hourSet);
    }
  }
  
  //Check if alarm is set for am or pm
  if (digitalRead(switch_PM) == HIGH)
    {
      hourSet += 12;
    }

  Serial.println(hourSet);

  if (time_hour == hourSet)
    {
      Serial.println("Alarm");
      tone(10, 500, 500);
    }



  //white space
  Serial.println("-------------------");
  //delay so all these print satements can keep up. 
  delay(1000);
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
  byte myDataIn = 0;

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
      pinState = 1;
      //set the bit to 0 no matter what
      myDataIn = myDataIn | (1 << i);
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
  //Serial.println(myDataIn, BIN);
  return myDataIn;
}
