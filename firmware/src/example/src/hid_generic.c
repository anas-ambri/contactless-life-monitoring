/*
 * @brief This file contains USB HID Generic example using USB ROM Drivers.
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
#include <stdint.h>
#include <string.h>
#include "usbd_rom_api.h"
#include "hid_generic.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

typedef struct {
	USBD_HANDLE_T hUsb;	/*!< Handle to USB stack. */
	uint8_t report[DEVICE_BUFFER_SIZE];	/*!< Last report data  */
	uint8_t tx_busy;	/*!< Flag indicating whether a report is pending in endpoint queue. */
} Device_Ctrl_T;

/** Singleton instance of Keyboard control */
static Device_Ctrl_T g_device;


/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

extern const uint8_t HID_ReportDescriptor[];
extern const uint16_t HID_ReportDescSize;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

static void Device_UpdateReport(uint16_t data) {

	HID_CLEAR_REPORT(&g_device.report[0]);

	HID_REPORT_SET_VALUE(g_device.report[0], data);
	HID_REPORT_SET_VALUE(g_device.report[1], (data >> 8));
}

/*  HID get report callback function. */
static ErrorCode_t HID_GetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t *plength)
{
	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		memcpy(*pBuffer, &g_device.report[0], DEVICE_BUFFER_SIZE);
		*plength = DEVICE_BUFFER_SIZE;
		break;

	case HID_REPORT_OUTPUT:
		return ERR_USBD_STALL;			/* Not Supported */

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL;			/* Not Supported */
	}
	return LPC_OK;
}

/* HID set report callback function. */
static ErrorCode_t HID_SetReport(USBD_HANDLE_T hHid, USB_SETUP_PACKET *pSetup, uint8_t * *pBuffer, uint16_t length)
{
	/* we will reuse standard EP0Buf */
	if (length == 0) {
		return LPC_OK;
	}

	/* ReportID = SetupPacket.wValue.WB.L; */
	switch (pSetup->wValue.WB.H) {
	case HID_REPORT_INPUT:
		return ERR_USBD_STALL;			/* Not Supported */

	case HID_REPORT_OUTPUT:
		return ERR_USBD_STALL;

	case HID_REPORT_FEATURE:
		return ERR_USBD_STALL;			/* Not Supported */
	}
	return LPC_OK;
}

/* HID Interrupt endpoint event handler. */
static ErrorCode_t HID_Ep_Hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	USB_HID_CTRL_T *pHidCtrl = (USB_HID_CTRL_T *) data;
	unsigned int report_pending = 0;

	switch (event) {
	case USB_EVT_IN:
		g_device.tx_busy = 0;
		break;

	return LPC_OK;
}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* HID init routine */
ErrorCode_t usb_hid_init(USBD_HANDLE_T hUsb,
						 USB_INTERFACE_DESCRIPTOR *pIntfDesc,
						 uint32_t *mem_base,
						 uint32_t *mem_size)
{
	USBD_HID_INIT_PARAM_T hid_param;
	USB_HID_REPORT_T reports_data[1];
	ErrorCode_t ret = LPC_OK;

	memset((void *) &hid_param, 0, sizeof(USBD_HID_INIT_PARAM_T));
	/* HID paramas */
	hid_param.max_reports = 1;
	/* Init reports_data */
	reports_data[0].len = HID_ReportDescSize;
	reports_data[0].idle_time = 0;
	reports_data[0].desc = (uint8_t *) &HID_ReportDescriptor[0];

	if ((pIntfDesc == 0) || (pIntfDesc->bInterfaceClass != USB_DEVICE_CLASS_HUMAN_INTERFACE)) {
		return ERR_FAILED;
	}

	hid_param.mem_base = *mem_base;
	hid_param.mem_size = *mem_size;
	hid_param.intf_desc = (uint8_t *) pIntfDesc;
	/* user defined functions */
	hid_param.HID_GetReport = HID_GetReport;
	hid_param.HID_SetReport = HID_SetReport;
	hid_param.HID_EpIn_Hdlr  = HID_Ep_Hdlr;
	hid_param.report_data  = reports_data;

	ret = USBD_API->hid->init(hUsb, &hid_param);
	/* update memory variables */
	*mem_base = hid_param.mem_base;
	*mem_size = hid_param.mem_size;
	g_device.hUsb = hUsb;
	g_device.tx_busy = 0;
	return ret;
}

void sender_tasks(uint16_t data) {
	/* check device is configured before sending report. */
	if ( USB_IsConfigured(g_device.hUsb)) {
		/* send report data */
		if (g_device.tx_busy == 0) {
			g_device.tx_busy = 1;
			/* update report based on board state */
			Device_UpdateReport(data);
			USBD_API->hw->WriteEP(g_device.hUsb, HID_EP_IN, &g_device.report[0], DEVICE_BUFFER_SIZE);
		}
	}
	else {
		/* reset busy flag if we get disconnected. */
		g_device.tx_busy = 0;
	}
}

