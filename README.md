# Food Scale - A small scale project

![Food Scale](https://user-images.githubusercontent.com/22509729/119909055-d8085380-bf08-11eb-9ddb-e0cf538b476f.png)


## Demo

[Link](https://www.youtube.com/watch?v=jWenBSRKTng&feature=youtu.be)

## Project Overview

This project was created by Michael De Sousa in the winter of 2019. It was designed to be an easy to use food scale with a warning when approaching the maximum allowed weight. 

The scale has 3 buttons for the user to press and 3 outputs for the user to pay attention to. The first button controls both the power and the first output, which is the LCD display. When on, the scale will continuously measure any weight placed on top of the wooden plate and display it on the LCD screen. The second button is the tare button, and will zero out whatever is currently on the plate so that it does not affect future measurements. The third button allows the user to change the units between Grams, Ounces, and Pounds. 

The maximum weight allowed on the scale is 8 pounds. Alongside the LCD display is an 8x8 LED matrix which represents the current weight as a percentage of the max possible load. As the user places more weight on the plate the matrix will begin to fill up, with each lit row representing 12.5% of the max load. At 80% of the max load a buzzer will sound an alarm to indicate that the max load has nearly been reached.


## Technologies Used

Hardware:
- Atmega1284 Microcontroller
- MAX7219 LED matrix
- 5kg Load Cell + HX711 Load Cell Amplifier
- WT1205 Buzzer
- LCD Display

Programming Languages:
- C

Libraries:
- [HX711](https://github.com/aguegu/ardulibs/tree/master/hx711) 
  *Note: This library is for an Arduino. I took the functions I needed out of this library and rewrote them to work with my Atmega1284.*
  
 Additional Tools:
 - SPI communication (between Atmega1284 and LED matrix)
 
![Component Connections docx-1](https://user-images.githubusercontent.com/22509729/119908543-b2c71580-bf07-11eb-8616-d5687bec4ae8.png)

