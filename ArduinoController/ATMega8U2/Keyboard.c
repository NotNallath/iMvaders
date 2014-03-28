#include "Leds.h"
#include "Keyboard.h"
#include "LightweightRingBuff.h"

#define AVR_RESET_LINE_PORT PORTD
#define AVR_RESET_LINE_DDR DDRD
#define AVR_RESET_LINE_MASK     (1 << 7)

#define SERIAL_2X_UBBRVAL(baud) ((((F_CPU / 8) + (baud / 2)) / (baud)) - 1)

typedef struct
{
    uint8_t Modifier; /**< Keyboard modifier byte, indicating pressed modifier keys (a combination of
                       *   \c HID_KEYBOARD_MODIFER_* masks).
                       */
    uint8_t Reserved; /**< Reserved for OEM use, always set to 0. */
    uint8_t KeyCode[22]; /**< Key codes of the currently pressed keys. */
} ATTR_PACKED USB_KeyboardReport22_Data_t;

RingBuff_t USART_Buffer;

/** Buffer to hold the previously generated Keyboard HID report, for comparison purposes inside the HID class driver. */
static uint8_t PrevKeyboardHIDReportBuffer[sizeof(USB_KeyboardReport22_Data_t)];
static uint8_t PrevMouseHIDReportBuffer[sizeof(USB_MouseReport_Data_t)];

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_HID_Device_t Keyboard_HID_Interface =
 	{
		.Config =
			{
				.InterfaceNumber              = 0,
				.ReportINEndpoint             =
					{
						.Address              = KEYBOARD_IN_EPADDR,
						.Size                 = KEYBOARD_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevKeyboardHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevKeyboardHIDReportBuffer),
			},
    };

/** LUFA HID Class driver interface configuration and state information. This structure is
 *  passed to all HID Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another. This is for the mouse HID
 *  interface within the device.
 */
USB_ClassInfo_HID_Device_t Mouse_HID_Interface =
	{
		.Config =
			{
				.InterfaceNumber              = 1,
				.ReportINEndpoint             =
					{
						.Address              = MOUSE_IN_EPADDR,
						.Size                 = MOUSE_EPSIZE,
						.Banks                = 1,
					},
				.PrevReportINBuffer           = PrevMouseHIDReportBuffer,
				.PrevReportINBufferSize       = sizeof(PrevMouseHIDReportBuffer),
			},
	};


/** Main program entry point. This routine contains the overall program flow, including initial
 *  setup of all components and the main program loop.
 */
char recvState = 0;
char cmd, cmd_data;

struct
{
    uint8_t joystick;
    uint8_t buttons;
} playerState[2];

void handleCmd()
{
	LEDs_ToggleLEDs(LEDS_LED2);
	switch(cmd)
	{
	case 0:
		playerState[0].joystick = cmd_data;
		break;
	case 1:
		playerState[0].buttons = cmd_data;
		break;
	case 2:
		playerState[1].joystick = cmd_data;
		break;
	case 3:
		playerState[1].buttons = cmd_data;
		break;
	}
}

int main(void)
{
	RingBuffer_InitBuffer(&USART_Buffer);
	SetupHardware();

	GlobalInterruptEnable();

	for (;;)
	{
		HID_Device_USBTask(&Keyboard_HID_Interface);
		HID_Device_USBTask(&Mouse_HID_Interface);
		USB_USBTask();
		
		while(USART_Buffer.Count > 0)
		{
			LEDs_ToggleLEDs(LEDS_LED1);
			char c = RingBuffer_Remove(&USART_Buffer);
			switch(recvState)
			{
			case 0:
				if (c == 0x5F)
					recvState = 1;
				break;
			case 1:
				cmd = c;
				recvState = 2;
				break;
			case 2:
				cmd_data = c;
				recvState = 0;
				handleCmd();
				break;
			}
		}
	}
}

/** ISR to manage the reception of data from the serial port
 */
ISR(USART1_RX_vect, ISR_BLOCK)
{
	uint8_t ReceivedByte = UDR1;
	RingBuffer_Insert(&USART_Buffer, ReceivedByte);
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware()
{
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	//clock_prescale_set(clock_div_1);
#elif (ARCH == ARCH_XMEGA)
	/* Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it */
	XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
	XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

	/* Start the 32MHz internal RC oscillator and start the DFLL to increase it to 48MHz using the USB SOF as a reference */
	XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
	XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
#endif

	/* Hardware Initialization */
	USB_Init();

	/* Pull target /RESET line high */
	AVR_RESET_LINE_PORT |= AVR_RESET_LINE_MASK;
	AVR_RESET_LINE_DDR  |= AVR_RESET_LINE_MASK;

	/* Must turn off USART before reconfiguring it, otherwise incorrect operation may occur */
	UCSR1B = 0;
	UCSR1A = 0;
	UCSR1C = 0;

	UBRR1  = SERIAL_2X_UBBRVAL(115200);

	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));
	UCSR1A = (1 << U2X1);
	UCSR1B = ((1 << RXCIE1) | (1 << TXEN1) | (1 << RXEN1));
	
	LEDs_Init();
}

/** Event handler for the library USB Connection event. */
void EVENT_USB_Device_Connect(void)
{
}

/** Event handler for the library USB Disconnection event. */
void EVENT_USB_Device_Disconnect(void)
{
}

