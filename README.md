# LEMSv2
This is the repository for the second version of the LEMS Sensor Stations. Stay tuned, information will be added as it is generated. A majority of the documentation will be added to this file.

### General TODO
1. Create time set code that's easily accessible
2. Keep documentation and File List updated
3. Devleop WiFi Code
4. Develop SDI-12 code
5. Investigate Serial multiplexing for multiple 5TMs on one serial port

### PCB TODO
1. Put solder jumper between enable pin and ground for optional regulator shutdown
2. Put space for 5V regulator. 5V regulator runs to BAT_IN on arduino
3. Solder jumper on enable pin. Consider disabling the charger using enable pin, default closed?
4. Put anti-static hardware?


### File List
* Data Sheets/
	* `Adafruit Feather M0 ATWINC1500.pdf` - Adafruit instructions for Feather
	* `ADS10105.pdf` - ADS1015 ADC
	* `Atmel SAM D21.pdf` - Feather processor datasheet
	* `DS3231.pdf` - Maxim DS3231 real time clock datasheet
	* `Feather Proto Pinout.png` - Pinout for Feather with protoboard. NOTE: Not exactly same as pinout for LEMSv2 Feather, but close.
	* `MLX90614.pdf` - Melexis MLX90614 infrared temperature sensor datasheet
	* `SHT21.pdf` - Sensirion SHT21 temperature/humidity sensor datasheet
* Hardware Information/
	* `Hardware Description.xlsx` - Pinouts, connections, calibration constants etc.
	* `Bill of Materials.xlsx`	- Complete parts list
	* Feather Eagle Files/ - Eagle files for Adafruit Feather
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
	
