
# Color Detection System-on-Chip

Implemented a SoC style integrated Processing System (PS) and a Programmable Logic (PL) to record object color data. Used for Object Detection. 

## Features

- Set Pmod communication with Zybo via I²C protocol with ADC conversion for enhancing data acquisition & control systems
- Captured color data from everyday objects for use with Zybo FPGA via Diligent COLOR Pmod for use with object detection
- Utilized ZYNQ Processing System to return RGB data over UART using C in order to manage communication between devices via serial communication 
- Implemented Python scripting with Pygame and Pyserial to create a user-friendly GUI that streamlines the transmission of data between the Zybo FPGA and the COLOR Pmod, making it easy for users to extract and analyze color data with precision and speed

## Documentation
Theory:
The Digilent Pmod COLOR is a color sensor module with the ability to sense red, green, blue and clear light. The onboard AMS's TCS3472 integrates an IR blocking filter to accurately determine the color of objects as well as sense ambient light under varying lighting conditions and through attenuating materials

![Picture1](https://user-images.githubusercontent.com/60726311/220038156-3598a782-3082-4918-8047-ad08aaaa5fa8.png)


## Demo

https://www.youtube.com/watch?v=zzlk4EczFhI


## Appendix

Any additional information goes here

