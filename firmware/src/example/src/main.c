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
#include "hid_generic.h"

//#define MUXOUT
//#define TEST_FREQ

//PLL programming bits
static uint32_t R7 = 0x00000007;
static uint32_t R6_1 = 0x00030D46;
static uint32_t R6_2 = 0x00830D46;

#ifdef TEST_FREQ
static uint32_t R5_1 = 0x0600FBAD;
static uint32_t R5_2 = 0x0680FBAD;
#else
static uint32_t R5_1 = 0x0800FBAD;
static uint32_t R5_2 = 0x0880FBAD;
#endif

#ifdef MUXOUT
static uint32_t R4 = 0x01860084;
#else
static uint32_t R4 = 0x01980084;
#endif
static uint32_t R3 = 0x00008063;
static uint32_t R2 = 0x0F408012;
static uint32_t R1 = 0x00000001;
#ifdef MUXOUT
static uint32_t R0 = 0xF8394000;
#else
static uint32_t R0 = 0x80394000;
#endif

//ADC private fields
static uint16_t dataADC0;

//USB private fields
static USBD_HANDLE_T g_hUsb;

/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr; /* variable to store the pointer to base EP0 handler */

//USB public fields
const USBD_API_T *g_pUsbApi;

//USB private functions
/* EP0_patch part of WORKAROUND for artf45032. */
ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event) {
	switch (event) {
	case USB_EVT_OUT_NAK:
		if (g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		} else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP: /* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		g_ep0RxBusy = 0;
		break;
	}
	return g_Ep0BaseHdlr(hUsb, data, event);
}

//USB public functions

/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */
void USB_IRQHandler(void) {
	USBD_API->hw->ISR(g_hUsb);
}

/**
 * @brief	Find the address of interface descriptor for given class type.
 * @return	If found returns the address of requested interface else returns NULL.
 */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc,
		uint32_t intfClass) {
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

void setupUSB() {

	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	/* enable clocks and pinmux */
	USB_init_pin_clk();

	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;
	usb_param.max_num_ep = 2;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;

#ifdef USE_USB0
	desc.high_speed_desc = USB_HsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = (uint8_t *) USB_DeviceQualifier;
#else
	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = 0;
#endif

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {

		/*	WORKAROUND for artf45032 ROM driver BUG:
		 Due to a race condition there is the chance that a second NAK event will
		 occur before the default endpoint0 handler has completed its preparation
		 of the DMA engine for the first NAK event. This can cause certain fields
		 in the DMA descriptors to be in an invalid state when the USB controller
		 reads them, thereby causing a hang.
		 */
		pCtrl = (USB_CORE_CTRL_T *) g_hUsb; /* convert the handle to control structure */
		g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
		pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */

		ret =
				usb_hid_init(g_hUsb,
						(USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
						&usb_param.mem_base, &usb_param.mem_size);
		if (ret == LPC_OK) {
			/*  enable USB interrrupts */
			NVIC_EnableIRQ(LPC_USB_IRQ);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}
	}
}

//ADC public functions

void setupADC() {
	DEBUGSTR("ADC sequencer demo\r\n");

	ADC_CLOCK_SETUP_T setup;
	/* Setup ADC for defaults*/
	Chip_ADC_Init(LPC_ADC0, &setup);

	/* Setup for maximum ADC clock rate */
	Chip_ADC_SetSampleRate(LPC_ADC0, &setup, ADC_MAX_SAMPLE_RATE);

	/* Setup for maximum resolution */
	Chip_ADC_SetResolution(LPC_ADC0, &setup, ADC_10BITS);

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

void sendBitsToPLL(uint32_t bits) {
	//When sending data, we send always to GPIO5[5]
	int k;
	for(k = 0; k < 32; ++k) {
		if (CHECK_BIT(bits, k)) {
			Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT, 5, 5);
		} else {
			Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 5);
		}
	}
}

void setupPLLProgramming() {
	//Configure the pins as IO
	//ADF_MUXOUT <-> GPIO5[2]
	//ADF_DATA   <-> GPIO5[5]
	//ADF_LE     <-> GPIO5[7]
	//ADF_CE     <-> GPIO0[9]
	Chip_SCU_PinMuxSet(5, 2, SCU_PINIO_FAST); //5_2
	Chip_SCU_PinMuxSet(5, 5, SCU_PINIO_FAST); //5_5
	Chip_SCU_PinMuxSet(5, 7, SCU_PINIO_FAST); //5_7
	Chip_SCU_PinMuxSet(0, 9, SCU_PINIO_FAST); //0_9

	//Configure pins as output or input
	//Only muxout is input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, 5, 2);
	//Other are output
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 5);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 7);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 0, 9);
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

void startPLLProgramming() {
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 7);
}

void stopPLLProgramming() {
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 7);
}

/**
 * @brief	main routine for program
 * @return	Function should not exit.
 */
int main(void) {
	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();

	setupUSB();
	setupADC();
//	setupPLLProgramming();

	//Flash to signal beginning of setup
	flashLED();

//	startPLLProgramming();
//	sendBitsToPLL(R7);
//	sendBitsToPLL(R6_1);
//	sendBitsToPLL(R6_2);
//	sendBitsToPLL(R5_1);
//	sendBitsToPLL(R5_2);
//	sendBitsToPLL(R4);
//	sendBitsToPLL(R3);
//	sendBitsToPLL(R2);
//	sendBitsToPLL(R1);
//	sendBitsToPLL(R0);

//	stopPLLProgramming();

	flashLED();

	while (1) {
		/* Sleep until next IRQ happens */
		//__WFI();
		/* Start A/D conversion */
		Chip_ADC_SetStartMode(LPC_ADC0, ADC_START_NOW, ADC_TRIGGERMODE_RISING);

		/* Waiting for A/D conversion complete */
		while (Chip_ADC_ReadStatus(LPC_ADC0, ADC_CH0, ADC_DR_DONE_STAT) != SET)
			;

		/* Read ADC value */
		Chip_ADC_ReadValue(LPC_ADC0, ADC_CH0, &dataADC0);

		USBD_API->hw->WriteEP(hUsb, pHidCtrl->epin_adr, &dataADC0, 1);
	}
}
