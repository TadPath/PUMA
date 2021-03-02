// Photo slideshow via serial port
// ST7789 & RRE library example
// (c) 2019 Pawel A. Hernik
// YT video: https://youtu.be/vQY5ILjSZBc

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
#define SCR_HT   240   // 320 - to allow access to full 240x320 frame buffer
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Arduino_ST7789_Fast.h>
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

void setup() 
{
  Serial.begin(115200);
  lcd.init(SCR_WD, SCR_HT);
  lcd.fillScreen(BLACK);
  lcd.drawRect(0,0,240,240,RED);
  lcd.setTextColor(WHITE);
  lcd.setTextSize(2);
  lcd.setCursor(24,110);
  lcd.println("WAITING FOR DATA");
  lcd.setTextColor(BLACK,WHITE);
}

int cnt=0;
int i=0,j=239;
uint8_t r,g,b,c;
int header=54;
unsigned long ms;
char buf[30];

void loop()
{
  while(Serial.available())  {
    c = Serial.read();
    if(header>0) { 
      header--; 
      if(header==0) ms=millis();
      continue; 
    }
    if(cnt==0)
      b = c;
    else if(cnt==1)
      g = c;
    else if(cnt==2) {
      r = c;
      cnt=-1;
      lcd.drawPixel(i,j,RGBto565(r,g,b));
      i++;
      if(i>=240) {
        i=0; j--;
        if(j<0) {
          j=239; 
          header=54; 
          snprintf(buf,30," Time: %d s ",(millis()-ms)/1000);
          lcd.setCursor(50,200);
          lcd.println(buf);
        }
      }
    }
    cnt++;
  }
}

