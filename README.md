
# Color Detection System-on-Chip

Implemented a SoC style integrated Processing System (PS) and a Programmable Logic (PL) to record object color data. Used for Object Detection. 

## Features

- Set Pmod communication with Zybo via I²C protocol with ADC conversion for enhancing data acquisition & control systems
- Captured color data from everyday objects for use with Zybo FPGA via Diligent COLOR Pmod for use with object detection
- Utilized ZYNQ Processing System to return RGB data over UART using C in order to manage communication between devices via serial communication 
- Implemented Python scripting with Pygame and Pyserial to create a user-friendly GUI that streamlines the transmission of data between the Zybo FPGA and the COLOR Pmod, making it easy for users to extract and analyze color data with precision and speed

## Demo

https://www.youtube.com/watch?v=zzlk4EczFhI

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

![Picture6](https://user-images.githubusercontent.com/60726311/220056976-5dec20f8-e798-4a4e-afb0-7778e95535d0.png)

- We now run connection automation to automatically make connections to the slave interface (PmodCOLOR) to a selected Master address space(ZYNQ).  We then set the COLOR Pmod_out pin as an external pin. 

![Picture7](https://user-images.githubusercontent.com/60726311/220057169-28b348d5-8b10-4d9c-b93f-904bb6d615ab.png)

In order for the Pmod to work properly, we need to Re-customize the COLOR Pmod IP. We need to select the proper Board interface we will be using. I used the middle interface JD.for the ZYBO Z710 as shown in the image below.

![Picture10](https://user-images.githubusercontent.com/60726311/220057270-73eae88d-52cb-4112-a6bd-2b7257d88504.png)

After our design is validated, we now open elaborated design and customize I/O planning to add a XDC file .This file contains the constraints that your board places on designs using it - specific interfaces wired up to specific pins, clock frequencies, and FPGA bank voltages. 
I used the values provided for us by the Digilent Zybo Z7 master xdc Github repository. 

![picture 111](https://user-images.githubusercontent.com/60726311/220057602-8ab68e94-99bb-4337-ad00-a7e6810f84d0.png)

Software: 
- Once the constraint file is generated, we can validate our design, generate HDL wrapper, generate bitstream , and export our hardware .xsa file to Xilinx Vitis for software development.

## Installation and Software 

To run this program you must require Vivado and Vitis installed on your computer. 
You wll beed to have any Python 3 installation
You will also need these python libraries

```bash
pygame==1.9.3
pyserial==3.3
```

```C
/******************************************************************************/
/*                                                                            */
/* main.c -- Example program using the PmodCOLOR IP                           */
/*                                                                            */
/******************************************************************************/
/* Author: Arthur Brown                                                       */
/*                                                                            */
/******************************************************************************/
/* File Description:                                                          */
/*                                                                            */
/* This demo prints RGB color information collected from the                  */
/* PmodCOLOR when requested, note that typical baud rate is 115200            */
/*                                                                            */
/******************************************************************************/
/* Revision History:                                                          */
/*                                                                            */
/*    10/10/2017(artvvb): Created                                             */
/*                                                                            */
/******************************************************************************/

#include "xparameters.h"
#include "xil_cache.h"
#include "PmodCOLOR.h"
#include "sleep.h"
#include "xuartps.h"

typedef struct {
	COLOR_Data min, max;
} CalibrationData;

void DemoInitialize();
void DemoRun();
CalibrationData DemoInitCalibrationData(COLOR_Data firstSample);
void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib);
COLOR_Data DemoNormalizeToCalibration(COLOR_Data sample, CalibrationData calib);
void EnableCaches();
void DisableCaches();

PmodCOLOR myDevice; // Pmod COLOR driver device
XUartPs sys; // Zynq PS UART driver device

int main(void)
{
	DemoInitialize();
	DemoRun();
	return 0;
}

void DemoInitialize()
{
  //initialize the UART drivers
  XUartPs_Config *myUartCfgPtr;
  myUartCfgPtr = XUartPs_LookupConfig(XPAR_PS7_UART_1_DEVICE_ID);
  XUartPs_CfgInitialize(&sys, myUartCfgPtr, myUartCfgPtr->BaseAddress);

  //initialize the Pmod COLOR drivers
	COLOR_Begin(&myDevice, XPAR_PMODCOLOR_0_AXI_LITE_IIC_BASEADDR, XPAR_PMODCOLOR_0_AXI_LITE_GPIO_BASEADDR, 0x29);

  //turn on the Pmod COLOR
	COLOR_SetENABLE(&myDevice, COLOR_REG_ENABLE_PON_MASK);
	usleep(2400);
	COLOR_SetENABLE(&myDevice, COLOR_REG_ENABLE_PON_MASK | COLOR_REG_ENABLE_RGBC_INIT_MASK);
	usleep(2400);
}

void DemoRun()
{
	u8 ID, buf[1];
	int n;
	COLOR_Data data;
	CalibrationData calib;

	xil_printf("Pmod COLOR Demo launched\r\n");

	data = COLOR_GetData(&myDevice);
	calib = DemoInitCalibrationData(data);
	usleep(2400);
	//make sure the Pmod COLOR's LED is turned off
	COLOR_SetLED(&myDevice, 0);

	while (1) {
        n = XUartPs_Recv(&sys, buf, 1);
		if (n != 0 && buf[0] == '?') {
		  // when a question character is received over serial
			data = COLOR_GetData(&myDevice);
			DemoCalibrate(data, &calib);
			data = DemoNormalizeToCalibration(data, calib);
			//capture data from the Pmod COLOR and transmit it over serial
			xil_printf("r=%04x g=%04x b=%04x\r\n",data.r,data.g,data.b);
			usleep(2400);
		}
	}
}

CalibrationData DemoInitCalibrationData(COLOR_Data firstSample)
{
	CalibrationData calib;
	calib.min = firstSample;
	calib.max = firstSample;
	return calib;
}

void DemoCalibrate(COLOR_Data newSample, CalibrationData *calib)
{
	if (newSample.c < calib->min.c) calib->min.c = newSample.c;
	if (newSample.r < calib->min.r) calib->min.r = newSample.r;
	if (newSample.g < calib->min.g) calib->min.g = newSample.g;
	if (newSample.b < calib->min.b) calib->min.b = newSample.b;

	if (newSample.c > calib->max.c) calib->max.c = newSample.c;
	if (newSample.r > calib->max.r) calib->max.r = newSample.r;
	if (newSample.g > calib->max.g) calib->max.g = newSample.g;
	if (newSample.b > calib->max.b) calib->max.b = newSample.b;
}

COLOR_Data DemoNormalizeToCalibration(COLOR_Data sample, CalibrationData calib)
{
	COLOR_Data norm;
	norm.c = (sample.c - calib.min.c) * (0xFFFF / (calib.max.c - calib.min.c));
	norm.r = (sample.r - calib.min.r) * (0xFFFF / (calib.max.r - calib.min.r));
	norm.g = (sample.g - calib.min.g) * (0xFFFF / (calib.max.g - calib.min.g));
	norm.b = (sample.b - calib.min.b) * (0xFFFF / (calib.max.b - calib.min.b));
	return norm;
```
Below are all the Register Definitions for the Pmod drivers. Each of the RGB has 2 registers to store the high and low data bytes for each measurement as shown below.

![Picture11](https://user-images.githubusercontent.com/60726311/220062376-a03cb0bc-5431-4523-9172-71922e39eb76.png)
![Picture12](https://user-images.githubusercontent.com/60726311/220062428-bc8e09fc-e99d-4916-a63c-8d836bdb73f5.png)

This function Initializes the Pmod Color by obtaining the base addresses of the Pmod GPIO and I2C.
![Picture13](https://user-images.githubusercontent.com/60726311/220062596-d866a2e8-9c32-4813-a79b-3630859555c2.png)

This snippet of code grabs the min and max values for each RGB register and normalizes it as seen in lines 103 - 113 Normalization streamlines the color data, simplifying our samples and making it more concise and precise as seen in lines 116 to 124

![Picture14](https://user-images.githubusercontent.com/60726311/220062726-34e5b21b-5a44-4d60-be17-c70488ff68dc.png)

This project requires the use of pygame and pyserial libraries.Pygame includes computer graphics and sound libraries designed to be used with the Python. Pyserial allows us to exchange data between Vitis and Pygame over the serial communication protocol.

This Python  script will launch a window that will connect to the Zybo and launch a window that displays the last color received. We launch vitis from the terminal and the program will open the serial port and receive color data, set the baud rate to 115200 as seen line 11. 

```python
import pygame
import serial
import sys

if len(sys.argv) < 2:
	print('please provide a port number to connect to (format COM#)')
	sys.exit()

try:
	#try to open the serial port
	ser = serial.Serial(sys.argv[1], 115200)
except serial.serialutil.SerialException:
	print('could not find port %s' % sys.argv[1])
	sys.exit()

#create a 480x480 window
screen = pygame.display.set_mode((480,480))
#register a timer event to fire every 100ms
pygame.time.set_timer(pygame.USEREVENT, 100)

while True:
	#wait until an event fires
	event = pygame.event.wait()
	if event.type == pygame.QUIT:
		#user wants to close the window
		break
	elif event.type == pygame.USEREVENT:
		#timer fired, request some data, and update the display
		ser.write(bytes('?', 'utf-8'))
		s = ser.readline().decode('utf-8')
		color = [0]
		idx = 0
		
		#expected format: 'r=%04x g=%04x b=%04x\r\n'
		color = [s[2:6],s[9:13],s[16:20]]
		#pygame color expects values in the range 0-255, strip off the bottom eight bits of each color channel
		rgb = [int(v,16)>>8 for v in color]

		# fill the screen surface with the received color and update the display
		screen.fill(pygame.Color(rgb[0],rgb[1],rgb[2],255))
		pygame.display.flip()
pygame.quit()
```
The code will read the color data gathered from the C code in line 35 and will fill the window it just created with the color it's detecting in line 40. 

## Appendix

https://digilent.com/shop/pmod-color-color-sensor-module/#:~:text=The%20Digilent%20Pmod%20COLOR%20is,conditions%20and%20through%20attenuating%20materials.

https://projects.digilentinc.com/arthur-brown/displaying-color-readings-with-the-pmod-color-and-python-ebd794

https://ams.com/en/tcs34725

And a big thanks for my ECE 520 FPGA Professor Saba Janamian for guiding me and providing feedback. 


