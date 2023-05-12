// This work is licensed under a Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
// by IW1QLH

// Arduino Nano - ATmega328

#include <LiquidCrystal.h>
#include "CwTrainer.h"

// HACK-1: paddle - connect jack pin RING to Arduino pin D9
#define HACK-1

// HACK-40: 15 up to 40WPM
#define HACK-40


const PROGMEM byte charSet[] = {
  B0110, 0,     // A
  B10010101, 0, // B
  B10011001, 0, // C
  B100101, 0,   // D
  B01, 0,       // E
  B01011001, 0, // F
  B101001, 0,   // G
  B01010101, 0, // H
  B0101, 0,     // I
  B01101010, 0, // J
  B100110, 0,   // K
  B01100101, 0, // L
  B1010, 0,     // M
  B1001, 0,     // N
  B101010, 0,   // O
  B01101001, 0, // P
  B10100110, 0, // Q
  B011001, 0,   // R
  B010101, 0,   // S
  B10, 0,       // T
  B010110, 0,   // U
  B01010110, 0, // V
  B011010, 0,   // W
  B10010110, 0, // X
  B10011010, 0, // Y
  B10100101, 0, // Z

  B10101010, B10, // 0
  B10101010, B01, // 1
  B01101010, B01, // 2
  B01011010, B01, // 3
  B01010110, B01, // 4
  B01010101, B01, // 5
  B01010101, B10, // 6
  B10010101, B10, // 7
  B10100101, B10, // 8
  B10101001, B10  // 9
};

#define space1c 0
#define space1w 1
#define space1s 2
#define space2s 3
#define space3s 4
const char string_0[] PROGMEM = "1CHAR";
const char string_1[] PROGMEM = "1WORD";
const char string_2[] PROGMEM = "1SEC";
const char string_3[] PROGMEM = "2SEC";
const char string_4[] PROGMEM = "3SEC";
const char* const string_table[] PROGMEM = {string_0, string_1, string_2, string_3, string_4};

const char stringOnlyChars[] PROGMEM = "LETTERS";
const char stringOnlyNumbers[] PROGMEM = "NUMBERS";
const char stringMixed[] PROGMEM = "MIXED";
const char stringCallsign[] PROGMEM = "CALLSIGN";

const char callsign_iz[] PROGMEM = "IZ#...";
const char callsign_ik[] PROGMEM = "IK#...";
const char callsign_i[] PROGMEM = "I#...";
const char callsign_iw[] PROGMEM = "IW#...";
const char callsign_iu[] PROGMEM = "IU#...";
const char callsign_ea[] PROGMEM = "EA#...";
const char callsign_dl[] PROGMEM = "DL#...";
const char callsign_f[] PROGMEM = "F#...";
const char callsign_sp[] PROGMEM = "SP#...";
const char callsign_lz[] PROGMEM = "LZ#...";
const char callsign_ha[] PROGMEM = "HA#...";
const char callsign_ok[] PROGMEM = "OK#...";
const char callsign_yt[] PROGMEM = "YT#...";
const char callsign_it9[] PROGMEM = "IT9...";
const char callsign_hb9[] PROGMEM = "HB9...";
const char callsign_tk[] PROGMEM = "TK#...";
const char callsign_9a[] PROGMEM = "9A#...";
const char callsign_IW1QLH[] PROGMEM = "IW1QLH";
const char callsign_E51DWC[] PROGMEM = "E51DWC";
const char callsign_FW5JJ[] PROGMEM = "FW5JJ";
const char callsign_5E2E[] PROGMEM = "5E2E";
const char callsign_S79HN[] PROGMEM = "S79HN";
const char callsign_VK100ANZAC[] PROGMEM = "VK100ANZAC";
const char callsign_MC0RYC[] PROGMEM = "MC0RYC";

