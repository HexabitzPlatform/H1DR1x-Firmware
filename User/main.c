/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */

/* Includes ****************************************************************/
#include "BOS.h"

/* Private variables *******************************************************/

/* Private Function Prototypes *********************************************/

/* Main Function ***********************************************************/
int main(void) {

	/* Initialize Module &  BitzOS */
	Module_Init();

	/* Don't place your code here */
	for (;;) {
	}
}
/***************************************************************************/

extern UART_HandleTypeDef huart3;
uint8_t d=50;
int yy ;
uint16_t aa=0x44;
uint16_t w[10]={0x55 ,0x11 ,0x22 ,0x33};
unsigned short rr[10] ,oo[10] ;
Module_Status s ,ss ;
void UserTask(void const *argument) {
	  SetupModbusRTU();
	    SetTimeOut(500);


	/* Infinite loop */
	for (;;) {
		yy++;
				w[2] ++;
				if (w[2]  >= 1000)
					{w[2] =0;}
		HAL_Delay(50);
		ss=ReadModbusRegister(2, 0, 10, rr);
		HAL_Delay(50);
		s=WriteModbusMultiRegisters(2, 0, 10, w);
//		HAL_UART_Transmit_IT(&huart3, &d, 1);
//		HAL_UART_Transmit(&huart3, &d, 1, 0xffff);
//
//		WriteModbusRegister(1, 2, aa);
//		WriteModbusMultiRegisters(1, 0, 6, w);
}
	/* USER CODE END StartDefaultTask */
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
