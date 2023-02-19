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
}