const Callsign callsign[] PROGMEM = {
  {callsign_IW1QLH,1}, {callsign_E51DWC,1}, {callsign_FW5JJ,1}, {callsign_5E2E,1}, {callsign_S79HN,1}, {callsign_VK100ANZAC,1}, {callsign_MC0RYC,1},
  {callsign_iz,690}, {callsign_ik,500}, {callsign_i,310}, {callsign_iw,250}, {callsign_iu,30},
  {callsign_ea,190}, {callsign_dl,130}, {callsign_f,110}, {callsign_sp,30}, {callsign_lz,30}, {callsign_ha,20},
  {callsign_ok,30}, {callsign_yt,20}, {callsign_it9,60}, {callsign_hb9,60}, {callsign_tk,10}, {callsign_9a,70},
  {0,0} };

#define CALLBUFF_SIZE 10  
char callBuff[CALLBUFF_SIZE];
int totCallScore;

const char revStr[] PROGMEM = "IW1QLH 0.5";


// DIGITALI
// 0 -> RX
// 1 -> TX
#define LCD_D7 2
#define LCD_D6 3
#define LCD_D5 4
#define LCD_D4 5
#define KEY_MODE 6
#define CW_KEY 7
#define AUDIO_OUT 8
#define PADDLE_CW 9
#define LCD_EN 11
#define LCD_RS 12

// ANALOGICI
#define CW_SPEED 0
#define CW_SPACE 1
#define VCC 2

#define LCD_SIZE 16
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

modes mode = mode_only_letters;
states state = state_waiting;

#define MAX_TICK 50
int tick = MAX_TICK;
int old_an_speed = 32767;
int old_an_space = 32767;
int cw_speed = 5;
int cw_space = 0;
int msDot = 120;
int nGroup = 0;
bool isPaddle = false;

int oldKeyMode = -1;
int oldCwKey = -1;

int value;
char buffer[16];
char exChars[LCD_SIZE];

#define SPEED_SIZE 32
int speedBuff[SPEED_SIZE];

//#define DEBUG
void debug(char txt[]) {
  #ifdef DEBUG
    Serial.print(txt);
  #endif
}

unsigned long keyTime;
unsigned long keySpan;
word keyChar;
int paddle;

void setup() {
  // put your setup code here, to run once:
  double vbatt;
  String vbattStr;

  pinMode(KEY_MODE, INPUT_PULLUP);
  pinMode(CW_KEY, INPUT_PULLUP);
  pinMode(AUDIO_OUT, OUTPUT);
  pinMode(PADDLE_CW, INPUT_PULLUP);
  
  randomSeed(analogRead(CW_SPEED) + analogRead(CW_SPACE) + analogRead(VCC) + analogRead(3));
  
  lcd.begin(LCD_SIZE, 2);
  lcd.clear();
  Serial.begin(9600);
  
  strcpy_P(buffer, revStr);
  lcd.print(buffer);
  #ifdef HACK-1
    lcd.print("H1");
  #endif
  #ifdef HACK-40
    lcd.print("H40");
  #endif
  Serial.print(buffer);
  Serial.println(" CW Trainer");
  
  value = analogRead(VCC);
  vbatt = 0.7 + value * 0.011894;
  vbattStr = String(vbatt, 1);
  lcd.setCursor(0, 1);
  lcd.print(vbattStr);
  lcd.print("V");

  for (int i = 0; i < 36; i++) {
    keyChar = pgm_read_word_near(charSet + i * 2);
    #ifdef DEBUG
      Serial.print(intToChar(i));
      Serial.print("=");
      Serial.print(keyChar, BIN);
      Serial.println();
    #endif
  }
  
  callBuff[0] = -1;
  totCallScore = 0;
  int i = 0;
  do {
    value = pgm_read_word(&callsign[i].score);
    totCallScore += value;
    i++;
  } while (value > 0);

  // CW_KEY (tip) PADDLE_CW (ring)
  // HIGH         HIGH        no key
  // HIGH         LOW         key connected
  // HIGH         HIGH        paddle connected
  // LOW          LOW         key pressed (key or paddle)

  delay(1000);

}

bool readCwParams() {
  bool changed = false;
  
  value = analogRead(CW_SPEED);
  if (abs(value - old_an_speed) > 50) {
    old_an_speed = value;
    #ifdef HACK-40
      cw_speed = 15 + value / 40;
    #else
      cw_speed = 5 + value / 65;
    #endif
    msDot = 1200 / cw_speed;
    changed = true;
  }
  
  value = analogRead(CW_SPACE);
  if (abs(value - old_an_space) > 50) {
    old_an_space = value;
    cw_space = value / 205;
    changed = true;
  }
  
  return changed;
}

