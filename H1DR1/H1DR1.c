/*
    BitzOS (BOS) V0.2.3 - Copyright (C) 2017-2019 Hexabitz
    All rights reserved

    File Name     : H1DR1.c
    Description   : Source code for module H1DR1.
										RS-485 Serial Transceiver (MAX14840EASA+)
		
		Required MCU resources : 
		
			>> USARTs 2,3,4,5,6 for module ports.
			>> USART 1 for MAX14840EASA+.
			>> PA12 for RE/DE (receiver output enable).
			>> TIM16 for MB port.
			
*/
	
/* Includes ------------------------------------------------------------------*/
#include "BOS.h"

/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Module exported parameters ------------------------------------------------*/
uint8_t MB_Param = 0;
module_param_t modParam[NUM_MODULE_PARAMS] = {{.paramPtr=&MB_Param, .paramFormat=FMT_UINT8, .paramName="Mode"}};

/* Private variables ---------------------------------------------------------*/
uint8_t H1DR1_Mode;
uint8_t Src_port;
uint32_t Br_baud_rate;
uint32_t Parity_bit;
uint16_t TimeOut;
//xMBMHandle xMBMaster;
unsigned short ReadBuffer[253]={0};
unsigned short WriteBuffer;
TaskHandle_t ModbusHandle = NULL;



/* Private function prototypes -----------------------------------------------*/	 
void ModbusTask(void * argument);
	
/* -----------------------------------------------------------------------
	|												 Private Functions	 														|
   ----------------------------------------------------------------------- 
*/

/* --- H1DR1 module initialization. 
*/
void Module_Init(void)
{
	/* Array ports */
	MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
	MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  MX_USART6_UART_Init();
	
	/* RS485 port */
	RS485_DE_RE_Init();
	RS485_RECEIVER_EN();

}
/*-----------------------------------------------------------*/

/* --- H1DR1 message processing task. 
*/
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift)
{
	Module_Status result = H1DR1_OK;
	
	switch (code)
	{
		case (CODE_H1DR1_MODE): 
			switch(cMessage[port-1][shift])
			{
				case (BRIDGE):
					Src_port = cMessage[port-1][1+shift];
					Br_baud_rate = ( (uint32_t) cMessage[port-1][2+shift] << 24 ) + ( (uint32_t) cMessage[port-1][3+shift] << 16 ) + ( (uint32_t) cMessage[port-1][4+shift] << 8 ) + cMessage[port-1][5+shift];
					SetupBridgeMode(Src_port,Br_baud_rate);
					break;
				default :
					break;		
			}
			
		default:
			result = H1DR1_ERR_UnknownMessage;
			break;
	}			

	return result;	
}

/*-----------------------------------------------------------*/

/* --- Register this module CLI Commands 
*/
void RegisterModuleCLICommands(void)
{

}

/*-----------------------------------------------------------*/

/* --- Get the port for a given UART. 
*/
uint8_t GetPort(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART4)
			return P1;
	else if (huart->Instance == USART2)
			return P2;
	else if (huart->Instance == USART6)
			return P3;
	else if (huart->Instance == USART3)
			return P4;
	else if (huart->Instance == USART5)
			return P5;
	else if (huart->Instance == USART1)
			return P6;
		
	return 0;
}

/* -----------------------------------------------------------------------
	|																APIs	 																 	|
   ----------------------------------------------------------------------- 
*/

/* --- setup RS485 port as bridge
*/
Module_Status SetupBridgeMode(uint8_t Source_p, uint32_t baud_rate)
{

	// Set module mode
	H1DR1_Mode=BRIDGE;
	Src_port=Source_p;
	if(baud_rate<=19200 && baud_rate>=2400)
	{
	// Reinit the RS485 port to required settings 
		if ( MB_PORT_Init(baud_rate, UART_WORDLENGTH_8B, UART_PARITY_NONE, UART_STOPBITS_1) == H1DR1_OK )
		{    
		// Set the baud rate of the src port to baud_rate
			if ( UpdateBaudrate(Source_p, baud_rate) == BOS_OK )
			{
			// Bridge between the src port and RS485 port
				if ( Bridge(Source_p, P_RS485) == BOS_OK )
				{
				// Set the RS485 to Receiver
					RS485_RECEIVER_EN();
					return H1DR1_OK;
				}
				else return H1DR1_ERROR;
			}
			else return H1DR1_ERROR;
		}
		else return H1DR1_ERROR;	
	}
	else return H1DR1_ERROR;	
}


/* -----------------------------------------------------------------------
	|															Commands																 	|
   ----------------------------------------------------------------------- 
*/


/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
