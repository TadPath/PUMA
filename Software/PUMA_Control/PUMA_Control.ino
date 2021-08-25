/* ****************************************************************** */
/*                                                                    */
/* PUMA Control v1.1                                                  */
/* =================                                                  */
/*                                                                    */
/* V 1.1 is the first full release (after the pre-release).           */
/* Compared to v1.0 is has all the same feature plus the ability to   */
/* set different colours for the various components of the aperture   */
/* function of the SLM thereby allowing the easy use of custom        */
/* Rheinberg filters.                                                 */
/* You also now have the option of setting a custom colour which can  */
/* be assigned to anything that takes a colour. The custom colour     */
/* value is defined via the Defaults menu. Multiple custom colours    */
/* can be used simultaneously (see Manual for how to do this).        */
/* In light of the fact that the ammeter readings can be spuriously   */
/* high and unreliable when batteries are used, this version gives   */
/* you the option to disable the ammeter although this option must be  */
/* used with caution because you will loose the warning of a power    */
/* regulator failure that could blow the LED light source. See the    */
/* user's Manual for details.                                         */
/* Other features are identical to v 1.0.                             */
/*                                                 PJT 31.07.2021     */
/*                                                                    */
/* ------------------------------------------------------------------ */
/* Description for v 1.0                                              */
/*                                                                    */
/* This is the source code for the Arduino Nano to run the control    */
/* operations for the Optarc PUMA microscope.                         */
/*                                                                    */
/* This software performs the following functions:                    */
/* > Allows the user to control the Z-stage focus motor via the 5-way */
/*   joystick controller.                                             */
/* > Drives the ST7789 TFT display screen for the AR module.          */
/* > Provides the GUI for menu navigation to select various functions */
/*   and user modes.                                                  */
/* > Provides an interactive pointer so the user can demonstrate      */
/*   structures of interest to other viewers.                         */
/* > Provides the ability to mark and count objects interactively.    */
/* > Provides the ability to measure objects - either counts, angles  */
/*   or arbitrary line and boundary length measurements in calibrated */
/*   units.                                                           */
/* > Provides the ability to display a number of custom grids, scale  */
/*   bars and graticules in a user-selectable scale and colour.       */
/* > Provides the ability to display a plain background in a number of */
/*   colour options to act as illumination for low power projection   */
/*   of specimens via the AR projector (with the appropriate slide    */
/*   holder adapter in place).                                        */
/* > Provides a timer facility.                                       */
/* > Provides some aperture control functions for use with a PUMA SLM */
/*   filter (spatial light modulator aperture control).               */
/* > Provides the ability to integrate several of the above functions */
/*   so they can be used simultaneously.                              */                               
/*                                                                    */
/* For full functionality the user must have the PUMA Augmented       */
/* Reality module (AR Module) installed on their scope because this   */
/* contains the TFT display which shows all graphical feedback and so */
/* allows the user to see the GUI menus and select their functions.   */
/* However, the default mode of operation when powered up is Z-motor  */
/* control so even if you do not have the AR Module you can still use */
/* the joystick to control the focus motor without any visual         */
/* feedback but most of this software's functionality will not be     */
/* available to you. It is possible to connect a bare ST7789 TFT      */
/* screen to the control box to get visual feedback provided the TFT  */
/* screen can run on 5v (3v only models must not be used because the  */
/* control box supplies 5v to the TFT screen and 3v only models will  */
/* be permanently destroyed if connected). However, if this is done,  */
/* be aware that all textual information will be in mirror writing    */
/* because the display is designed for viewing via the AR projector   */
/* and the AR projector gives a mirror image of the screen to the     */
/* viewer. Thus you will need to view the screen via a mirror to see  */
/* easily readable information.                                       */
/*                                                                    */
/* This program uses a modified version of the Adafruit GFX library   */
/* where the default font has been customised to draw mirror writing  */
/* and also to draw custom graphics characters. I place this custom   */
/* version into a copy of that library called GFXM (in the library     */
/* folder GFXM_Library) so that this does not get overwritten each    */
/* time the standard Adafruit graphics library is updated). This also */
/* includes its own version of the standard Arduino library Print.cpp */
/* file which ensures writing is right to left - i.e. mirrored.       */
/* This also uses a slightly modified version of  Pawel A. Hernik's   */
/* "Arduino_ST7789_Fast" library which I made called 'ST7789_FastM'   */
/*                                                                    */
/*           This program was written by Dr P. J. Tadrous  20.11.2020 */
/*                                               Last edit 08.07.2021 */
/*                                                                    */
/* ****************************************************************** */

// 'Puma_115_x_144', 115x144px, 23 chars across x 18 chars down
const unsigned char PumaIcon [] PROGMEM = {
32, 11, 176, 177, 178, 179, 32, 8, 180, 181, 182, 183, 181, 181, 181, 184, 182, 185, 186,
225, 2, 187, 188, 32, 8, 189, 225, 12, 190, 32, 9, 225, 13, 191, 192, 32, 8, 225, 14, 193,
194, 195, 32, 6, 225, 16, 196, 197, 195, 32, 4, 225, 18, 191, 32, 4, 225, 19, 198, 32, 3,
225, 19, 199, 32, 3, 225, 19, 200, 201, 32, 2, 225, 20, 202, 32, 2, 225, 21, 203, 195,
225, 22, 204, 225, 22, 187, 225, 21, 205, 206, 207, 208, 208, 208, 208, 209, 210, 211, 212,
213, 225, 9, 214, 215, 216, 32, 10, 217, 218, 225, 6, 219, 220, 32, 15, 221, 222, 223, 225,
1, 214, 224, 0 };

#include <limits.h>

// For TFT Screen functionality
#include <SPI.h>
//#include <SD.h>
#include <GFXM.h> // Adafruit's GFX library modified to write mirror-flipped text and numbers.
#include <ST7789_FastM.h> // Pawel A. Hernik's fast ST7789 library modified for mirror-flipped text and numbers.

#define TFT_DC    7
#define TFT_RST   8
#define TFT_BLK   9
#define SCR_WD   240
#define SCR_HT   240
#define CS_ALWAYS_LOW

#define SCR_WD 240
#define SCR_HT 240
Arduino_ST7789 lcd = Arduino_ST7789(TFT_DC, TFT_RST);

// For Joystick controller functionality
#define NAVI_UP    2
#define NAVI_DOWN  3
#define NAVI_LEFT  4
#define NAVI_RIGHT 5
#define NAVI_MID   12
#define NAVI_SET   A5
#define NAVI_RST   10
// The following are arbitrary values used as ID flags
// for simultaneous press events
#define NAVI_UPLEFT    32
#define NAVI_UPRIGHT   33
#define NAVI_DOWNLEFT  34
#define NAVI_DOWNRIGHT 35
#define NAVI_SETRST    36
#define NAVI_SETDOWN   37
#define NAVI_SETUP     38
#define NAVI_SETLEFT   39
#define NAVI_SETRIGHT  40
#define NAVI_SETMID    41

// For the sound buzzer
#define BUZZER 6

// For the light meter
#define LAMP_METER  A7

// For Z-Motor functionality
#define ZMOTOR_PIN_1 A1
#define ZMOTOR_PIN_2 A3
#define ZMOTOR_PIN_3 A2
#define ZMOTOR_PIN_4 A4

// Control Modes
// These #defines must be in the same order as the #defines for
// the menu levels 'ML_...' (which see)
#define CM_Z_MOTOR      1
#define CM_POINTER      2
#define CM_TIMER        3
#define CM_GRATICULE    4
#define CM_PLAIN        5
#define CM_APERTURE     6
#define CM_MENU_DELTA   6
// NB: The 'CM_MENU' (not #defined) is anything > CM_MENU_DELTA from
//     (CM_Z_MOTOR + CM_MENU_DELTA) to (CM_GRATICULE + CM_MENU_DELTA).
//     Simply subtract CM_MENU_DELTA from the current Control_mode value
//     to get the Control_mode the program was in prior to entering the
//     'CM_MENU'.
//     The 'CM_MENU' is entered into by simply adding CM_MENU_DELTA to 
//     the current (non-menu) Control_mode.
//     This way the program 'knows' what mode it was in prior to 
//     entering the menu mode and so can return to that mode if menu is 
//     exited without changing the control mode (e.g. due to a timeout 
//     or if the menu items changed did not include the control mode) -
//     without the need for a second global variable keeping track of 
//     what the previous control mode was. Clearly, if the user selects
//     a new control mode while in the menu mode, this will be recorded
//     in the Control_mode global as the selected CM + CM_MENU_DELTA
//     until the user exits the Menu CM (at which point CM_MENU_DELTA 
//     will be subtracted from Control_mode to activate the currently
//     selected control mode). I also do it this way so if, in future, I
//     add further control modes beyond the current maximum value
//     (CM_GRATICULE in this case) I can simply change CM_MENU_DELTA to
//     equal the new maximum value and recompile.

// Pointer Characters
#define PC_ARROW_1 0x18 
#define PC_ARROW_2 0x19 
#define PC_ARROW_3 0x1a 
#define PC_ARROW_4 0x1b 
#define PC_ARRHD_1 0x10 
#define PC_ARRHD_2 0x11 
#define PC_ARRHD_3 0x1e 
#define PC_ARRHD_4 0x1f 
#define PC_ARRHD_5 0xae 
#define PC_ARRHD_6 0xaf 
#define PC_CURSR_1 0x2b 
#define PC_CURSR_2 0x58 

// Marker Characters
#define MC_ASTER_1  0x2a 
#define MC_ASTER_2  0x2b 
#define MC_CIRCLE 0x4f 
#define MC_HASH   0x23 
#define MC_CROSS  0x58 
#define MC_PIXEL    0x00 

// Countdown Complete Signal Codes
#define CC_NOSIGNAL 0 // Nothing at all 
#define CC_VISUAL   1 // Visual signal only 
#define CC_VISBEEP  2 // Visual signal and Beeping 
#define CC_BEEP     3 // Beeping only 

// Graticule Styles
#define GS_GRID_1   0  
#define GS_GRID_2   1  
#define GS_GRID_C   2  
#define GS_RULER_X  3  
#define GS_RULER_XY 4   
#define GS_SCALE_X  5  
#define GS_SCALE_XY 6  

// Aperture Types
#define AT_BRIGHTFIELD   0  
#define AT_DARKFIELD     1  
#define AT_PHASECONTRAST 2  
#define AT_RASTERSCAN    3  // Set point diameter, scan diameter, delta_t (dwell time for each point, 0 = manual advancement)

// Calibration Units
#define CU_MICROM 0
#define CU_NANOM  1
#define CU_MILLIM 2
#define CU_ARBIT  3

// Binary Modes
#define BM_OFF 0
#define BM_ON  1

// Marker array size. This dictates how many marker points a user can lay
// down. It therefore determines the maximum number of objects that can be
// counted in this way. It also represents the maximum number of nodes in a
// polyline for measurement of line length. However this takes up precious
// PSRAM hence the relatively low number: PSRAM used is 2 * MKR_MAX.
// If I have SRAM to spare once the whole program is complete I should
// increase this to a maximum of 255 (because the array index global Mkr_idx
// is of size uint8_t so can only register up to 255 points max without also
// increaeing the size of this index variable.

#define MKR_MAX 127

// Control Mode Function Options
#define CO_POINT   0
#define CO_COUNT   1
#define CO_LENGTH  2
#define CO_ANGLE   3

// Timer Status
#define TM_COLD    0
#define TM_STOPPED 1
#define TM_RUNNING 2

// Buzzer Signals
#define BZ_SILENCE      0
#define BZ_ACKNOWLEDGED 1
#define BZ_PROMPT       2
#define BZ_FORBIDDEN    3
#define BZ_TRILL        4
#define BZ_LAMPHOT      5
#define BZ_TIMEOUT      6

// PUMA Icon Defs
#define PI_SPACE 32
#define PI_BLOCK 225

// Necessary Gloabals
 uint16_t Acklen = 50;     // Key action acknowledgement beep length in ms
 uint8_t  Z_homed = 0;      // Flag to tell if the Z-motor is homed
 uint32_t Z_delta = 16;     // Steps to take on a single activation
 uint16_t Z_speed = 16;     // Motor speed setting (minimum delay ms between each step)
 uint32_t Zpos_minimum = 0; // Lower limit relative to home = 0;
 uint32_t Zpos_curr = 0;    // Current Z position realive to home (0)
 uint32_t Zpos_sw_start = 0;// Z-sweep start position
 uint32_t Zsweep_steps = 16;// Z-sweep number of Z_delta steps to sweep
 uint32_t Zsweep_delay = 0; // Z-sweep delay in ms
 uint32_t Zbacklash = 640;  // Z-backlash correction steps
 uint8_t  Zenergised_always = 0; // Whether to leave the coild energised till user disables them (the default is 0 to avoid problems with motor overheating and power wastage)
 int8_t   Zchangedir=1;     // Keep track of direction changes
 int8_t   Zlastdir;         //     to apply backlash steps.
 int8_t   Zstep = 0;        // Current motor coil step in sequence
 uint8_t  Zenergised = 1; // Motor coil energised status
 uint8_t  Zlim_init;  // Initialisation state of Z limit switch.
 uint8_t  Pointer_char = PC_ARROW_1; // Character to use for the pointer
 uint8_t  Marker_char = MC_ASTER_1;  // Character to use to leave marks
 uint8_t  Boot_complete = 0; // Whether the program hassuccessfully completed its boot-up sequence.
 uint8_t  Timer_hrs    = 0; // Hours to count 
 uint8_t  Timer_min    = 0; // Minutes to count 
 uint8_t  Timer_sec    = 0; // Seconds to count 
 uint8_t  Time_curr_hrs    = 0; // Current Hours
 uint8_t  Time_curr_min    = 0; // Current Minutes
 uint8_t  Time_curr_sec    = 0; // Current Seconds
 uint8_t  Timer_status     = TM_COLD; // Current state of the timer
 uint8_t  Timer_signal = CC_VISBEEP; // Signal on countdown complete
 uint8_t  Timer_dirn;     // Direction of timer count: 1 = counting UP. 0 = counting DOWN.
 unsigned long Timer_micros;       // Current microsecond counter for timer
 uint8_t  Aperture_type = AT_BRIGHTFIELD; // Type of aperture to control
 uint8_t  Aperture_rotation = 0; // Orientation of aperture for phase contrast
 uint8_t  Aperture_dfsuperposed = 0; // Draw aperture with superposed darkfield stop
 uint16_t  Aperture_radius = 0;   // Main aperture radius
 uint16_t  Aperture_dfradius = 0; // Darkdield stop radius
 uint8_t  Graticule_style = GS_GRID_1; // Style of graticule to draw
 uint8_t  Graticule_orient = 0; // Orientation of graticule to draw
 uint8_t  Graticule_period = 10; // Spacing of graticule parts
 uint8_t  Graticule_showing = 0; // Whether a graticule is displayed (1) or not (0)
 uint16_t Calib_int = 1; // Integer part of calibration value
 uint16_t Calib_dec = 0; // Fractional part of calibration value
 double   Calib_fac = 1.0; // The floating point representation of Calib_int.Calib_dec
 uint8_t  Calib_unit = CU_ARBIT; // Calibration unit name
 uint8_t  Lamp_denom = 0, Lamp_update = 0; // Lamp meter flags
 uint16_t Lamp_curr = 0, Lamp_prev = 0; // Lamp meter values
 uint8_t  Bckl_level = 255;
 uint8_t  Beeper_mode = BM_ON;
 uint8_t  Control_mode = CM_Z_MOTOR;
 uint8_t  Previous_cm  = CM_Z_MOTOR;
 uint16_t Colour[10]={WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK};
 uint16_t CustomColour = 39321;
 uint8_t  Ptr_X=120,Ptr_Y=120; // Pointer (arrow, etc.) coordinates
 uint8_t  Mkr_X[MKR_MAX],Mkr_Y[MKR_MAX],Mkr_idx=0; // Marker storage arrays 
 uint8_t  Ptr_mode = CO_POINT; // Measurement mode for pointer / marker
 uint8_t  HUD_Disabled = 0;  // HUD disable flag - set while in Aperture control mode

