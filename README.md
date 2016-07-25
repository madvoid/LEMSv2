# LEMSv2
This is the repository for the second version of the LEMS Sensor Stations. Stay tuned, information will be added as it is generated. A majority of the documentation will be added to this file.

### General TODO
1. Add battery voltage measurement to code and PCB
5. Investigate MOSFET as reverse voltage protection
1. Create time set code that's easily accessible
2. Keep documentation and File List updated
4. Try SDI-12 code
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
	* `DS3231_Alarm1.h` - Header file for corresponding `.cpp` file
	* `DS3231_Alarm1.c` - File used to set DS3231 Alarm 1. Developed in test code then copied to this folder
	* `d5TM.h` - Header file for corresponding `.cpp` file
	* `d5TM.cpp` - Library used to interact with 5TM. Developed in test code then copied to this folder
* LEMSv2 PCB/
	* `LEMSv2.sch` - LEMSv2 shield schematic
* LEMSv2 PCB Library/ 
	* `LEMSv2.lbr` - Parts used in the LEMSv2 shield
* README.md - If you read this line you're actually reading the README. Good job!
* Sensor Stick/
	* `SensorStick.sch` - PCB schematic for board with sensors for easy assembly
	* `SensorStick.brd` - PCB board with sensors for easy assembly
	* `Bill of Materials.xslx` - Sensor stick BOM
* Test Code/
	* DS3231_Test/
		* `DS3231_Test.ino` - Test code for DS3231. Test DS3231 alarm 1
		* `DS3231_Alarm1.h` - Header file for corresponding `.cpp` file
		* `DS3231_Alarm1.c` - Library used to set DS3231 Alarm 1. Developed in here then copied to main code folder
	* Test_5TM/
		* `Test_5TM.ino` - Test code for 5TM Library. 5TM has been working since LEMSv1 but is now made into a library.
		* `d5TM.h` - Header file for corresponding `.cpp` file
		* `d5TM.cpp` - Library used to interact with 5TM. Developed here then copied to main code folder
		
### Dependencies
Only 3rd party software dependencies are included in this list. Libraries built into the Arduino software or created for the LEMSv2 software isn't included.

* [Adafruit RTC Library](https://github.com/adafruit/RTClib)
* [Adafruit SHT31 Library](https://github.com/adafruit/Adafruit_SHT31)
* [Adafruit Sensor Library](https://github.com/adafruit/Adafruit_Sensor)
* [Adafruit BMP280 Library](https://github.com/adafruit/Adafruit_BMP280_Library)
* [Adafruit MLX90614 Library](https://github.com/adafruit/Adafruit-MLX90614-Library)
* In addition, the Sparkfun SAMD21 [board definition](https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json
) needs to be installed into the Arduino IDE. See [here](https://learn.sparkfun.com/tutorials/samd21-minidev-breakout-hookup-guide/setting-up-arduino) for more info.
		
### Acknowledgements
* Thanks to [Sparkfun](https://www.sparkfun.com) and [Adafruit](https://www.adafruit.com) for Arduino libraries, components, tutorials, and eagle parts. Many designs and eagle components are directly used from them
	
