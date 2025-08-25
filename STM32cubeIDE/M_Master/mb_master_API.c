/*
* mb_master_API.c
* Description   : Source file contains all the functions for the Modbus master API.
*  Created on   : Sep 4, 2024
*      Author   : MAHMOOD_REDA @ Hexabitz
******************************************************************************
* @attention
*
* Copyright (c) 2017-2020 Hexabitz.
* All rights reserved.
*
******************************************************************************
*/

/* ----------------------- System includes ----------------------------------*/
#include "H1DR1.h"

/* ----------------------- Platform includes --------------------------------*/
#include "task.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mbm.h"
#include "H1DR1.h"
#include "mb_master_API.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "H1DR1.h"
/* Private variables ---------------------------------------------------------*/

/* Handle for Modbus Master */
xMBMHandle xMBMaster;

/* Flag to trigger Modbus operations */
uint8_t mbTriggerFlag = 0;

/* Variables to store RTC time */
uint8_t Hour;
uint8_t Minute;
uint8_t Second;

/* Watchdog timer timeout flag */
uint8_t wmTimeOut;

/* Millisecond counter for RTC */
uint16_t msCounter;

/* Timeout counter for Modbus operations */
uint16_t TMOUT_Counter;

/* Structure for receiving messaging data */
struct rxMessaging_TypeDef rxMessaging;

/* Handle for RTC software timer */
TimerHandle_t zTimer = NULL;

/* Handle for Timeout software timer */
TimerHandle_t sTimer = NULL;

/* Handle for Messaging task */
osThreadId MessagingTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* Task for handling messaging */
void MessagingTask(void const *argument);

/* Handler for RTC software timer */
static void RTC_HandleTimer(TimerHandle_t zTimer);

/* Handler for Timeout software timer */
static void TMOUT_HandleTimer(TimerHandle_t sTimer);

/* ISR for Modbus protocol port */
void prvvMBPUSART_RXNE_ISR(void);

/* -----------------------------------------------------------------------
 |							 Private Functions	 	                      |
 -------------------------------------------------------------------------
 */

/*
 * @brief: Task for handling messaging.
 * @param: argument - pointer to the task argument.
 * @retval: None
 */
void MessagingTask(void const *argument) {

	vTaskDelete(NULL);
}


/********************************************************************/
/*
 * @brief: Handles the TimeOut software timer.
 * @param: sTimer - handle to the software timer.
 * @retval: None
 */
static void TMOUT_HandleTimer(TimerHandle_t sTimer) {
	uint32_t Timer_ID = 0;

	Timer_ID = (uint32_t) pvTimerGetTimerID(sTimer);
	if (TIMERID_TMOUT_TIMER == Timer_ID) {
		TMOUT_Counter++;
		if (TMOUT_Counter == 1000) {
			wmTimeOut++;
			TMOUT_Counter = 0;
		}
	}
}

/*-----------------------------------------------------------*/

/********************************************************************/
/*
 * @brief: Handles the RTC software timer.
 * @param: zTimer - handle to the software timer.
 * @retval: None
 */
static void RTC_HandleTimer(TimerHandle_t zTimer) {
	uint32_t Timer_ID = 0;

	Timer_ID = (uint32_t) pvTimerGetTimerID(zTimer);
	if (TIMERID_RTC_TIMER == Timer_ID) {
		msCounter++;
		if (msCounter == 1000) {
			Second++;
			msCounter = 0;
		}
		if (Second == 60) {
			Second = 0;
			Minute++;
		}
		if (Minute == 60) {
			Minute = 0;
			Hour++;
		}
		if (Hour == 24) {
			Hour = 0;
		}
	}
}

/********************************************************************/
/*
 * @brief: Sets the RTC time.
 * @param1: hour - the hour to set.
 * @param2: min - the minute to set.
 * @param3: sec - the second to set.
 * @retval: None
 */
void SetRTC(uint8_t hour, uint8_t min, uint8_t sec) {

	Hour = hour;
	Minute = min;
	Second = sec;
}

/********************************************************************/
/*
 * @brief: UART receive complete callback.
 * @param: huart - handle to the UART.
 * @retval: None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	rxMessaging.Flag = 1;

	if (huart == P_RS485uart) {
		prvvMBPUSART_RXNE_ISR(); /* Call Modbus protocol port ISR */
	}

	/* NOTE : This function should not be modified, when the callback is needed,
	 the HAL_UART_RxCpltCallback can be implemented in the user file.
	 */
}
/* -----------------------------------------------------------------------
 |				         			API	 	                              |
 -------------------------------------------------------------------------
 */

/********************************************************************/
/*
 * @brief: Initializes the Modbus module.
 * @param: None
 * @retval: None
 */