// Source code substitution labels

// Main HUD Display Panel Coordinates
// Top Panel
#define DTYU  11
#define DTYD  14
#define DTYE  12
#define DTY2  24
#define DTX1  156
#define DTX2  144
#define DTX3  136
#define DTX4  118
#define DTX5  131
#define DTX6  105
#define DTX7  79

// Bottom Panel
#define DBY1 218
#define DBY2 208
#define DBX1 114

// Left Panel
#define DLX  216
#define DLY1 117

// Right Panel
#define DRX  28
#define DRY1 78
#define DRYM 94
#define DRY2 110
#define DRY3 124
#define DRY4 138

// Redefinition of block char due to custom programming
// of default charset
#define CH_BLOCK 0xe1

// Main Display Icon Characters
#define DC_ZUP    0x1e
#define DC_ZDOWN  0x1f
#define DC_ZOFF   0xec // Motor coils de-energised marker
#define DC_ZCRSE  0x2b // Coarse focus (super-speed) indicator
#define DC_MUTE   0x4d
#define DC_ZHOME  0x48
#define DC_ZSWEEP 0xe3
#define DC_MICRO  0xe5
#define DC_NANO   0x6e
#define DC_MILLI  0x6d
#define DC_ARBI   0x78
#define DC_CMZ    0x12 // Z-motor control mode icon
#define DC_CMT    0xe9 // Timer control mode icon

// HUD Item IDs
#define HU_ZOFF    0  // Motor coils de-energised
#define HU_ZIUP    1  // This MUST have the same value as ZDIRN_UP
#define HU_ZIDOWN  -1 // This MUST have the same value as ZDIRN_DOWN
#define HU_ZHEAD   2
#define HU_ZHOME   3
#define HU_ZPOS    4
#define HU_ZCRSE   13 // Coarse (i.e. super) speed focussing
#define HU_ZFINE   14 // Fine (i.e. chosen) speed focussing
#define HU_ZSWEEPON   15 // Z-sweep mode is active
#define HU_ZSWEEPOFF  16 // Z-sweep mode is inactive
#define HU_ZIHIDE  6
#define HU_THRS    8
#define HU_TMIN    9
#define HU_TSEC    10
#define HU_UNITS   11
#define HU_PERIOD  12
#define HU_LAMP    17



// Maximum elevation of Z platform
// 80000 represents about 3.5 mm of elevation
// from the homed position (so there are approx.
// 22857 steps per mm or 23 steps per micron elevation).
// The stage can travel higher physically by about
// 1 mm so this represents a safe upper limit.
#define ZPOS_MAX 80000

// Restrict backlash correction steps to relatively low number
// because backlash correction is applied without regards to
// limits - a high number could push the stage past safety
// limits up or down and a proper meaningful backlash correction
// would never be that high. Practice shows that the default
// setting of 640 is about right.
#define ZBACKLASH_MAX 1000

// Iset the maximum speed limit at 4 because at 2 there was
// considerable stalling of the motor.
#define Z_SPEED_MAX 4

// Z-Motor Directions (these MUST be 1 and -1)
#define ZDIRN_UP 1
#define ZDIRN_DOWN -1
#define DOWN_LIMIT_TRIPPED analogRead(A6)<255


// Integer types
#define UINT8   1
#define UINT16  2
#define UINT32  3


// The menu LEVELs (a.k.a. pages)
#define ML_MAIN    1
#define ML_ZMOTOR  2
#define ML_POINTER 3
#define ML_TIMER   4
#define ML_GRATIC  5
#define ML_PLAIN   6
#define ML_APERT   7
#define ML_DEFAULT 8

// Menu and other GUI Key Coordinates
#define Y_MH  12   // Y-pos for the menu header text
#define Y_MO  64   // Y-pos for the first menu option text line
#define DY_MO 12   // Y-increment for successive menu option text lines
#define X_MOD 194  // X-pos for the menu option descriptor text of 1 char length
#define X_MOV 116  // X-pos for the menu option value text of 1 char length
#define DX1 6      // X-pos offset for right justification (i.e. left post mirror)
#define DX2 3      // X-pos offset for central justification
#define X_MODI 206 // X-pos for the menu option descriptor indicator
#define XLMOVI 128 // X-pos for the L menu option value indicator
#define XRMOVI 40  // X-pos for the R menu option value indicator

// Menu Option lines Y positions
#define Y_MO_1  76
#define Y_MO_2  88
#define Y_MO_3  100
#define Y_MO_4  112
#define Y_MO_5  124
#define Y_MO_6  136
#define Y_MO_7  148
#define Y_MO_8  160
#define Y_MO_10 184

// Menu Header Settings X Coords

#define XMS_ZMOTOR  81
#define XMS_POINTER 102
#define XMS_TIMER   108
#define XMS_GRATIC  96
#define XMS_PLAIN   108
#define XMS_APERT   96
#define XMS_DEFAULT 96
#define XMS_TFCARD  99

// Menu Header Settings Y Coord
#define YMS 28

// Right aligning text (i.e. left align for viewer due to mirror flip)
// is achieved by the following formula for X-pos:
// X-pos = X_MOD - (strlen(text-1)*DX1
// In practice I don't use the strlen function - just count the chars
// in my fixed string and subtract one.

// Menu GUI Refresh Options
#define MR_NONE         0
#define MR_LEVEL        1
#define MR_OPTION       2
#define MR_VALUE_FOCUS  3
#define MR_VALUE        4

// Text and Graphics Colour IDs
#define COLOUR_ZMOTOR     Colour[0]
#define COLOUR_POINTER    Colour[1]
#define COLOUR_MARKER     Colour[2]
#define COLOUR_TIMER      Colour[3]
#define COLOUR_GRATICULE  Colour[4]
#define COLOUR_LAMP       Colour[5]
#define COLOUR_PLAIN      Colour[6]
#define COLOUR_AP_FG      Colour[7]
#define COLOUR_AP_BG      Colour[8]
#define COLOUR_AP_CS      Colour[9]

#define C_BLACK   1
#define C_BLUE    2
#define C_RED     3
#define C_GREEN   4
#define C_CYAN    5
#define C_MAGENTA 6
#define C_YELLOW  7
#define C_WHITE   8
#define C_CUST    9


// If no lamp is connected there will be an abnormally high
// lamp current reading in the lamp meter ()~1000) so we need
// to check for this to ensure we read the meter only when a
// lamp is connected to avoid false alarms.
#define LAMP_DISCONNECTED 895

void setup() 
{
 uint8_t go_action,xctr;
 uint16_t chx,chy;
 unsigned char ctile,citer,idx,rdx;
 
  // Initialise the navigation control pins:
  pinMode(NAVI_UP,    INPUT_PULLUP);
  pinMode(NAVI_DOWN,  INPUT_PULLUP);
  pinMode(NAVI_LEFT,  INPUT_PULLUP);
  pinMode(NAVI_RIGHT, INPUT_PULLUP);
  pinMode(NAVI_MID,   INPUT_PULLUP);
  pinMode(NAVI_SET,   INPUT_PULLUP);
  pinMode(NAVI_RST,   INPUT_PULLUP);

  // Initialise the TFT:
  pinMode(TFT_BLK, OUTPUT);
  analogWrite(TFT_BLK, Bckl_level);
  lcd.init();
  lcd.setCursor(0, 0);
  lcd.setTextColor(WHITE,BLACK);  lcd.setTextSize(2);
    
  // Initialise the buzzer pin:
  pinMode(BUZZER, OUTPUT);
  
  
  // Set up the Z motor pins:
  pinMode(ZMOTOR_PIN_1, OUTPUT);
  pinMode(ZMOTOR_PIN_2, OUTPUT);
  pinMode(ZMOTOR_PIN_3, OUTPUT);
  pinMode(ZMOTOR_PIN_4, OUTPUT);

  // Print the boot-up logo / intro screen
  lcd.fillScreen(); // clear the screen

  // Draw the PUMA icon
  chx=62; chy=48; // Start character coordinates
  xctr=0; // X position counter.
  idx = 0;
  while(1){
    // Read a character from PROGMEM
    ctile = (unsigned char)pgm_read_byte(PumaIcon+idx);
    if(ctile==0) break; // end of data stream
    idx++;
    if(ctile == PI_SPACE || ctile == PI_BLOCK){ // Read the RLE iteration factor
      citer = (unsigned char) pgm_read_byte(PumaIcon+idx);
      idx++;
      for(rdx=0;rdx<citer;rdx++){
       // Print the char at the next available position
       if(ctile == PI_BLOCK) lcd.drawChar(chx,chy, ctile, BLUE, BLACK,1);
       chx+=5; xctr++;
       if(xctr==23) {xctr=0; chx=62; chy+=8;}
      }
    } else {
       // Print the char at the next available position
       lcd.drawChar(chx,chy, ctile, BLUE, BLACK,1);
       chx+=5; xctr++;
       if(xctr==23) {xctr=0; chx=62; chy+=8;}
    }
  }
  

  // Draw the initial GUI
  peripheral_ring(GREEN);
  lcd.setCursor(97, 10);
  lcd.setTextSize(2);
  lcd.print(F("PUMA"));
  lcd.setCursor(80, 30);
  lcd.print(F("CONTROL"));
  lcd.setTextSize(1);
  lcd.setCursor(40,104);
  lcd.print(F("v."));
  lcd.setCursor(34,116);
  lcd.print(F("1.1")); // The latest version number
  lcd.setTextColor(YELLOW);
  lcd.setCursor(170,54);
  lcd.print(F("Press"));
  lcd.setCursor(144,66);
  lcd.print(F("LEFT or RIGHT"));
  lcd.setCursor(166,78);
  lcd.print(F("button."));

 
  // Draw initial navigation buttons
  lcd.fillRect(140,200, 32,16, RED);
  lcd.setCursor(141,204);
  lcd.print(F("HomeZ"));
  lcd.fillRect(68,200, 32,16, RED);
  lcd.setCursor(72,204);
  lcd.print(F("Menu"));

  // Start the clock
  Timer_micros = micros();

  // Boot up complete - beep twice so the user knows:
  beep_signal(BZ_PROMPT);
  Boot_complete=1;
  
  // Start a loop looking for either:
  // LEFT_BTN (home Z then go to loop() with Z-motor as default CM) or
  // RIGHT_BTN (go to loop() in menu mode with Z-motor as default CM)
  
  while(1){
    go_action = navi_loop(0);
    switch(go_action){
     case NAVI_SET:
       lcd.fillRect(140,200, 32,16, GREEN);
       lcd.fillScreen(); // Clear the screen
       Z_home();
     return;
     case NAVI_RST:
       lcd.fillRect(68,200, 32,16, GREEN);
       Control_mode += CM_MENU_DELTA;
       Previous_cm  = Control_mode;
     return;
     case NAVI_MID: // Quick mute shortcut.
       toggle_mute(Beeper_mode);
     break;
     default: break;
    }
   }
   
 return;
}

void loop() 
{
 // The over-arching switch is the 'Control_mode' (CM) which determines
 // what the screen shows and what the controller buttons do.

 lcd.drawChar(DRX,DRYM, PI_BLOCK, BLACK, BLACK,1); // Clear any previous control mode character
 switch(Control_mode){
  case CM_Z_MOTOR      :  
    zmotor_cm();
  break;
  case CM_POINTER      :
    pointer_cm();
  break;
  case CM_TIMER        :
    timer_cm();
  break;
  case CM_GRATICULE    :
    graticule_cm();
  break;
  case CM_PLAIN        :
    plain_cm();
  break;
  case CM_APERTURE     :
    aperture_cm();
  break;
  default: // The Menu CM
    menu_cm();
  break;
 }

}



    ////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\    .
   //                                                              \\   .
  //                  CONTROL MODE CORE FUNCTIONS                   \\  .
 //                                                                  \\ .
////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\.


