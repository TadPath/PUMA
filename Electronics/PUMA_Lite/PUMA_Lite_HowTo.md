PUMA Project: PUMA Lite Control Box How To Guide
================================================
![PUMA Lite](Images/PL_Complete.png)  
This project is released under the following licenses:

For source code (including CAD files, STL files and computer programs) and compiled code: the GNU GENERAL PUBLIC LICENSE Version 3

For documentation: the GNU Free Documentation License, Version 1.3

Introduction
------------
The PUMA Lite controller (PL) is the device which powers the microscope lamp (only). If you require additional control features (such as the focus motor or AR projector) then you will need the more complex 'PUMA Control Console' (PCC) which does these other things in addition to powering the microscope lamp. The PCC has its own 'How to' guide on this GitHub repository.
The PL can power and control any of the PUMA lamps - for white light or fluorescence - because the different LEDs used for these purposes are electronically cross compatible.

Required Components
-------------------
![Required Components](Images/PL_Parts.jpg)
* The following 3D printed parts as specified in the 'PUMA_3D_Printing_Guide':
    1. All parts in the chapter 'PUMA Lite'
* The following pre-assembled PCBs:
    1. XL4015-based DC-DC step-down buck converter module with both current and voltage regulation of the type shown in the below picture.
    ![PUMA Buck converter](Images/PCC_Buck_converter.png)
* Solder
* Phono socket panel mount type (and a phono plug to assist fitting the socket).
* P-Channel MOSFET, 78 A, 60 V, 3-Pin TO-220F ON Semiconductor BMS3003-1E
* Through hole resistor: 5 Ohm, 1%, 1W, Axial
* Precision potentiometer 200 ohm: Wire wound, bushing mount, Bourns 3590s type.
* ON/Off rectangular rocker switch, 10A/125V or 6A/250V, SPST type. Dimensions: Flange (2.05 cm x 1.5 cm), body size (1.8 cm x 1.15 cm) with 0.48 cm wide connector lugs. Switch body height below flange 1.05 cm and connector lug height 0.85 cm.
* One 9v PP3-type battery snap connectors with 150 mm leads
* A power supply: Either a 9v battery (type 6LR61 PP3) or a mains regulated power adapter that can put out 9 volts regulated DC and at least 1 amp of current with a PP3 connector.
* Solderable spade connectors with insulator covers of sizes 4.8 mm and 2.8 mm (widths).
* Scissors, screw driver, long nose pliars, some insulated wire and wire strippers


Additional Peripherals Required for Testing
-------------------------------------------
* A PUMA microscope LED lamp module


Fixtures and Fittings
---------------------
* All screws are of Allen socket type and made of stainless steel
* The case screws are countersunk. All other screws are socket cap screws.
* The number and type of screws used for each component are given below starting with the component they are to be used for. Lengths refer to the length of the threaded part of the screws (i.e. it does not include the head / socket).
* Case screws: 3x M3, 12 mm long
* Power board: 4x M2, 6 mm long

 
Tools
-----
* Wire cutters and wire strippers
* Long nose pliers
* Soldering iron, sponge, stand and fume extractor
* Allen keys (2 mm and 2.5 mm)
* Small Philips head screwdriver
* Multimeter with voltmeter and ammeter capabilities (to more than 1 amp tolerance)


Assembly Procedure
------------------
* Pre-thread all holes in the casing that are designed to take the thread of a screw. Don't go all the way in and be especially careful of the shallow holes on the front panel otherwise you could deform the external surface or go all the way through with the screw.
* Bend the pins of the p-MOSFET as shown in the figure. The middle pin is bent right back and the other two pins are bent and turned to line up so that they fit into the input terminal block of the buck converter as shown. The negative pin will need to be cut short by a few mm to allow this.
![p-MOSFET](Images/PL_MOSFET.jpg)

* Take the PP3 connector and wrap the negative lead end around the negative pin of the p-MOSFET as shown in the figure (no need to solder it to the pin). To the positive lead of the PP3 connector solder a 4.8 mm spade connector. To the middle pin of the p-MOSFET solder a wire that has a 4.8 mm connector at its other end. You can then insert the p-MOSFET into the buck converter input block and connect the spades to the on/off switch. Finally, connect the 9V battery to the PP3 connector. The buck converter can now be powered up by turning the switch on.
![Battery connections](Images/PL_BattCon.jpg)

* At this stage,with the buck converter powered on, you should adjust the trimmer potentiometers on the buck converter to ensure the correct current and voltage regulations are set (see figures below).
* To set the correct current set your multimeter to measure current in the range of 20 Amps DC, place the meter probes on the output terminals and adjust the trimmer potentiometer closest to the output side of the board until a reading of 0.33 A is achieved.
![Set the current](Images/PL_SetCurrent.jpg)

* To set the correct voltage set your multimeter to measure volts (20 V DC range), place the meter probes on the output terminals and adjust the trimmer potentiometer closest to the inut side of the board until a reading of 5.0 V is achieved. The buck converter board can now be powered down and the on/off switch disconnected from the spades.
![Set the voltage](Images/PL_SetVoltage.jpg)