void displaySettings() {
  lcd.setCursor(0, 1);
  lcd.print(cw_speed);
  lcd.print("WPM ");
  if (state != state_replay) {
    strcpy_P(buffer, (char*)pgm_read_word(&(string_table[cw_space])));
    lcd.print(buffer);
    lcd.print(" ");
    switch (mode) {
      case mode_only_letters:
        strcpy_P(buffer, stringOnlyChars);
        break;
      case mode_only_numbers:
        strcpy_P(buffer, stringOnlyNumbers);
        break;
      case mode_mixed:
        strcpy_P(buffer, stringMixed);
        break;
      case mode_callsign:
        strcpy_P(buffer, stringCallsign);
        break;      
    }
    lcd.print(buffer);
    lcd.print(" ");
    lcd.print(tick / 10);
  }
}

charTypes getCharType(int ch) {
  if (ch <= 25)
    return type_letter;
  if ((ch >= 26) && (ch <= 35))
    return type_number;
  return type_unknow;
}

char intToChar(int ch) {
  switch (getCharType(ch)) {
    case type_letter:
      return char(ch + 65);
      break;
    case type_number:
      return char(ch + 48 - 26);
      break;
  }
  return ' ';
}

int charToInt(char ch) {
  if ((ch >= 65) && (ch <= 90))
    return ch - 65;
  if ((ch >= 48) && (ch <= 57))
    return ch - 48 + 26;
  return 0;
}

void clearScroll() {
  for (int i = 0; i < LCD_SIZE; i++)
    exChars[i] = ' ';
}

void scrollLcd(char c) {
  for (int i = 0; i < LCD_SIZE - 1; i++)
    exChars[i] = exChars[i + 1];
  exChars[LCD_SIZE - 1] = c;
  lcd.setCursor(0, 0);
  lcd.write(exChars);
  Serial.print(c);
}

void play(int ch) {
  word cw;
  word p;
  cw = pgm_read_word_near(charSet + ch * 2);
  //lcd.print(cw);
  while (cw != 0) {
    p = cw & word(B11000000, 00000000);
    if (p == word(B01000000, 00000000)) {  // dot
      tone(AUDIO_OUT, 800);
      delay(msDot);
      noTone(AUDIO_OUT);
      delay(msDot);
    }
    if (p == word(B10000000, 00000000)) {  // line
      tone(AUDIO_OUT, 800);
      delay(msDot * 3);
      noTone(AUDIO_OUT);
      delay(msDot);
    }
    cw = cw << 2;
  }
  
  switch (cw_space) {
    case space1c:
      delay(msDot * 2);   // 1 * msDot già fatto
      break;
    case space1w:
      delay(msDot * 6);   // 1 * msDot già fatto
      break;
    case space1s:
      delay(1000);
      break;
    case space2s:
      delay(2000);
      break;
    case space3s:
      delay(3000);
      break;
  }
  nGroup++;
  scrollLcd(intToChar(ch));
  
  if ((cw_space == space1c) && (mode != mode_callsign) && (nGroup >= 5)) {
    nGroup = 0;
    delay(msDot * 7);
    scrollLcd(' ');
  }
  if (cw_space != space1c)
    scrollLcd(' ');
}

void setState(states newState) {
  noTone(AUDIO_OUT);
  lcd.clear();
  lcd.setCursor(0, 0);
  switch (newState) {
    case state_waiting:
      lcd.print("Mimmo emulator");
      tick = MAX_TICK;
      //displaySettings();
      break;
    case state_running:
      lcd.print("Running");
      clearScroll();
      nGroup = 0;
      //displaySettings();
      break;
    case state_replay:
      lcd.print("Replay ");
      clearScroll();
      for (int i = 0; i < SPEED_SIZE; i++)
        speedBuff[i] = msDot;
      lcd.setCursor(0, 1);
      lcd.print("Pse release key ...");
      while (digitalRead(CW_KEY) == LOW) {}
      delay(200);
      #ifdef HACK-1
        isPaddle = (digitalRead(PADDLE_CW) == HIGH);
      #endif
      lcd.clear();
      lcd.print(isPaddle ? "Paddle" : "Key");
      delay(200);
      keyChar = 0;
      paddle = 0;
      break;
  }
  state = newState;
  displaySettings();
  
}