// Menu Control Mode
void menu_cm(void)
{
 uint8_t go_action,m_level,m_oline,m_focus;
 uint8_t m_refresh,updown_dirn;
 uint8_t max_olines; // Maximum number of option lines in each menu page
 uint16_t int_delta = 1,Y_moline,tmpcol;
 
 // SETUP:
 lcd.fillScreen(); // Clear the screen
 HUD_Disabled = 0; // Enable the HUD
 m_level = ML_MAIN; // Start at the main menu
 analogWrite(TFT_BLK,Bckl_level); // The backlight may have toggled off
 peripheral_ring(GREEN); // Draw the FOV boundary
 lcd.drawRoundRect(36,36,168,168,3, BLUE); // Draw the common frame
 m_oline = 0; // Start at the first menu option line
 Y_moline = Y_MO+m_oline*DY_MO;
 m_focus = 0; // No value is in focus till user makes it so.
 m_refresh = MR_LEVEL;
 Previous_cm  = Control_mode;

 // LOOP:
 while(1){
  
 switch(m_refresh){ // Decide what needs drawing
   case MR_LEVEL:
     // Menu LEVEL Selector
     lcd.fillRect(37,37,166,166,BLACK);      // Erase frame area
     lcd.fillRoundRect(76,10,88,32,8, BLUE); // Erase HEADER box
     lcd.setTextColor(YELLOW); // The first thing to be printed will be
                               // the title and I do this in yellow.
     // Update the HEADER                         
     lcd.setTextSize(1);
     lcd.setCursor(85,Y_MH);
     lcd.print(F("Settings for"));
     switch(m_level){
       case ML_MAIN    :
         lcd.fillRoundRect(76,10,88,32,8, BLUE);
         lcd.setTextSize(2);
         lcd.setCursor(98,Y_MH+5);
         lcd.print(F("MAIN"));
         lcd.setTextSize(1);
       break;
       case ML_ZMOTOR  :
         lcd.setCursor(XMS_ZMOTOR,YMS);
         lcd.print(F("Z-Motor/Sweep"));
       break;
       case ML_POINTER :
         lcd.setCursor(XMS_POINTER,YMS);
         lcd.print(F("Pointer"));
       break;
       case ML_TIMER   :
         lcd.setCursor(XMS_TIMER,YMS);
         lcd.print(F("Timer"));
       break;
       case ML_GRATIC  :
         lcd.setCursor(XMS_GRATIC,YMS);
         lcd.print(F("Graticule"));
       break;
       case ML_PLAIN  :
         lcd.setCursor(XMS_PLAIN,YMS);
         lcd.print(F("Plain"));
       break;
       case ML_APERT  :
         lcd.setCursor(XMS_APERT,YMS);
         lcd.print(F("Aperture"));
       break;
       case ML_DEFAULT :
         lcd.setCursor(XMS_DEFAULT,YMS);
         lcd.print(F("Defaults"));
       break;
       default: break;
     }
     lcd.setTextColor(WHITE);

     // Update the OPTIONS                         
     switch(m_level){
       case ML_MAIN    :
         max_olines = 3; // Main menu has 3 OPTIONs
         
         // First OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-3*DX1,Y_MO);
         lcd.print(F("Mode"));
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-7*DX1,Y_MO_1);
         lcd.print(F("Settings"));
         
         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-7*DX1,Y_MO_2);
         lcd.print(F("Defaults"));

         // Update the values
         menu_value_update(m_level,0,2);
         
       break;
       case ML_ZMOTOR  :
         max_olines = 9; // Z-Motor menu has 9 OPTIONs 
         
         // First OPTION DESCRIPTOR
         print_colour_option(m_level);
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-6*DX1,Y_MO_1);
         lcd.print(F("Z Speed"));
         
         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-6*DX1,Y_MO_2);
         lcd.print(F("Z Delta"));
         
         // Fourth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-8*DX1,Y_MO_3);
         lcd.print(F("Z Minimum"));
                  
         // Fifth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_4);
         lcd.print(F("Sweep Start"));
         
         // Sixth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_5);
         lcd.print(F("Sweep Steps"));
         
         // Seventh OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_6);
         lcd.print(F("Sweep Delay"));
         
         // Eighth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-7*DX1,Y_MO_7);
         lcd.print(F("Backlash"));
         
         // Ninth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_8);
         lcd.print(F("PowerAlways"));
         
        // Update the values
         menu_value_update(m_level,1,8);

       break;
       case ML_POINTER :
         max_olines = 4; // Pointer has 4 OPTIONs
         
         // First OPTION DESCRIPTOR
         print_colour_option(m_level);
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-6*DX1,Y_MO_1);
         lcd.print(F("Pointer"));

         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_2);
         lcd.print(F("Mark Colour"));

         // Fourth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-5*DX1,Y_MO_3);
         lcd.print(F("Marker"));
         
        // Update the values
         menu_value_update(m_level,1,3);

       break;
       case ML_TIMER   :
         max_olines = 5;  // Timer Down has 5 OPTIONs 
         
         // First OPTION DESCRIPTOR
         print_colour_option(m_level);
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-4*DX1,Y_MO_1);
         lcd.print(F("Hours"));
         
         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-6*DX1,Y_MO_2);
         lcd.print(F("Minutes"));
         
         // Fourth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-6*DX1,Y_MO_3);
         lcd.print(F("Seconds"));
         
         // Fifth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-5*DX1,Y_MO_4);
         lcd.print(F("Signal"));
         
        // Update the values
         menu_value_update(m_level,1,4);

       break;
       case ML_GRATIC  :
         max_olines = 2;  // Graticule has 2 OPTIONs 

         // First OPTION DESCRIPTOR
         print_colour_option(m_level);
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-8*DX1,Y_MO_1);
         lcd.print(F("Graticule"));

        // Update the values
         menu_value_update(m_level,1,1);
                                  
       break;
       case ML_PLAIN  :
         max_olines = 1;  // Plain has 1 OPTION 

         // First OPTION DESCRIPTOR
         print_colour_option(m_level);
                                           
       break;
       case ML_APERT  :
         max_olines = 4;  // Aperture has 4 OPTIONS 

         // First OPTION DESCRIPTOR
         print_colour_option(m_level);

         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_1);
         lcd.print(F("Bgnd Colour"));

         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_2);
         lcd.print(F("Spot Colour"));

        // Fourth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-7*DX1,Y_MO_3);
         lcd.print(F("Aperture"));
                                           
        // Update the values
         menu_value_update(m_level,1,4);

       break;
       case ML_DEFAULT :
         max_olines = 8;  // Defaults has 8 OPTIONs 

         // First OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-5*DX1,Y_MO);
         lcd.print(F("Beeper"));
         
         // Second OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-8*DX1,Y_MO_1);
         lcd.print(F("Backlight"));
                                  
         // Third OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-8*DX1,Y_MO_2);
         lcd.print(F("Calib_Int"));
         
         // Fourth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-8*DX1,Y_MO_3);
         lcd.print(F("Calib_Dec"));
         
         // Fifth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-4*DX1,Y_MO_4);
         lcd.print(F("Units"));

         // Sixth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_5);
         lcd.print(F("Ammeter_Col"));

         // Seventh OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_6);
         lcd.print(F("Custom__Col"));

         // Eighth OPTION DESCRIPTOR
         lcd.setCursor(X_MOD-10*DX1,Y_MO_7);
         lcd.print(F("Ammeter_on?"));

        // Update the values
         menu_value_update(m_level,0,8);

       break;
       default: break;
     }
     m_refresh = MR_NONE;
   // No 'break;' because if the LEVEL needed drawing then the OPTIONs
   // need updating too. 
   case MR_OPTION:
     // Update focus markers:
     // 1. Menu OPTION Selector
     lcd.drawChar(X_MODI,Y_moline, 0x10, WHITE, BLACK,1);
     m_refresh = MR_NONE;
   // No 'break;' because if the OPTIONs needed drawing then VALUE_FOCUS
   // needs updating too. 
   case MR_VALUE_FOCUS:
     // 2. Menu VALUE Focus Selector
     if(m_focus){ // VALUE has come into focus so draw the indicators
       lcd.drawChar(XLMOVI,Y_moline, 0x1e, GREEN, BLACK,1);
       lcd.drawChar(XRMOVI,Y_moline, 0x1f, GREEN, BLACK,1);
       lcd.setCursor(XRMOVI+6*DX1,Y_MO_10);
       lcd.print(F("Delta = "));
       print_int_string(XRMOVI+5*DX1,Y_MO_10,(void *)&int_delta,UINT16,0,GREEN);
     } else {    // VALUE has gone out of focus so delete them.
       lcd.drawChar(XLMOVI,Y_moline, CH_BLOCK, BLACK, BLACK,1);
       lcd.drawChar(XRMOVI,Y_moline, CH_BLOCK, BLACK, BLACK,1);
       erase_string(XRMOVI+13*DX1,Y_MO_10,13);
       int_delta = 1;
     }
     m_refresh = MR_NONE;
   break;
   case MR_VALUE:
     menu_value_update(m_level,m_oline,m_oline);
     m_refresh = MR_NONE;
   break;    
   default:break; // Nothing needs GUI updating
 }

 // NAVI-SWITCH LOOP
 go_action = navi_loop(0);
 switch(go_action){
  case NAVI_MID:
   if(m_level == ML_MAIN){ // Shortcut to settings page
     if(m_focus==1){ 
       m_focus = 0;
       m_oline = 1;
       goto Enter_settings;
      }
   }
  break;
  case NAVI_SET:
   if(m_focus){ // Cycle the integer delta if VALUE is in focus. 
    int_delta *= 10;
    if(int_delta>10000) int_delta = 1;
    erase_string(XRMOVI+5*DX1,Y_MO_10,6);
    print_int_string(XRMOVI+5*DX1,Y_MO_10,(void *)&int_delta,UINT16,0,GREEN);
   }
  break;
  case NAVI_RST:
   if(m_level == ML_MAIN){
    Control_mode -= CM_MENU_DELTA;
    lcd.fillScreen(); // Clear the screen
    Lamp_update = 1; // Ensure lamp meter HUD refresh
    return;
   } else {
    m_level = ML_MAIN;
    m_focus = 0;
    lcd.drawChar(X_MODI,Y_moline, 0x10, BLACK, BLACK,1); // Delete the previous option selector focus indicator
    m_oline = 0;
    Y_moline = Y_MO+m_oline*DY_MO;    
    m_refresh = MR_LEVEL;
   }
  break;
  case NAVI_UP:
   updown_dirn = 1;
   goto Val_change_UpDown;
  case NAVI_DOWN:
   updown_dirn = 0;
   Val_change_UpDown:
   if(m_focus==0){ // When the VALUE is not in focus, this moves the selected OPTION line
     lcd.drawChar(X_MODI,Y_moline, 0x10, BLACK, BLACK,1); // Delete the previous
     if(updown_dirn){
      if(m_oline==0) m_oline=max_olines-1; else m_oline--;
     } else {
      if(m_oline==max_olines-1) m_oline=0; else m_oline++;      
     }
     Y_moline = Y_MO+m_oline*DY_MO;
     m_refresh = MR_OPTION;
   } else { // VALUE is in focus so increment/decrement it. The combination
            // of m_oline and m_level determine what value to change.
     uint8_t estr=0;
     switch(m_level){
       case ML_MAIN    :
         switch(m_oline){
           case 0    : // Control_mode
            if(updown_dirn){
              Control_mode --;
              if(Control_mode < (CM_Z_MOTOR + CM_MENU_DELTA))
                Control_mode = CM_MENU_DELTA + CM_MENU_DELTA;
            } else {
              Control_mode ++;
              if(Control_mode > (CM_MENU_DELTA + CM_MENU_DELTA))
                Control_mode = CM_Z_MOTOR + CM_MENU_DELTA;              
            }
            estr=12;
           break;
           default: break;
         }
       break;
       case ML_ZMOTOR  :
         switch(m_oline){
           case 0    : // Colour
            COLOUR_ZMOTOR = cycle_colour(COLOUR_ZMOTOR,updown_dirn);
            estr=7;
           break;
           case 1    : // Z_speed
            change_int(&Z_speed,UINT16,int_delta,Z_SPEED_MAX,UINT16_MAX,updown_dirn);
            estr=12;
           break;
           case 2    : // Z_delta
            change_int(&Z_delta,UINT32,int_delta,1,UINT32_MAX,updown_dirn);
            estr=12;
           break;
           case 3    : // Zpos_minimum
            change_int(&Zpos_minimum,UINT32,int_delta,0,ZPOS_MAX,updown_dirn);
            estr=12;
           break;
           case 4    : // Zpos_sw_start
            change_int(&Zpos_sw_start,UINT32,int_delta,Zpos_minimum,ZPOS_MAX,updown_dirn);
            estr=12;
           break;
           case 5    : // Zsweep_steps
            change_int(&Zsweep_steps,UINT32,int_delta,1,ZPOS_MAX/Z_delta,updown_dirn);
            estr=12;
           break;
           case 6    : // Zsweep_delay
            change_int(&Zsweep_delay,UINT32,int_delta,0,UINT32_MAX,updown_dirn);
            estr=12;
           break;
           case 7    : // Zbacklash
            change_int(&Zbacklash,UINT32,int_delta,0,ZBACKLASH_MAX,updown_dirn);
            estr=12;
           break;
           case 8    : // Zenergised_always
            change_int(&Zenergised_always,UINT8,int_delta,0,1,updown_dirn);
            estr=1;
           break;
           default: break;
         }
       break;
       case ML_POINTER :
         switch(m_oline){
           case 0    : // Colour
            COLOUR_POINTER = cycle_colour(COLOUR_POINTER,updown_dirn);
            estr=7;
           break;
           case 1    : // Pointer_char
            Pointer_char = cycle_pointer_char(Pointer_char,updown_dirn);
            estr=7;
           break;
           case 2    : // Marker Colour
            COLOUR_MARKER = cycle_colour(COLOUR_MARKER,updown_dirn);
            estr=7;
           break;
           case 3    : // Marker_char
            Marker_char = cycle_marker_char(Marker_char,updown_dirn);
            estr=7;
           break;
           default: break;
         }
       break;
       case ML_TIMER   :
         switch(m_oline){
           case 0    : // Colour
            COLOUR_TIMER = cycle_colour(COLOUR_TIMER,updown_dirn);
            estr=7;
           break;
           case 1    : // Timer_hrs
            change_int(&Timer_hrs,UINT8,int_delta,0,99,updown_dirn);
            if(Timer_status==TM_COLD) Time_curr_hrs=Timer_hrs;
            estr=2;
           break;
           case 2    : // Timer_min
            change_int(&Timer_min,UINT8,int_delta,0,59,updown_dirn);
            if(Timer_status==TM_COLD) Time_curr_min=Timer_min;
            estr=2;
           break;
           case 3    : // Timer_sec
            change_int(&Timer_sec,UINT8,int_delta,0,59,updown_dirn);
            if(Timer_status==TM_COLD) Time_curr_sec=Timer_sec;
            estr=2;
           break;
           case 4    : // Signal at end of countdown
            if(updown_dirn){
              if(Timer_signal == CC_BEEP)  Timer_signal=CC_NOSIGNAL;
               else Timer_signal++;
            } else {
              if(Timer_signal == CC_NOSIGNAL)  Timer_signal=CC_BEEP;
               else Timer_signal--;
            }
            estr=12;
           break;
           default: break;
         }
       break;
       case ML_GRATIC  :
         switch(m_oline){
           case 0    : // Colour
            COLOUR_GRATICULE = cycle_colour(COLOUR_GRATICULE,updown_dirn);
            estr=7;
           break;
           case 1    : // Graticule_style
            if(updown_dirn){
             if(Graticule_style == GS_SCALE_XY)  Graticule_style=GS_GRID_1;
              else Graticule_style++;
            }  else {
             if(Graticule_style == GS_GRID_1)  Graticule_style=GS_SCALE_XY;
              else Graticule_style--;
            }             
            estr=12;
           break;
           default: break;
         }
       break;
       case ML_PLAIN :
         switch(m_oline){
           case 0    : // Colour
            COLOUR_PLAIN = cycle_colour(COLOUR_PLAIN,updown_dirn);
            estr=7;
           break;
           default: break;
         }
       break;
       case ML_APERT :
         switch(m_oline){
           case 0    : // Colour [foreground,default = black i.e. completely opaque]
            COLOUR_AP_FG = cycle_colour(COLOUR_AP_FG,updown_dirn);
            estr=7;
           break;
           case 1    : // Colour Background [default  = 'white' i.e. plain transparent]
            COLOUR_AP_BG = cycle_colour(COLOUR_AP_BG,updown_dirn);
            estr=7;
           break;
           case 2    : // Colour Central Spot [default = black]
            COLOUR_AP_CS = cycle_colour(COLOUR_AP_CS,updown_dirn);
            estr=7;
           break;
           case 3    : // Aperture_type
            if(updown_dirn){
             if(Aperture_type == AT_PHASECONTRAST)  Aperture_type=AT_BRIGHTFIELD;
              else Aperture_type++;
            }  else {
             if(Aperture_type == AT_BRIGHTFIELD)  Aperture_type=AT_PHASECONTRAST;
              else Aperture_type--;
            }             
            estr=12;
           break;
           default: break;
         }
       break;
       case ML_DEFAULT :
         switch(m_oline){
           case 0    : // Beeper_mode
            toggle_mute(Beeper_mode);
            estr=3;
           break;
           case 1    : // Bckl_level
            change_int(&Bckl_level,UINT8,int_delta,1,255,updown_dirn);
            analogWrite(TFT_BLK,Bckl_level);
            estr=4;
           break;
           case 2    : // Calib_int
            change_int(&Calib_int,UINT16,int_delta,0,UINT16_MAX,updown_dirn);
            estr=12;
           break;
           case 3    : // Calib_dec
            change_int(&Calib_dec,UINT16,int_delta,0,9999,updown_dirn);
            estr=12;
           break;
           case 4    : // Calib_unit
            if(updown_dirn){
             if(Calib_unit == CU_ARBIT)  Calib_unit=CU_MICROM;
              else Calib_unit++;
            } else {
             if(Calib_unit == CU_MICROM)  Calib_unit=CU_ARBIT;
              else Calib_unit--;
            }             
            estr=11;
           break;
           case 5    : // Lamp meter display Colour
            tmpcol = cycle_colour(COLOUR_LAMP,updown_dirn);
            if(tmpcol == RED){ // Forbidden - only the system can use red
              beep_signal(BZ_FORBIDDEN);
            } else COLOUR_LAMP = tmpcol;
            estr=7;
           break;
           case 6    : // CustomColour
            change_int(&CustomColour,UINT16,int_delta,1,65534,updown_dirn);
            switch(CustomColour){ // Ensure it is different from any of the fixed colours
              case BLUE:
              case GREEN:
              case RED:
              case CYAN:
              case MAGENTA:
              case YELLOW:
               CustomColour++;
              break;
              default: break;
            }
            estr=12;
           break;
           case 7    : // Ammeter_on (Boot_complete==2 means No, 1 means Yes)
            Boot_complete=(Boot_complete==2)?1:2;
            estr=3;
           break;
           default: break;
         }
       break;
       default: break;
     }
    if(estr) erase_string(X_MOV,Y_moline,estr);
    m_refresh = MR_VALUE;
   }
  break;
  case NAVI_LEFT:
   if(m_focus==1){
    m_focus = 0;
    m_refresh = MR_VALUE_FOCUS;
   }
  break;
  case NAVI_RIGHT:
  Enter_settings:
   if(m_focus==0){
     if(m_level == ML_MAIN && m_oline == 1){
      // Special case of transition to Settings submenu ...
      m_level = Control_mode - CM_MENU_DELTA + 1;
      m_refresh = MR_LEVEL;
      lcd.drawChar(X_MODI,Y_moline, 0x10, BLACK, BLACK,1); // Delete the previous option selector focus indicator
      m_oline = 0;
      Y_moline = Y_MO+m_oline*DY_MO;
      break;
     }
     if(m_level == ML_MAIN && m_oline == 2){
      // Special case of transition to Defaults Settings submenu ...
      m_level = ML_DEFAULT;
      m_refresh = MR_LEVEL;
      lcd.drawChar(X_MODI,Y_moline, 0x10, BLACK, BLACK,1); // Delete the previous option selector focus indicator
      m_oline = 0;
      Y_moline = Y_MO+m_oline*DY_MO;
      break;
     }
    m_focus = 1;
    m_refresh = MR_VALUE_FOCUS;
   }
  break;
 }

 }

  Lamp_update = 1; // Ensure lamp meter HUD refresh
  return;
 }


