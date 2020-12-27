/*
 * @brief LPC15xx ADC example
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
#include <stdio.h>
#include "app_usbd_cfg.h"
#include "hid_mouse.h"
/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

static volatile int ticks;
bool sequence0Complete, sequence1Complete, threshold1Crossed;

#define TICKRATE_HZ (100)	/* 100 ticks per second */

#if defined(BOARD_KEIL_MCB1500)
/* ADC is connected to the pot */
#define BOARD_ADC_CH 0
#define ANALOG_INPUT_PORT   1
#define ANALOG_INPUT_BIT    1
#define ANALOG_FIXED_PIN    SWM_FIXED_ADC1_0

#elif defined(BOARD_NXP_LPCXPRESSO_1549)
/* ADC is connected to the pot on LPCXPresso base boards */
#define BOARD_ADC_CH 1
#define ANALOG_INPUT_PORT   0
#define ANALOG_INPUT_BIT    9
#define ANALOG_FIXED_PIN    SWM_FIXED_ADC1_1

#else
#warning "Using ADC channel 8 for this example, please select for your board"
#define BOARD_ADC_CH 8
#endif



/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

static USBD_HANDLE_T g_hUsb;
const  USBD_API_T *g_pUsbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
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

#define ADC_MY_RESULT(n)           ((((n) >> 8) & 0xFF))

void showValudeADC( uint8_t *report)
{
	int index = 1, j;
	uint32_t rawSample;

	/* Get raw sample data for channels 0-11 */
	for (j = 0; j < 12; j++)
	{
		rawSample = Chip_ADC_GetDataReg(LPC_ADC1, j);

		/* Show some ADC data */
		if ((rawSample & (ADC_DR_OVERRUN | ADC_SEQ_GDAT_DATAVALID)) != 0)
		{
			DEBUGOUT("ADC%d_%d: Sample value = 0x%x (Data sample %d)\r\n", index, j,
					 ADC_DR_RESULT(rawSample), j);
             uint32_t temp = ADC_MY_RESULT(rawSample);
             temp -= 128;
             report[0] = temp;

		}
	}
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	static uint32_t count;

	/* Every 1/2 second */
	count++;
	if (count >= (TICKRATE_HZ / 8)) {
		count = 0;

		/* Manual start for ADC1 conversion sequence A */
		Chip_ADC_StartSequencer(LPC_ADC1, ADC_SEQA_IDX);
	}
}


/**
 * @brief	Handle interrupt from ADC1 sequencer A
 * @return	Nothing
 */
void ADC1A_IRQHandler(void)
{
	uint32_t pending;

	/* Get pending interrupts */
	pending = Chip_ADC_GetFlags(LPC_ADC1);

	/* Sequence A completion interrupt */
	if (pending & ADC_FLAGS_SEQA_INT_MASK) {
		sequence1Complete = true;
	}

	/* Clear Sequence A completion interrupt */
	Chip_ADC_ClearFlags(LPC_ADC1, ADC_FLAGS_SEQA_INT_MASK);
}


/**
 * @brief	main routine for ADC example
 * @return	Function should not exit
 */
