/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xgpio.h"
#include <unistd.h> // usleep()
#include <stdbool.h> // bool

#define LED_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID
#define KEY_DEVICE_ID XPAR_AXI_GPIO_0_DEVICE_ID

XGpio LEDInst;
XGpio KEYInst;

u8 key_value_pre=0;
u8 key_value_now=0;

int main()
{
    int status;
    init_platform();

    status = XGpio_Initialize(&KEYInst, KEY_DEVICE_ID); // initial KEY
    if(status != XST_SUCCESS) return XST_FAILURE;
    status = XGpio_Initialize(&LEDInst, LED_DEVICE_ID); // initial LED
    if(status != XST_SUCCESS)return XST_FAILURE;
    XGpio_SetDataDirection(&KEYInst, 1, 1); // set KEY IO direction as in
    XGpio_SetDataDirection(&LEDInst, 1, 0); // set LED IO direction as out

    printf("Press KEY2 to control LED3-LED6\n");
    while(1)
    {
        usleep(100000); // 0.1s sleep, to debounce, in common, the meta-state will sustain no more than 20ms
        key_value_pre=key_value_now;
        key_value_now= XGpio_DiscreteRead(&KEYInst, 1) & 0x0F;
        if(key_value_now==1)XGpio_DiscreteWrite(&LEDInst, 1, 0x0f);
        else XGpio_DiscreteWrite(&LEDInst, 1, 0x00);

        if(key_value_pre!=key_value_now) printf("key state_changed!\n");
    }
    cleanup_platform();
    return 0;
}