void showKeyed() {
  word cw;
  
  if (keyChar == 0)
    return;
  //Serial.println();
  //Serial.println(keyChar, BIN);
  for (int i = 0; i < 36; i++) {
    cw = pgm_read_word_near(charSet + i * 2);
    if (cw == keyChar) {
      //Serial.print(">");
      scrollLcd(intToChar(i));
      keyChar = 0;
      break;
    }
  }
  if (keyChar != 0)
    scrollLcd('?');
  keyChar = 0;
}

void setSpeed() {
  int totSpeed = 0;
  
  for (int i = 0; i < SPEED_SIZE - 1; i++) {
    value = speedBuff[i + 1];
    speedBuff[i] = value;
    totSpeed += value;
  }
  speedBuff[SPEED_SIZE - 1] = keySpan;
  totSpeed += keySpan;
  msDot = totSpeed / SPEED_SIZE;
  lcd.setCursor(0, 1);
  lcd.print(1200 / msDot);
  lcd.print("WPM ");
}

int delayPaddle(unsigned long ms, int skip) {
  int result = 0;
  bool anyPressed = true;
  unsigned long tt;
  //Serial.println();
  //Serial.print("delayPaddle ");
  tt = millis() + ms - 5;
  while (millis() < tt) {
    if ((digitalRead(CW_KEY) == LOW) && (skip != 1))
      result = 1;
    if ((digitalRead(PADDLE_CW) == LOW) && (skip != -1))
      result = -1;
    if ((result != 0) && (skip == 0))
      break;
  }
  //Serial.print(result);
  return result;
}

/*
int checkPaddle() {
  unsigned long tt;
  int dot = 0;
  int line = 0;
  int space = 0;
  Serial.println(); Serial.print("checkPaddle ");
  tt = millis() + msDot;
  while (millis() < tt) {
    if (digitalRead(CW_KEY) == LOW)
      dot++;
    else if (digitalRead(PADDLE_CW) == LOW)
      line++;
    else space++;
  }
  Serial.print("s="); Serial.print(space); Serial.print(" d="); Serial.print(dot); Serial.print(" l="); Serial.print(line);
  if ((space > dot) && (space > line))
    return 0;
  if (dot >= line)
    return 1;
  return -1;
}
*/

