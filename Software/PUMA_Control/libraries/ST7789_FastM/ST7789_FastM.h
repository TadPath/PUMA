// Fast ST7789 IPS 240x240 SPI display library
// (c) 2019 by Pawel A. Hernik
// Modified by Optarc for use in mirror display HUD

#ifndef _ST7789_FASTM_H_
#define _ST7789_FASTM_H_

// ------------------------------
// remove "define COMPATIBILITY_MODE" for best performance on 16MHz AVR Arduinos
// if defined - the library should work on all Arduino compatible boards
//#define COMPATIBILITY_MODE

// define for LCD boards where CS pin is internally connected to the ground
#define CS_ALWAYS_LOW
// ------------------------------

#include "Arduino.h"
#include "Print.h"
#include <GFXM.h>
#include <avr/pgmspace.h>

#define ST7789_TFTWIDTH 	240
#define ST7789_TFTHEIGHT 	240

#define ST7789_240x240_XSTART 0
#define ST7789_240x240_YSTART 0

#define ST_CMD_DELAY   0x80

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01

#define ST7789_SLPIN   0x10  // sleep on
#define ST7789_SLPOUT  0x11  // sleep off
#define ST7789_PTLON   0x12  // partial on
#define ST7789_NORON   0x13  // partial off
#define ST7789_INVOFF  0x20  // invert off
#define ST7789_INVON   0x21  // invert on
#define ST7789_DISPOFF 0x28  // display off
#define ST7789_DISPON  0x29  // display on
#define ST7789_IDMOFF  0x38  // idle off
#define ST7789_IDMON   0x39  // idle on

#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_PTLAR    0x30   // partial start/end
#define ST7789_VSCRDEF  0x33   // SETSCROLLAREA
#define ST7789_VSCRSADD 0x37

#define ST7789_WRDISBV  0x51
#define ST7789_WRCTRLD  0x53
#define ST7789_WRCACE   0x55
#define ST7789_WRCABCMB 0x5e

#define ST7789_POWSAVE    0xbc
#define ST7789_DLPOFFSAVE 0xbd

// bits in MADCTL
#define ST7789_MADCTL_MY  0x80
#define ST7789_MADCTL_MX  0x40
#define ST7789_MADCTL_MV  0x20
#define ST7789_MADCTL_ML  0x10
#define ST7789_MADCTL_RGB 0x00


// Color definitions
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define RGBto565(r,g,b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)) 

class Arduino_ST7789 : public Adafruit_GFX {

 public:
  Arduino_ST7789(int8_t DC, int8_t RST, int8_t CS = -1);

  void init(uint16_t width, uint16_t height);
  void begin() { init(ST7789_TFTWIDTH,ST7789_TFTHEIGHT); }
  void init() { init(ST7789_TFTWIDTH,ST7789_TFTHEIGHT); }
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
//  void pushColor(uint16_t color);
  void fillScreen(uint16_t color=BLACK);
  void clearScreen() { fillScreen(BLACK); }
  void cls() { fillScreen(BLACK); }
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
//  void drawImage(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *img);
//  void drawImageF(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *img16);
//  void drawImageF(int16_t x, int16_t y, const uint16_t *img16) { drawImageF(x,y,pgm_read_word(img16),pgm_read_word(img16+1),img16+3); } 
//  void setRotation(uint8_t r);
  void invertDisplay(boolean mode);
//  void partialDisplay(boolean mode);
//  void sleepDisplay(boolean mode);
//  void enableDisplay(boolean mode);
//  void idleDisplay(boolean mode);
//  void resetDisplay();
//  void setScrollArea(uint16_t tfa, uint16_t bfa);
//  void setScroll(uint16_t vsp);
//  void setPartArea(uint16_t sr, uint16_t er);
//  void setBrightness(uint8_t br);
//  void powerSave(uint8_t mode);

//  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);
//  uint16_t color565(uint8_t r, uint8_t g, uint8_t b) { return Color565(r, g, b); } 
//  void rgbWheel(int idx, uint8_t *_r, uint8_t *_g, uint8_t *_b);
//  uint16_t rgbWheel(int idx);

 protected:
  uint8_t  _colstart, _rowstart, _xstart, _ystart;

  void displayInit(const uint8_t *addr);
  void writeSPI(uint8_t);
  void writeCmd(uint8_t c);
  void writeData(uint8_t d);
  void commonST7789Init(const uint8_t *cmdList);

 private:
  int8_t  csPin, dcPin, rstPin;
  uint8_t  csMask, dcMask;
  volatile uint8_t  *csPort, *dcPort;

};

#endif
