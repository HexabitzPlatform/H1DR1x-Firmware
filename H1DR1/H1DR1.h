/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H1DR1.h
 Description: Header for H1DR1 module, defining hardware and software interfaces.
 Module: RGB LED control with UART communication.
 Ports: 6 UART ports (USART1-6) mapped to P1-P6.
 Timers: TIM2-4 for RGB PWM (red, green, blue).
 LED: Indicator LED on GPIOB14.
 Enums: Basic colors, RGB LED modes (pulse, sweep, dim).
 Status: Module-specific error codes.
 */

/* Define to prevent recursive inclusion ***********************************/
#ifndef H1DR1_H
#define H1DR1_H
#define	NUM_OF_PORTS	5
/* Includes ****************************************************************/
#include "BOS.h"
#include "H1DR1_MemoryMap.h"
#include "H1DR1_uart.h"
#include "H1DR1_gpio.h"
#include "H1DR1_dma.h"
#include "H1DR1_inputs.h"
#include "H1DR1_eeprom.h"
#include "Port.h"
/* Exported Macros *********************************************************/
#define	MODULE_PN		_H1DR1

/* Port-related Definitions */
#define	NUM_OF_PORTS	5
#define P_PROG 			P2		/* ST factory bootloader UART */

/* Define Available ports */
#define _P1
#define _P2
#define _P3
#define _P4
#define _P5
//#define _P6

/* Define Available USARTs */
#define _USART1
#define _USART2
//#define _USART3
#define _USART4
#define _USART5
#define _USART6

/* Port-UART mapping */
#define UART_P1 &huart4
#define UART_P2 &huart2
#define UART_P3 &huart6
#define UART_P4 &huart1
#define UART_P5 &huart5
//#define UART_P6 &huart3

/* Module-specific Hardware Definitions ************************************/
/* Port Definitions */
#define	USART1_TX_PIN		GPIO_PIN_9
#define	USART1_RX_PIN		GPIO_PIN_10
#define	USART1_TX_PORT		GPIOA
#define	USART1_RX_PORT		GPIOA
#define	USART1_AF			GPIO_AF1_USART1

#define	USART2_TX_PIN		GPIO_PIN_2
#define	USART2_RX_PIN		GPIO_PIN_3
#define	USART2_TX_PORT		GPIOA
#define	USART2_RX_PORT		GPIOA
#define	USART2_AF			GPIO_AF1_USART2
//
//#define	USART3_TX_PIN		GPIO_PIN_10
//#define	USART3_RX_PIN		GPIO_PIN_11
//#define	USART3_TX_PORT		GPIOB
//#define	USART3_RX_PORT		GPIOB
//#define	USART3_AF			GPIO_AF4_USART3

#define	USART4_TX_PIN		GPIO_PIN_0
#define	USART4_RX_PIN		GPIO_PIN_1
#define	USART4_TX_PORT		GPIOA
#define	USART4_RX_PORT		GPIOA
#define	USART4_AF			GPIO_AF4_USART4

#define	USART5_TX_PIN		GPIO_PIN_3
#define	USART5_RX_PIN		GPIO_PIN_2
#define	USART5_TX_PORT		GPIOD
#define	USART5_RX_PORT		GPIOD
#define	USART5_AF			GPIO_AF3_USART5

#define	USART6_TX_PIN		GPIO_PIN_4
#define	USART6_RX_PIN		GPIO_PIN_5
#define	USART6_TX_PORT		GPIOA
#define	USART6_RX_PORT		GPIOA
#define	USART6_AF			GPIO_AF3_USART6
/* Indicator LED */
#define _IND_LED_PORT		GPIOA
#define _IND_LED_PIN		GPIO_PIN_6
/* Module-specific Macro Definitions ***************************************/

#define NUM_MODULE_PARAMS		1
/* Pointer to RS485 UART handle */
#define P_RS485uart 	        	&MB_UART_HANDLE
/* Timer IDs for RTC and Timeout */
#define TIMERID_RTC_TIMER           0xFE
#define TIMERID_TMOUT_TIMER         0xFD


/* Module-specific Enumeration Definitions *********************************/
/* Basic colors */


/* Module-specific Type Definition *****************************************/
/* Module-status Type Definition */
typedef enum {
	H1DR1_OK = 0,
	H1DR1_ERR_UNKNOWNMESSAGE,
	H1DR1_ERR_WRONGMODE,
	H1DR1_ERROR =255

} Module_Status;
/* Structure for receiving messaging data */
struct rxMessaging_TypeDef
{
	uint8_t Flag;
	uint8_t Port;
};
/* Export UART variables */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;

/* Define UART Init prototypes */
extern void MX_USART1_UART_Init(void);
extern void MX_USART2_UART_Init(void);
extern void MX_USART3_UART_Init(void);
extern void MX_USART4_UART_Init(void);
extern void MX_USART5_UART_Init(void);
extern void MX_USART6_UART_Init(void);
extern void SystemClock_Config(void);


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

/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/

#endif /* H01R0_H */

/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