void loop() {
  // put your main code here, to run repeatedly:
  charTypes ct;
  
  if (readCwParams()) {
    displaySettings();
  }
   
  switch (state) {
    case state_waiting:
      value = digitalRead(KEY_MODE);
      if (value != oldKeyMode) {
        oldKeyMode = value;
        if (value == LOW) {
          value = (int)mode + 1;
          if (value > 3)
            value = 0;
          mode = modes(value);
          tone(AUDIO_OUT, 500, 10);
          Serial.println();
          tick = MAX_TICK;
          displaySettings();
        }
      }
      
      value = digitalRead(CW_KEY);
      if (value != oldCwKey) {
        oldCwKey = value;
        if (value == LOW) {
          tone(AUDIO_OUT, 500);
          delay(10);
          noTone(AUDIO_OUT);
          tick = 0;
          setState(state_replay);
        }
      }

      if (state == state_waiting) {
        tick--;
        if (tick < 0) {
          setState(state_running);
        }
        delay(100);
      }
      break;

    case state_running:
      switch (mode) {
        case mode_only_letters:
        case mode_only_numbers:
        case mode_mixed:
          do {
            value = random(36);
            ct = getCharType(value);
          } while (!((mode == mode_mixed) || ((mode == mode_only_letters) && (ct == type_letter)) || ((mode == mode_only_numbers) && (ct == type_number))));
          break;
        case mode_callsign:
          if (callBuff[0] == -1) {

            callBuff[0] = charToInt('I');
            callBuff[1] = charToInt('W');
            callBuff[2] = charToInt('1');
            callBuff[3] = charToInt('Q');
            callBuff[4] = charToInt('L');
            callBuff[5] = charToInt('H');
            callBuff[6] = -1; 
                        
            int rnd = random(totCallScore);
            #ifdef DEBUG
              Serial.print(">rnd=");
              Serial.println(rnd);
            #endif
            int i = 0;
            int prog = 0;
            do {
              value = pgm_read_word(&callsign[i].score);
              #ifdef DEBUG
                Serial.print("score=");
                Serial.println(value);
              #endif
              prog += value;
              if (prog > rnd) {
                strcpy_P(callBuff, (char*)pgm_read_word(&(callsign[i].prefix)));
                #ifdef DEBUG
                  Serial.println(callBuff);
                #endif
                for (i = 0; i < CALLBUFF_SIZE; i++) {
                  switch (callBuff[i]) {
                    case 0:
                      callBuff[i] = -1;
                      break;
                    case '#':
                      callBuff[i] = random(10) + 26;
                      break;
                    case '.':
                      callBuff[i] = random(26);
                      break;
                    default:
                      callBuff[i] = charToInt(callBuff[i]);
                  }
                }
                break;
              }
              i++;
            } while (value > 0);            
    
            delay(msDot * 7);
            scrollLcd(' ');
          }
          value = callBuff[0];
          for (int i = 0; i < CALLBUFF_SIZE - 1; i++)
            callBuff[i] = callBuff[i + 1];
          break;
      }
      
      play(value);
      if ((digitalRead(KEY_MODE) == LOW) || (digitalRead(CW_KEY) == LOW))
        setState(state_waiting);
      break;
      
    case state_replay:
      // PADDLE -------------------------
      if (isPaddle) {
        //Serial.print("<"); Serial.print(paddle); Serial.print(">");
        if (paddle == 0) {
          if (tick > 5)
            paddle = delayPaddle(msDot, 0);
          else
            paddle = delayPaddle(msDot, 99);
        } else
          delay(msDot);
        if (paddle == 1) {
          //Serial.print(".");
          tick = 0;
          keyChar = keyChar << 2;
          keyChar = keyChar | B01;  // dot
          tone(AUDIO_OUT, 800);
          paddle = delayPaddle(msDot, 1);
          noTone(AUDIO_OUT);
        } else if (paddle == -1) {
          //Serial.print("-");
          tick = 0;
          keyChar = keyChar << 2;
          keyChar = keyChar | B10;  // line
          tone(AUDIO_OUT, 800);
          paddle = delayPaddle(3 * msDot, -1);
          noTone(AUDIO_OUT);
        } else {
          tick++;
        }
        if (tick == 3) {
          showKeyed();
        }
        if (tick == 7) {
          scrollLcd(' ');
        }
        if (tick * msDot >= 5000)
          setState(state_waiting);

      } else {
        // KEY ----------------------------
        if (digitalRead(CW_KEY) == LOW) {
          if (oldCwKey != LOW) {
            oldCwKey = LOW;
            keySpan = millis() - keyTime;
            keyTime = millis();
            if (keySpan < (msDot * 2)) {
              #ifdef DEBUG
                Serial.print("S=");
                Serial.println(keySpan);
              #endif
              setSpeed();
            }
            if (keySpan > (msDot * 5)) {
              scrollLcd(' ');
            }
          }
          tone(AUDIO_OUT, 800);
          tick = 0;
        }
        else {
          keySpan = millis() - keyTime;
          if (oldCwKey != HIGH) {
            oldCwKey = HIGH;
            keyTime = millis();
            keyChar = keyChar << 2;
            if (keySpan < (msDot * 2)) {
              #ifdef DEBUG
                Serial.print("D=");
                Serial.println(keySpan);
              #endif
              setSpeed();
              keyChar = keyChar | B01;  // dot
            }
            else
              keyChar = keyChar | B10;  // line
          }
          else
          {
            if (keySpan > (msDot * 2)) {
              showKeyed();
            }
          }
          noTone(AUDIO_OUT);
          tick++;
          if (tick >= 1000) {
            setState(state_waiting);
          }
        }
        delay(5);
        
      }
      break;
  }
  
  //displayInt = pgm_read_word_near(charSet + 0);

}


