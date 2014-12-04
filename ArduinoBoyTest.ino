#include <LiquidCrystal.h>
#include <EEPROM.h>
LiquidCrystal lcd(40, 42, 22, 24, 26, 28);

byte statusByte;
byte channel;
byte pitch;
byte velocity;

String keyStroked[128] = 
{
  "C-1", "C#-1", "D-1", "D#-1", "E-1", "F-1", "F#-1", "G-1", "G#-1", "A-1", "A#-1", "B-1", 
  "C0", "C#0", "D0", "D#0", "E0", "F0", "F#0", "G0", "G#0", "A0", "A#0", "B0", 
  "C1", "C#1", "D1", "D#1", "E1", "F1", "F#1", "G1", "G#1", "A1", "A#1", "B1", 
  "C2", "C#2", "D2", "D#2", "E2", "F2", "F#2", "G2", "G#2", "A2", "A#2", "B2", 
  "C3", "C#3", "D3", "D#3", "E3", "F3", "F#3", "G3", "G#3", "A3", "A#3", "B3", 
  "C4", "C#4", "D4", "D#4", "E4", "F4", "F#4", "G4", "G#4", "A4", "A#4", "B4", 
  "C5", "C#5", "D5", "D#5", "E5", "F5", "F#5", "G5", "G#5", "A5", "A#5", "B5", 
  "C6", "C#6", "D6", "D#6", "E6", "F6", "F#6", "G6", "G#6", "A6", "A#6", "B6", 
  "C7", "C#7", "D7", "D#7", "E7", "F7", "F#7", "G7", "G#7", "A7", "A#7", "B7", 
  "C8", "C#8", "D8", "D#8", "E8", "F8", "F#8", "G8", "G#8", "A8", "A#8", "B8",
  "C9", "C#9", "D9", "D#9", "E9", "F9", "F#9", "G9"
};

long double baseNoteFreq = 16.351;
long double freqCtrl = 1.059435080074915;

float noteFreq[116];

int e_address = 0;
int t_index = 0;

unsigned long runTime;
unsigned long prevTime;

unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long keyDuration[512];

void setup()
{
  lcd.begin(16, 2);
  Serial.begin(31250);

  for(int i = 0; i <= 4096; i++)
  {
    if(EEPROM.read(i) == 0x00000000)
    {
      i = 4096;
    }
    else
    {
      EEPROM.write(i, 0);
    }
  }
  
  lcd.setCursor(0, 0);
  lcd.print("Clearing EEPROM!");
  lcd.clear();
  lcd.print("EEPROM whiped!");
  delay(3000);
  lcd.clear();
  
  pinMode(11, OUTPUT);
  pinMode(3, INPUT);

  for(int i = 12; i <= 128; i++)
  {
    if(i == 12)
    {
      noteFreq[i] = baseNoteFreq;
    }
    else
    {
      noteFreq[i] = noteFreq[i - 1] * freqCtrl;
    }
  }
}

void loop()
{
  if(digitalRead(3) != HIGH)
  {
    if(Serial.available() > 0)
    {
      Serial.println("==============================================");
      statusByte = Serial.read();
      Serial.flush();
      Serial.println(statusByte, BIN);

      pitch = Serial.read();
      Serial.flush();
      Serial.println(pitch, BIN);
      Serial.println(noteFreq[pitch]);

      velocity = Serial.read();
      Serial.flush();
      Serial.println(velocity, BIN);

      lcd.setCursor(0, 0);
      lcd.print(keyStroked[pitch]);
      lcd.setCursor(0, 1);
      lcd.print(noteFreq[pitch]);

      if(statusByte >= 0x90 && statusByte <= 0x9F)
      {
        channel = statusByte - 0x90 + 1;
        tone(7, noteFreq[pitch]);
        EEPROM.write(e_address, pitch);
        startTime = millis();
        e_address++;
      }
      if(statusByte >= 0x80 && statusByte <= 0x8F)
      {
        lcd.clear();
        channel = statusByte - 0x80 + 1;
        noTone(7);
        endTime = millis();
        keyDuration[t_index] = endTime - startTime;
        t_index++;
      }
    }
  }
  if(digitalRead(3) == HIGH)
  {
    for(int i = 0; i <= 512; i++)
    {
      runTime = millis();

      if(runTime - prevTime >= keyDuration[i]);
      {
        pitch = EEPROM.read(i);

        tone(7, noteFreq[pitch], keyDuration[i]);
        prevTime = runTime;
      }
    }
  }
}


