void Modbus_task_Init(void) {

	/* RS485 port */
	RS485_TX_ENABLE();
	/* Definition and creation of software timers */
	zTimer = xTimerCreate("RTC_Timer", pdMS_TO_TICKS(1), pdTRUE,
			(void*) TIMERID_RTC_TIMER, RTC_HandleTimer);
	xTimerStart(zTimer, portMAX_DELAY);

	sTimer = xTimerCreate("TimeOut_Timer", pdMS_TO_TICKS(1), pdTRUE,
			(void*) TIMERID_TMOUT_TIMER, TMOUT_HandleTimer);
	xTimerStart(sTimer, portMAX_DELAY);

	osThreadDef(defaultTask, MessagingTask, osPriorityNormal, 0, 8 * 128);
	MessagingTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

}

/********************************************************************/
/********************************************************************/
/*
 * @brief: Sets up the Modbus RTU communication.
 * @param1: BaudRate - the baud rate for communication.
 * @param2: ParityBit - the parity setting (e.g., none, even, odd).
 * @retval: Module_Status - status of the setup process.
 */
Module_Status SetupModbusRTU() {
	Module_Status Status;
	ULONG Pbit = 0;
	ULONG ulBaudRate = 9600;
//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	/* Initialize Modbus port as RTU */
	if (MB_ENOERR != eMBMSerialInit(&xMBMaster, MB_RTU, 1, ulBaudRate, Pbit)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	return Status;
}

/********************************************************************/
/*
 * @brief: Sets up the Modbus mode as ASCII.
 * @param1: BaudRate - the baud rate for communication.
 * @param2: ParityBit - the parity setting (e.g., none, even, odd).
 * @retval: Module_Status - status of the setup process.
 */
Module_Status SetupModbusASCII() {
	Module_Status Status;
	ULONG Pbit = 0;
	ULONG ulBaudRate = 9600;
//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	/* Initialize Modbus port as ASCII */
	if (MB_ENOERR != eMBMSerialInit(&xMBMaster, MB_ASCII, 1, ulBaudRate, Pbit)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	return Status;
}

/********************************************************************/
/*
 * @brief: Reads data from a slave on Modbus port FC: 0x03.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to read from.
 * @param3: NofReg - number of registers to read.
 * @param4: DataBuffer - pointer to a buffer where the read data will be stored.
 * @retval: Module_Status - status of the read process.
 */
Module_Status ReadModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd,
		uint8_t NofReg, unsigned short *DataBuffer) {
	Module_Status Status;
	unsigned short *Buffer = DataBuffer;

//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	if (MB_ENOERR != eMBMReadHoldingRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, Buffer)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);

	return Status;
}

/********************************************************************/
/*
 * @brief: Writes data to a slave on Modbus port FC: 0x06.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to write to.
 * @param3: Data - the data to be written.
 * @retval: Module_Status - status of the write process.
 */
Module_Status WriteModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd,
		uint32_t Data) {
	Module_Status Status;
//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	if (MB_ENOERR != eMBMWriteSingleRegister(xMBMaster, SlaveAdd, RegAdd, Data)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	return Status;
}

/********************************************************************/
/*
 * @brief: Writes multiple data bytes to a slave on Modbus port FC: 0x10.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to write to.
 * @param3: NofReg - number of registers to write.
 * @param4: Data - pointer to the data to be written.
 * @retval: Module_Status - status of the write process.
 */
Module_Status WriteModbusMultiRegisters(uint8_t SlaveAdd, uint32_t RegAdd,
		uint8_t NofReg, uint16_t *Data) {
	Module_Status Status;
	USHORT *InBuffer = (USHORT*) Data;
//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	if (MB_ENOERR != eMBMWriteMultipleRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, InBuffer)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	return Status;
}

/********************************************************************/
/*
 * @brief: Sets timeout for a slave on Modbus port.
 * @param1: MiliSeconds - the timeout duration in milliseconds.
 * @retval: Module_Status - status of the timeout setting process.
 */
/********************************************************************/
/*
 * @brief: Sets timeout for a slave on Modbus port.
 * @param1: MiliSeconds - the timeout duration in milliseconds.
 * @retval: Module_Status - status of the timeout setting process.
 */
Module_Status SetTimeOut(uint16_t MiliSeconds) {
	Module_Status Status;
	USHORT timeout = (USHORT) MiliSeconds;
//	HAL_NVIC_DisableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
//	HAL_NVIC_DisableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
	if (MB_ENOERR != eMBMSetSlaveTimeout(xMBMaster, timeout)) {
		Status = Modbus_ERROR;
	} else {
		Status = Modbus_OK;
	}
//	HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch3_5_IRQn);
//	HAL_NVIC_EnableIRQ(DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
	return Status;
}

/*-----------------------------------------------------------*/


/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