#define ZM_NORMAL 1
#define ZM_SWEEP  2

// Z-Motor Control Mode
void zmotor_cm()
{
 uint8_t go_action,zmoved=0,zud,lighton,super_speed;
 uint8_t zmode = ZM_NORMAL;
 uint16_t chosen_speed;
 uint32_t idx;

 
 // Can't use this control mode if the Z-axis is not homed
 home_check:
 if(!Z_homed){
   lcd.setCursor(60, 120);
   lcd.print(F("Z-MOTOR IS NOT HOMED"));
   beep_signal(BZ_FORBIDDEN);
   go_action = navi_loop(1);
   erase_string(174, 120,20);
   if(go_action== NAVI_RST){
      Control_mode += CM_MENU_DELTA;
      return;
    } else Z_home();
 }

 // Can't use this control mode if the lower Z-limit
 // set by the user is reached, breached or >= the max limit
 if(Zpos_minimum >= ZPOS_MAX){
   lcd.setCursor(60, 120);
   lcd.print(F("LOWER LIMIT TOO HIGH"));
   beep_signal(BZ_FORBIDDEN);
   go_action = navi_loop(1);
   erase_string(174, 120,20);
   Control_mode += CM_MENU_DELTA;
   return;
 }
 // Go to the lower limit if it is higher than the current
 // Z-position (e.g. the user just changed the limit in
 // the menu)
 check_user_min();

 
 // SETUP:
 zmotor_cm_setup:

 // Draw the HUD for Z motor ops
 HUD_panel_update(HU_ZHEAD);
 HUD_panel_update(HU_ZPOS);
 HUD_panel_update(HU_ZHOME);
 if(zmode == ZM_SWEEP) HUD_panel_update(HU_ZSWEEPON);
 else HUD_panel_update(HU_ZSWEEPOFF);
 // Draw the Z-motor control mode icon
 lcd.drawChar(DRX,DRYM, DC_CMZ, COLOUR_ZMOTOR, BLACK,1);

 lighton = 1;
 chosen_speed=Z_speed;
 super_speed = 0;
 if(!Zenergised)HUD_panel_update(HU_ZOFF);
 
 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 if(!Z_homed) goto home_check;
 switch(go_action){
    case NAVI_MID:
      if(lighton){ analogWrite(TFT_BLK,0); lighton=0;}
       else { 
         analogWrite(TFT_BLK,Bckl_level);
         lighton=1;
        }
    break;
    case NAVI_SET:
      if(Zenergised) de_energise_motor();
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      Z_speed=chosen_speed;
      return;
    break;
    case NAVI_UP:
      zud = ZDIRN_UP;
    goto move_attempt;
    case NAVI_DOWN:
      zud = ZDIRN_DOWN;
      move_attempt:
        if(lighton){HUD_panel_update(HU_ZIHIDE); HUD_panel_update(zud);}
        zmoved=1;
        if(Z_move(Z_delta, zud)!=Z_delta) beep_signal(BZ_FORBIDDEN);    
    break;
    case NAVI_LEFT:
       if(!super_speed){
         Z_speed=Z_SPEED_MAX;
         super_speed=1;
         HUD_panel_update(HU_ZCRSE);
        }
       else {
         Z_speed=chosen_speed;
         super_speed=0;
         HUD_panel_update(HU_ZFINE);
        }
    break;
    case NAVI_RIGHT:    // Toggle Zsweep / Normal
     if(zmode == ZM_NORMAL){
       zmode = ZM_SWEEP;
       HUD_panel_update(HU_ZSWEEPON);
     } else {
       HUD_panel_update(HU_ZSWEEPOFF);
       zmode = ZM_NORMAL;
     }
    break;
    case NAVI_SETRST:  // Revert to caller
      Control_mode = Previous_cm;
      analogWrite(TFT_BLK,Bckl_level);
      Z_speed=chosen_speed;
    return;
    case NAVI_SETRIGHT: // Toggle Zsweep Goto / Normal
     if(zmode == ZM_NORMAL){
       lcd.setCursor(60, 120);
       lcd.print(F("Going to start pos.n"));
       Z_move_to(Zpos_sw_start);
       erase_string(174, 120,20);
       zmode = ZM_SWEEP;
       zmoved=1;
       HUD_panel_update(HU_ZSWEEPON);
     } else {
       HUD_panel_update(HU_ZSWEEPOFF);
       zmode = ZM_NORMAL;
     }
    break;
    case NAVI_SETUP:    // Nothing or Up-sweep
     if(zmode == ZM_SWEEP){
      zud = ZDIRN_UP;
      goto sweep_attempt;
     }
    break;
    case NAVI_SETDOWN: // Homing or Down-sweep
     if(zmode == ZM_NORMAL){
       analogWrite(TFT_BLK,Bckl_level);
       HUD_panel_update(HU_ZFINE);
       Z_speed=chosen_speed;
       Z_home();
       goto zmotor_cm_setup;
     } else {
      zud = ZDIRN_DOWN;
      sweep_attempt:
      if(lighton) HUD_panel_update(zud);
      zmoved=1;
      for(idx=0;idx<Zsweep_steps;idx++){
        if(Z_move(Z_delta, zud)!=Z_delta){ beep_signal(BZ_FORBIDDEN); break;}
        timer_delay((unsigned long)Zsweep_delay);
      }
     }
    break;
    default:
    break;
   }  
  if(zmoved){
    HUD_panel_update(HU_ZPOS);
    if(Zenergised){
      if(Zenergised_always == 0) de_energise_motor();
      if(Zenergised && lighton)  HUD_panel_update(HU_ZIHIDE);
    }
    zmoved=0;    
  }

 } // end of while(1) loop

 return;
}


#define MAX_DISP 234

// Pointer / Counter / Length and Angle Control Modes
void pointer_cm(void)
{
 uint8_t go_action,nx,ny,idx,mdx;
 uint8_t redraw_pointer,ptr_speed;
 
 // SETUP:
 // Place pointer at mid FOV
 nx=Ptr_X;
 ny=Ptr_Y;
 lcd.drawChar(Ptr_X,Ptr_Y, Pointer_char,  COLOUR_POINTER, BLACK,1);
 redraw_pointer=0;
 mdx=Mkr_idx;
 ptr_speed = 4;
 Previous_cm  = Control_mode;
 Calib_fac = int_to_double(); // Update the calibration factor value.
 // Update the HUD and any measurement that is stored.
 HUD_panel_update(HU_UNITS);
 erase_string(174,DBY2,20); // Erase any previous measurement
 redraw_marker(0,mdx); 

 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 switch(go_action){
    case NAVI_MID: // Add a marker and increment count
     if(Mkr_idx == MKR_MAX){
      beep_signal(BZ_FORBIDDEN); // Limit reached - no more markers allowed
      timer_delay(150);
     } else {
      Mkr_X[Mkr_idx]=Ptr_X;
      Mkr_Y[Mkr_idx]=Ptr_Y;
      Mkr_idx++;
      mdx=Mkr_idx;
      redraw_marker(0,mdx);
     }
    break;
    case NAVI_SET:
     Ptr_mode++;  // Cycle between the
     Ptr_mode%=4;  // pointer modes.
     // Now set the HUD according to the new mode
     HUD_panel_update(HU_UNITS);
     // Force a redraw so any line or measurement
     // result appropriate to the mode is printed.
     if(Ptr_mode<=CO_COUNT) redraw_marker(2,mdx); 
      else redraw_marker(0,mdx);
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      return;
    break;
    case NAVI_UP:
     if(Ptr_Y>1){ny=Ptr_Y-1;redraw_pointer = 1;}
    break;
    case NAVI_UPLEFT:
     if(Ptr_Y>1){ny=Ptr_Y-1;redraw_pointer = 1;}
     if(Ptr_X<MAX_DISP){nx=Ptr_X+1;redraw_pointer = 1;}
    break;
    case NAVI_UPRIGHT:
     if(Ptr_Y>1){ny=Ptr_Y-1;redraw_pointer = 1;}
     if(Ptr_X>1){nx=Ptr_X-1;redraw_pointer = 1;}
    break;
    case NAVI_DOWN:
     if(Ptr_Y<MAX_DISP){ny=Ptr_Y+1;redraw_pointer = 1;}
    break;
    case NAVI_DOWNLEFT:
     if(Ptr_Y<MAX_DISP){ny=Ptr_Y+1;redraw_pointer = 1;}
     if(Ptr_X<MAX_DISP){nx=Ptr_X+1;redraw_pointer = 1;}
    break;
    case NAVI_DOWNRIGHT:
     if(Ptr_Y<MAX_DISP){ny=Ptr_Y+1;redraw_pointer = 1;}
     if(Ptr_X>1){nx=Ptr_X-1;redraw_pointer = 1;}
    break;
    case NAVI_LEFT:
     if(Ptr_X<MAX_DISP){nx=Ptr_X+1;redraw_pointer = 1;}
    break;
    case NAVI_RIGHT:
     if(Ptr_X>1){nx=Ptr_X-1;redraw_pointer = 1;}
    break;
    case NAVI_SETUP: // Increase pointer speed
     if(ptr_speed>2) ptr_speed/=2;
    break;
    case NAVI_SETDOWN: // Reduce pointer speed
     if(ptr_speed<128) ptr_speed*=2;
    break;
    case NAVI_SETRST:  // Revert to Z-motor mode
      if(swap_to_focus_control()) return;
    break;
    case NAVI_SETLEFT:  // Undo
     if(Mkr_idx == 0){
      beep_signal(BZ_FORBIDDEN); // Limit reached - no more markers to undo
     } else {
      Mkr_idx--;
      redraw_marker(0,mdx);
     }
    break;
    case NAVI_SETRIGHT:  // Re-do
     if(Mkr_idx == mdx){
      beep_signal(BZ_FORBIDDEN); // Limit reached - no more markers to re-do
     } else {
      Mkr_idx++;
      redraw_marker(0,mdx);
     }
    break;
    case NAVI_SETMID:  // Reset marker count to zero
     if(Mkr_idx > 0){
       lcd.setCursor(66, 120);
       lcd.setTextColor(YELLOW);
       lcd.print(F("REALLY DELETE ALL?"));
       beep_signal(BZ_PROMPT);
       go_action = navi_loop(1);
       erase_string(168, 120,18);
       if(go_action== NAVI_MID){
         redraw_marker(1,mdx);
         Mkr_idx = mdx = 0;
         // Now set the HUD according to the new mode
         HUD_panel_update(HU_UNITS);
        } else {
          redraw_marker(0,mdx);
        }
     } else {
      beep_signal(BZ_FORBIDDEN); // Nothing to reset
      timer_delay(150);
     }
    break;
    default: break;
   }
  if(redraw_pointer){
    uint8_t tmpx,tmpy,overdrawmarker=0;
    uint8_t minx,miny;
    // This uses the 'print' method because that allows for transparent background
    // so destroys les of whatever the pointer overlies.

    // Before updating the pointer, check if the current position
    // overlies a marker char (and so would have deleted part of it).
    for(idx=0;idx<Mkr_idx;idx++){
      if(Mkr_X[idx]<6) minx=0; else minx=Mkr_X[idx]-6;
      if(Ptr_X > minx && Ptr_X < (Mkr_X[idx]+6)){ // If there is overlap in X, check Y
        if(Mkr_Y[idx]<8) miny=0; else miny=Mkr_Y[idx]-8;
        if(Ptr_Y > miny && Ptr_Y < (Mkr_Y[idx]+8)){ // If there is overlay in Y as well ...
          tmpx=Mkr_X[idx];tmpy=Mkr_Y[idx];
          overdrawmarker=1;
          goto update_ptr;
        }
      }
    }
    
    update_ptr:
   
    lcd.setTextColor(BLACK);                        // erase previous
    lcd.setCursor((uint16_t)Ptr_X,(uint16_t)Ptr_Y);
    lcd.write(char(Pointer_char));

    Ptr_X=nx; Ptr_Y=ny;                             // update coords
    lcd.setTextColor(COLOUR_POINTER);
    lcd.setCursor((uint16_t)Ptr_X,(uint16_t)Ptr_Y);
    lcd.write(char(Pointer_char));                  // draw at new pos.
    redraw_pointer=0; 

    if(overdrawmarker){
      lcd.setTextColor(COLOUR_MARKER);
      lcd.setCursor((uint16_t)tmpx,(uint16_t)tmpy);
      lcd.write(char(Marker_char));
    }
    timer_delay(ptr_speed); 
  }

  
 }

}


// Timer Control Mode
void timer_cm(void)
{
 uint8_t go_action;
 

 // SETUP:
 Previous_cm  = Control_mode;
 // Update the HUD for Timer info
 update_timer_HUD();
 // Draw the Timer control mode icon
 //(This must be separate from update_timer_HUD because that
 // function is called to update the timer no matter what
 // control mode the user is in.
 lcd.drawChar(DRX,DRYM, DC_CMT, COLOUR_TIMER, BLACK,1);

 
 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 switch(go_action){
    case NAVI_MID:
     switch(Timer_status){
      case TM_COLD:
        if(timer_at_zero())Timer_dirn=1; else Timer_dirn=0;
      // No 'break;' because COLD is a special case of STOPPED
      case TM_STOPPED:
      Timer_status = TM_RUNNING;
      break;
      case TM_RUNNING:
      Timer_status = TM_STOPPED;
      break;
      default: break;
     }
    break;
    case NAVI_SET:
      COLOUR_TIMER = cycle_colour(COLOUR_TIMER,1);
      update_timer_HUD();
      // Draw the Timer control mode icon
      lcd.drawChar(DRX,DRYM, DC_CMT, COLOUR_TIMER, BLACK,1);
    break;
    case NAVI_SETMID:  // Reset timer to a cold start
      Timer_status=TM_COLD;
      Time_curr_hrs=Timer_hrs;
      Time_curr_min=Timer_min;
      Time_curr_sec=Timer_sec;
      update_timer_HUD();
    break;
    case NAVI_SETRST:  // Revert to Z-motor mode
      if(swap_to_focus_control()) return;
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      return;
    break;
    case NAVI_UP:
     if(Timer_status==TM_RUNNING){
      Control_mode = CM_POINTER;
      if(Graticule_showing){
        update_graticule(1);
        lcd.drawChar(DRX,DRY1, PI_BLOCK, BLACK, BLACK,1);     // Delete graticule
        lcd.drawChar(DRX-DX1,DRY1, PI_BLOCK, BLACK, BLACK,1); // Period if showing
      }
      return;      
     } else {
      Timer_min++;Timer_min%=60;
      if(Timer_status==TM_COLD){
        Time_curr_min=Timer_min;
        HUD_panel_update(HU_TMIN);
      }
     }
    break;
    case NAVI_DOWN:
     if(Timer_status==TM_RUNNING){
      Control_mode = CM_GRATICULE;
      return;      
     } else {
      if(Timer_min==0) Timer_min=59; else Timer_min--;
      if(Timer_status==TM_COLD){
        Time_curr_min=Timer_min;
        HUD_panel_update(HU_TMIN);
      }
     }
    break;
    case NAVI_LEFT:
     if(Timer_sec==0) Timer_sec=59; else Timer_sec--;
     if(Timer_status==TM_COLD){
        Time_curr_sec=Timer_sec;
        HUD_panel_update(HU_TSEC);
      }
    break;
    case NAVI_RIGHT:
     Timer_sec++;Timer_sec%=60;
     if(Timer_status==TM_COLD){
        Time_curr_sec=Timer_sec;
        HUD_panel_update(HU_TSEC);
      }
    break;
   }  
 }

}


