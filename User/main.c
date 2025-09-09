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
#define STARTING_ADDRESS     0
#define NUMBER_OF_REGISTERS  10
uint16_t Master_Receive_buffer[10];
uint16_t Master_Transmit_buffer[10] = { 0, 1, 2, 3, 4, 0, 1, 2, 3, 4 };
Module_Status Master_Rstatus, Master_wstatus;
void UserTask(void const *argument) {
	SetupModbusRTU();
	SetTimeOut(500);

	/* Infinite loop */
	while(1) {
//		Delay_ms(100);
//		Master_wstatus = WriteModbusMultiRegisters(2, Master_Transmit_buffer, STARTING_ADDRESS, NUMBER_OF_REGISTERS);
//		Delay_ms(100);
//		Master_Rstatus = ReadModbusRegister(2, Master_Receive_buffer, STARTING_ADDRESS, NUMBER_OF_REGISTERS);
	}
	/* USER CODE END StartDefaultTask */
}
/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
