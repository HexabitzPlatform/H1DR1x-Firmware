/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name     : main.c
 Description   : Main program body.
 */

/* Includes ****************************************************************/
#include "BOS.h"
#include "mbtypes.h"
#define BAUD_RATE                9600
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
extern UART_HandleTypeDef huart3;
uint8_t d=50;
int yy ;
uint16_t aa=0x44;
unsigned short rr [20];
uint8_t p[10] ;
/***************************************************************************/
/* User Task */
void UserTask(void *argument) {
    SetupModbusRTU(BAUD_RATE, MB_PAR_NONE);
    SetTimeOut(200);
//    HAL_UART_Receive_DMA(&huart3,p , 5);
	/* put your code here, to run repeatedly. */
	while (1) {
//		HAL_UART_Receive(&huart3, &p, 1, 0xffff);
//		yy ++;

//		HAL_UART_Transmit_IT(&huart3, &d, 1);
//		HAL_UART_Transmit(&huart3, &d, 1,0xffff);
//		HAL_Delay(200);
//		 aa=0x44 ;
//		WriteModbusRegister(1,0,aa);
//		HAL_Delay(200);
//		aa=20;
//		WriteModbusRegister(1,1,aa);
//		HAL_Delay(200);
//		aa=1;
		WriteModbusRegister(1,2,aa);
//		HAL_Delay(200);
//		ReadModbusRegister(1, 0, 10, rr);
		HAL_Delay(300);
//HAL_UART_Transmit(&huart3, &d, 1, 0xffff);
	}
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
