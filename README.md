# LEMSv2
This is the repository for the second version of the LEMS Sensor Stations. Stay tuned, information will be added as it is generated. A majority of the documentation will be added to this file.

### General TODO
5. Investigate MOSFET as reverse voltage protection
1. Create time set code that's easily accessible
2. Keep documentation and File List updated
4. Try SDI-12 code
6. Convert 5TM code into own library
7. Refactor 5TM code
8. Update everything for current setup

### PCB TODO
2. Add 5V regulator
4. Put anti-static hardware?
5. Put reverse voltage hardware
5. Consider assembly. Possible sources:
	* https://www.screamingcircuits.com
	* http://www.nexpcb.com
	* http://www.4pcb.com
	* http://www.tinkerbee.com/pcb-assembly-service.html/
	* http://www.seeedstudio.com/fusion_pcb.html 


### File List
* Data Sheets/
	* Data sheets for components
* Hardware Information/
	* `Bill of Materials.xlsx`	- Complete parts list
	* Transimpedance Amplifier/
		* `Transimpedance_Amplifier_Calculations.ipynb` - Jupyter notebook containing transimpedance amplifier calculations
* LEMSv2/
	* `LEMSv2.ino` - LEMS Arduino Code
	* `DS3231_Alarm1.h` - Header file for corresponding `.c` file
	* `DS3231_Alarm1.c` - File used to set DS3231 Alarm 1. Developed in test code then copied to this folder
* LEMSv2 PCB/
	* `LEMSv2.sch` - LEMSv2 shield schematic
* LEMSv2 PCB Library/ 
	* `LEMSv2.lbr` - Parts used in the LEMSv2 shield
* README.md - If you read this line you're actually reading the README. Good job!
* Sensor Stick/
	* `SensorStick.sch` - PCB schematic with sensors for easy assembly
	* `SensorStick.brd` - PCB board with sensors for easy assembly
	* `Bill of Materials.xslx` - Sensor stick BOM
* Test Code/
	* DS3231_Test/
		* `DS3231_Test.ino` - Test code for DS3231. Test square wave output with interrupts
		* `DS3231_Alarm1.h` - Header file for corresponding `.c` file
		* `DS3231_Alarm1.c` - File used to set DS3231 Alarm 1. Developed in here then copied to main code folder
		
### Acknowledgements
* Thanks to [Sparkfun](https://www.sparkfun.com) and [Adafruit](https://www.adafruit.com) for Arduino libraries, components, tutorials, and eagle parts. Many designs and eagle components are directly used from them
* 
	
