/*

 */

//Pin connected to latch pin (ST_CP) of 74HC595
#include "Arduino.h"
#include "Time.h"
#include "TimeAlarms.h"
#include "pitches.h"
// notes in the melody:
int startMelody[] = {
  NOTE_C4,4,
  NOTE_G3,8,
  NOTE_G3,8,
  NOTE_A3,4,
  NOTE_G3,4,
  0,4,
  NOTE_B3,4,
  NOTE_C4,4};
int startMelodySize=8;
// notes in the melody:
//G3 G3 G3 D#3 A#4 G3 D#3 A#4 G3 D4 D4 D4 D#4 A#4 F#3 D#3 A#4 G3
int pauseMelody[] = {\
  NOTE_G3,4,\
  NOTE_G3,4,\
  NOTE_G3,4,\
  NOTE_DS3,4,\
  NOTE_AS4,8,\
  NOTE_G3,8,\
  NOTE_DS3,4,\
  NOTE_AS4,8,\
  NOTE_G3,8,\
  0,4,\
  NOTE_D4,4,\
  NOTE_D4,4,\
  NOTE_D4,4,\
  NOTE_DS4,4,\
  NOTE_AS4,8,\
  NOTE_FS4,8,\
  NOTE_DS3,4,\
  NOTE_AS4,8,\
  NOTE_G3,8};
int pauseMelodySize=20;

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

void setup();
void loop();

void strobeDigitWrite(int digit,int num);
void digitWrite(int num);
byte numberToByte(int numToDisplay,boolean dp);
const int latchPin = 32;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 31;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = 30;
const int tonePin = 37;

const int pauseButtonPin = 36;
const int digitsSize=4;
const int digits[]={22,23,24,25};

int countdownmax=59*60+59;
boolean pauseButtonState;
boolean pauseButtonLast;
boolean pauseButtonDebouncer;

int bms=10;
int bmstime=10;
int countdown1;
int countdown2;
int bm1,bm2,bmc1,bmc2;
int turn;


long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers


void setup() {
  //set pins to output because they are addressed in the main loop
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  pinMode(pauseButtonPin, INPUT);
  digitalWrite(pauseButtonPin, HIGH);
  for(int i=0;i<digitsSize;i++)
  {
    pinMode(digits[i],OUTPUT);
    digitalWrite(digits[i],LOW);
  }
  Serial.begin(9600);
  Serial.println("rese3t");
  setTime(1351728593);
  countdown1=countdownmax;
  countdown2=countdownmax;
  bm1=bms;
  bm2=bms;
  bmc1=bmstime;
  bmc2=bmstime;
  turn=0;
  pauseButtonState=LOW;
  pauseButtonDebouncer=HIGH;
  pauseButtonLast=LOW;
  playMelody(startMelody,startMelodySize);
  Alarm.timerRepeat(1, ticktack);            // timer for every 1 seconds
  setupRefreshTimer();
  //
}
int values1[digitsSize]={0,0,0,0};
int values2[digitsSize]={0,0,0,0};
int currentVal=0;

void loop() {
  if (Serial.available() > 0) {
    // ASCII '0' through '9' characters are
    // represented by the values 48 through 57.
    // so if the user types a number from 0 through 9 in ASCII, 
    // you can subtract 48 to get the actual value:
    int num = Serial.read() - 48;
    values1[currentVal]=num;
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
  debounceReadPause();
  Alarm.delay(10);
  
}
void refreshDisplay()
{
  for(int i=0;i<digitsSize;i++)
  {
    strobeDigitWrite(i,values2[i],values1[i]);
  }
}
void ticktack()
{
  countdown1--;
  makeValuesOnMain();
  Serial.print("tick");
}
void makeValuesOnMain()
{
  int mins=countdown1/60;
  int secs=countdown1%(mins*60);
  values1[0]=mins/10;
  values1[1]=mins%10;
  values1[2]=secs/10;
  values1[3]=secs%10;
}
void strobeDigitWrite(int digit,int num1,int num2)
{  
  digitWrite(num1);
  digitWrite(num2);
  
  digitalWrite(digits[digit],HIGH);
  delayMicroseconds(250);
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

void playMelody(int* notes,int notesSize) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < notesSize; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/notes[thisNote*2+1];
    tone(tonePin, notes[thisNote*2],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    Alarm.delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(tonePin);
  }
}

void debounceReadPause()
{
  // read the state of the switch into a local variable:
  boolean reading = digitalRead(pauseButtonPin);
  digitalWrite(13,pauseButtonDebouncer);
    // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != pauseButtonDebouncer) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  } 
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
     if(pauseButtonLast<pauseButtonDebouncer)
     {  
       onPauseButton();
       pauseButtonLast=pauseButtonDebouncer
     }
  }
  
  
  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  pauseButtonDebouncer = reading;
}
void onPauseButton()
{
  pauseButtonState=!pauseButtonState;
  if(pauseButtonState==HIGH)
    Serial.print("PAUSE");
  if(pauseButtonState==LOW)
    Serial.print("RESUME");
}
void playPauseMelody()
{
  playMelody(pauseMelody,pauseMelodySize);
}
void setupRefreshTimer()
{
   // initialize Timer1
    cli();             // disable global interrupts
    TCCR1A = 0;        // set entire TCCR1A register to 0
    TCCR1B = 0;
 
    // enable Timer1 overflow interrupt:
    TIMSK1 = (1 << TOIE1);
    // Set CS10 bit so timer runs at clock speed:
    TCCR1B |= (1 << CS10);
    // enable global interrupts:
    sei();
}
ISR(TIMER1_OVF_vect)
{
    refreshDisplay();
}

