#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>

#define cRECV_BUFF_MAX 16
typedef struct tRecvBuff {
	char chArrRecvBuff[cRECV_BUFF_MAX];
	int iCnt;
} tRecvBuff;

tRecvBuff oRecvBuff;
int LedState = 0;

void RCC_Configuration(void) {
	// --- System Clocks Configuration
	//RCC_SYSCLKConfig(83890000);
	// USART2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// GPIOA clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// GPIOC clock enable
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE);
}

void GPIO_Configuration(void) {
	GPIO_InitTypeDef GPIO_InitStructure;

	// GPIO Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // PA.2 USART2_TX, PA.3 USART2_RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Alternative Functions: Connect USART pins to AF
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// Configure LED pin
	//A5 A is gate and 5 is the numebr of a pin
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Configure button PIN
	// PXX = USER Button
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InitStructure);

}

void USART2_Configuration(void) {
	USART_InitTypeDef USART_InitStructure;

	// USARTx configuration
	//  - BaudRate = 115200 baud
	//  - Word Length = 8 Bits
	//  - One Stop Bit
	//  - No parity
	//  - Hardware flow control disabled (RTS and CTS signals)
	//  - Receive and transmit enabled

	USART_InitStructure.USART_BaudRate = (115200) * 3;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	USART_HardwareFlowControl_None;

	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);
}

// *******************************************************************************
void OutString(char *s) {
	int i = 0;
	while (s[i]) {
		// Wait for Tx Empty
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
		// Send Char
		USART_SendData(USART2, s[i++]);
	}

}

void ClearCommandBuffer() {
	for (int i = 0; i < cRECV_BUFF_MAX; ++i)
		oRecvBuff.chArrRecvBuff[i] = '\0';
	oRecvBuff.iCnt = 0;
}

int AddCharToCommandBuffer(char c) {
	//end of buffer
	if (oRecvBuff.iCnt == cRECV_BUFF_MAX - 1)
		return 0;
	oRecvBuff.chArrRecvBuff[oRecvBuff.iCnt++];
	return 1;
}

void LoadCommandBuffer() {
	ClearCommandBuffer();
	//keep reading until \r\n
	while (1) {
		char c = (char) USART_ReceiveData(USART2);
		//bufer owerflow protection
		if (!AddCharToCommandBuffer(c))
			break;
		if (oRecvBuff.iCnt > 2)
			if (oRecvBuff.chArrRecvBuff[oRecvBuff.iCnt - 2] == '\r')
				if (oRecvBuff.chArrRecvBuff[oRecvBuff.iCnt - 1] == '\n')
					break;
	}
}

void ProcessCommand() {
	if (strcmp(oRecvBuff.chArrRecvBuff, "LED\r\n") == 0) {
		BlinkLed();
	} else if (strcmp(oRecvBuff.chArrRecvBuff, "BTN\r\n") == 0) {
		PrintButtonState();
	} else if (strcmp(oRecvBuff.chArrRecvBuff, "LED\r\n") == 0) {
	} else {
		OutString("Unknown command ");
		OutString(oRecvBuff.chArrRecvBuff);
		OutString("\r\n");
	}
}

void BlinkLed() {
	//Turn on LED - set pin to value 1
	//It is the same pin I was configurating
	if (!LedState) {
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		LedState = 1;
	} else {
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		LedState = 0;
	}
}

void PrintButtonState() {
	uint8_t data = GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_13);
	if (!data) {
		OutString("\rButton Activated  ");
	} else
		OutString("\rButton Deactivated");
}

int main(void) {
	// Init
	RCC_Configuration();

	GPIO_Configuration();

	USART2_Configuration();

	// Process
	OutString("== Nucleo communication protocol ==\r\n");
	OutString("Command '1': Flash LED \r\n");
	OutString("Command '2': Read button state \r\n");
	OutString("Command '3': Read joystick \r\n");
	OutString("Command '4': Control display \r\n");

	uint16_t Data;
	int led = 0;

	// Wait for Char
	while (1) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
			;
		Data = USART_ReceiveData(USART2); // Collect Char
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			; // Wait for Empty
		switch ((char) Data) {
		case '1':
			BlinkLed();
			break;
		case '2':
			PrintButtonState();
			break;
		case '3':
			OutString("Joystick");
			break;
		case '4':
			OutString("Display");
			break;
		case '*':
			//load command buffer
			LoadCommandBuffer();
			ProcessCommand();
			break;
		default:
			break;
		}
	}
	return 1;
}

// *******************************************************************************

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	while (1)
	{}
}
#endif
