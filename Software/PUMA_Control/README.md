PUMA Control
============

Introduction
------------
The 'PUMA_Control' software is a package for Arduino Nano that serves the PUMA Control Console (PCC) including the heads-up display (HUD) of the microscope and some basic aperture generation tools for the spatial light modulator (SLM). This includes interactive measurement functions, a timer, a pointer, various graticule overlays, Z-motor control, a real-time lamp current meter and other facilities.

Special Libraries
-----------------
This program uses a modified version of the Adafruit GFX library where the default font has been customised to draw mirror writing and also to draw custom graphics characters. I place this custom version into a copy of that library called GFXM (in the libraries folder GFXM_Library) so that this does not get overwritten each time the standard Adafruit graphics library is updated). This also includes its own version of the standard Arduino library Print.cpp file which ensures writing is right to left - i.e. mirrored.
This also uses a slightly modified version of  Pawel A. Hernik's "Arduino_ST7789_Fast" library which I made called 'ST7789_FastM'

How to Use
----------
This is only a brief quick start guide. See the full [PUMA Control_User_Manual](../PUMA_Control_Manual.pdf) for detailed and illustrated usage information.
Once loaded onto the Arduino Nano the Nano should be connected to the peripherals of the PUMA Control Console as described in the 'How To' guides for making that console. The following assumes all is working correctly - both hardware and software.
Turn on the console power switch.

Boot Up Screen
--------------
After a few seconds a double beep occurs and the boot up screen appears to let you know the program is ready to use.
In the boot up screen you have 3 options:
1. Press the left button to initiate a Z-Homing procedure (the Z-stage motor must be homed before you can use it but you are free to use the rest of the software function without homing the motor - e.g. if you do not have a motorised stage or if you just want to use manual focus)
2. Press the right button to take you to the main menu to select a control mode
3. Press the central joystick button to mute the device (you will hear a trill indicating that the device has gone into mute). Press the central joystick button again to undo mute (there will be no audible signal for this).

Main Menu
---------
The main menu is entered by a press of the right button. You go back one menu level or exit the menu also by a press of this button.
The joystick buttons allow you to navigate the menus

Control Modes
-------------
The 'control modes' are the main working modes outside the menu and you select which control mode you want to work in via the menu. When in the control modes you can use the joystick and the two buttons on the control pad either individually or in combinations. These are the available control modes and a brief description of what they allow you to do:
1. Z-Motor: Home the Z-stage via its motor and limit switch. Control the focus of the scope. There is a quick shortcut that allows you to go into and out of Z-motor control mode from almost any other control mode and that is to simultaneously press the left and right buttons (but you must press the left button first briefly). This allows you to control the focus of the scope motor while working in other control modes.
2. Pointer: Control an interactive pointer on the HUD which can be used for pointing, marking objects, counting objects and drawing lines to be measured in length and angle.
3. Timer: Set and use a count-up or count-down timer. The time will continue to count time even when you move to a different control mode so it can be used simultaneously with other modes.
4. Graticule: This allows you to superimpose a range of graticules on the field of view via the HUD. Each graticule can be varied in scale and colour.
5. Plain: This sets a completely plain field of view on the HUD which can be varied in colour. Use this for illumination effects. When used with the SLM this can act as a programmable colour filter to colour the illumination of the scope.
6. Aperture: This allows apertures of various sizes to be generated for use with the SLM. You can cycle between a plain bright field circular aperture (open and close it in graded steps), a half-field Schlieren aperture for phase contrast (you can rotate it by 90 degrees as desired) and a central stop aperture of various sizes for dark field microscopy. You can also superimpose the dark field central spot onto either of the two preceding patterns.

The Meaning of Audible Signals
------------------------------
The only source of visual feedback is the HUD or SLM display. If you do not use either of these devices then the PCC can still be used e.g. for control of the Z-motor and also it gives an audible warning if the LED bulb is being over-powered. You could always attach a free 5 volt ST7789 TFT module to the unit to get visual feedback of the GUI if you like ('free' as in not part of any module) but you would need a mirror to read it! If you don't want to do that then the audible signals have been carefully designed to have meanings that will help you know what you are doing. These are the audible signals and what they mean:

// Buzzer Signals  
#define BZ_SILENCE      0  
#define BZ_ACKNOWLEDGED 1  
#define BZ_PROMPT       2  
#define BZ_FORBIDDEN    3  
#define BZ_TRILL        4  
#define BZ_LAMPHOT      5  
#define BZ_TIMEOUT      6  


**SILENCE**  
A special case override to the ACKNOWLEDGED signal for certain key actions. Specifically when moving the pointer around or moving focus up or down then the ACKNOWLEDGED signal is replaced by SILENCE (i.e. no sound and no delay in the navi_loop function that looks for key actions).

**ACKNOWLEDGED**  
Single beep = acknowledge a key action. The total amount of delay introduced by a buzzer signal must be at least 200 ms regardless of Acklen in order to minimise the risk of inadvertent double-key activation.

**PROMPT**  
Double beep = bring user's attention to a normal state or decision. For example when boot up is complete (so the system is ready to accept key commands) or when a confirmation question requiring a response is being asked by the system ('Do you really want to delete all data points?')

**FORBIDDEN**  
Triple fast beep = bring user's attention to an inability to perform the desired action (due, e.g. to a limit having been reached or a condition not met). This will occur if a user tries to access motor functions if the motor has not been homed or a min/max motion limit is reached. It will also sound if you try to undo a drawing mark when there are no more marks left to undo, etc.

**TRILL**  
Trill = The system is entering silent mode. This is essentially a very fast triple beep. This is the only meaning for this signal.

**TIMEOUT**
6 beeps = the end of countdown audible signal for the timer. This will override the mute setting if the user has set an audible signal for a count-down (either alone or in combination with a visible signal).

**LAMPHOT**  
4 beeps  = the lamp is being over-driven with too much current and may be destroyed if immediate action is not taken. This will override the user's mute preference because it is a safety warning.

Notes:  
Sometimes a PROMPT or FORBIDDEN signal will be immediately followed by the single beep of ACKNOWLEDGED, at other times it won't.
