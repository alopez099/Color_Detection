
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

 <p style="text-align: center;">TCS34725 Color Sensor</p>
 
![Picture1](https://user-images.githubusercontent.com/60726311/220038156-3598a782-3082-4918-8047-ad08aaaa5fa8.png)

The color detector follows Digilent Pmod Interface Specification Type 6 which is used for general purpose logic. In order for the Pmod to function properly, we must set theI/O signals to be bi-directional.

![Picture1](https://user-images.githubusercontent.com/60726311/220050914-adab382f-b719-49ab-9ddb-e73803fc0cbe.png)

The Pmod communicates with the Zybo via I²C protocol by sending a specific address 0x29, which then reads the corresponding color data. Each of the four analog to digital channels (red, green, blue, and clear) sends it's conversion from the ADC to the host buffer

Data Registers:
 - Each of the three colors (RGB) and the clear color byte has two registers to store the high and low data bytes for each measurement as shown below 
 
![Picture2](https://user-images.githubusercontent.com/60726311/220051662-3ce37679-88b7-49c7-89aa-fade23405226.png)

Hardware: 

- First, we create a project in Vivado and design a new block diagram. I used the ZYNQ processing system since we are using the Zybo z710 board. As always, we run block automation and set up external ports DDR and FIXED_IO and sets up peripherals on the PS side enabling UART 1 UART 2 GPIO, USB 0, ENET 0 and QUAD SPI. 

![Picture3](https://user-images.githubusercontent.com/60726311/220052201-bda21298-17c4-49ff-bbb4-658df45bf7f9.png)

- Next, we add the COLOR Pmod to our block design from the Diligent Library Repository. 

![Picture4](https://user-images.githubusercontent.com/60726311/220052344-aa1c2a51-7079-4d75-bcac-d0949e58cd07.png)
![Picture5](https://user-images.githubusercontent.com/60726311/220052445-58534ea2-18ad-4b97-8db2-310ac1fd59ac.png)

- With the COLOR Pmod IP added to our library, we can place the IP in the IP block design integrator as shown below.
- 
![Picture6](https://user-images.githubusercontent.com/60726311/220056976-5dec20f8-e798-4a4e-afb0-7778e95535d0.png)

- We now run connection automation to automatically make connections to the slave interface (PmodCOLOR) to a selected Master address space(ZYNQ).  We then set the COLOR Pmod_out pin as an external pin. 
- 
![Picture7](https://user-images.githubusercontent.com/60726311/220057169-28b348d5-8b10-4d9c-b93f-904bb6d615ab.png)

In order for the Pmod to work properly, we need to Re-customize the COLOR Pmod IP. We need to select the proper Board interface we will be using. I used the middle interface JD.for the ZYBO Z710 as shown in the image below.

![Picture10](https://user-images.githubusercontent.com/60726311/220057270-73eae88d-52cb-4112-a6bd-2b7257d88504.png)

After our design is validated, we now open elaborated design and customize I/O planning to add a XDC file .This file contains the constraints that your board places on designs using it - specific interfaces wired up to specific pins, clock frequencies, and FPGA bank voltages. 
I used the values provided for us by the Digilent Zybo Z7 master xdc Github repository. 

![picture 111](https://user-images.githubusercontent.com/60726311/220057602-8ab68e94-99bb-4337-ad00-a7e6810f84d0.png)

Software: 
- Once the constraint file is generated, we can validate our design, generate HDL wrapper, generate bitstream , and export our hardware .xsa file to Xilinx Vitis for software development.


## Demo

https://www.youtube.com/watch?v=zzlk4EczFhI


## Appendix

Any additional information goes here

