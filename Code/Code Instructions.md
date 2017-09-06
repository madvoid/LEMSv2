# Instructions

Please read through all instructions before starting!

1. Ensure all libraries and board definitions are installed (see [README.md](../README.md))

2. Obtain a LEMSv2

3. Ensure that a CR1220 coin cell battery is inserted into the RTC battery holder

4. Plug the LEMSv2 into computer via a micro-USB cable

5. Double press the reset button on the red SAMD21 breakout board. This puts the board in a state where it's waiting for a code to be uploaded

6. Open `Time_Set.ino` in the Arduino IDE. Compile and upload the code but do not unplug the USB cable. The RTC battery should be installed. This will set the time on the RTC to whatever time it is on the host computer. When this code is running, opening the serial monitor should display the time at regular intervals. This can be used to check the time is working.

7. While the time set code is still running, open `LEMSv2.ino` in the Arduino IDE. 

8. In lines 34 through 41 (under the "Sensor Defines" heading), set measurements to 0 or 1 depending on whether that sensor is plugged in or not. (0 for not plugged in, 1 for plugged in). The pressure sensor is soldered onto the board, so it should always be set as 1 unless there are other concerns (file size, timing, etc.)

9. On line 50 (under the "Other Defines" heading), set DEBUG to 0

10. On line 102, set the filename to a desired filename. Filenames with more than 8 characters (not counting file extension) haven't been tested. The last two characters in the filename (not counting the file extension) **need** to be base-10 numbers. It is recommended those two are left as "00".

11. On line 147, enter the LiCor Li200R calibration constant, found in `../Hardware Information/Calibration Constants.xlsx`. They should be entered in the units of (Amps/(W/m^2)). It is easiest to type the given calibration constant and divide it by 1000. For example, a calibration constant of 92.54 $\mu A$ per 1000 W/m^2^ should look like this in the code:

   ```c++
   const double liConst = 92.54E-6 / 1000;  // Licor Calibration Constant. Units of (Amps/(W/m^2))
   ```
   Of course, this doesn't matter if a Li200R isn't plugged in.

12. On line 147, enter the resistor value of the resistor soldered into R14 of the LEMSv2 PCB. This information can be found in `../Hardware Information/Calibration Constants.xlsx`. The resistor value to enter is under the *Actual Resistor* column of the spreadsheet. Enter the value in Ohms. Also, please remember that each LEMSv2 is designed to work with a specific pyranometer, which is why each LEMSv2 has a very specific resistor soldered to it. This means that using a different pyranometer on a given LEMSv2 can give less precise/accurate results.

13. Compile and upload code to the LEMSv2. Unplug soon after upload is finished so program doesn't start and file isn't made.

14. Insert an SD card into the holder

15. After wiring and installation, if a battery is plugged in, turning the switch from off to on should turn on the LEMSv2. 

16. After a short amount of time, the green light should turn on for about three seconds. If a blinking red light shows up, something is wrong. First check the SD card and make sure it works, is formatted, and is plugged in correctly.

17. After the three seconds, the green light should turn off. After that, wait until the green light blinks a few times. If the green light doesn't blink after about 20 seconds, something is wrong. Check the sensors and connections, and try power cycling the LEMSv2.

18. When everything is running properly, the green light should blink every 10 seconds. 

19. To turn off, wait until green light blinks, and turn off shortly after.