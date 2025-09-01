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
///* User Task */
//void UserTask(void *argument) {
//
//	/* put your code here, to run repeatedly. */
//	while (1) {
//
//	}
//}
extern UART_HandleTypeDef huart3;
uint8_t d=50;
int yy ;
uint16_t aa=0x44;
uint16_t w[10]={0x55 ,0x11 ,0x22 ,0x33};
unsigned short rr[10];
void UserTask(void const *argument) {
	  SetupModbusRTU();
	    SetTimeOut(500);


	/* Infinite loop */
	for (;;) {
		yy ++;

		HAL_Delay(100);
		ReadModbusRegister(1, 0, 10, rr);
//		HAL_UART_Transmit_IT(&huart3, &d, 1);
//		HAL_UART_Transmit(&huart3, &d, 1,0xffff);

//		WriteModbusRegister(1,2,aa);
//		WriteModbusMultiRegisters(1, 0, 6, w);

//HAL_UART_Transmit(&huart3, &d, 1, 0xffff);
	}
	/* USER CODE END StartDefaultTask */
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
