//
//  ******************************************************************************
//  @file    main.c
//  @author  CPL (Pavel Paces, based on STM examples and HAL library)
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


UART_HandleTypeDef hUART2;

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */

  while(1)
  {
  }
}

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
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType 			= RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState 				= RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue 	= 6; //0x10;
  RCC_OscInitStruct.PLL.PLLState 			= RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource 			= RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM 				= 16;
  RCC_OscInitStruct.PLL.PLLN 				= 336;
  RCC_OscInitStruct.PLL.PLLP 				= RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ 				= 7;
  if( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
  {
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType 				= (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource 			= RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider 			= RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider 			= RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider 			= RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  /*
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  */

}

/**
  * @brief  Serial port settings
  * @param  None
  * @retval None
  */
static void initUSART( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();
	__USART2_CLK_ENABLE();

    // **USART2 GPIO Configuration
    // PA2     ------> USART2_TX
    // PA3     ------> USART2_RX

	GPIO_InitStruct.Pin 		= GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode 		= GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull 		= GPIO_NOPULL;
	GPIO_InitStruct.Speed 		= GPIO_SPEED_LOW;
	GPIO_InitStruct.Alternate 	= GPIO_AF7_USART2;
	HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );


	hUART2.Instance 			= USART2;
	hUART2.Init.BaudRate 		= 9600;//115200;
	hUART2.Init.WordLength 		= UART_WORDLENGTH_8B;
	hUART2.Init.StopBits 		= UART_STOPBITS_1;
	hUART2.Init.Parity 			= UART_PARITY_NONE;
	hUART2.Init.Mode 			= UART_MODE_TX_RX;
	hUART2.Init.HwFlowCtl 		= UART_HWCONTROL_NONE;
	hUART2.Init.OverSampling 	= UART_OVERSAMPLING_16;
	if (HAL_UART_Init( &hUART2 ) != HAL_OK)
	{
		Error_Handler();
	}

} // END void initUSART( void )


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

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/**
  * @brief  main function
  * @param  None
  * @retval None
  */
void main(void)
{
	// timer settings
	uint32_t uiTicks, uiSec;
	// display tmp variables
	uint32_t uiDispCentX, uiDispCentY;
	// joystick operation
	JOYState_TypeDef oJoyState;

	// serial port i/o and status
#define cREVC_MAX 8
	uint8_t chArr[cREVC_MAX];
	HAL_StatusTypeDef oRecvStatus;

	// data reception
	uint32_t uiSerRecv;

	// initialization of variables
	uiSec = 0;
	uiSerRecv = 0;

	//
	// System init
	HAL_Init();
	// Configure the System clock to 84 MHz
	SystemClock_Config();
	// serial port
	initUSART();

	// Nucleo User LED init
	BSP_LED_Init( LED2 );
	// and usage
	BSP_LED_Off( LED2 );
	HAL_Delay( 1000 );
	BSP_LED_On( LED2 );
	HAL_Delay( 1000 );
	BSP_LED_Toggle( LED2 );

	// Nucleo user button init
	BSP_PB_Init( BUTTON_USER, BUTTON_MODE_GPIO );

	// Adafruit joystick init
	(void)BSP_JOY_Init();

	//
	// Adafruit LCD init
	BSP_LCD_Init();
	uiDispCentX = BSP_LCD_GetXSize()/2;
	uiDispCentY = BSP_LCD_GetYSize()/2;

	//
	// some drawings
	BSP_LCD_Clear( LCD_COLOR_GREEN );
	BSP_LCD_DrawLine( 10, 10, 128-10, 10 );
	BSP_LCD_SetFont( &Font24 );
	BSP_LCD_SetTextColor( LCD_COLOR_RED );
	BSP_LCD_DisplayStringAtLine( 1, (uint8_t *)" Welcome to Nucleo! " );


	while(1) // main loop
	{
		// Receive serial data example
		oRecvStatus = HAL_UART_Receive( &hUART2, chArr, 1, 100 );
		if( oRecvStatus == HAL_OK )
		{
			uiSerRecv = chArr[0];
		}
		// Joystick reading example
		oJoyState = BSP_JOY_GetState();

		switch( oJoyState )
		{
		default:
		case JOY_NONE:
			break;
		case JOY_SEL:
			BSP_LCD_Clear(LCD_COLOR_WHITE);
			{
				char *msg = "Welcome to Nucleo!\n\r";
				HAL_UART_Transmit( &hUART2, (uint8_t*)msg, strlen(msg), 0xFFFF);
			}
			BSP_LED_Toggle(LED2);
			break;
		case JOY_DOWN:
			BSP_LCD_SetTextColor( LCD_COLOR_GREEN );
			BSP_LCD_DrawLine( uiDispCentX, uiDispCentY, uiDispCentX, uiDispCentY + 40 );
			BSP_LCD_DrawCircle(BSP_LCD_GetXSize() - 15, BSP_LCD_GetYSize() - 15, 10);
			break;
		case JOY_LEFT:
			BSP_LCD_SetTextColor( LCD_COLOR_GREEN );
			BSP_LCD_DrawLine( uiDispCentX - 40, uiDispCentY, uiDispCentX, uiDispCentY );
			BSP_LCD_DrawCircle(15, BSP_LCD_GetYSize() - 15, 10);
			break;
		case JOY_RIGHT:
			BSP_LCD_SetTextColor( LCD_COLOR_GREEN );
			BSP_LCD_DrawLine( uiDispCentX, uiDispCentY, uiDispCentX + 40, uiDispCentY);
			BSP_LCD_DrawCircle( BSP_LCD_GetXSize() - 15, 15, 10);
			break;
		case JOY_UP:
			BSP_LCD_SetTextColor( LCD_COLOR_GREEN );
			BSP_LCD_DrawLine( uiDispCentX, uiDispCentY, uiDispCentX, uiDispCentY - 40 );
			BSP_LCD_DrawCircle(15, 15, 10);
			break;
		}
		// Button reading - the button is hidden under the display
		// uiButtState = BSP_PB_GetState( BUTTON_USER );

		// Timming - second counter
		if( (HAL_GetTick() - uiTicks) > 1000)
		{
			uint8_t chArr[40];
			uiTicks = HAL_GetTick();
			uiSec++;

			BSP_LCD_SetFont( &Font8 );
			BSP_LCD_SetTextColor( LCD_COLOR_RED );
			sprintf( (char *)chArr, (char *)"  %d   ", (int)uiSec );
			BSP_LCD_DisplayStringAtLine( 14, chArr);
		}
		// string manipulation example
		if( uiSerRecv == 0x30 )
		{
			int iResult;
			char chArrTmpData[] = "DRAW:CIRCLE 64,80,30";

			// !! has to be here
			uiSerRecv = 0;

			iResult = strstr(chArrTmpData, "DRAW");
			if( iResult )
			{
				int iXs, iYs, iRad;
				int iResult;

				iResult = sscanf( chArrTmpData, "DRAW:CIRCLE %d,%d,%d", &iXs, &iYs, &iRad );
				if( iResult > 2 )
				{
					BSP_LCD_DrawCircle( iXs, iYs, iRad );
				}
			}
		}
	}
}
