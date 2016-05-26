# LEMSv2
This is the repository for the second version of the LEMS Sensor Stations. Stay tuned, information will be added as it is generated. A majority of the documentation will be added to this file.

### TODO
1. Create time set code that's easily accessible
2. Keep documentation and File List updated

### File List
* `Bill of Materials.xlsx`	- Complete parts list
* Data Sheets/
	* `Adafruit Feather M0 ATWINC1500.pdf` - Adafruit instructions for Feather
	* `ADS10105.pdf` - ADS1015 ADC
	* `Atmel SAM D21.pdf` - Feather processor datasheet
	* `DS3231.pdf` - Maxim DS3231 real time clock datasheet
	* `Feather Proto Pinout.png` - Pinout for Feather with protoboard. NOTE: Not exactly same as pinout for LEMSv2 Feather, but close.
	* `MLX90614.pdf` - Melexis MLX90614 infrared temperature sensor datasheet
	* `SHT21.pdf` - Sensirion SHT21 temperature/humidity sensor datasheet
* `Hardware Description.xlsx` - Pinouts, connections, calibration constants etc.
* LEMSv2/
	* `LEMSv2.ino` - LEMS Arduino Code
	* `DS3231_Alarm1.h` - Header file for corresponding `.c` file
	* `DS3231_Alarm1.c` - File used to set DS3231 Alarm 1. Developed in test code then copied to this folder
* LEMSv2 PCB/
	* 
* README.md - If you read this line you're actually reading the README. Good job!
* Test Code/
	* DS3231_Test/
		* `DS3231_Test.ino` - Test code for DS3231. Test square wave output with interrupts
		* `DS3231_Alarm1.h` - Header file for corresponding `.c` file
		* `DS3231_Alarm1.c` - File used to set DS3231 Alarm 1. Developed in here then copied to main code folder
	