// Graticule Control Mode
void graticule_cm(void)
{
 uint8_t go_action,lighton=1;
 uint8_t redraw_grid=0;
 double linelen;
 
 // SETUP:
 Previous_cm  = Control_mode;
 Calib_fac = int_to_double(); // Update the calibration factor value.
 if(Graticule_showing) update_graticule(1);
 update_graticule(0); // Draw the new
 linelen = (double)Graticule_period * Calib_fac;
 print_float(linelen,COLOUR_GRATICULE);
 HUD_panel_update(HU_UNITS);
 HUD_panel_update(HU_PERIOD);
 
 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 switch(go_action){
    case NAVI_MID:
      if(Graticule_showing) update_graticule(1); // Delete the existing
      if(Graticule_style == GS_GRID_1)  Graticule_style=GS_SCALE_XY;
       else Graticule_style--;
       redraw_grid=1;
    break;
    case NAVI_SET:
      COLOUR_GRATICULE = cycle_colour(COLOUR_GRATICULE,1);
      redraw_grid=3;
    break;
    case NAVI_SETRST:  // Revert to Z-motor mode
      if(swap_to_focus_control()) return;
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      return;
    break;
    case NAVI_UP:
     if(Graticule_period<50){
      if(Graticule_showing) update_graticule(1); // Delete the existing
      redraw_grid=2;
      Graticule_period++;
     } else beep_signal(BZ_FORBIDDEN);
    break;
    case NAVI_DOWN:
     if(Graticule_period>2){
      if(Graticule_showing) update_graticule(1); // Delete the existing
      redraw_grid=2;
      Graticule_period--;
     } else beep_signal(BZ_FORBIDDEN);
    break;
    case NAVI_LEFT:
     if(Graticule_showing) update_graticule(1); // Delete the graticule
     else if(lighton){ analogWrite(TFT_BLK,0); lighton=0;}
     else { 
           analogWrite(TFT_BLK,Bckl_level);
           lighton=1;
           redraw_grid=1;
          }
    break;
    case NAVI_RIGHT:
     Control_mode = CM_TIMER;
     if(lighton==0)analogWrite(TFT_BLK,Bckl_level);
     return;     
    break;
   } 

  if(redraw_grid){
    update_graticule(0); // Draw the new grid
    if(redraw_grid>1){
      HUD_panel_update(HU_PERIOD);
      linelen = Graticule_period * Calib_fac;
      print_float(linelen,COLOUR_GRATICULE);
      if(redraw_grid==3) HUD_panel_update(HU_UNITS);
    }
   redraw_grid=0;
  }
 }

}


// Plain Control Mode
void plain_cm(void)
{
 uint8_t go_action,lighton=1;

 
 // SETUP:
 Previous_cm  = Control_mode;
 lcd.fillScreen(COLOUR_PLAIN);
 
 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 switch(go_action){
    case NAVI_SET:
      COLOUR_PLAIN = cycle_colour(COLOUR_PLAIN,1);
      lcd.fillScreen(COLOUR_PLAIN);
    break;
    case NAVI_SETRST:  // Revert to Z-motor mode
      if(swap_to_focus_control()) return;
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      return;
    break;
    default:     
    break;
   } 
 }
 
}

// Aperture Control Mode
void aperture_cm(void)
{
 uint8_t go_action,redraw_aperture=0,aperture_off=0,delta_r=1;
 
 // SETUP:
 Previous_cm  = Control_mode;
 update_aperture();
 // For Aperture functions using PUMA Control you are using a SLM filter - not a HUD projector.
 // Therefore all HUD work must be disabled and backlight status is irrelevant.
 HUD_Disabled = 1;
 
 // LOOP:
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(1);
 switch(go_action){
    case NAVI_SET:   // Cycle aperture type
      if(Aperture_type == AT_BRIGHTFIELD)  Aperture_type=AT_PHASECONTRAST;
       else Aperture_type--;
      redraw_aperture=1;
    break;
    case NAVI_MID:     // Toggle aperture on/off (off = fully open plain aperture)
     if(aperture_off){ redraw_aperture=1; aperture_off=0;}
     else {lcd.fillScreen(WHITE); aperture_off=1; redraw_aperture=0;}
    break;
    case NAVI_SETMID:  // Toggle toggle darkfield superposition
     if(Aperture_dfsuperposed) Aperture_dfsuperposed=0;
     else Aperture_dfsuperposed = 1;
     redraw_aperture=1;
    break;
    case NAVI_LEFT:    // Decrement aperture change step size
      if(delta_r<=1) beep_signal(BZ_FORBIDDEN); else delta_r--;
    break;
    case NAVI_RIGHT:  // Increment aperture change step size
      if(delta_r>=64) beep_signal(BZ_FORBIDDEN); else delta_r++;
    break;
    case NAVI_UP:      // Increase radius of current aperture
      redraw_aperture=Aperture_radius_adjust(-1*(long int)delta_r);
      if(!redraw_aperture) beep_signal(BZ_FORBIDDEN); 
    break;
    case NAVI_DOWN:    // Decrease radius of current aperture
      redraw_aperture=Aperture_radius_adjust((long int)delta_r);
      if(!redraw_aperture) beep_signal(BZ_FORBIDDEN); 
    break;
    case NAVI_SETUP:    // Rotate display in -90 degree intervals
      if(Aperture_rotation==0) Aperture_rotation=3; else Aperture_rotation--;
      lcd.setRotation(Aperture_rotation);
      redraw_aperture=1;
    break;
    case NAVI_SETDOWN: // Rotate display in +90 degree intervals
      if(Aperture_rotation==3) Aperture_rotation=0; else Aperture_rotation++;
      lcd.setRotation(Aperture_rotation);
      redraw_aperture=1;
    break;
    case NAVI_SETRST:  // Revert to Z-motor mode
      if(swap_to_focus_control()) return;
    break;
    case NAVI_RST:
      Control_mode += CM_MENU_DELTA;
      return;
    break;
    default:     
    break;
   } 

  // Draw the new aperture if there has been a change
  if(redraw_aperture){
    update_aperture();
    redraw_aperture=0;
  }


 }

}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////////
 //                                                                  // 
  //                  CONTROL MODE CORE FUNCTIONS                   //
   //                                                              //
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////






    ////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\    .
   //                                                              \\   .
  //                     COMMON UTILITY FUNCTIONS                   \\  .
 //                                                                  \\ .
////////////////////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\.


// Delay function substitute for 'delay()' that allows the timer to be
// updated during delay requests
void timer_delay(unsigned long ms)
{
  unsigned long start_micros,running_micros,ms_delay;
  start_micros = running_micros = micros();
  ms_delay = 1000*ms;
  
  while((running_micros - start_micros) < ms_delay){
    update_timer();
    running_micros = micros();
  }
  return;
}

// Function to update the count-down / count-up timer
// This also updates the reading of the lamp current.

void update_timer(void)
{
 unsigned long current_micros,sec_delay;
 uint8_t idx,tmp;
 uint16_t lamp_now,lamp_col;
 float adc_val;

 // Only start checking the lamp once bootup is complete
 if(Boot_complete==1){
    
     lamp_now = (uint16_t)analogRead(LAMP_METER);
     if(lamp_now<LAMP_DISCONNECTED){
       Lamp_curr += lamp_now;
       Lamp_denom ++;
      }

   if(Lamp_denom >= 64){
      Lamp_curr /= Lamp_denom;
      adc_val = (float)Lamp_curr;
      Lamp_curr = (uint16_t)fabs(round(832.6124 + (adc_val*adc_val)*0.004993237-4.243488*adc_val));
      if(fabs((float)Lamp_curr-(float)Lamp_prev)>10.0){
        Lamp_prev=Lamp_curr;
        HUD_panel_update(HU_LAMP);
      } else if(Lamp_update) HUD_panel_update(HU_LAMP);

      if(COLOUR_LAMP != RED){ // If 'RED' alarm has already tripped - so skip this.
        tmp = Beeper_mode;
        lamp_col = COLOUR_LAMP;  // Save this so we can return to it when alarm conditions ceases
        if(Lamp_prev>350 && Lamp_prev<LAMP_DISCONNECTED){ // Too bright - sound alarm!
          Beeper_mode = BM_ON; // Override mute 
          COLOUR_LAMP = RED; // Avoid recursive feedback
          HUD_panel_update(HU_LAMP); // Ensure the current value is displayed in RED.
          beep_signal(BZ_LAMPHOT);
          timer_delay(200);
          Beeper_mode = tmp; // Undo mute override
          COLOUR_LAMP = lamp_col; // Avoid recursive feedback
        }
      }

       Lamp_curr = 0;
       Lamp_denom = 0;
      
     }
     
     
 } // End of 'if(Boot_complete)' block
 
 if(Timer_status!=TM_RUNNING) return;
  current_micros = micros();
  sec_delay = 1000000;
  if((current_micros - Timer_micros) >= sec_delay){
    
     Timer_micros = micros();
     
     if(Timer_dirn == 1){ // We are counting UP (forever)
      
      Time_curr_sec++; Time_curr_sec%=60;
      if(Time_curr_sec==0){
       Time_curr_min++; Time_curr_min%=60;
       if(Time_curr_min==0){
         Time_curr_hrs++; Time_curr_hrs%=256;
        }
      }
      
     } else { // We are counting DOWN (to zero)
      
      idx=0;
      if(Time_curr_sec==0) {
        if(Time_curr_min==0){      
          if(Time_curr_hrs==0){
             // The end has come - give the signal and stop the clock
             Timer_status=TM_COLD;
             tmp = Beeper_mode;
             switch(Timer_signal){
              case CC_BEEP:
              case CC_VISBEEP:
              Beeper_mode = BM_ON; // Override mute
              case CC_VISUAL:
                for(idx=0;idx<3;idx++){
                    if(Timer_signal!=CC_BEEP) lcd.invertDisplay(true);
                    if(Timer_signal!=CC_VISUAL) beep_signal(BZ_TIMEOUT);
                    else timer_delay(100);
                    if(Timer_signal!=CC_BEEP) lcd.invertDisplay(false);
                    if(Timer_signal!=CC_VISUAL) beep_signal(BZ_TIMEOUT);
                    else timer_delay(100);
                   }
              Beeper_mode = tmp; // Undo mute override
              break;
              case CC_NOSIGNAL:
              default: break;
             }
             Time_curr_hrs=Timer_hrs;
             Time_curr_min=Timer_min;
             Time_curr_sec=Timer_sec;
             update_timer_HUD();
             return;
            } 
          } 
          Time_curr_sec=59;
          idx=1; // flag to decrement minutes
        } else {
          Time_curr_sec--;
          idx=0;
        }
        if(idx){ // Minutes need decrementing
           if(Time_curr_min==0){
            Time_curr_min=59;
            idx=1; // flag to decrement hours
           } else {
            Time_curr_min--;
            idx=0; 
           }
           if(idx){ // Hours need decrementing
               if(Time_curr_hrs==0) Time_curr_hrs=255; else Time_curr_hrs--;
            }
         }
     
      }

     update_timer_HUD(); 
  } 
  return;
 
}


// Update the Timer display
void update_timer_HUD(void)
{
   HUD_panel_update(HU_THRS);
   HUD_panel_update(HU_TMIN);
   HUD_panel_update(HU_TSEC);
}


// Return non-zero if timer start variables are all zero
uint8_t timer_at_zero(void)
{
  if(Timer_hrs==0 && Timer_min==0 && Timer_sec==0) return 1;
  return 0;
}


// Allows caller to switch over to focus control mode if
// Z is homed.
uint8_t swap_to_focus_control(void)
{
 if(Z_homed){
        Control_mode = CM_Z_MOTOR;
        return 1;
      } else beep_signal(BZ_FORBIDDEN);
 timer_delay(150);
 return 0;
}

// Draw the currently selected graticule pattern.
// If modus is non-zero - draw it in black
void update_graticule(uint8_t modus)
{
 uint16_t colour;
 uint8_t idx,vdx,smin,gstart,gfin;
 uint16_t glen,gdiam;
 
 colour = (modus==1)?BLACK:COLOUR_GRATICULE;
 Graticule_showing = (colour==BLACK)?0:1;

 smin=(168%Graticule_period)/2; // Centre the graticule in the frame
 gstart=36+smin;                // To avoid overwriting the HUD panels
 gfin = 204-smin;
 glen = (uint16_t)(gfin-gstart);
 lcd.setTextColor(colour,BLACK);

 switch(Graticule_style){
    case GS_RULER_XY:         
     lcd.drawFastVLine(120, (uint16_t)gstart,glen,colour);           
    case GS_RULER_X:
      lcd.drawFastHLine((uint16_t) gstart, 120,glen,colour);
    break;
    case GS_SCALE_XY:
      lcd.drawFastVLine((uint16_t)gfin, (uint16_t)(gfin-Graticule_period),Graticule_period,colour);
    case GS_SCALE_X:
      lcd.drawFastHLine((uint16_t)(gfin-Graticule_period), gfin,Graticule_period,colour);
    return;
    default: break;
  }

 for(idx=gstart;idx<=gfin;idx+=Graticule_period){
  update_timer();
  switch(Graticule_style){
    case GS_GRID_1:
     lcd.drawFastVLine((uint16_t) idx, (uint16_t)gstart,glen,colour);
     lcd.drawFastHLine((uint16_t) gstart, (uint16_t)idx,glen,colour);
    break;
    case GS_GRID_2:
      for(vdx=gstart;vdx<=gfin;vdx+=Graticule_period){
        lcd.setCursor((uint16_t) idx-2, (uint16_t)vdx-3);
        lcd.print(F("+"));
        update_timer();
      }
    break;
    case GS_GRID_C:
     gdiam = (uint16_t)(gfin-idx-idx);
     lcd.drawRoundRect(120-gdiam/2,120-gdiam/2,gdiam,gdiam,gdiam/2, colour);
    break;
    case GS_RULER_XY:
      lcd.drawFastHLine(116, (uint16_t) idx,10,colour);
    case GS_RULER_X:
      lcd.drawFastVLine((uint16_t) idx, 116,10,colour);
    break;
    default: break;
  }
 }
   update_timer();
 return; 
}

// Piezo beep function.
// The pitch of the beep cannot be changed with this function (the 
// harmnonics change a little with different duty cycle values but that
// is due to physical characteristics of the piezo device, the actual
// PWM frequency does not change and I have set the duty cycle at a
// constant 127 (i.e. 50% on 50% off).
// This function has three modes (selected by the value of 'delayms'):
// 1. Turn on the beeper (and leave it on continuous beep till you call
//    the function again to turn it off)
// 2. Turn off the beeper
// 3. Give a finite duration beep of 'delayms' ms duration
// The last one is handy if you just want to make a beep and it doesn't
// matter about delaying execution of the program while the beeper is
// sounding. This is used (e.g.) at the end of the boot sequence to tell
// the user that bootup is complete.
// The component modes (separate on and off calls) are useful if you
// want the program to continue running while the beeper sounds so as
// not to delay the execution of the program.
void beep(uint16_t delayms){
  if(Beeper_mode==BM_OFF){ // The system is in mute - so no sounding 
    timer_delay(delayms); // but we still implement the delay bacuse this
    timer_delay(delayms); // aspect is used to prevent inadvertent 'double-clicks'
    return;
  }
  switch(delayms){ // delayms doubles up as a behaviour flag
    case 0: analogWrite(BUZZER, 0); break;   // turn beep off
    case 1: analogWrite(BUZZER, 127); break; // turn beep on
    default: // beep for 'delayms'ms then stop
     analogWrite(BUZZER, 127);
     timer_delay(delayms);
     analogWrite(BUZZER, 0);
     timer_delay(delayms); // Repeat the delay to avoid continuous sounding if
    break;           // the function is called multiple times to effect
  }                  // multiple beeps in a row for signalling.
     
}  

