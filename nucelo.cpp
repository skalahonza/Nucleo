//
//  ******************************************************************************
//  @file    main.c
//  @author  CPL (Pavel Paces, based on STM examples and HAL library)
//  student  JAN SKÁLA skalaja7@fel.cvut.cz
//  @version V0.0
//  @date    02-November-2016
//  @brief   Adafruit 802 display shield and serial line over ST-Link example
//           Nucleo STM32F401RE USART2 (Tx PA.2, Rx PA.3)
//
//  ******************************************************************************
//

//
// Basic framework
#include "stm32f4xx.h"
#include "stm32f4xx_nucleo.h"

#include "stm32_adafruit_lcd.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char *JoyStateStrings[] = { "JOY_NONE", "JOY_SEL", "JOY_DOWN", "JOY_LEFT",
		"JOY_RIGHT", "JOY_UP" };

#define BUFFLEN 50
#define NOT_FOUND -1

typedef struct tRecvBuff {
	int iRecvLength;
	char chArrBuff[BUFFLEN];
} tRecvBuff;

tRecvBuff oRecv;

//tombstone - first pos of the joystick
JOYState_TypeDef PrevJoyState = -10;
UART_HandleTypeDef hUART2;
int LedState = 0;

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (HSI)
 *            SYSCLK(Hz)                     = 84000000
 *            HCLK(Hz)                       = 84000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 2
 *            APB2 Prescaler                 = 1
 *            HSI Frequency(Hz)              = 16000000
 *            PLL_M                          = 16
 *            PLL_N                          = 336
 *            PLL_P                          = 4
 *            PLL_Q                          = 7
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale2 mode
 *            Flash Latency(WS)              = 2
 * @param  None
 * @retval None
 */
