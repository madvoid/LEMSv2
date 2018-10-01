# Possible Improvements

This document outlines possible improvements or repeatable bugs that need to be fixed.

* Multiple LEMS have now failed in an unusual way. The LEMS fails, the SD card has weird, corrupted files on it, and the LEMS may or may not blink on a restart. It is either correlated with high moisture or high electromagnetic interference (lightning). It often damages that MLX90614 beyond repair, but may damage other things. The LEMS behaves erratically after this happens.
* The DS3231 chip may reset back to 1/1/2000 00:00:00 on a restart. This is probably because of a power cycle issue, and is currently being investigated.
* Add wireless transmission for remote monitoring
* The microcontroller that runs the whole thing is currently separate from the other circuitry. It would be nice to incorporate the two so we aren’t dependent on a separate manufacturer for the boards
* The real-time clock is one of the most unreliable components on the board. Improving the reliability of this through hardware or software would improve data quality.
* Adding a GPS chip for location and accurate time would be beneficial for multiple reasons. This would help keep the real-time clock calibrated
* Making the board larger and have screw terminals for every pin would increase the flexibility of the board
* Adding electronic reliability components (fuses, transient voltage suppressors, etc.) would help with overall reliability.
* Adding a thermocouple chip would increase measurement flexibility.
* The hardware around the electronics can always use improvements!