// Beeps n-number of times, each with duration 'duration'
void beep_signal(uint8_t sigtype)
{
  uint8_t idx,n;
  uint16_t duration,elapsed;

 // Get selected signal parameters
 switch(sigtype){
  case BZ_SILENCE      :
  return;
  case BZ_ACKNOWLEDGED :
   n=1;
   duration = Acklen;
  break;
  case BZ_PROMPT       :
   n=2;
   duration = 100;
  break;
  case BZ_FORBIDDEN    :
   n=3;
   duration = 50;
  break;
  case BZ_TRILL        :
   n=3;
   duration = 10;
  break;
  case BZ_LAMPHOT      :
   n=4;
   duration = 100;
  break;
  case BZ_TIMEOUT      :
   n=1;
   duration = 100;
  break;
  default:
  break;
 }

 // Make the signal sound
 for(idx=0;idx<n;idx++) beep(duration); 

 // Ensure at least 200 ms in total has elapsed
 elapsed = 2*n*duration;
 if(elapsed<200) timer_delay(200-elapsed);

 return;
}

void toggle_mute(uint8_t status){
 if(status==BM_ON){
  beep_signal(BZ_TRILL);
  Beeper_mode=BM_OFF;
 } else Beeper_mode=BM_ON;
}


void Z_home(void)
{
 uint8_t go_action;
 uint16_t tmpspeed;

  Lamp_update = 1;
        
  // Present the user with a PROMPT beep and start prompt
  lcd.setTextColor(YELLOW);
  lcd.setCursor(75, 108);
  lcd.print(F("REMOVE SPECIMEN"));
  lcd.setCursor(60, 120);
  lcd.print(F("PRESS CENTRAL BUTTON"));
  lcd.setCursor(69, 132);
  lcd.print(F("TO BEGIN Z-HOMING"));
  beep_signal(BZ_PROMPT);
 
 // LOOP:
 
 while(1){
 // NAVI-SWITCH LOOP
 go_action = navi_loop(0);
 // Clear the previous warning text
 erase_string(159, 108,15);
 erase_string(174, 120,20);
 erase_string(165, 132,17);
 switch(go_action){
    case NAVI_MID:
    goto Start_homing;
    default:
    return;
   }  
 }

 Start_homing:
  Z_homed=0;
  tmpspeed = Z_speed;

  lcd.setCursor(78, 120);
  lcd.print(F("Homing Z-Motor"));

  
  // Move down till the limit switch is tripped.
  Z_speed=Z_SPEED_MAX;
  while(1){
    if(Z_move(10000,ZDIRN_DOWN)!=10000) break;
  }

 if(Zenergised){
      if(Zenergised_always == 0) de_energise_motor();
      if(Zenergised) lcd.drawChar(DTX2,DTYE, DC_ZOFF, BLACK, BLACK,1);
    }

 // No need to go up again then down slowly (as with homing
 // a 3D printer) because even Z_SPEED_MAX is very slow in
 // this setup.

 Zpos_curr=0;
 Zlim_init = 0;
 Z_homed = 1;
 Z_speed=tmpspeed;
 erase_string(156, 120,14);
 check_user_min();
 beep_signal(BZ_PROMPT);

 return;
}

uint32_t Z_move_to(uint32_t zdestination)
{
  int8_t dirn;
  uint32_t nsteps;
  if(!Z_homed) return 0;
  if(Zpos_curr==zdestination) return Zpos_curr;
  if(zdestination > Zpos_curr){
     dirn=ZDIRN_UP;
     nsteps = zdestination - Zpos_curr;
  } else {
     dirn=ZDIRN_DOWN;
     nsteps =  Zpos_curr - zdestination;  
  }
 return Z_move(nsteps,dirn);
}

// Move the Z-motor nsteps steps in direction dirn
// return the number of steps moved (if less than
// nsteps it means a limit was reached prior to completion)
// Apply backlash correction steps as required but these do not
// contribute to the total step count.
uint32_t Z_move(uint32_t nsteps, int8_t dirn)
{
 uint32_t tsteps=0; // steps taken
 uint32_t   idx;
 if(nsteps<1) return 0;

 while(!Z_limit(dirn)){
  ZStepper_step(dirn);
  tsteps++;
  if(Z_homed){
    if(Zchangedir==1 && Zbacklash>0){
      for(idx=0;idx<Zbacklash;idx++) ZStepper_step(dirn);
    }
    if(dirn<0) Zpos_curr--; else Zpos_curr++;
  }
  if(tsteps==nsteps) break;
 }
 return tsteps;
}

// Check if a limit has been reached in Z
// Return non-zero if so.
uint8_t Z_limit(int8_t dirn)
{
 if(dirn==ZDIRN_UP){
   return (Zpos_curr < ZPOS_MAX)?0:1; 
 } else {
   if(Z_homed){
     if(Zpos_curr == 0) return 1;
     if(Zpos_curr <= Zpos_minimum) return 1;
     if(DOWN_LIMIT_TRIPPED){
      if(Zlim_init==0) return 0; else return 1;
     } else {
              Zlim_init=1;
              return 0;
            }
   }
   return (DOWN_LIMIT_TRIPPED)?1:0; 
 }
}

 // Go to the lower limit if it is higher than the current
 // Z-position (e.g. the user just changed the limit in
 // the menu)
void check_user_min(void)
{
 if(Zpos_minimum > Zpos_curr){
   lcd.setCursor(60, 120);
   lcd.print(F("Going to user's Zmin"));
   Z_move_to(Zpos_minimum);
   erase_string(174, 120,20);
 }
 return; 
}
 
void ZStepper_step(int8_t dirn)
{
 Zstep+=dirn;
 if(Zstep<0) Zstep=3;
 Zstep%=4;
 if(Zlastdir != dirn) Zchangedir=1; else Zchangedir=0;

 switch (Zstep) {
  case 0:  // 1010
    digitalWrite(ZMOTOR_PIN_1, HIGH);
    digitalWrite(ZMOTOR_PIN_2, LOW);
    digitalWrite(ZMOTOR_PIN_3, HIGH);
    digitalWrite(ZMOTOR_PIN_4, LOW);
  break;
  case 1:  // 0110
    digitalWrite(ZMOTOR_PIN_1, LOW);
    digitalWrite(ZMOTOR_PIN_2, HIGH);
    digitalWrite(ZMOTOR_PIN_3, HIGH);
    digitalWrite(ZMOTOR_PIN_4, LOW);
  break;
  case 2:  //0101
    digitalWrite(ZMOTOR_PIN_1, LOW);
    digitalWrite(ZMOTOR_PIN_2, HIGH);
    digitalWrite(ZMOTOR_PIN_3, LOW);
    digitalWrite(ZMOTOR_PIN_4, HIGH);
  break;
  case 3:  //1001
    digitalWrite(ZMOTOR_PIN_1, HIGH);
    digitalWrite(ZMOTOR_PIN_2, LOW);
    digitalWrite(ZMOTOR_PIN_3, LOW);
    digitalWrite(ZMOTOR_PIN_4, HIGH);
  break;
 }
 Zenergised = 1;
 Zlastdir = dirn;
 timer_delay(Z_speed);
}


void de_energise_motor(void)
{
if(Zenergised){
       digitalWrite(ZMOTOR_PIN_1,LOW);
       digitalWrite(ZMOTOR_PIN_2,LOW);
       digitalWrite(ZMOTOR_PIN_3,LOW);
       digitalWrite(ZMOTOR_PIN_4,LOW);
       Zenergised = 0;
       update_energised_icon();
      }
}

void update_energised_icon(void)
{
  if(Zenergised)  lcd.drawChar(DTX2,DTYE, DC_ZOFF, BLACK, BLACK,1);
         else  lcd.drawChar(DTX2,DTYE, DC_ZOFF, COLOUR_ZMOTOR, BLACK,1);
}


// Refresh the interactive drawing on the TFT and any measurement
// values. The 'modus' argument has the following meanings:
// modus == 0: Draw all current lines and marks and erase any obsolete ones
// modus == 1: Erase all lines and marks
// modus == 2: Erase all lines and any obsolete marks but draw all current marks
void redraw_marker(uint8_t modus, uint8_t mdx){
    double linelen = 0.0,dx1,dy1,dx3,dy3;
    uint16_t colour;
    uint8_t idx;

    // Set the marker colour - to erase (BLACK) or to draw
    colour = (modus==1)?BLACK:COLOUR_MARKER;
    lcd.setTextColor(colour);
    
  // First draw (or erase) lines if in a line-requring mode  
  if((Ptr_mode>CO_COUNT || modus == 2)){
    if(modus==2) colour = BLACK;
    for(idx=1;idx<Mkr_idx;idx++){
        lcd.drawLine((uint16_t)Mkr_X[idx]+2,(uint16_t)Mkr_Y[idx]+3,(uint16_t)Mkr_X[idx-1]+2,(uint16_t)Mkr_Y[idx-1]+3, colour);
      }
    }
  // Next, draw (or erase) all current marks. We draw marks in a separate
  // step after the lines to avoid them being eroded by any line erasure above.
  for(idx=0;idx<Mkr_idx;idx++){
      lcd.setCursor((uint16_t)Mkr_X[idx],(uint16_t)Mkr_Y[idx]);
      lcd.write(char(Marker_char));
    }
  // Next erase all lines and marks no longer current.
    lcd.setTextColor(BLACK);
    for(;idx<mdx;idx++){
      lcd.setCursor((uint16_t)Mkr_X[idx],(uint16_t)Mkr_Y[idx]);
      lcd.write(char(Marker_char));
      if(Ptr_mode>CO_COUNT){
        if(idx>0) lcd.drawLine((uint16_t)Mkr_X[idx]+2,(uint16_t)Mkr_Y[idx]+3,(uint16_t)Mkr_X[idx-1]+2,(uint16_t)Mkr_Y[idx-1]+3, BLACK);
      }
    }

   
   // Add measurement data to HUD as appropriateto the mode
   // Skip measurement if in count-only mode 
   if(Ptr_mode==CO_POINT){
    erase_string(174,DBY2,20); // erase old measurement value
    goto skip_measurement;
   }
   
   
   switch(Ptr_mode){
      case CO_COUNT  : // HUD for number of objects
       if(modus==1) Mkr_idx=0; // Special case for delete-all mode
       print_int_string(DBX1,DBY2,(void *)&Mkr_idx,UINT8,0,COLOUR_MARKER); 
      break;
      case CO_LENGTH : // HUD for calibrated length measurements
        // Print a zero value measrement if in delete mode.
        if(modus==1)goto print_angle; 
        // Calculate the  current length of the polyline in pixels
        if(Mkr_idx==0) goto print_line_len; // Length of a single point is 0
        dx1=(double)Mkr_X[0];
        dy1=(double)Mkr_Y[0];
        for(idx=1;idx<Mkr_idx;idx++){
          dx1=fabs(dx1-(double)Mkr_X[idx]);
          dy1=fabs(dy1-(double)Mkr_Y[idx]);
          linelen += sqrt(dx1*dx1+dy1*dy1); // Euclidean length of line segment by Pythagoras
          dx1=(double)Mkr_X[idx];
          dy1=(double)Mkr_Y[idx];
         }
        // Convert to calibrated units
        linelen *= Calib_fac;
        // Print the result
        print_line_len:
        print_float(linelen,COLOUR_MARKER);
      break;
      case CO_ANGLE  : // HUD for degrees measurements
        // Print a zero value measrement if in delete mode.
        if(modus==1)goto print_angle; 
        if(Mkr_idx<3){beep_signal(BZ_FORBIDDEN); goto print_angle;} // Need at least 3 points for an angle
        // Select the last three points and shift so the middle point is at the XY origin
        linelen = (double)Mkr_X[Mkr_idx-2];        // X component of P2
        dx1 =  (double)Mkr_X[Mkr_idx-3] - linelen; // X component of P1' 
        dx3 =  (double)Mkr_X[Mkr_idx-1] - linelen; // X component of P3' 
        linelen = (double)Mkr_Y[Mkr_idx-2];        // Y component of P2
        dy1 =  (double)Mkr_Y[Mkr_idx-3] - linelen; // Y component of P1' 
        dy3 =  (double)Mkr_Y[Mkr_idx-1] - linelen; // Y component of P3' 
        // Check that neither P1' nor P3' are are 0,0
        // [All coordinates are integers so it will suffice to check if <0.5]
        if(fabs(dx1)<0.5 && fabs(dy1)<0.5){beep_signal(BZ_FORBIDDEN); break;}
        if(fabs(dx3)<0.5 && fabs(dy3)<0.5){beep_signal(BZ_FORBIDDEN); break;}
        // Now we know it is safe and meaningful to compute an angle:
        linelen = atan2(dy1,dx1)-atan2(dy3,dx3);
        // Remove the sign and convert to degrees in the range 0 - 180:
        linelen = (360.0*fabs(linelen))/(2.0*3.14159265358979);
        if(linelen>180.0) linelen = 360.0 - linelen;
        // Now print it:
        print_angle:
        print_float(linelen,COLOUR_MARKER);        
      break;
      default: break;
     }

   skip_measurement:
    return;
  }


uint8_t Aperture_radius_adjust(long int delta)
{
  long int tmpradius;

  tmpradius=(Aperture_type==AT_DARKFIELD)?(long int)Aperture_dfradius:(long int)Aperture_radius;
  if(delta>0){if(tmpradius==120)return 0;}
  else if(tmpradius<delta)return 0;
  tmpradius+=delta; 
  if(tmpradius>120 || tmpradius<0) return 0;
  if(Aperture_type==AT_DARKFIELD)Aperture_dfradius=(uint16_t)tmpradius; else Aperture_radius=(uint16_t)tmpradius;
  return 1;
}


// Draws an aperture function
void update_aperture(void)
{
  uint16_t double_rad;
  
  switch(Aperture_type){
    case AT_BRIGHTFIELD :
    case AT_PHASECONTRAST:
      lcd.fillScreen(COLOUR_AP_BG);
      double_rad = Aperture_radius+Aperture_radius;
      lcd.fillRoundRect(0+Aperture_radius,0+Aperture_radius,240-double_rad,240-double_rad,120-Aperture_radius, COLOUR_AP_FG);
      if(Aperture_type == AT_PHASECONTRAST){
        switch(Aperture_rotation){
          case 0: lcd.fillRect(0,0,120,240, COLOUR_AP_BG); break;
          case 1: lcd.fillRect(120,0,120,240, COLOUR_AP_BG); break;
          case 2: lcd.fillRect(0,0,240,120, COLOUR_AP_BG); break;
          case 3: lcd.fillRect(0,120,240,120, COLOUR_AP_BG); break;
        }

       }
    if(!Aperture_dfsuperposed) break;
    case AT_DARKFIELD:
      double_rad = Aperture_dfradius+Aperture_dfradius;
      if(!Aperture_dfsuperposed) lcd.fillScreen(COLOUR_AP_FG); 
      lcd.fillRoundRect(0+Aperture_dfradius,0+Aperture_dfradius,240-double_rad,240-double_rad,120-Aperture_dfradius, COLOUR_AP_CS);
    break;
    default:
    break;
  }
}


// Draws a ring around the periphery of the FOV
void peripheral_ring(uint16_t col)
{
  lcd.drawRoundRect(0,0,240,240,120, col);
  lcd.drawRoundRect(1,1,238,238,119, col);
  lcd.drawRoundRect(2,2,236,236,118, col);
}


void print_colour_option(uint8_t m_level)
{
 lcd.setCursor(X_MOD-5*DX1,Y_MO);
 lcd.print(F("Colour"));
 menu_value_update(m_level,0,0);
}

