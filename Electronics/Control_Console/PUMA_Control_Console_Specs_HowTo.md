PUMA Project: Control Console Specifications and How To Guide
=============================================================

This project is released under the following licenses:

For source code (including CAD files, STL files and computer programs) and compiled code: the GNU GENERAL PUBLIC LICENSE Version 3

For documentation: the GNU Free Documentation License, Version 1.3

Introduction
------------
The PUMA Control Console (PCC) is the device which lets the user control the Z-motor, lamp brightness and TFT displays of the spatial light modulator (SLM) and head-up-display (HUD).
A single PCC can only control one TFT device at a time so can only control the HUD or a SLM at any one time.
If all you need is electronic control of the microscope lamp and nothing else then the PCC is overkill. I recommend you use the PUMA Lite console instead which is much easier to build (see the relevant section of the GitHub page for details on the PUMA Lite console).
The PCC uses generic parts that are widely available with the exception of the custom PUMA motherboard. However the motherboard can be made as a DIY project (see the relevant section of the GitHub repository for details on how to make a PUMA motherboard).
A video tutorial was made which shows how to construct the PCC. This 'How To' guide gives details of the materials tools and components you need but only sparse assembly instruction - see the video for a more comprehensive illustrated construction guide.

Required Components
-------------------
* The following 3D printed parts as specified in the 'PUMA_3D_Printing_Guide':
    1. All parts in the chapter 'PUMA Control Console'
    2. The part 'PL_Knob.stl' from the 'PUMA Lite' chapter.
* The following pre-assembled PCBs:
    1. PUMA Motherboard
    2. Arduino Nano v.3 microcontroller
    3. ULN2003 stepper controller board (for a 28BYJ48  motor) of the type shown in the below picture.
    ![PUMA Stepper controller](Images/PCC_Stepper_controller.png)
    4. Something else 
* Solder
* Track connecting wire: Single core wire and multicore ribbon cables that can be soldered and that can fit in the prototyping board's holes.
* JST XH PCB connector cables (20 cm long each) and headers (right angled versions) of the following pin numbers: 2, 3, 4, 5, 7, 8
* Through hole resistor: 5 Ohm, 1%, 1W, Axial
* 

Tools
-----
* Wire cutters and wire strippers
* Long nose pliers
* Soldering iron, sponge, stand and fume extractor
* Tools to cut tripad board to size
* Fine tipped permanent marker pens.
* Drill with 3.2 mm bit



Assembly Procedure
------------------
* See the video tutorial for detailed instructions.


 

PJT

First written: 02.03.2021 
