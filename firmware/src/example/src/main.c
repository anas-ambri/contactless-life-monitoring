/*
 * @brief This file contains USB Generic HID example using USB ROM Drivers.
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"
#include "usbd_rom_api.h"
#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "libusbdev.h"

//#define TEST_FREQ

#define TIME_INTERVAL   (2)
static uint16_t indexData = 0;

/* The size of the packet buffer. */
#define PACKET_BUFFER_SIZE        576

/* Application defined LUSB interrupt status  */
#define LUSB_DATA_PENDING       _BIT(0)

/* Packet buffer for processing */
static uint8_t g_rxBuff[PACKET_BUFFER_SIZE];

//PLL programming bits
#define MAX_BITS 32
#define NUM_REGS 10
static int16_t currentRegister = 0;
static int16_t currentBit = MAX_BITS -1;
static bool clkState = FALSE;

static bool debugMode = FALSE;
static bool isProgrammed = FALSE;

static uint32_t R7 = 7;
static uint32_t R6_1 = 200006;
static uint32_t R6_2 = 8588614;

#ifdef TEST_FREQ
static uint32_t R5_1 = 100727725;
static uint32_t R5_2 = 109116333;
#else
static uint32_t R5_1 = 134282157;
static uint32_t R5_2 = 142670765;
#endif

#ifdef TEST_FREQ
static uint32_t R4 = 5767300;
#else
static uint32_t R4 = 26738820;
#endif
static uint32_t R3 = 32835;
static uint32_t R2 = 255885330;
static uint32_t R1 = 1;
#ifdef TEST_FREQ
static uint32_t R0 = 4168253440;
#else
static uint32_t R0 = 2154987520;
#endif

static uint32_t R7_debug = 7;
static uint32_t R6_1_debug = 6;
static uint32_t R6_2_debug = 8388614;
static uint32_t R5_1_debug = 134217733;
static uint32_t R5_2_debug = 142606341;
static uint32_t R4_debug = 4;
static uint32_t R3_debug = 32835;
static uint32_t R2_debug = 255885314;
static uint32_t R1_debug = 1;
static uint32_t R0_debug = 7602176;


static uint32_t regs[NUM_REGS];

//ADC private fields
static uint16_t dataADC0;


//ADC public functions

void setupADC() {
	DEBUGSTR("ADC sequencer demo\r\n");

	ADC_CLOCK_SETUP_T setup;
	/* Setup ADC for defaults*/
	Chip_ADC_Init(LPC_ADC0, &setup);

	/* Setup for maximum ADC clock rate */
	Chip_ADC_SetSampleRate(LPC_ADC0, &setup, ADC_MAX_SAMPLE_RATE);

	/* Setup for maximum resolution */
	Chip_ADC_SetResolution(LPC_ADC0, &setup, ADC_8BITS);

	Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH0, ENABLE);
}

//LED functions

void flashLED() {
	int x = 1000000;
	Board_LED_Set(0, true);
	while (x > 0) {
		x--;
	}
	Board_LED_Set(0, false);
}


//PLL functions

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))

#define TICKRATE_HZ1 (1000)

void SysTick_Handler(void) {

	//Toggle clock

	setPin(5, 0, !clkState);

	if(!isProgrammed) {
		//Board_LED_Set(1, TRUE);
		if(clkState) {

			if(currentRegister < NUM_REGS) {

				if(currentBit == MAX_BITS - 1) {
					//First bit in register, set LE to low
					setPin(1, 12, FALSE);
				}

				//Write bit at position currentBit in the register
				setPin(0, 3, (regs[currentRegister] >> (currentBit)) & 1); //DATA
				currentBit--;

			}
		} else {

			//If we got to the last bit on the register
			if (currentBit < 0) {
			    //we need to set LE to high
				setPin(1, 12, TRUE);

			    //Reset the position of the bit counter
			    currentBit = MAX_BITS -1;
			    //Move to the next register
			    currentRegister++;

			    if(currentRegister >= NUM_REGS) {
			    	isProgrammed = TRUE;
			    	if(!isProgrammed) {
				    	currentRegister = 0;
			    	}

			    	Board_LED_Set(1, FALSE);
			    }
			}

		}
	} else {
		if(clkState) {
			if(libusbdev_QueueSendDone() == 0) {
				/* Queue send request */

				libusbdev_QueueSendReq(g_rxBuff, PACKET_BUFFER_SIZE);
			}

			memset(&g_rxBuff, 0, PACKET_BUFFER_SIZE);
			indexData = 0;
		}
	}

	//Then toggle state of CLK
	clkState = !clkState;
}

void setupPLLRegisters() {
	if(debugMode) {
		regs[0] = R7_debug;
		regs[1] = R6_1_debug;
		regs[2] = R6_2_debug;
		regs[3] = R5_1_debug;
		regs[4] = R5_2_debug;
		regs[5] = R4_debug;
		regs[6] = R3_debug;
		regs[7] = R2_debug;
		regs[8] = R1_debug;
		regs[9] = R0_debug;

	} else {

		regs[0] = R7;
		regs[1] = R6_1;
		regs[2] = R6_2;
		regs[3] = R5_1;
		regs[4] = R5_2;
		regs[5] = R4;
		regs[6] = R3;
		regs[7] = R2;
		regs[8] = R1;
		regs[9] = R0;
	}
}