// Get a navigation button signal
// If srduo is non-zero then the function checks for
// the simultaneous press of SET + RST by implementing a 500ms
// delay subloop on detecting SET to look to see if RST
// is also pressed. If RST is pressed in this time then
// the NAVI_SETRST signal is given, otherwise the simple
// NAVI_SET signal is returned. The reason this is made an
// option is so that any function not requiring this double
// signal doesn't have to put up with the half second delay.
// Double signals of the joystick are looked for before
// single signals (otherwise doubles would never be
// detected unless they were pressed absolutely simultaneously.
uint8_t navi_loop(uint8_t srduo)
{
 uint8_t returnval,acksignal;

 acksignal = BZ_ACKNOWLEDGED;
 
 while(1){

  if(!digitalRead(NAVI_MID)){ returnval = NAVI_MID; break;}
  if(!digitalRead(NAVI_UP)){
    if(!digitalRead(NAVI_LEFT))  { returnval = NAVI_UPLEFT; break;}
    if(!digitalRead(NAVI_RIGHT)) { returnval = NAVI_UPRIGHT; break;}
    returnval = NAVI_UP; break;
  }
  if(!digitalRead(NAVI_DOWN)){
    if(!digitalRead(NAVI_LEFT))  { returnval = NAVI_DOWNLEFT; break;}
    if(!digitalRead(NAVI_RIGHT)) { returnval = NAVI_DOWNRIGHT; break;}
    returnval = NAVI_DOWN; break;
  }
  if(!digitalRead(NAVI_LEFT)){
    if(!digitalRead(NAVI_UP))    { returnval = NAVI_UPLEFT; break;}
    if(!digitalRead(NAVI_DOWN))  { returnval = NAVI_DOWNLEFT; break;}
    returnval = NAVI_LEFT; break;
  }
  if(!digitalRead(NAVI_RIGHT)) {
    if(!digitalRead(NAVI_UP))    { returnval = NAVI_UPRIGHT; break;}
    if(!digitalRead(NAVI_DOWN))  { returnval = NAVI_DOWNRIGHT; break;}
    returnval = NAVI_RIGHT; break;
  }
  if(!digitalRead(NAVI_SET)){
    if(srduo){
      unsigned long startmillis; 
      unsigned long currentmillis=0; 
      startmillis = millis();
      while(currentmillis < 500){
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_RST))   { returnval = NAVI_SETRST; goto nl_exit;}
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_DOWN))  { returnval = NAVI_SETDOWN; goto nl_exit;}
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_UP))    { returnval = NAVI_SETUP; goto nl_exit;}
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_LEFT))  { returnval = NAVI_SETLEFT; goto nl_exit;}
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_RIGHT)) { returnval = NAVI_SETRIGHT; goto nl_exit;}
        if(!digitalRead(NAVI_SET) && !digitalRead(NAVI_MID))   { returnval = NAVI_SETMID; goto nl_exit;}
        currentmillis = millis()-startmillis;
      }
    }
    returnval = NAVI_SET; break;
  }
  if(!digitalRead(NAVI_RST))   { returnval = NAVI_RST; break;}
  update_timer();
 }
 
 nl_exit:

 // Generate audible feedback to acknowledge key press 
 // (unless moving pointer around or focus up / down)
 
 if(Control_mode==CM_POINTER){
  switch(returnval){
    case NAVI_UP      :
    case NAVI_DOWN    :
    case NAVI_LEFT    :
    case NAVI_RIGHT   :
    case NAVI_UPLEFT  :
    case NAVI_UPRIGHT :
    case NAVI_DOWNLEFT  :
    case NAVI_DOWNRIGHT :
    acksignal = BZ_SILENCE;
    break;
    default:
    break;
  }
 }
 if(Control_mode==CM_Z_MOTOR){
  switch(returnval){
    case NAVI_UP      :
    case NAVI_DOWN    :
     acksignal = BZ_SILENCE;
    break;
    default:
    break;
  }
 }
 
 beep_signal(acksignal); 
 
 return returnval;
}


// Increments / decrements an integer value with a non-negative
// minimum limit
void change_int(void *iptr,uint8_t itype,uint16_t idelta,uint16_t imin,uint32_t imax,uint8_t dirn)
{
 uint16_t u16val;
 uint32_t u32val;
 uint8_t u8val;
 
 switch(itype){
  case UINT16:
   u16val = *(uint16_t *)iptr;
   if(dirn){
    if((uint16_t)u16val >= (uint16_t)(UINT16_MAX - idelta)) u16val=(uint16_t)imax; 
    else u16val+=(uint16_t)idelta;
    if(u16val>(uint16_t)imax) u16val=(uint16_t)imax;
   } else {
    if((uint16_t)u16val <= (uint16_t)(idelta+imin)) u16val=(uint16_t)imin; 
    else u16val -= (uint16_t)idelta;
   }
   *(uint16_t *)iptr = u16val;
  break;
  case UINT32:
   u32val = *(uint32_t *)iptr;
   if(dirn){
    if((uint32_t)u32val >= (uint32_t)(UINT32_MAX - idelta)) u32val=(uint32_t)imax; 
    else u32val+=(uint32_t)idelta;
    if(u32val>(uint32_t)imax) u32val=(uint32_t)imax;
   } else {
    if((uint32_t)u32val <= (uint32_t)(idelta+imin)) u32val=(uint32_t)imin; 
    else u32val -= (uint32_t)idelta;
   }
   *(uint32_t *)iptr = u32val;
  break;
  case UINT8:
   u8val = *(uint8_t *)iptr;
   if(dirn){
    if(idelta>100)u8val=255;
    else if((uint8_t)u8val >= (uint8_t)(255 - idelta)) u8val=(uint8_t)imax; 
    else u8val+=(uint8_t)idelta;
    if(u8val>(uint8_t)imax) u8val=(uint8_t)imax;
   } else {
    if((uint8_t)u8val <= (uint8_t)(idelta+imin)) u8val=(uint8_t)imin; 
    else u8val -= (uint8_t)idelta;
   }
   *(uint8_t *)iptr = u8val;
  break;
  default: break; 
 }
 
}

uint16_t colour_idx(uint16_t colour)
// Generate a fixed integer index to use with a colour switch
// statement. This is needed because the CustomColour is a variable
// so cannot be a member of a switch list like the constant fixed
// colours can. It works because we forbid CustomColour to take
// any of the fixed colour values (otherwise we could get stuck
// in a local closed loop situation)
{
   switch(colour){
    case BLACK:   return C_BLACK;
    case BLUE:    return C_BLUE;
    case RED:     return C_RED;
    case GREEN:   return C_GREEN;
    case CYAN:    return C_CYAN;
    case MAGENTA: return C_MAGENTA;
    case YELLOW:  return C_YELLOW;
    case WHITE:   return C_WHITE;
    default:      return C_CUST;
  }

 return C_CUST;
}

// Cycle the colour value
uint16_t cycle_colour(uint16_t colour,uint8_t dirn)
{
 uint16_t return_colour;
 // Colour values are: 0, 31, 63488, 2016, 2047, 63519, 65504, 65535, CustomColour
 if(dirn){ // 'up' from black to white to CustomColour
  switch(colour_idx(colour)){
    case C_BLACK:
     return_colour = BLUE;
    break;
    case C_BLUE:
     return_colour = RED;
    break;
    case C_RED:
     return_colour = GREEN;
    break;
    case C_GREEN:
     return_colour = CYAN;
    break;
    case C_CYAN:
     return_colour = MAGENTA;
    break;
    case C_MAGENTA:
     return_colour = YELLOW;
    break;
    case C_YELLOW:
     return_colour = WHITE;
    break;
    case C_WHITE:
     return_colour = CustomColour;
    break;
    case C_CUST:
    default:
     return_colour = BLACK;
    break;
  }
 } else { // 'down' from CustomColour to white to black
  switch(colour_idx(colour)){
    case C_BLACK:
     return_colour = CustomColour;
    break;
    case C_CUST:
     return_colour = WHITE;
    break;
    case C_BLUE:
     return_colour = BLACK;
    break;
    case C_RED:
     return_colour = BLUE;
    break;
    case C_GREEN:
     return_colour = RED;
    break;
    case C_CYAN:
     return_colour = GREEN;
    break;
    case C_MAGENTA:
     return_colour = CYAN;
    break;
    case C_YELLOW:
     return_colour = MAGENTA;
    break;
    case C_WHITE:
    default:
     return_colour = YELLOW;
    break;
  }
 }
  return return_colour;
}
// Cycle the Pointer character
uint8_t cycle_pointer_char(uint8_t pchar,uint8_t dirn)
{
 uint8_t return_char;
 if(dirn){ // 'up'
  switch(pchar){
    case PC_ARROW_1:
     return_char = PC_ARROW_2;
    break;
    case PC_ARROW_2:
     return_char = PC_ARROW_3;
    break;
    case PC_ARROW_3:
     return_char = PC_ARROW_4;
    break;
    case PC_ARROW_4:
     return_char = PC_ARRHD_1;
    break;
    case PC_ARRHD_1:
     return_char = PC_ARRHD_2;
    break;
    case PC_ARRHD_2:
     return_char = PC_ARRHD_3;
    break;
    case PC_ARRHD_3:
     return_char = PC_ARRHD_4;
    break;
    case PC_ARRHD_4:
     return_char = PC_ARRHD_5;
    break;
    case PC_ARRHD_5:
     return_char = PC_ARRHD_6;
    break;
    case PC_ARRHD_6:
     return_char = PC_CURSR_1;
    break;
    case PC_CURSR_1:
     return_char = PC_CURSR_2;
    break;
    case PC_CURSR_2:
    default:
     return_char = PC_ARROW_1;
    break;
  }
 } else { // 'down' 
  switch(pchar){
    case PC_ARROW_1:
     return_char = PC_CURSR_2;
    break;
    case PC_ARROW_2:
     return_char = PC_ARROW_1;
    break;
    case PC_ARROW_3:
     return_char = PC_ARROW_2;
    break;
    case PC_ARROW_4:
     return_char = PC_ARROW_3;
    break;
    case PC_ARRHD_1:
     return_char = PC_ARROW_4;
    break;
    case PC_ARRHD_2:
     return_char = PC_ARRHD_1;
    break;
    case PC_ARRHD_3:
     return_char = PC_ARRHD_2;
    break;
    case PC_ARRHD_4:
     return_char = PC_ARRHD_3;
    break;
    case PC_ARRHD_5:
     return_char = PC_ARRHD_4;
    break;
    case PC_ARRHD_6:
     return_char = PC_ARRHD_5;
    break;
    case PC_CURSR_1:
     return_char = PC_ARRHD_6;
    break;
    case PC_CURSR_2:
    default:
     return_char = PC_CURSR_1;
    break;
  }
 }
  return return_char;
}

// Cycle the Marker character
uint8_t cycle_marker_char(uint8_t pchar,uint8_t dirn)
{
 uint8_t return_char;
 if(dirn){ // 'up'
  switch(pchar){
    case MC_ASTER_1:
     return_char = MC_ASTER_2;
    break;
    case MC_ASTER_2:
     return_char = MC_CIRCLE;
    break;
    case MC_CIRCLE:
     return_char = MC_HASH;
    break;
    case MC_HASH:
     return_char = MC_CROSS;
    break;
    case MC_CROSS:
     return_char = MC_PIXEL;
    break;
    case MC_PIXEL:
    default:
     return_char = MC_ASTER_1;
    break;
  }
 } else { // 'down' 
  switch(pchar){
    case MC_ASTER_1:
     return_char = MC_PIXEL;
    break;
    case MC_PIXEL:
     return_char = MC_CROSS;
    break;
    case MC_CROSS:
     return_char = MC_HASH;
    break;
    case MC_HASH:
     return_char = MC_CIRCLE;
    break;
    case MC_CIRCLE:
     return_char = MC_ASTER_2;
    break;
    case MC_ASTER_2:
    default:
     return_char = MC_ASTER_1;
    break;
  }
 }
  return return_char;
}

// Update menu VALUE on the GUI
void menu_value_update(uint8_t m_level,uint8_t m_oline_min, uint8_t m_oline_max)
{
 uint8_t m_oline;
 uint16_t Y_moline;

 for(m_oline = m_oline_min; m_oline <=m_oline_max; m_oline++){

 Y_moline = Y_MO+m_oline*DY_MO;
 
 switch(m_level){
   case ML_MAIN    : // max_olines = 3;
     switch(m_oline){
      case 0: // First option value
         switch(Control_mode - CM_MENU_DELTA){
          case CM_Z_MOTOR      :  
            lcd.setCursor(X_MOV-6*DX1,Y_moline);
            lcd.print(F("Z-Motor"));
          break;
          case CM_POINTER      :
            lcd.setCursor(X_MOV-6*DX1,Y_moline);
            lcd.print(F("Pointer"));
          break;
          case CM_TIMER        :
            lcd.setCursor(X_MOV-4*DX1,Y_moline);
            lcd.print(F("Timer"));
          break;
          case CM_GRATICULE    :
            lcd.setCursor(X_MOV-8*DX1,Y_moline);
            lcd.print(F("Graticule"));
          break;
          case CM_PLAIN        :
            lcd.setCursor(X_MOV-4*DX1,Y_moline);
            lcd.print(F("Plain"));
          break;
          case CM_APERTURE     :
            lcd.setCursor(X_MOV-7*DX1,Y_moline);
            lcd.print(F("Aperture"));
          break;
          default: break;
         }
      break;
      case 1: // Second option value
      lcd.setCursor(X_MOV-4*DX1,Y_moline);
      lcd.print(F("[...]"));
      break;
      case 2: // Second option value
      lcd.setCursor(X_MOV-4*DX1,Y_moline);
      lcd.print(F("[...]"));
      break;
      default:break;
     }
   break;
   case ML_ZMOTOR  : // max_olines = 9; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_ZMOTOR,m_oline);
      break;
      case 1: // Second option value
        print_int_string(X_MOV,Y_moline,(void *)&Z_speed,UINT16,0,WHITE);
      break;
      case 2: // Third option value
        print_int_string(X_MOV,Y_moline,(void *)&Z_delta,UINT32,0,WHITE);
      break;
      case 3: // Fourth option value
        print_int_string(X_MOV,Y_moline,(void *)&Zpos_minimum,UINT32,0,WHITE);
      break;
      case 4: // Fifth option value
        print_int_string(X_MOV,Y_moline,(void *)&Zpos_sw_start,UINT32,0,WHITE);
      break;
      case 5: // Sixth option value
        print_int_string(X_MOV,Y_moline,(void *)&Zsweep_steps,UINT32,0,WHITE);
      break;
      case 6: // Seventh option value
        print_int_string(X_MOV,Y_moline,(void *)&Zsweep_delay,UINT32,0,WHITE);
      break;
      case 7: // Eighth option value
        print_int_string(X_MOV,Y_moline,(void *)&Zbacklash,UINT32,0,WHITE);
      break;
      case 8: // Ninth option value
        print_int_string(X_MOV,Y_moline,(void *)&Zenergised_always,UINT8,0,WHITE);
      break;
      default:break;
     }   
   break;
   case ML_POINTER : // max_olines = 4; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_POINTER,m_oline);
      break;
      case 1: // Second option value
       lcd.drawChar(X_MOV-24,Y_moline, Pointer_char, WHITE, BLACK,1);
      break;
      case 2: // First option value
        update_colour_string(COLOUR_MARKER,m_oline);
      break;
      case 3: // Second option value
       lcd.drawChar(X_MOV-24,Y_moline, Marker_char, WHITE, BLACK,1);
      break;
      default:break;
     }
   break;
   case ML_TIMER   : // max_olines = 5; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_TIMER,m_oline);
      break;
      case 1: // First option value
        print_int_string(X_MOV,Y_moline,(void *)&Timer_hrs,UINT8,0,WHITE);
      break;
      case 2: // First option value
        print_int_string(X_MOV,Y_moline,(void *)&Timer_min,UINT8,0,WHITE);
      break;
      case 3: // Second option value
        print_int_string(X_MOV,Y_moline,(void *)&Timer_sec,UINT8,0,WHITE);
      break;
      case 4: // Third option value
         switch(Timer_signal){
          case CC_NOSIGNAL      :  
            lcd.setCursor(X_MOV-6*DX1,Y_moline);
            lcd.print(F("Nothing"));
          break;
          case CC_VISUAL      :  
            lcd.setCursor(X_MOV-9*DX1,Y_moline);
            lcd.print(F("Flash only"));
          break;
          case CC_VISBEEP      :  
            lcd.setCursor(X_MOV-11*DX1,Y_moline);
            lcd.print(F("Flash + Beep"));
          break;
          case CC_BEEP      :  
            lcd.setCursor(X_MOV-8*DX1,Y_moline);
            lcd.print(F("Beep only"));
          break;
          default: break;
         }
      break;
      default:break;
     }   
   break;
   case ML_GRATIC  : // max_olines = 2; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_GRATICULE,m_oline);
      break;
      case 1: // Second option value
         switch(Graticule_style){
          case GS_GRID_1      :  
            lcd.setCursor(X_MOV-10*DX1,Y_moline);
            lcd.print(F("Square grid"));
          break;
          case GS_GRID_2      :  
            lcd.setCursor(X_MOV-9*DX1,Y_moline);
            lcd.print(F("Cross grid"));
          break;
          case GS_GRID_C      :  
            lcd.setCursor(X_MOV-6*DX1,Y_moline);
            lcd.print(F("Circles"));
          break;
          case GS_RULER_X      :  
            lcd.setCursor(X_MOV-6*DX1,Y_moline);
            lcd.print(F("Ruler X"));
          break;
          case GS_RULER_XY      :  
            lcd.setCursor(X_MOV-7*DX1,Y_moline);
            lcd.print(F("Ruler XY"));
          break;
          case GS_SCALE_X      :  
            lcd.setCursor(X_MOV-10*DX1,Y_moline);
            lcd.print(F("Scale bar X"));
          break;
          case GS_SCALE_XY     :  
            lcd.setCursor(X_MOV-11*DX1,Y_moline);
            lcd.print(F("Scale bar XY"));
          break;
          default: break;
         }
      break;
      default:break;
     }   
   break;
   case ML_PLAIN  : // max_olines = 1; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_PLAIN,m_oline);
      break;
      default:break;
     }   
   break;
   case ML_APERT  : // max_olines = 4; 
     switch(m_oline){
      case 0: // First option value
        update_colour_string(COLOUR_AP_FG,m_oline);
      break;
      case 1: // Second option value
        update_colour_string(COLOUR_AP_BG,m_oline);
      break;
      case 2: // Third option value
        update_colour_string(COLOUR_AP_CS,m_oline);
      break;
      case 3: // Fourth option value
         switch(Aperture_type){
          case AT_BRIGHTFIELD      :  
            lcd.setCursor(X_MOV-10*DX1,Y_moline);
            lcd.print(F("Brightfield"));
          break;
          case AT_DARKFIELD        :  
            lcd.setCursor(X_MOV-8*DX1,Y_moline);
            lcd.print(F("Darkfield"));
          break;
          case AT_PHASECONTRAST   :  
            lcd.setCursor(X_MOV-11*DX1,Y_moline);
            lcd.print(F("Phase cntrst"));
          break;
          default: break;
         }
      break;
      default:break;
     }   
   break;
   case ML_DEFAULT : // max_olines = 8; 
     switch(m_oline){
      case 0: // First option value
         switch(Beeper_mode){
          case BM_OFF      :  
            lcd.setCursor(X_MOV-2*DX1,Y_moline);
            lcd.print(F("Off"));
          break;
          case BM_ON      : 
          default: 
            lcd.setCursor(X_MOV-1*DX1,Y_moline);
            lcd.print(F("On"));
          break;
         }
      break;
      case 1: // Second option value
        print_int_string(X_MOV,Y_moline,(void *)&Bckl_level,UINT8,0,WHITE);
      break;
      case 2: // Third option value
        print_int_string(X_MOV,Y_moline,(void *)&Calib_int,UINT16,0,WHITE);
      break;
      case 3: // Fourth option value
        lcd.drawChar(X_MOV,Y_moline, 0x2e, WHITE, BLACK,1); // Decimal point
        print_int_string(X_MOV-DX1,Y_moline,(void *)&Calib_dec,UINT16,4,WHITE);
      break;
      case 4: // Fifth option value
         switch(Calib_unit){
          case CU_MICROM      :  
            lcd.setCursor(X_MOV-10*DX1,Y_moline);
            lcd.print(F("Micrometers"));
          break;
          case CU_NANOM      :  
            lcd.setCursor(X_MOV-9*DX1,Y_moline);
            lcd.print(F("Nanometers"));
          break;
          case CU_MILLIM      :  
            lcd.setCursor(X_MOV-10*DX1,Y_moline);
            lcd.print(F("Millimeters"));
          break;
          case CU_ARBIT      :  
            lcd.setCursor(X_MOV-8*DX1,Y_moline);
            lcd.print(F("Arbitrary"));
          break;
          default: break;
         }
      break;
      case 5: // Sixth option value
        update_colour_string(COLOUR_LAMP,m_oline);
      break;
      case 6: // Seventh option value
        print_int_string(X_MOV,Y_moline,(void *)&CustomColour,UINT16,0,WHITE);
      break;
      case 7: // Eighth option value
         if(Boot_complete==2){  
            lcd.setCursor(X_MOV-1*DX1,Y_moline);
            lcd.print(F("No"));
         } else {
            lcd.setCursor(X_MOV-2*DX1,Y_moline);
            lcd.print(F("Yes"));
         }
      break;
      default:break;
     }   
   break;
   default: break;
 }

 }
 
 }