int main(void)
{

	/*******************/
	USBD_API_INIT_PARAM_T usb_param;
    USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	uint32_t prompt = 0, rdCnt = 0;


	 /**/
	SystemCoreClockUpdate();
	Board_Init();
	/* enable clocks */
	Chip_USB_Init();

	DEBUGSTR("ADC sequencer demo\r\n");

	/* Setup ADC for 12-bit mode and normal power */
	Chip_ADC_Init(LPC_ADC0, 0);
	Chip_ADC_Init(LPC_ADC1, 0);

	/* Setup for maximum ADC clock rate */
	Chip_ADC_SetClockRate(LPC_ADC1, ADC_MAX_SAMPLE_RATE);


	/* Use higher voltage trim for both ADCs */
	Chip_ADC_SetTrim(LPC_ADC1, ADC_TRIM_VRANGE_HIGHV);

	/* For ADC1, sequencer A will be used with threshold events.
	   It will be triggered manually by the sysTick interrupt and
	   only monitors the ADC1 input. */
	Chip_ADC_SetupSequencer(LPC_ADC1, ADC_SEQA_IDX,
							(ADC_SEQ_CTRL_CHANSEL(BOARD_ADC_CH) | ADC_SEQ_CTRL_MODE_EOS));

	/* Disables pullups/pulldowns and disable digital mode */
	Chip_IOCON_PinMuxSet(LPC_IOCON, ANALOG_INPUT_PORT, ANALOG_INPUT_BIT, 
		(IOCON_MODE_INACT | IOCON_DIGMODE_EN));

	/* Assign ADC1_0 to PIO1_1 via SWM (fixed pin) */
	Chip_SWM_EnableFixedPin(ANALOG_FIXED_PIN);

	/* Need to do a calibration after initialization and trim */
	Chip_ADC_StartCalibration(LPC_ADC1);
	while (!(Chip_ADC_IsCalibrationDone(LPC_ADC1))) {}

	/* Setup threshold 0 low and high values to about 25% and 75% of max for
	     ADC1 only */
	Chip_ADC_SetThrLowValue(LPC_ADC1, 0, ((1 * 0xFFF) / 4));
	Chip_ADC_SetThrHighValue(LPC_ADC1, 0, ((3 * 0xFFF) / 4));

	/* Clear all pending interrupts */
	Chip_ADC_ClearFlags(LPC_ADC1, Chip_ADC_GetFlags(LPC_ADC1));


	/* Enable sequence A completion and threshold crossing interrupts for ADC1_1 */
	Chip_ADC_EnableInt(LPC_ADC1, ADC_INTEN_SEQA_ENABLE |
					   ADC_INTEN_CMP_ENABLE(ADC_INTEN_CMP_CROSSTH, BOARD_ADC_CH));

	/* Use threshold 0 for ADC channel and enable threshold interrupt mode for
	   channel as crossing */
	Chip_ADC_SelectTH0Channels(LPC_ADC1, ADC_THRSEL_CHAN_SEL_THR1(BOARD_ADC_CH));
	Chip_ADC_SetThresholdInt(LPC_ADC1, BOARD_ADC_CH, ADC_INTEN_THCMP_CROSSING);

	/* Enable related ADC NVIC interrupts */
	NVIC_EnableIRQ(ADC1_SEQA_IRQn);

	/* Enable sequencers */
	Chip_ADC_EnableSequencer(LPC_ADC1, ADC_SEQA_IDX);



	/* This example uses the periodic sysTick to manually trigger the ADC,
	   but a periodic timer can be used in a match configuration to start
	   an ADC sequence without software intervention. */
	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ);

	/* initialize USBD ROM API pointer. */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->pUSBD;

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB0_BASE;
	/*	WORKAROUND for artf44835 ROM driver BUG:
	    Code clearing STALL bits in endpoint reset routine corrupts memory area
	    next to the endpoint control data. For example When EP0, EP1_IN, EP1_OUT,
	    EP2_IN are used we need to specify 3 here. But as a workaround for this
	    issue specify 4. So that extra EPs control structure acts as padding buffer
	    to avoid data corruption. Corruption of padding memory doesn’t affect the
	    stack/program behaviour.
	 */
	usb_param.max_num_ep = 2 + 1;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;

	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = 0;

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {

		ret = Mouse_Init(g_hUsb,
						 (USB_INTERFACE_DESCRIPTOR *) &USB_FsConfigDescriptor[sizeof(USB_CONFIGURATION_DESCRIPTOR)],
						 &usb_param.mem_base, &usb_param.mem_size);
		if (ret == LPC_OK) {
			/*  enable USB interrupts */
			NVIC_EnableIRQ(USB0_IRQn);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}
	}

	/* Endless loop */
	while (1) {



		/* Sleep until something happens */

		Mouse_Tasks();
		__WFI();
	/*	if (sequence1Complete) {
			showValudeADC(LPC_ADC1);
		}*/
	}

	/* Should not run to here */
	return 0;
}