void setupPLLProgramming() {
	isProgrammed = FALSE;
	setupPLLRegisters();

	//Configure the pins as IO
	//ADF_CLK <-> GPIO5[0] <-> P2_0
	//ADF_DATA   <-> GPIO0[3] <-> P1_16
	//ADF_LE     <-> GPIO1[12] <-> P2_12
	//ADF_CE     <-> GPIO0[2] <-> P1_15
	//SW2 <-> GPIO0[7] <-> P2_7

	Chip_SCU_PinMuxSet(2, 7, SCU_MODE_FUNC0); //P2_7
	Chip_SCU_PinMuxSet(2, 0, SCU_MODE_FUNC4); //P2_0
	Chip_SCU_PinMuxSet(1, 16, SCU_MODE_FUNC0); //P1_16
	Chip_SCU_PinMuxSet(2, 12, SCU_MODE_FUNC0); //P2_12
	Chip_SCU_PinMuxSet(1, 15, SCU_MODE_FUNC0); //P1_15

	//Configure pins as output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 0);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 3);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 1, 12);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 2);

	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 0, 2); //CE

	Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 1, 12); //LE

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);
}

void setPin(uint8_t port, uint8_t pin, bool level) {
	if (level) {
		Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, port, pin);
	} else {
		Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, port, pin);
	}
}

void getClocks() {
	//find rates of various clocks
	//to see the values on your chip, halt the program and read these variables
	volatile static long int sysCoreClk = 0, maxSysClk = MAX_CLOCK_FREQ,
			mainPllClk = 0;
	volatile static long int chipBaseClk = 0, mainPllDivisor = 0, xtalClk = 0;
	volatile static long int periphBaseClk = 0, spifiBaseClk = 0, hsadcBaseClk =
			0, usbBaseClk = 0;
	volatile static long int spifiClk = 0, hsadcClk = 0, usbClk = 0,
			gpioClk = 0;
	uint32_t sysCoreClk1 = 0;
	sysCoreClk1 = Chip_Clock_GetRate(CLK_MX_MXCORE);
	sysCoreClk = (long int) sysCoreClk1;
	mainPllClk = (long int) Chip_Clock_GetMainPLLHz();
	chipBaseClk = (long int) Chip_Clock_GetBaseClocktHz(CLKIN_CLKIN);
	mainPllDivisor = (long int) Chip_Clock_GetDividerDivisor(CLKIN_MAINPLL);
	xtalClk = (long int) Chip_Clock_GetClockInputHz(CLKIN_CRYSTAL);
	periphBaseClk = (long int) Chip_Clock_GetBaseClocktHz(CLK_BASE_PERIPH);
	spifiBaseClk = (long int) Chip_Clock_GetBaseClocktHz(CLK_BASE_SPIFI);
	spifiClk = Chip_Clock_GetRate(CLK_MX_SPIFI);
	hsadcBaseClk = (long int) Chip_Clock_GetBaseClocktHz(CLK_BASE_ADCHS);
	hsadcClk = Chip_Clock_GetRate(CLK_MX_ADCHS);
	usbBaseClk = (long int) Chip_Clock_GetBaseClocktHz(CLK_BASE_ADCHS);
	usbClk = Chip_Clock_GetRate(CLK_MX_USB0);
	gpioClk = Chip_Clock_GetRate(CLK_MX_GPIO);
}


/**
 * @brief	main routine for program
 * @return	Function should not exit.
 */
int main(void) {
	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();
	bool inDebugMode = FALSE;

	//flashLED();

	/* Init USB subsystem and LibUSBDevice */
	libusbdev_init(USB_STACK_MEM_BASE, USB_STACK_MEM_SIZE);
	setupADC();
	setupPLLProgramming();
	Board_Buttons_Init();

	while (libusbdev_Connected() == 0) {
		/* Sleep until next IRQ happens */
		__WFI();
	}

	while (1) {
		/* Start A/D conversion */
		Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(LPC_ADC0, ADC_CH0, ADC_DR_DONE_STAT) != SET)
			;

		/* Read ADC value */
		Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0, &dataADC0);

		if(indexData < PACKET_BUFFER_SIZE){
			g_rxBuff[indexData] = dataADC0;
			indexData++;
		}

		/*
		if(isProgrammed) {
			if (!inDebugMode) {
				debugMode = Buttons_GetStatus();
				Board_LED_Set(1, TRUE);
				if(debugMode) {
					isProgrammed = FALSE;
					currentRegister = 0;
					inDebugMode = TRUE;
					//	setupPLLProgramming();
					setupPLLRegisters();
				}
			} else {
				Board_LED_Set(0,TRUE);
			}
		}*/

	}
}
