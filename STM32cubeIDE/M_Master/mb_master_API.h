/*
* mb_master_API.h
* Description   : Header file contains all the function prototypes for the Modbus master API.
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

#ifndef MB_MASTER_API_H_
#define MB_MASTER_API_H_

/* Includes ------------------------------------------------------------------*/
#include "H1DR1.h"
#include <string.h>
#include <stdio.h>
#include "cmsis_os.h"
#include "mbm.h"
#include "Port.h"
/* Exported definitions -------------------------------------------------------*/

/* Timer IDs for RTC and Timeout */
#define TIMERID_RTC_TIMER           0xFE
#define TIMERID_TMOUT_TIMER         0xFD

/* Pointer to RS485 UART handle */
#define P_RS485uart 	        	&MB_UART_HANDLE

/* Modbus_Status Type Definition */
//typedef enum
// {
//	Modbus_OK = 0,
//	Modbus_ERR_UnknownMessage = 1,
//	Modbus_ERR_WrongParams,
//	Modbus_ERROR = 255
//} Module_Status;

/* Structure for receiving messaging data */
struct rxMessaging_TypeDef
{
	uint8_t Flag;
	uint8_t Port;
};

/* Function prototypes -------------------------------------------------------*/

/*
 * @brief: Initializes the Modbus task.
 * @param: None
 * @retval: None
 */
void Modbus_task_Init(void);
/*
 * @brief: Sets up the Modbus RTU communication.
 * @param1: BaudRate - the baud rate for communication.
 * @param2: ParityBit - the parity setting (e.g., none, even, odd).
 * @retval: Module_Status - status of the setup process.
 */
Module_Status SetupModbusRTU();

/*
 * @brief: Sets up the Modbus mode as ASCII.
 * @param1: BaudRate - the baud rate for communication.
 * @param2: ParityBit - the parity setting (e.g., none, even, odd).
 * @retval: Module_Status - status of the setup process.
 */
Module_Status SetupModbusASCII();

/*
 * @brief: Reads data from a slave on Modbus port FC: 0x03.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to read from.
 * @param3: NofReg - number of registers to read.
 * @param4: DataBuffer - pointer to a buffer where the read data will be stored.
 * @retval: Module_Status - status of the read process.
 */
Module_Status ReadModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd, uint8_t NofReg, unsigned short * DataBuffer);

/*
 * @brief: Writes data to a slave on Modbus port FC: 0x06.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to write to.
 * @param3: Data - the data to be written.
 * @retval: Module_Status - status of the write process.
 */
Module_Status WriteModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd, uint32_t Data);

/*
 * @brief: Writes multiple data bytes to a slave on Modbus port FC: 0x10.
 * @param1: SlaveAdd - the address of the slave device.
 * @param2: RegAdd - the starting address of the register to write to.
 * @param3: NofReg - number of registers to write.
 * @param4: Data - pointer to the data to be written.
 * @retval: Module_Status - status of the write process.
 */
Module_Status WriteModbusMultiRegisters(uint8_t SlaveAdd, uint32_t RegAdd, uint8_t NofReg, uint16_t *Data);

/*
 * @brief: Sets timeout for a slave on Modbus port.
 * @param1: MiliSeconds - the timeout duration in milliseconds.
 * @retval: Module_Status - status of the timeout setting process.
 */
Module_Status SetTimeOut(uint16_t MiliSeconds);
/*
 * @brief: Sets the RTC time.
 * @param1: hour - the hour to set.
 * @param2: min - the minute to set.
 * @param3: sec - the second to set.
 * @retval: None
 */
void SetRTC(uint8_t hour, uint8_t min, uint8_t sec);

#endif /* MB_MASTER_API_H_ */

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