* Now create the three cables you will need to connect the output side of the buck converter board to the lamp brightness potentiometer and output phono socket (see picture). The shortest cable (~ 5cm) has a 4.8 mm spade at one end and a 2.8 mm spade at the other - it will be used to connect the brightness potentiometer to the phono socket centre terminal (positive). The second cable has a 4.8 mm spade at one end and a bare end - this connects the negative terminal of the buck converter directly to the negative / ground terminal of the phono socket. The third cable is made of the 5 Ohm resistor and a piece of wire with a 2.8 mm spade at the end of the wire and the other end of the resistor is just bent into a small loop - the looped end of the resistor will fit into the positive output terminal of the buck converter and the spade end will eventually go to the potentiometer.  
![Output cables](Images/PL_OutputCables.jpg)

![Power outputs](Images/PL_Power_outputs.jpg)

* The buck converter can now be secured in the main body of the casing with the 4 M2 screws as shown.
![Power board in place](Images/PL_PowerBoard_Fixed.jpg)

* Now prepare the front cover part of the casing by inserting the connectors as shown in the below figure starting with the potentiometer, then the phono socket (having a phono plug inserted into the socket while fitting it will ensure good clearance of the socket from the walls of its receptacle) and finally the on/off switch is pushed into its rectangular hole with the bent spades pointing towards the top cover as shown. The small connector cable can then be used to connect the end connector of the potentiometer to the central connection of the phono plug - note that the small 3D printed plastic washer goes over the central lug of the phono socket prior to connecting the spade connector of this connecting cable (parts C and D of the fingure below).
![Controls in place](Images/PL_Controls.jpg) 

* Connect the output cables from the buck converter to the phono socket and central lug of the rheostat as shown in the figure below.
![Power output cables to controls](Images/PL_Controls_power_out.jpg) 

* Now connect the spades to the on/off switch and ensure that the battery connector leads are fit into the notch in the side wall of the casing as shown in the figure below.
![Connect the on/off switch](Images/PL_Controls_power_in.jpg) 

* Now fit the cover to the main casing, attach a battery, put the battery into the battery compartment and close that compartment with its cover. Put the knob on the shaft of the potentiometer. These steps are illustrated below. Take care to ensure spade connectors don't come loose. You may need to crimp them with the long nosed pliars. Take care also that the battery connector leads remain in the slot in the casing.
![Close the cover](Images/PL_Case_batt_cover.jpg) 

* The unit is now ready for testing. At first just switch it on without any lamp attached (below figure, insert). The red LED should come on - if it doesn't you will need to double check all connections and components. If the red LED comes on then switch off, attach a PUMA lamp and switch on again  - turn the potentiometer to ensure brightness can be varied. On the latter point note two things:
1. The resistance is increased as you rotate the knob clockwise which means that brightness of the lamp will increased with anti-clockwise rotation and decrease with clockwise rotation.
2. The potentiometer is linear - for many turns you will not notice much change in brightness but there will be anoticeable increase in brighntess for the last few turns anti-clockwise.
![Testing](Images/PL_Test.jpg) 


Tripping the Lamp Power Regulator
---------------------------------
* It is important to be aware of a problem that can arise relating to the behaviour of the particular XL4015 power regulator board that we use in this unit. This problem occurs when using the unit in a 'low current supply situation'. Examples of a 'low current supply situation' are:
    1. when the battery is running low or
    3. if an external power source is used that cannot deliver enought current for the lamp.
* In any of these circumstances the power regulator may 'trip' if it experiences a sudden drop in input voltage and there is insufficient resistance across its output terminals (which will be the case if the brightness potentiometer is at or near its brightest setting – i.e. its lowest resistance setting).
* By 'Trip' I mean that the output current suddenly becomes unregulated and the LED bulb of the microscope will draw too much current, something like 700 - 800 mA when the bulb’s maximum rating is 330 mA. You may notice this as a sudden increase in brightness of the bulb.
* If this happens you must turn the brightness down immediately using the brightness potentiometer or else the bulb life will be dramatically shortened. Also, if you use a battery it will be drained faster.
* It it for this reason that we employ a fixed 5 Ohm resistor in the cable that connects the brightness potentiometer to the power regulator. That 5 Ohm resistor ensures that there is always at least some resistance in the circuit, even when the potentiometer is at its minimum-resistance setting. This is important because the intrinsic resistance of the LED bulb is negligible. So this 5 ohm resistor will limit the current if a trip occurs. Without this resistor the current to the LED could exceed 1 Amp in a trip situation and could blow the LED instantly. It only provides partial protection however, buying you a little time, because 700 - 800 mA is still way too high for this LED and you must act quickly to preserve the life of your bulb by turning the brightness down with the brightness potentiometer.
* A common scenario that induces a 'trip' of the power board is if the unit is switched on with the lamp potentiometer set at or near full brightness because the sudden inrush of current to the bulb causes the input voltage to drop and so trip the power regulator.
* As you can probably work out from the above, one way to minimise the risk of a power board trip is to always keep the bulb brightness potentiometer at a low setting whenever you switch on the unit.
 

PJT

First written: 07.06.2021 
