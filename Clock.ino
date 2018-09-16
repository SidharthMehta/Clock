//Paul Stoffregen's modified TimerOne
#include <TimerOne.h>
#include <SoftwareSerial.h>

const int RX = 8;
const int TX = 9;

const int dataIn    = 12;
const int CS        = 10;
const int clock     = 11;

const int Buzzer = 13;

volatile int Hours   = 0;
volatile int Minutes = 0;
volatile int Seconds = 0;

int HoursAlarm   = 0;
int MinutesAlarm = 0;
int SecondsAlarm = 0;
int flag         = 0;
String AlarmString = "";

SoftwareSerial bluetooth(RX,TX);

// define max7219 registers
byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data) 
{
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( clock, LOW);   // tick
    if (data & mask){            // choose bit
      digitalWrite(dataIn, HIGH);// send 1
    }else{
      digitalWrite(dataIn, LOW); // send 0
    }
    digitalWrite(clock, HIGH);   // tock
    --i;                         // move to lesser bit
  }
}

void maxSingle( byte reg, byte col) {    

  digitalWrite(CS, LOW);       // begin     
  putByte(reg);                  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data   
  digitalWrite(CS, LOW);       // and CS da stuff
  digitalWrite(CS,HIGH); 
}

void setup () {

  pinMode(dataIn, OUTPUT);
  pinMode(clock,  OUTPUT);
  pinMode(CS,   OUTPUT);
  pinMode(Buzzer,  OUTPUT);
  
  digitalWrite(Buzzer, LOW);  

  bluetooth.begin(9600);
  
  //initiation of the max 7219
  maxSingle(max7219_reg_scanLimit, 0x05);      
  maxSingle(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxSingle(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxSingle(max7219_reg_displayTest, 0x00); // no display test
  for (int e=1; e<=6; e++) {    // empty registers, turn all LEDs off 
    maxSingle(e,0);
  }
  maxSingle(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f
  Timer1.initialize(1000000);
  Timer1.attachInterrupt( timerIsr );
  
}  

void loop () 
{
  Display(6,Hours/10);
  Display(5,Hours%10);
  Display(4,Minutes/10);
  Display(3,Minutes%10);
  Display(2,Seconds/10);
  Display(1,Seconds%10);
  
  while(bluetooth.available()>0)
  {
   int inChar = bluetooth.read();
     if (isDigit(inChar))
       AlarmString += (char)inChar;
   flag++;
  }
  if(flag == 7)
  {
    Serial.println(AlarmString);
    long int Alarm = AlarmString.toInt();
    Serial.println(Alarm);
    HoursAlarm = Alarm/10000;
    MinutesAlarm = (Alarm%10000)/100;
    SecondsAlarm = Alarm%100;
    Serial.print(HoursAlarm);
    Serial.print(":");
    Serial.print(MinutesAlarm);
    Serial.print(":");
    Serial.println(SecondsAlarm);
    flag = 0;
    AlarmString ="";
  }
  
  if(HoursAlarm==Hours && MinutesAlarm==Minutes && SecondsAlarm==Seconds)
  {
    digitalWrite(Buzzer,HIGH);
  }
  else if(HoursAlarm==Hours && MinutesAlarm==Minutes && SecondsAlarm==Seconds)
  {
    digitalWrite(Buzzer,LOW);
  }
  
}

void clean(int i)
{
  maxSingle(i,0);
}

void One(int i)
{
  maxSingle(i,B01100000);
}

void Two(int i)
{
  maxSingle(i,B11011010);
}

void Three(int i)
{
  maxSingle(i,B11110010);
}

void Four(int i)
{
  maxSingle(i,B01100110);
}

void Five(int i)
{
  maxSingle(i,B10110110);
}

void Six(int i)
{
  maxSingle(i,B10111110);
}

void Seven(int i)
{
  maxSingle(i,B11100000);
}

void Eight(int i)
{
  maxSingle(i,B11111110);
}

void Nine(int i)
{
  maxSingle(i,B11100110);
}

void Zero(int i)
{
  maxSingle(i,B11110000);
}

void Display(int i,int z)
{
  clean(i);
  switch(z)
  {
     case 1: 
       One(i);
       break;
     case 2:
       Two(i);
       break;
     case 3:
       Three(i);
       break;
     case 4:
       Four(i);
       break;
     case 5:
       Five(i);
       break;
     case 6:
       Six(i);
       break;
     case 7:
       Seven(i);
       break;
     case 8:
       Eight(i);
       break;
     case 9:
       Nine(i);
       break;
     case 0:
       Zero(i);
       break;
  } 
}

void timerIsr()
{
    Seconds++;
    if(Seconds == 60)
    {
      Minutes++;
      Seconds = 0;
      if(Minutes == 60)
      {
        Hours++;
        Minutes = 0;
        if(Hours = 24)
        {
          Hours = 0;
        }
      }
    }
}