static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE()
	;

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/* Enable HSI Oscillator and activate PLL with HSI as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = 6; //0x10;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 16;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		//TODO HANDLE ERROR
	}

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		//TODO HANDLE ERROR
	}
}

int strpos(char *haystack, char *needle) {
	char *p = strstr(haystack, needle);
	if (p)
		return p - haystack;
	return NOT_FOUND;
}

static void initUSART(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOA_CLK_ENABLE()
	;
	__USART2_CLK_ENABLE()
	;

	// **USART2 GPIO Configuration
	// PA2     ------> USART2_TX
	// PA3     ------> USART2_RX

	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hUART2.Instance = USART2;
	hUART2.Init.BaudRate = 9600; //115200;
	hUART2.Init.WordLength = UART_WORDLENGTH_8B;
	hUART2.Init.StopBits = UART_STOPBITS_1;
	hUART2.Init.Parity = UART_PARITY_NONE;
	hUART2.Init.Mode = UART_MODE_TX_RX;
	hUART2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	hUART2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&hUART2) != HAL_OK) {
		//TODO HANDLE ERROR
	}
}

void LedOn() {
	BSP_LED_On(LED2);
	LedState = 1;
}
void LedOff() {
	BSP_LED_Off(LED2);
	LedState = 0;
}

void BlinkLed() {
	if (!LedState) {
		LedOn();
	} else {
		LedOff();
	}
}

void PrintButtonState() {
	uint32_t data = BSP_PB_GetState(BUTTON_USER);
	if (!data) {
		OutString("BUTTON PRESSED\r\n");
	} else
		OutString("BUTTON NON-PRESSESD\r\n");
}

void OutString(char *s) {
	HAL_UART_Transmit(&hUART2, (uint8_t *) s, strlen(s), 0xFFFF);
}

void OutError(char *s) {
	OutString("ERROR ");
	OutString(s);
}

void CheckJoyState(JOYState_TypeDef new) {
	if (new != JOY_NONE) {
		if (new != PrevJoyState) {
			//Print required joystate
			OutString("EVENT:");
			OutString(JoyStateStrings[new]);
			OutString("\r\n");
		}
	}
	PrevJoyState = new;
}

uint32_t ReadUART() {
	uint8_t chArr[2];
	uint32_t uiSerRecv = 0;
	// Receive serial data example
	if (HAL_UART_Receive(&hUART2, chArr, 1, 100) == HAL_OK) {
		uiSerRecv = chArr[0];
	}
	return uiSerRecv;
}

void ClearCommandBuffer() {
	int i = 0;
	oRecv.iRecvLength = 0;
	while (i != BUFFLEN) {
		oRecv.chArrBuff[i] = '\0';
		i++;
	}
}

void AddCharToCommandBuffer() {
	uint32_t result = ReadUART();
	if (result) {
		oRecv.chArrBuff[oRecv.iRecvLength] = (char) result;
		oRecv.iRecvLength++;
	}
}

void AnalyzeBuffer() {
	if (oRecv.iRecvLength > 3 && oRecv.chArrBuff[oRecv.iRecvLength - 1] == '\n'
			&& oRecv.chArrBuff[oRecv.iRecvLength - 2] == '\r') {
		//resolve drawing commands first
		if (strcmp(oRecv.chArrBuff, "LED\r\n") == 0) {
			BlinkLed();
		} else if (strcmp(oRecv.chArrBuff, "BUTTON?\r\n") == 0) {
			PrintButtonState();
		} else if (strcmp(oRecv.chArrBuff, "*IDN?\r\n") == 0) {
			OutString("Nucleo 401 RE\r\n");
		}
		//DRAW:SETTEXTCOLOR
		else if (strstr(oRecv.chArrBuff, "DRAW:SETTEXTCOLOR") != NULL) {
			int color = -1;
			int result = sscanf(oRecv.chArrBuff, "DRAW:SETTEXTCOLOR %d",
					&color);
			if (result != 1 || (color < 1 || color > 9)) {
				OutError(oRecv.chArrBuff);
			} else {
				BSP_LCD_SetTextColor(color);
			}
		}
		//DRAW:CLEAR
		else if (strstr(oRecv.chArrBuff, "DRAW:CLEAR") != NULL) {
			int color = -1;
			int result = sscanf(oRecv.chArrBuff, "DRAW:CLEAR %d", &color);
			if (result != 1 || (color < 1 || color > 9)) {
				OutError(oRecv.chArrBuff);
			} else {
				BSP_LCD_Clear(color);
			}
		}
		//DRAW:PIXEL
		else if (strstr(oRecv.chArrBuff, "DRAW:PIXEL") != NULL) {
			int x = 0;
			int y = 0;
			int color = -1;
			int result = sscanf(oRecv.chArrBuff, "DRAW:PIXEL %d,%d,%d", &x, &y,
					&color);
			if (result != 3 || (color < 1 || color > 9)) {
				OutError(oRecv.chArrBuff);
			} else {
				BSP_LCD_DrawPixel(x, y, color);
			}
		}
		//DRAW:LINE
		else if (strstr(oRecv.chArrBuff, "DRAW:LINE") != NULL) {
			int x1 = 0;
			int x2 = 0;
			int y1 = 0;
			int y2 = 0;
			int result = sscanf(oRecv.chArrBuff, "DRAW:LINE %d,%d,%d,%d", &x1,
					&y1, &x2, &y2);
			if (result != 4) {
				OutError(oRecv.chArrBuff);
			} else {
				BSP_LCD_DrawLine(x1, y1, x2, y2);
			}
		}
		//DRAW:CIRCLE
		else if (strstr(oRecv.chArrBuff, "DRAW:CIRCLE") != NULL) {
			int x = 0;
			int y = 0;
			int r = 0;
			int result = sscanf(oRecv.chArrBuff, "DRAW:CIRCLE %d,%d,%d", &x, &y,
					&r);
			if (result != 3) {
				OutError(oRecv.chArrBuff);
			} else {
				BSP_LCD_DrawCircle(x, y, r);
			}
		}
		//DRAW:SETFONT
		else if (strstr(oRecv.chArrBuff, "DRAW:SETFONT") != NULL) {
			int size = -1;
			int result = sscanf(oRecv.chArrBuff, "DRAW:SETFONT %d", &size);
			if (result != 1) {
				OutError(oRecv.chArrBuff);
			} else {
				//validate font size
				switch (size) {
				case 8:
					BSP_LCD_SetFont(&Font8);
					break;
				case 12:
					BSP_LCD_SetFont(&Font12);
					break;
				case 16:
					BSP_LCD_SetFont(&Font16);
					break;
				case 20:
					BSP_LCD_SetFont(&Font20);
					break;
				case 24:
					BSP_LCD_SetFont(&Font24);
					break;
				default:
					OutError(oRecv.chArrBuff);
					break;
				}

			}
		}
		//DRAW:TEXT
		else if (strstr(oRecv.chArrBuff, "DRAW:TEXT") != NULL) {
			int x = 0;
			int y = 0;
			char text[100] = { 0 };
			int align = -1;
			int result = sscanf(oRecv.chArrBuff, "DRAW:TEXT %d,%d,%100[^,],%d",
					&x, &y, text, &align);
			if (result != 4) {
				OutError(oRecv.chArrBuff);
			} else {
				//validate align
				switch (align) {
				case 1:
				case 2:
				case 3:
					BSP_LCD_DisplayStringAt(x, y, text, align);
					break;
				default:
					OutError(oRecv.chArrBuff);
					break;
				}
			}

		} else {
			OutString("UNKNOWN ");
			OutString(oRecv.chArrBuff);
		}
		ClearCommandBuffer();
	}
}

void main(void) {
// display tmp variables
	uint32_t uiDispCentX, uiDispCentY;
// joystick operation

// System init
	HAL_Init();
// Configure the System clock to 84 MHz
	SystemClock_Config();
// serial port
	initUSART();

// Nucleo User LED init
	BSP_LED_Init(LED2);

// Nucleo user button init
	BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

// Adafruit joystick init
	(void) BSP_JOY_Init();

//
// Adafruit LCD init
	BSP_LCD_Init();
	uiDispCentX = BSP_LCD_GetXSize() / 2;
	uiDispCentY = BSP_LCD_GetYSize() / 2;

	ClearCommandBuffer();
	while (1) // main loop
	{
		AddCharToCommandBuffer();
		AnalyzeBuffer();
		JOYState_TypeDef state = BSP_JOY_GetState();		
		CheckJoyState(state);
	}
}
