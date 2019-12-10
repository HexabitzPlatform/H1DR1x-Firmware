/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
	
/*
		MODIFIED by Hexabitz for BitzOS (BOS) V0.2.0 - Copyright (C) 2017-2019 Hexabitz
    All rights reserved
*/

/* Includes ------------------------------------------------------------------*/
#include "BOS.h"
//#include "mbm.h"

/* Private variables ---------------------------------------------------------*/
uint32_t data;
char DataToSend;
Module_Status state;
//xMBHandle xMBMMaster;
uint8_t port=3;
UART_HandleTypeDef *handle;

/* Private function prototypes -----------------------------------------------*/



/* Main functions ------------------------------------------------------------*/

int main(void)
{


  /* MCU Configuration----------------------------------------------------------*/

  /* Reset all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all user peripherals */

	/* Initialize BitzOS */
	BOS_Init();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  while (1)
  {


  }


}

/*-----------------------------------------------------------*/

/* User Task */
void UserTask(void * argument)
{

//	RS485_RECEIVER_EN();
//	Bridge(P1, P6);
	//eMBMSerialInit( &xMBMMaster, MB_RTU, 1, 19200, UART_PARITY_NONE );
	
			//MB_PORT_Init(19200, 1, 0);     // no meanings for values 1, 0 they are just to avoid error syntax
	// Bridge between the src port and RS485 port
	Bridge(port, P_RS485);
	//handle=&huart1;
	//SET_BIT(handle->Instance->CR3, USART_CR3_DEM);
	RS485_RECEIVER_EN();
	/* Infinite loop */
	for(;;)
	{

	// check the src port 
		handle=GetUart(P_RS485);
		//if ((handle->Instance->ISR & USART_ISR_IDLE) != USART_ISR_IDLE)
		//{
		
	//}
		
	// check if the transmission is complete
		//RS485_RECEIVER_EN();
	if ((handle->Instance->ISR & USART_ISR_TC) == USART_ISR_TC)   //!= 0    && (handle->Instance->ISR & USART_ISR_TXE) == USART_ISR_TXE
	{
	
		RS485_RECEIVER_EN();
		CLEAR_BIT(handle->Instance->RQR, USART_RQR_RXFRQ);
		
	}
	else
	{
		while ((handle->Instance->ISR & USART_ISR_TXE) == USART_ISR_TXE)            //USART_ISR_TXE)
		{
			RS485_RECEIVER_DIS();
			taskYIELD();
		}
	}
/*
	// Check if the whole data buffer has been transmitted
  if ((handle->State != HAL_UART_STATE_BUSY))
  {
		RS485_RECEIVER_EN();
		__HAL_UART_DISABLE_IT(&huart1, UART_IT_TC);
		SET_BIT(handle->Instance->RQR, USART_RQR_RXFRQ);
  }
	// data buffer is not empty	
  else
  {	
						while ((handle->Instance->ISR & USART_ISR_TXE) == USART_ISR_TXE)            //USART_ISR_TXE)
					{
						RS485_RECEIVER_DIS();
						taskYIELD();
					}	
	//Check if the data to be transmitted correspond to the last byte
    if(handle->TxXferCount == 0)
    {
    //   Disable the UART Transmit Data Register Empty Interrupt 
      __HAL_UART_DISABLE_IT(handle, UART_IT_TXE);

    //   Enable the UART Transmit Complete Interrupt 
      __HAL_UART_ENABLE_IT(handle, UART_IT_TC);
    }
		// Check if the data to be transmitted correspond to the first byte
    else   // if (handle->TxXferCount == 0)
    {
		
    }
  }*/
	
			
	}
}

/*-----------------------------------------------------------*/

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
