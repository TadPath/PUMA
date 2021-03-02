PUMA Control
============

Introduction
------------
The 'PUMA_Control' software is a package for Arduino Nano that serves the PUMA Control Console including the heads-up display (HUD) of the microscope and some basic aperture generation tools for the spatial light modulator (SLM). This includes interactive measurement functions, a timer, a pointer, various graticule overlays, Z-motor control, a real-time lamp current meter and other facilties.

Special Libraries
-----------------
This program uses a modified version of the Adafruit GFX library where the default font has been customised to draw mirror writing and also to draw custom graphics characters. I place this custom version into a copy of that library called GFXM (in the libaries folder GFXM_Library) so that this does not get overwritten each time the standard Adafruit graphics library is updated). This also includes its own version of the standard Arduino library Print.cpp file which ensures writing is right to left - i.e. mirrored.
This also uses a slightly modified version of  Pawel A. Hernik's "Arduino_ST7789_Fast" library which I made called 'ST7789_FastM' 



PJT 02.03.2021
