//
//  ******************************************************************************
//  @file    main.c
//  @author  CPL (Pavel Paces, based on STM examples)
//  @version V0.0
//  @date    01-October-2016
//  @brief   Serial line over ST-Link example
//  Nucleo STM32F401RE USART2 (Tx PA.2, Rx PA.3)
//
//  ******************************************************************************
//

#include "stm32f4xx.h"
#include <stdio.h>

// *******************************************************************************

//
//  @brief  Enable MCU internal connections to USART and GPIO
//
void RCC_Configuration(void) {
	// --- System Clocks Configuration
	//RCC_SYSCLKConfig(83890000);
	// USART2 clock enable
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// GPIOA clock enable
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	// GPIOC clock enable
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOC, ENABLE );
} // END void RCC_Configuration(void)

// *******************************************************************************

//
//  @brief  Input/output pins configuration
//
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
	GPIO_InitStructure.GPIO_Pin         = GPIO_Pin_13 ;
	GPIO_InitStructure.GPIO_Mode        = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType       = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed       = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd        = GPIO_PuPd_UP;
	GPIO_Init( GPIOC, &GPIO_InitStructure );

} // END void GPIO_Configuration(void)

// *******************************************************************************

//
//  @brief  Baud rate settings
//
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
} // END void USART2_Configuration(void)

void Anlog_Configuration(){

}

// *******************************************************************************

//
//  @brief  The function sends characters until 0
//
void OutString(char *s) {

	while (*s) {
		// Wait for Tx Empty
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
			;
		// Send Char
		USART_SendData(USART2, *s++);
	} // END while

} // END void OutString(char *s)

// *******************************************************************************

//
//  @brief  Main loop
//
int main(void) {
	// Init
	RCC_Configuration();

	GPIO_Configuration();

	USART2_Configuration();

	// Process
	OutString("== program menu == \r\n");
	OutString("Item 1: Control LED \r\n");
	OutString("Item 2: Read button state \r\n");
	OutString("Item 3: Read joystick \r\n");
	OutString("Item 4: Control display \r\n");

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
			led = (led += 1)%2;
			//Turn on LED - set pin to value 1
			//It is the same pin I was configurating
			if(led)
			{
				GPIO_SetBits(GPIOA,GPIO_Pin_5);
				OutString("Led on\r\n");
			}
			else
			{
				GPIO_ResetBits(GPIOA,GPIO_Pin_5);
				OutString("Led off\r\n");
			}
			break;
		case '2':
			;//empty statement --> hack for compiler
			//uint16_t data = GPIO_ReadInputData(GPIOC);
			uint8_t data = GPIO_ReadInputDataBit( GPIOC, GPIO_Pin_13 );
			if(!data)
			{
				OutString("Button Activated\r\n");
			}
			else
				OutString("Button Deactivated\r\n");
			break;
		case '3':
			OutString("Joystick\r\n");
			//Turn off LED - set pin to value 0
			//It is the same pin I was configurating
			break;
		case '4':
			OutString("Display\r\n");
			break;
		case 'c':
			OutString("Enter command: \r\n");
			break;
		case 'e':
			OutString("Exit\r\n");
			break;
		default:
			OutString("Wrong option\r\n");
			break;
		}
	}

	/*
	 USART_SendData(USART2, Data); // Echo Char
	 OutString("\n");*/
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