/** Event handler for the library USB Configuration Changed event. */
void EVENT_USB_Device_ConfigurationChanged(void)
{
	bool ConfigSuccess = true;

	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Keyboard_HID_Interface);
	ConfigSuccess &= HID_Device_ConfigureEndpoints(&Mouse_HID_Interface);

	USB_Device_EnableSOFEvents();
}

/** Event handler for the library USB Control Request reception event. */
void EVENT_USB_Device_ControlRequest(void)
{
	HID_Device_ProcessControlRequest(&Keyboard_HID_Interface);
	HID_Device_ProcessControlRequest(&Mouse_HID_Interface);
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	HID_Device_MillisecondElapsed(&Keyboard_HID_Interface);
	HID_Device_MillisecondElapsed(&Mouse_HID_Interface);
}

/** HID class driver callback function for the creation of HID reports to the host.
 *
 *  \param[in]     HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in,out] ReportID    Report ID requested by the host if non-zero, otherwise callback should set to the generated report ID
 *  \param[in]     ReportType  Type of the report to create, either HID_REPORT_ITEM_In or HID_REPORT_ITEM_Feature
 *  \param[out]    ReportData  Pointer to a buffer where the created report should be stored
 *  \param[out]    ReportSize  Number of bytes written in the report (or zero if no report is to be sent)
 *
 *  \return Boolean \c true to force the sending of the report, \c false to let the library determine if it needs to be sent
 */
bool CALLBACK_HID_Device_CreateHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                         uint8_t* const ReportID,
                                         const uint8_t ReportType,
                                         void* ReportData,
                                         uint16_t* const ReportSize)
{
	if (HIDInterfaceInfo == &Keyboard_HID_Interface)
	{
        USB_KeyboardReport22_Data_t* KeyboardReport = (USB_KeyboardReport22_Data_t*)ReportData;
        uint8_t UsedKeyCodes = 0;

        if (playerState[0].joystick & _BV(0))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_UP_ARROW;
        else if (playerState[0].joystick & _BV(1))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_DOWN_ARROW;
        if (playerState[0].joystick & _BV(2))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_LEFT_ARROW;
        else if (playerState[0].joystick & _BV(3))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_RIGHT_ARROW;

        if (playerState[0].buttons & _BV(0))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_SPACE;
        if (playerState[0].buttons & _BV(1))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_Z;
        if (playerState[0].buttons & _BV(2))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_X;
        if (playerState[0].buttons & _BV(3))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_C;
        if (playerState[0].buttons & _BV(4))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_V;
        if (playerState[0].buttons & _BV(5))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_B;
        if (playerState[0].buttons & _BV(6))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_1_AND_EXCLAMATION;

        if (playerState[1].joystick & _BV(0))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_W;
        else if (playerState[1].joystick & _BV(1))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_S;
        if (playerState[1].joystick & _BV(2))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_A;
        else if (playerState[1].joystick & _BV(3))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_D;

        if (playerState[1].buttons & _BV(0))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_Q;
        if (playerState[1].buttons & _BV(1))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_E;
        if (playerState[1].buttons & _BV(2))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_R;
        if (playerState[1].buttons & _BV(3))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_F;
        if (playerState[1].buttons & _BV(4))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_T;
        if (playerState[1].buttons & _BV(5))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_G;
        if (playerState[1].buttons & _BV(6))
            KeyboardReport->KeyCode[UsedKeyCodes++] = HID_KEYBOARD_SC_2_AND_AT;

        //if (UsedKeyCodes)
        //    KeyboardReport->Modifier = HID_KEYBOARD_MODIFIER_LEFTSHIFT;

        *ReportSize = sizeof(USB_KeyboardReport22_Data_t);
        return false;
    }else{
		USB_MouseReport_Data_t* MouseReport = (USB_MouseReport_Data_t*)ReportData;
		MouseReport->X = 0;
		MouseReport->Y = 0;
		MouseReport->Button = 0;

        if (playerState[0].joystick & 0x01)
            MouseReport->Y = -3;
        else if (playerState[0].joystick & 0x02)
            MouseReport->Y = 3;
        if (playerState[0].joystick & 0x04)
            MouseReport->X = -3;
        else if (playerState[0].joystick & 0x08)
            MouseReport->X = 3;

        if (playerState[0].buttons & 0x01)
            MouseReport->Button |= _BV(0);

        *ReportSize = sizeof(USB_MouseReport_Data_t);
        return MouseReport->X || MouseReport->Y || MouseReport->Button;
    }
}

/** HID class driver callback function for the processing of HID reports from the host.
 *
 *  \param[in] HIDInterfaceInfo  Pointer to the HID class interface configuration structure being referenced
 *  \param[in] ReportID    Report ID of the received report from the host
 *  \param[in] ReportType  The type of report that the host has sent, either HID_REPORT_ITEM_Out or HID_REPORT_ITEM_Feature
 *  \param[in] ReportData  Pointer to a buffer where the received report has been stored
 *  \param[in] ReportSize  Size in bytes of the received HID report
 */
void CALLBACK_HID_Device_ProcessHIDReport(USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
                                          const uint8_t ReportID,
                                          const uint8_t ReportType,
                                          const void* ReportData,
                                          const uint16_t ReportSize)
{
    if (HIDInterfaceInfo == &Keyboard_HID_Interface)
	{
        uint8_t* LEDReport = (uint8_t*)ReportData;
        //if (*LEDReport & HID_KEYBOARD_LED_NUMLOCK)
        //if (*LEDReport & HID_KEYBOARD_LED_CAPSLOCK)
        //if (*LEDReport & HID_KEYBOARD_LED_SCROLLLOCK)
    }
}

