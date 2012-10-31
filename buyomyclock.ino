/*
  Shift Register Example
 for 74HC595 shift register

 This sketch turns reads serial input and uses it to set the pins
 of a 74HC595 shift register.

 Hardware:
 * 74HC595 shift register attached to pins 2, 3, and 4 of the Arduino,
 as detailed below.
 * LEDs attached to each of the outputs of the shift register

 Created 22 May 2009
 Created 23 Mar 2010
 by Tom Igoe

 */

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 28;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 27;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 26;

const int digit0=22;
const int digit1=23;
const int digit2=24;
const int digit3=25;


void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(digit0, OUTPUT);
  pinMode(digit1, OUTPUT);
  pinMode(digit2, OUTPUT);
  pinMode(digit3, OUTPUT);
  digitalWrite(digit0,HIGH);
  digitalWrite(digit1,HIGH);
  digitalWrite(digit2,HIGH);
  digitalWrite(digit3,HIGH);
  
  
  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  if (Serial.available() > 0) {
    // ASCII '0' through '9' characters are
    // represented by the values 48 through 57.
    // so if the user types a number from 0 through 9 in ASCII, 
    // you can subtract 48 to get the actual value:
    int num = Serial.read() - 48;
  // write to the shift register with the correct bit set high:
    digitWrite(num);
  }
}

// This method sends bits to the shift register:

void digitWrite(int num) {
  // the bits you want to send
  byte bitsToSend = numberToByte(num,0);
  Serial.print(bitsToSend,BIN);
  Serial.print("\n");
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
    byte byteOut = 0;
    switch(numToDisplay)
    {
      case 0:
        byteOut=B11111100;
        break;
  
      case 1:
        byteOut=B01100000;
        break;
  
      case 2:
        byteOut=B11011010;
        break;
  
      case 3:
        byteOut=B11110010;
        break;
  
      case 4:
        byteOut=B01100110;
        break;
  
      case 5:
        byteOut=B10110110;
        break;
  
      case 6:
        byteOut=B10111110;
        break;
  
      case 7:
        byteOut=B11100000;
          break;
  
      case 8:
        byteOut=B11111110;
        break;
  
      case 9:
        byteOut=B11110110;
        break;
  
      case 10:
        byteOut=B00000000;
        break;
    }
    if(inversed==true)
      byteOut=~byteOut;
    //byteOut&=dp;
    return byteOut;
}

