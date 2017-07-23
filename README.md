# Plant-Care-Kit
Microcontrollers Final Project

In our project we are using the PIC, the UV sensor, the soil moisture probe, the lcd screen, and the iLEDs. The UV sensor and the moisture sensor will be used to observe the current state of the plant. Based on their input, the lcd screen will update to show how long it has been in the sun(or out of it) and what the current moisture level is. The iLEDs will serve as a notification light where, if it is getting enough sun and water, it will glow green. The iLED will grow more red as the conditions become less ideal.


For this project, we designed a Plant Care Kit. We created libraries for two outside devices, the Soil Moisture Sensor and UV sensor. We also used two iLEDs and an LCD display with the libraries from previous labs. The moisture sensor was placed in the soil of the plant for watering purposes, and the UV sensor was used to measure the sunlight the plant was receiving. The LCD displayed 5 different modes that showed different characteristics and could be changed with the push of a button using the INT0 pin: 

1. Sleep Mode
4. Total Percent in the Sun
2. UV Index of the Sun
5. Soil Moisture Percentage
3. Amount of Time in the Sun



Each iLED was paired with a device. The first iLED was used to monitor the UV index of the sun that was being exposed to the plant. Similar to humans, plants can only handle a certain range of UV exposure. The iLED gradually shifted from green to red if the UV index was too high for the plant. The second iLED similarly monitored the moisture of the plant’s soil. If the plant was in need of water or overwatered, then the iLED moved from green to red. Our Plant Care Kit ultimately lets the owner know when to give the plant water and sunshine.

We used 4 libraries for the following devices: Soil moisture sensor, UV sensor, LCD Screen, and iLED library.

Basic usage example - bare minimum to test functionality of hardware
The basic usage of the device is the default states of the device.  A strong light can be used to determine if the UV sensor is able to detect a difference in light levels.  The UV index is shown upon one of the LCD screens.  Putting the soil sensor in water and checking to see if the reading on the LCD display is 100% is a simple check for the hardware to be functioning.

Advanced usage example - covering all the functions and features
One example of advanced usage is to use the Visible light measurement instead of the UV measurement to determine sunlight.  This is more accurate for the plants needs.  However,  the manual for the Sensor mentions that to have an accurate Visible light measurement, a formula to remove the IR interference is needed.  This can be accomplished with the data from the IR measurement.  This is a feature for more advanced usage.  To confirm that the soil sensor is fully functioning, the easiest test it to put the sensor in different soils and check the readings on the LCD display.  With multiple soils of different moisture levels the measurements should be reading different percentages from 0 to 100. Dry soil or air would read 0%, fully wet soil or water would read 100%, and a good amount of soil moisture would read between 80-90% leading the iLED to be lit up as green.
