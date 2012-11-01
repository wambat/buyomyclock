/*

 */

//Pin connected to latch pin (ST_CP) of 74HC595
#include "Arduino.h"
#include "Time.h"
#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 
void setup();
void loop();
void strobeDigitWrite(int digit,int num);
void digitWrite(int num);
byte numberToByte(int numToDisplay,boolean dp);
const int latchPin = 28;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 27;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 26;
const int digitsSize=4;
const int digits[]={22,23,24,25};


void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  for(int i=0;i<digitsSize;i++)
  {
    pinMode(digits[i],OUTPUT);
    digitalWrite(digits[i],LOW);
  }
  Serial.begin(9600);
  Serial.println("rese3t");
  setTime(1351728593);
}
int values[digitsSize]={0,0,0,0};
int currentVal=0;
void loop() {
  if (Serial.available() > 0) {
    // ASCII '0' through '9' characters are
    // represented by the values 48 through 57.
    // so if the user types a number from 0 through 9 in ASCII, 
    // you can subtract 48 to get the actual value:
    int num = Serial.read() - 48;
    Serial.print(">>>");
    values[currentVal]=num;
    currentVal++;
    if(currentVal>=digitsSize)
      currentVal=0;
    
    if(timeStatus()!= timeNotSet)   
    {
      Serial.print("time set");
      Serial.print(second(),3);
    }
    else
    {
      Serial.print("time not set");
    }
  // write to the shift register with the correct bit set high:
  }
  values[0]=minute()/10;
  values[1]=minute()%10;
  values[2]=second()/10;
  values[3]=second()%10;

  for(int i=0;i<digitsSize;i++)
  {
    strobeDigitWrite(i,values[i]);
  }
}
void strobeDigitWrite(int digit,int num)
{
  digitalWrite(digits[digit],HIGH);
  digitWrite(num);

  digitalWrite(digits[digit],LOW);
}
// This method sends bits to the shift register:

void digitWrite(int num) {
  // the bits you want to send
  byte bitsToSend = numberToByte(num,0);
  //Serial.print(bitsToSend,BIN);
  //Serial.print("\n");
  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);

  // turn on the next highest bit in bitsToSend:
  //bitWrite(bitsToSend, whichPin, whichState);

  // shift the bits out:
  shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend);

    // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);

}

byte numberToByte(int numToDisplay,boolean dp) 
{
    static boolean inversed=true;
    byte characters[]={B11111100,
                       B01100000,
                       B11011010,
                       B11110010,
                       B01100110,
                       B10110110,
                       B10111110,
                       B11100000,
                       B11111110,
                       B11110110,
                       B00000000};
    byte byteOut=characters[numToDisplay];
    byteOut|=dp;
      if(inversed==true)
    byteOut=~byteOut;
    return byteOut;
}


