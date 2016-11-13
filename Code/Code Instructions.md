# Instructions

Please read through all instructions before starting!

1. Ensure all libraries and board definitions are installed (see [README.md](../README.md))

2. Obtain LEMSv2

3. Plug LEMSv2 into computer via micro-USB cable

4. Double press the reset button on the red SAMD21 breakout board. This puts the board in a state where it's waiting for a code to be uploaded

5. Open `Time_Set.ino` in the Arduino IDE. Compile and upload the code but do not unplug the USB cable. The RTC battery should be installed. This will set the time on the RTC to whatever time it is on the host computer. When this code is running, opening the serial monitor should display the time at regular intervals. This can be used to check the time is working.

6. While the time set code is still running, open `LEMSv2.ino` in the Arduino IDE. 

7. In lines 34 through 40, set measurements to 0 or 1 depending on whether that sensor is plugged in or not. (0 for not plugged in, 1 for plugged in).

8. On line 45, set DEBUG to 0

9. On line 94, set the filename to a desired filename. Filenames with more than 8 characters (not counting file extension) haven't been tested. The last two characters in the filename (not counting the file extension) **need** to be numbers. It is recommended those two are left as "00".

10. On line 139, enter the LiCor Li200R calibration constant, found in `../Hardware Information/Calibration Constants.xlsx`. They should be entered in the units of (Amps/(W/m^2)). It is easiest to type the given calibration constant and divide it by 1000. For example, a calibration constant of 92.54 $\mu A$ per 1000 W/m^2^ should look like this in the code:

   ```c++
   const double liConst = 92.54E-6 / 1000;  // Licor Calibration Constant. Units of (Amps/(W/m^2))
   ```

11. Compile and upload code to the LEMSv2. Unplug soon after upload is finished so program doesn't start and file isn't made.

12. After wiring and installation, if a battery is plugged in, turning the switch from off to on should turn on the LEMSv2. 

13. After a short amount of time, the green light should turn on for about three seconds. If a blinking red light shows up, something is wrong. First check the SD card and make sure it works, is formatted, and is plugged in correctly.

14. After the three seconds, the green light should turn off. After that, wait until the green light blinks a few times. If the green light doesn't blink after about 20 seconds, something is wrong. Check the sensors and connections.

15. When everything is running properly, the green light should blink every 10 seconds. 

16. To turn off, wait until green light blinks, and turn off shortly after.