// Print an integer to the screen from right to left without dynamic
// memory allocation of strings. Only positive integers (or 0) of
// a limited range of types are allowed in this function. 
// 'prepad' is an integer dictating the number of zeros to prepend to
// force the printed number to have this minimum number of digits. It
// has no effect if the input integer has >= prepad digits already.
// This facility is useful for printing the fractional part of float
// numbers e.g. 2 can be printed as 002 which is necessary if you want
// to print this as the decimal part of "5.002"
// The function returns the number of digits in an integer.
// No actual printing will be done if y>240
uint8_t print_int_string(
  uint16_t startx,
  uint16_t y,
  void *iptr,
  int8_t itype,
  uint8_t prepad,
  uint16_t colour)
{
  uint32_t ulnum,ulcpy,digit;
  uint8_t idx,lenstr;

 // First convert the input integer to a uint32_t so we
 // repetition for various type. We don't use negative numbers in this
 // program so no need to cater for those.
 switch(itype){
  case UINT16:
   ulnum = (uint32_t)(*(uint16_t *)iptr);
  break;
  case UINT32:
   ulnum = (*(uint32_t *)iptr);
  break;
  case UINT8:
   ulnum = (uint32_t)(*(uint8_t *)iptr);
  break;
  default: return; // Can't do anything if not one of the above.
 }


 // Find the length of the required string
 idx=1;
 ulcpy=ulnum;
 if(ulnum>9) 
  {
   for(;idx<254;idx++){
     ulcpy/=10;
     if(ulcpy==0) break;
    }
  }

 lenstr=idx;

 // Put the cursor to the left by the appropriate amount
 startx-=(DX1*lenstr);
 // Adjust further if prepadding is requested and padding needed  
 if(prepad>lenstr){
 if(y>240) return prepad;
  prepad-=lenstr;
  startx-=(DX1*prepad);
 } else prepad=0;

 if(y>240) return lenstr;

 // Print 1 digit at a time starting with the least at the left
 ulcpy=ulnum; startx+=DX1;
 for(idx=0;idx<lenstr;idx++,ulcpy/=10){
    digit=ulcpy-10*(ulcpy/10);
    lcd.drawChar(startx,y, 0x30+(char)digit, colour, BLACK,1);
    startx+=DX1;
   }

 // Prepad with zeros if requested and padding needed  
 if(prepad){
  for(idx=0;idx<prepad;idx++){
    lcd.drawChar(startx,y, 0x30, colour, BLACK,1);
    startx+=DX1;
   }
 } 


 return 0;
}

// Special case function to convert the calibration factor from
// separate integer and decimal components into a proper floating
// point number for use in calibration calculations.
double int_to_double(void)
{
  uint32_t uicpy,digit;
  uint8_t idx,lenstr;
  double divisor = 0.0001,dresult = 0.0;

 lenstr=4; // We know that Calib_dec has only 4 sig. digits

 // Process 1 digit at a time starting with the least at the left
 uicpy=(uint32_t)Calib_dec;
 for(idx=0;idx<lenstr;idx++,uicpy/=10){
    digit=uicpy-10*(uicpy/10);
    dresult+=divisor*(double)digit;
    divisor*=10.0;
   }

 return (dresult+(double)Calib_int);
}

// Special case function to print a floating point number
void print_float(double fnum,uint16_t colour)
{
 uint16_t intpart, decpart;
 double dx1, dy1;
 uint8_t idx;

  erase_string(174,DBY2,20); // erase previous
  dx1=modf(fnum,&dy1);
  intpart = (uint16_t)dy1;
  decpart = (uint16_t)round(1e4*dx1);
//  decpart = (uint16_t)(10000.0*dx1);
  print_int_string(DBX1,DBY2,(void *)&decpart,UINT16,4,colour); 
  lcd.drawChar(DBX1+DX1,DBY2, 0x2e, colour, BLACK,1); // '.'
  idx=print_int_string(0,255,(void *)&intpart,UINT16,0,colour);
  idx*=DX1;
  print_int_string(DBX1+DX1+idx,DBY2,(void *)&intpart,UINT16,0,colour); 
 return;
}

void update_colour_string(uint16_t colour, uint16_t m_oline)
{
 uint16_t Y_moline;

 Y_moline = Y_MO+m_oline*DY_MO;
 
 switch(colour){
  case BLACK   :  
    lcd.setCursor(X_MOV-4*DX1,Y_moline);
    lcd.print(F("Black"));
  break;
  case BLUE    :
    lcd.setCursor(X_MOV-3*DX1,Y_moline);
    lcd.print(F("Blue"));
  break;
  case RED     :
    lcd.setCursor(X_MOV-2*DX1,Y_moline);
    lcd.print(F("Red"));
  break;
  case GREEN   :
    lcd.setCursor(X_MOV-4*DX1,Y_moline);
    lcd.print(F("Green"));
  break;
  case CYAN    :
    lcd.setCursor(X_MOV-3*DX1,Y_moline);
    lcd.print(F("Cyan"));
  break;
  case MAGENTA :
    lcd.setCursor(X_MOV-6*DX1,Y_moline);
    lcd.print(F("Magenta"));
  break;
  case YELLOW  :
    lcd.setCursor(X_MOV-5*DX1,Y_moline);
    lcd.print(F("Yellow"));
  break;
  case WHITE   :
    lcd.setCursor(X_MOV-4*DX1,Y_moline);
    lcd.print(F("White"));
  break;
  default:
    lcd.setCursor(X_MOV-5*DX1,Y_moline);
    lcd.print(F("Custom"));
  break;
 }
}

// Erase a string on the screen
void erase_string(uint16_t x,uint16_t y,uint8_t nchars)
{
  uint8_t idx;
  for(idx=0;idx<nchars;idx++,x-=DX1){
    lcd.drawChar(x,y, CH_BLOCK, BLACK, BLACK,1);
  }
}


// Display the Main HUD Info Panels
void HUD_panel_update(int8_t huditem)
{
  uint8_t lenint;
  uint16_t colour;

 // HUD should not be updated by this function unless bootup is complete:
 if(Boot_complete==0) return;

 // No HUD in aperture mode
 if(HUD_Disabled) return;

 // Display HUD items as appropriate to the current control mode
 switch(Control_mode){ 
  case CM_Z_MOTOR:
   switch(huditem){
    case HU_ZHEAD   :
        lcd.setCursor(DTX1,DTY2);
        lcd.setTextColor(COLOUR_ZMOTOR,BLACK);
        lcd.print(F("Z:")); 
      break;
    case HU_ZOFF  :
       update_energised_icon();
      break;
    case HU_ZHOME  :
       if(!Z_homed) lcd.drawChar(DTX4,DTYE, CH_BLOCK, BLACK, BLACK,1);
       else lcd.drawChar(DTX4,DTYE, DC_ZHOME, COLOUR_ZMOTOR, BLACK,1);
      break;
    case HU_ZPOS    :
       erase_string(DTX2,DTY2,10);
       print_int_string(DTX2,DTY2,(void *)&Zpos_curr,UINT32,0,COLOUR_ZMOTOR); 
      break;
    case HU_ZIUP    :
       lcd.drawChar(DTX2,DTYU, DC_ZUP, COLOUR_ZMOTOR, BLACK,1);
      break;
    case HU_ZIDOWN  :
       lcd.drawChar(DTX2,DTYD, DC_ZDOWN, COLOUR_ZMOTOR, BLACK,1);
      break;
    case HU_ZIHIDE  :
       lcd.drawChar(DTX2,DTYE, CH_BLOCK, BLACK, BLACK,1);
      break;
    case HU_ZCRSE  :
        lcd.drawChar(DTX5,DTYE, DC_ZCRSE, COLOUR_ZMOTOR, BLACK,1);
      break;
    case HU_ZFINE  :
       lcd.drawChar(DTX5,DTYE, DC_ZCRSE, BLACK, BLACK,1);
      break;
    case HU_ZSWEEPON  :
        lcd.drawChar(DTX6,DTYE, DC_ZSWEEP, COLOUR_ZMOTOR, BLACK,1);
      break;
    case HU_ZSWEEPOFF  :
      lcd.drawChar(DTX6,DTYE, DC_ZSWEEP, BLACK, BLACK,1);
      break;
      default: break;
   }
   break;
   case CM_POINTER:
     switch(huditem){
      case HU_UNITS   :
         erase_string(DBX1+2*DX1,DBY1,3); // Erase the previous
         switch(Ptr_mode){
              case CO_POINT: goto Timer_check;
              case CO_COUNT:
                lcd.setCursor(DBX1,DBY1);
                lcd.setTextColor(COLOUR_MARKER,BLACK);
                lcd.print(F("No.")); 
              break;
              case CO_LENGTH:
               colour=COLOUR_MARKER;
                goto calib_units;
              case CO_ANGLE:
                lcd.setCursor(DBX1,DBY1);
                lcd.setTextColor(COLOUR_MARKER,BLACK);
                lcd.print(F("Deg")); 
              break;
              default: break;
             }
        break;
      default: break;
    }
   break;
   case CM_GRATICULE:
     switch(huditem){
      case HU_UNITS   :
        erase_string(DBX1+DX1,DBY1,3); // Erase the previous
        colour=COLOUR_GRATICULE;
        calib_units:
        lcd.drawChar(DBX1,DBY1, 0x6d, colour, BLACK,1);  // 'm'
        switch(Calib_unit){
          case CU_MICROM :
            lcd.drawChar(DBX1+DX1,DBY1, DC_MICRO, colour, BLACK,1);
          break;        
          case CU_NANOM  :
            lcd.drawChar(DBX1+DX1,DBY1, DC_NANO,  colour, BLACK,1);
          break;        
          case CU_MILLIM :
            lcd.drawChar(DBX1+DX1,DBY1, DC_MILLI, colour, BLACK,1);
          break;        
          case CU_ARBIT  :
            lcd.drawChar(DBX1+DX1,DBY1, DC_ARBI,  colour, BLACK,1);
          default: break;       
        }
     break;
     case HU_PERIOD   :
      erase_string(DRX,DRY1,2); // Erase the previous
      print_int_string(DRX,DRY1,(void *)&Graticule_period,UINT8,0,COLOUR_GRATICULE); 
     break;
     default: break;    
    }
   default: break;    
 }   

 Timer_check:
 if(Timer_status == TM_RUNNING || Control_mode == CM_TIMER){
    switch(huditem){
     case HU_THRS    :
      print_int_string(DRX,DRY2,(void *)&Time_curr_hrs,UINT8,2,COLOUR_TIMER); 
      break;
     case HU_TMIN    :
      print_int_string(DRX,DRY3,(void *)&Time_curr_min,UINT8,2,COLOUR_TIMER); 
      break;
     case HU_TSEC    :
      print_int_string(DRX,DRY4,(void *)&Time_curr_sec,UINT8,2,COLOUR_TIMER); 
      break;
      default: break;
    }
 }

  if(Control_mode <= CM_MENU_DELTA && Lamp_curr<LAMP_DISCONNECTED){
    if(huditem == HU_LAMP || Lamp_update > 0){
     uint16_t lr;
     erase_string(DTX7,DTY2,3);
     lr = Lamp_curr/10;
     lr*=10;
     print_int_string(DTX7,DTY2,(void *)&lr,UINT16,3,COLOUR_LAMP);   
     Lamp_update = 0;
    }
  }

 
 return;
}


//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////////
 //                                                                  // 
  //                     COMMON UTILITY FUNCTIONS                   //
   //                                                              //
    //\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\////////////////////////////////
