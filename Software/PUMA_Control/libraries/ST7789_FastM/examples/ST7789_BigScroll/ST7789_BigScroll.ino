// ST7789 library example
// (c) 2019 Pawel A. Hernik

// requires RRE Font library:
// https://github.com/cbm80amiga/RREFont

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
#define SCR_HT   320   // 320 - to access to full 240x320 frame buffer
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

#include "RREFont.h"
#include "rre_times_98v.h"
#include "rre_tahoma_65v.h"

RRE_Font *rFont = &rre_Times98v;
//RRE_Font *rFont = &rre_tahoma_65v;

uint16_t bg = 0x0000, fg = 0xffff;
int maxy = 320; // internal ST7789 fb is 240x320
int screenWd = 240, screenHt = 240;
int spacing = 1;
int sx = 1, sy = 1;
int scrollStep = 2;
int scrollDelay = 10;
int cnt = 0;
int ycnt = 0, yscr = 0;
int offsY = 30;
unsigned long ms;


int scrollCharRRE(unsigned char c)
{
  if(c<rFont->firstCh || c>rFont->lastCh) return 0;
  uint16_t recIdx = pgm_read_word(&(rFont->offs[c-rFont->firstCh]));
  uint16_t recNum = pgm_read_word(&(rFont->offs[c-rFont->firstCh+1]))-recIdx;
  int chWd = recNum>0 ? pgm_read_byte(rFont->rects + (recNum-1+recIdx)*3)+1 : 0;
  //Serial.println(String(char(c))+": "+chWd);
  spacing = (c==' ') ? rFont->wd/3 : 4;
  int idx = recIdx*3;
  for(int col = 0; col<chWd+spacing; col++) {
    ms = millis();
    yscr += scrollStep;
    if(yscr>=maxy) yscr-=maxy;
    lcd.setScroll(yscr);
    ycnt=yscr+240;
    if(ycnt>=maxy) ycnt-=maxy;
    //Serial.println(String("ycnt=")+ycnt);
    int scrHOffs = screenHt-1;
    int scrWOffs = screenWd-1;
    int lineY = scrHOffs-(screenHt-sx-ycnt);
    int wd;
    if(col>=chWd) { // draw empty column (spacing)
      wd = sy*rFont->ht;
      lcd.fillRect(scrWOffs-offsY-wd, lineY, wd, sx, bg);
      while(millis()-ms<scrollDelay);
      continue;
    }
    int hf,yf,xf = pgm_read_byte(rFont->rects+idx+0);
    int ybg = 0;
    //yf = pgm_read_byte(font_Rects+idx+1); Serial.print("yf="); Serial.println(yf);
    while(xf==col) { // draw all lines for current column
      yf = pgm_read_byte(rFont->rects+idx+1);
      hf = pgm_read_byte(rFont->rects+idx+2);
      if(yf>0) { // bg line top
        wd = (yf-ybg)*sy;
        lcd.fillRect(scrWOffs-(offsY+ybg*sy)-wd, lineY, wd, sx, bg);
      }
      ybg = yf+hf;
      wd = hf*sy;
      lcd.fillRect(scrWOffs-(offsY+yf*sy)-wd, lineY, wd, sx, fg);
      idx+=3;
      xf = pgm_read_byte(rFont->rects+idx+0);
    }
   //Serial.println("ys = "+String(ys)+"  "+String(charHt));
    
    if(ybg<rFont->ht-1) { // last bg line
      wd = (rFont->ht-ybg)*sy;
      lcd.fillRect(scrWOffs-(offsY+ybg*sy)-wd, lineY, wd, sx, bg);
    }
    while(millis()-ms<scrollDelay);
  }
}

void scrollString(char *c)
{
  while(*c) {
    uint8_t r,g,b;
    lcd.rgbWheel(cnt*32, &r, &g ,&b);
    fg = RGBto565(r,g,b);
    bg = cnt&1 ? RGBto565(40,40,40) : RGBto565(20,20,20);
    cnt++;
    scrollCharRRE(*c++);
  }
}

void setup() 
{
  Serial.begin(9600);
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  lcd.setScrollArea(0,0);
}

void loop()
{
  scrollStep = 1; sx = 1;  sy = 2; scrollDelay = 6; offsY = (240-rFont->ht*sy)/2;
  //scrollString("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+-!?.: ");
  scrollString("This is an example of super-smooth scrolling with ordinary Arduino, ST7789 240x240 IPS LCD library and large 97x98 RRE font ...    ");
}

