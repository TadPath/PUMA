// Serial terminal display (VT100)
// 30x15 screen with 8x16 font
// ST7789 & RRE library example
// (c) 2019 Pawel A. Hernik
// YT video: https://youtu.be/Z4PMT5wUhMw
// Use case: https://youtu.be/a1vxOkcO5Fw

/*
 ST7789 240x240 IPS (without CS pin) connections (only 6 wires required):

 #01 GND -> GND
 #02 VCC -> VCC (3.3V only!)
 #03 SCL -> D13/SCK
 #04 SDA -> D11/MOSI
 #05 RES -> D8 or any digital
 #06 DC  -> D7 or any digital
 #07 BLK -> NC
*/

#define TFT_DC    7
#define TFT_RST   8 
#define SCR_WD   240
#define SCR_HT   320   // 320 - to allow access to full 240x320 frame buffer
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

#include "RREFont.h"
#include "rre_fjg_8x16.h"

RREFont font;

// needed for RREFont library initialization, define your fillRect
void customRect(int x, int y, int w, int h, int c) { return lcd.fillRect(x, y, w, h, c); }

int maxy = 320; // internal ST7789 fb is 240x320
int xp = 0, yp = 240;
int ys = 0;
uint16_t bg = BLACK, fg = WHITE;
int screenWd = 240, screenHt = 240;
int charWd, charHt;
int wrap = 0;  // 1 - for wrapping long lines (>30 characters)
int sx = 1, sy = 1;

int escMode = 0;
int nVals = 0;
int vals[10]={0};

uint16_t c0 = RGBto565(20,20,20);
uint16_t c1 = RGBto565(40,40,40);

void scrollScreen()
{
  xp = 0;
  ys += charHt*sy;
  if(ys>=maxy) ys-=maxy;
  yp = ys+screenHt;
  if(yp>=maxy) yp-=maxy;
  lcd.fillRect(0, yp, screenWd, charHt*sy, ((yp/charHt)&1) ? c1 : c0);
  lcd.setScroll(ys);
}

void printString(char *str)
{
  while(*str) printTermChar(*str++);
}

void printTermChar(char c)
{
  static const uint16_t colors[] = {
    0x0000, // 0-black
    0xf800, // 1-red
    0x07e0, // 2-green
    0xffe0, // 3-yellow
    0x001f, // 4-blue
    0xf81f, // 5-magenta
    0x07ff, // 6-cyan
    0xffff  // 7-white
  };

  if(c==0x1b) { escMode=1; return; }
  if(escMode==1) {
    if(c=='[') { escMode=2; nVals=0; } else escMode=0;
    return;
  }
  if(escMode==2) {
    if(isdigit(c))
      vals[nVals] = vals[nVals]*10+(c-'0');
    else if(c==';')
      nVals++;
    else if(c=='m') {
      escMode=0;
      nVals++;
      for(int i=0;i<nVals;i++) {
        int v = vals[i];
        if(v == 0) { // all attributes off
          if(nVals==1) {
            fg = WHITE;
            bg = BLACK;
            font.setColor(fg);
          }
          font.setBold(0);
        } else if(v == 1) { // bold
          font.setBold(1);
        } else if(v == 7) { // inverse
          font.setColor(bg, fg);
        } else if(v>=30 && v<38){ // fg colors
          fg = colors[v-30]; 
          if(bg==BLACK) font.setColor(fg); else font.setColor(fg, bg);
        } else if(v>=40 && v<48){
          bg = colors[v-40]; 
          if(bg==BLACK) font.setColor(fg); else font.setColor(fg, bg);
        } else if(v>=90 && v<98){ // fg colors
          fg = colors[v-90]; 
          if(bg==BLACK) font.setColor(fg); else font.setColor(fg, bg);
        } else if(v>=100 && v<108){
          bg = colors[v-100]; 
          if(bg==BLACK) font.setColor(fg); else font.setColor(fg, bg);
        }          
      }
      vals[0]=vals[1]=vals[2]=vals[3]=0;
      nVals=0;
    } else {
      escMode=0;
      vals[0]=vals[1]=vals[2]=vals[3]=0;
      nVals=0;
    }
    return;
  }
  if(c==10) { scrollScreen(); return; } // LF
  if(c==13) { xp=0; return; } // CR
  if(c==8) { // BS
    if(xp>0) xp-=charWd*sx; 
    lcd.fillRect(xp, yp, charWd*sx, charHt*sy, BLACK);
    return; 
  }
  if(xp<screenWd) font.drawChar(xp, yp, c); // 150-650us
  xp+=charWd*sx;
  if(xp>=screenWd && wrap) scrollScreen(); // 8900us
}

void testVT100()
{
  printString("\e[0;32;40mGreen on black\n");
  printString("\e[0;37;45mWhite on magenta\n");
  printString("\e[0;30;46mBlack on cyan\n");
  printString("\e[0;33;44mYellow on blue\n");
  printString("\e[0;31;47mRed on white\n");
  printString("\e[0mRegular\n");
  printString("\e[1mText in bold\e[0m\n");
  printString("\e[7mInverse\e[0m\n");
  printString("ABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}\n");
  printString("abcdefghijklmnopqrstuvwxyz:;()\n");
  printString("01234567890,.'!|?/*_=+-#%$^&\n");
  if(charHt*sy<24) {
    printString("\e[1m");
    printString("ABCDEFGHIJKLMNOPQRSTUVWXYZ[]{}\n");
    printString("abcdefghijklmnopqrstuvwxyz:;()\n");
    printString("01234567890,.'!|?/*_=+-#%$^&\n");
    printString("\e[0m");
  }
  //delay(2000);
} 

void setup() 
{
  //Serial.begin(9600); // perfect
  //Serial.begin(19200); // perfect
  Serial.begin(28800); // almost ok
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  font.init(customRect, SCR_WD, SCR_HT); // custom fillRect function and screen width and height values
  font.setFont(&rre_fjg_8x16);
  charWd = font.getWidth();
  charHt = font.getHeight();
  font.setColor(WHITE);
  font.setCharMinWd(8);  
  lcd.setScrollArea(0,0);
  //testVT100();
  printString("\e[0;44m *** Terminal Init *** \e[0m\n");
}

unsigned long us;

void loop()
{
  while(Serial.available()) {
    //us = micros();
    printTermChar(Serial.read());
    //Serial.println(micros()-us);
  }
}

