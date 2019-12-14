/*
    BitzOS (BOS) V0.1.7 - Copyright (C) 2017-2019 Hexabitz
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
//#include "mb.h"


/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Module exported parameters ------------------------------------------------*/
int MB_Param = 0;
module_param_t modParam[NUM_MODULE_PARAMS] = {{.paramPtr=&MB_Param, .paramFormat=FMT_FLOAT, .paramName="MB_Param"}};

/* Private variables ---------------------------------------------------------*/
enum module_mode{IDLE = 0x00, BRIDGE, RTU, ASCII};
uint8_t H1DR1_Mode;
uint8_t src_port;
uint32_t Br_baud_rate;
UART_HandleTypeDef *handle_P485;

TaskHandle_t H1DR1ModeHandle = NULL;
TaskHandle_t ModbusRTUTaskHandle = NULL;

/* Private function prototypes -----------------------------------------------*/	
void ModbusRTUTask(void * argument);    
void H1DR1ModeTask(void * argument); 


/* Create CLI commands --------------------------------------------------------*/
static portBASE_TYPE demoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
static portBASE_TYPE modeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

/* CLI command structure : demo */
const CLI_Command_Definition_t demoCommandDefinition =
{
	( const int8_t * ) "demo", /* The command string to type. */
	( const int8_t * ) "demo:\r\n Demo bidirectional RS485 communication. Requires two modules\r\n\r\n",
	demoCommand, /* The function to run. */
	0 /* no parameter is expected. */
};
/*-----------------------------------------------------------*/
/* CLI command structure : sample */
const CLI_Command_Definition_t modeCommandDefinition =
{
  ( const int8_t * ) "mode", /* The command string to type. */
  ( const int8_t * ) "mode:\r\n Set RS485 port mode\r\n\r\n",
  modeCommand, /* The function to run. */
  -1 /* Multiparameters are expected. */
};


/* -----------------------------------------------------------------------
	|												 Private Functions	 														|
   ----------------------------------------------------------------------- 
*/

/* --- H1DR1 module initialization. 
*/
void Module_Init(void)
{
	/* Array ports */
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
	MX_USART4_UART_Init();
  MX_USART5_UART_Init();
  MX_USART6_UART_Init();
	
	/* RS485 port */
	MB_PORT_Init(19200, UART_WORDLENGTH_8B, UART_PARITY_NONE);
	RS485_DE_RE_Init();
		UpdateBaudrate(P3, 19200);
	//eMBErrorCode eStatus = eMBInit( MB_RTU, 0x09, 1, 9600, MB_PAR_NONE );
  //eStatus = eMBEnable();

	//xTaskCreate(H1DR1ModeTask, (const char*) "H1DR1ModeTask", (2*configMINIMAL_STACK_SIZE), NULL, osPriorityNormal-osPriorityIdle, &H1DR1ModeHandle);
	//xTaskCreate(ModbusRTUTask, (const char*) "ModbusRTUTask", (2*configMINIMAL_STACK_SIZE), NULL, osPriorityNormal-osPriorityIdle, &ModbusRTUTaskHandle);

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
					H1DR1_Mode = BRIDGE;
					src_port = cMessage[port-1][1+shift];
					Br_baud_rate = ( (uint32_t) cMessage[port-1][2+shift] << 24 ) + ( (uint32_t) cMessage[port-1][3+shift] << 16 ) + ( (uint32_t) cMessage[port-1][4+shift] << 8 ) + cMessage[port-1][5+shift];
					break;
				case (RTU):
					H1DR1_Mode = RTU; break;
				case (ASCII):
					H1DR1_Mode = ASCII; break;
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
	FreeRTOS_CLIRegisterCommand( &demoCommandDefinition );
	FreeRTOS_CLIRegisterCommand( &modeCommandDefinition );
	
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

/* --- H1DR1 Mode Taak. 
*/
void H1DR1ModeTask(void * argument)
{
	/*switch (H1DR1_Mode)
	{
		case BRIDGE: 
			SetupBridgeMode(src_port,1); 
			break;
		case RTU: 
			SetupModbusRTU();
			break;
		case ASCII: 
			SetupModbusASCII();
			break;
		default: H1DR1_Mode=IDLE; break;
	
	}*/
}

/* -----------------------------------------------------------------------
	|																APIs	 																 	|
   ----------------------------------------------------------------------- 
*/

/* --- setup RS485 port as bridge
*/
void SetupBridgeMode(uint8_t Src_port, uint16_t baud_rate)
{
	// Disable MB in case it is running
	//eMB_Disable();  
	//MX_USART1_UART_Init(); 
	// Set the mode of the module
	H1DR1_Mode=BRIDGE;
	// Reinit the RS485 port to the needed settings 
	MB_PORT_Init(baud_rate, 1, 0);     // no meanings for values 1, 0 they are just to avoid error syntax
	// Set the baud rate of the src port to baud_rate
	UpdateBaudrate(Src_port, baud_rate);
	// Bridge between the src port and RS485 port
	Bridge(Src_port, P_RS485);
	// Set the RS485 to Receiver
	RS485_RECEIVER_DIS();
	// check the src port 
	handle_P485=GetUart(P_RS485);

}

/*-----------------------------------------------------------*/

/* --- setup the Modbus mode as RTU
*/
Module_Status SetupModbusRTU(void)
{
	// Reinit Modbus port
	
	// Enable Modbus port to RTU
	
	
	return H1DR1_OK;	
}

/*-----------------------------------------------------------*/

/* --- setup the Modbus mode as ASCII
*/
Module_Status SetupModbusASCII(void)
{
	// Reinit Modbus port
	
	// Enable Modbus port to ASCII
	
	
	return H1DR1_OK;
}

/*-----------------------------------------------------------*/

/* --- setup the Modbus mode as ASCII
*/
Module_Status Bridging(void)
{
	// check if the transmission is complete
	if ((handle_P485->Instance->ISR & USART_ISR_TC) == USART_ISR_TC)   //!= 0    && (handle->Instance->ISR & USART_ISR_TXE) == USART_ISR_TXE
	{
		// Enable receiver mode
		RS485_RECEIVER_EN();
		// check the src port
		CLEAR_BIT(handle_P485->Instance->RQR, USART_RQR_RXFRQ);
	}
	else
	{
		// Check if there are data on the line
		while ((handle_P485->Instance->ISR & USART_ISR_TXE) == USART_ISR_TXE)            //USART_ISR_TXE)
		{
			// Set the RS485 to transmitter
			RS485_RECEIVER_DIS();
			// Do nothing until the end of transmission
			taskYIELD();
		}
	}
	
	return H1DR1_OK;
}
/*-----------------------------------------------------------*/

/* -----------------------------------------------------------------------
	|															Commands																 	|
   ----------------------------------------------------------------------- 
*/
portBASE_TYPE demoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	static const int8_t *pcMessage = ( int8_t * ) "Bidirectional RS485 Modbus communication\r\n";
	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
	Module_Status result = H1DR1_OK;
	
	/* Remove compile time warnings about unused parameters, and check the
	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
	write buffer length is adequate, so does not check for buffer overflows. */
	( void ) pcCommandString;
	( void ) xWriteBufferLen;
	configASSERT( pcWriteBuffer );
	
	/* Respond to the command */
			strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);
		
		//
		/* Wait till the end of stream */
		//while(startMeasurementRanging != STOP_MEASUREMENT_RANGING){};
	
	if (result != H1DR1_OK){
		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
	}

	/* clean terminal output */
	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
			
	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}

/*-----------------------------------------------------------*/

static portBASE_TYPE modeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
{
	Module_Status result = H1DR1_OK;
  int8_t *pcParameterString1;
	int8_t *pcParameterString2;
	int8_t *pcParameterString3;
  portBASE_TYPE xParameterStringLength1 = 0;
	portBASE_TYPE xParameterStringLength2 = 0;
	portBASE_TYPE xParameterStringLength3 = 0;
  static const int8_t *pcMessage = ( int8_t * ) "Setup RS485 port mode!\r\n";
	static const int8_t *pcMessageWrongParam = ( int8_t * ) "Wrong parameter!\r\n";

  /* Remove compile time warnings about unused parameters, and check the
  write buffer is not NULL.  NOTE - for simplicity, this example assumes the
  write buffer length is adequate, so does not check for buffer overflows. */
  ( void ) xWriteBufferLen;
  configASSERT( pcWriteBuffer );

  /* 1st parameter for RS485 port mode */
  pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
	/* 2nd parameter for RS485 Modbus mode or src port in case of BRIDGE mode */
  pcParameterString2 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 2, &xParameterStringLength2);
	/* 3rd parameter for baud rate in case of BRIDGE mode */
  pcParameterString3 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 3, &xParameterStringLength3);
	
	/* Respond to the command */
	if (pcParameterString2[0] == 'p' && !strncmp((const char *)pcParameterString1, "bridge", 6)) 
	{
		H1DR1_Mode=BRIDGE;
		src_port = ( uint8_t ) atol( ( char * ) pcParameterString2+1 );
		Br_baud_rate = ( uint32_t ) atol( ( char * ) pcParameterString3 );
		
	}
	else if (NULL != pcParameterString2 && !strncmp((const char *)pcParameterString1, "mb", 2))
	{
		if (!strncmp((const char *)pcParameterString1, "rtu", 3))
		{
			H1DR1_Mode=RTU;
		}
		else if (!strncmp((const char *)pcParameterString1, "ascii", 5))
		{
			H1DR1_Mode=ASCII;
		}
		else result=H1DR1_ERR_WrongParams;
	}
	else result=H1DR1_ERR_WrongParams;
	
	
  if (H1DR1_ERR_WrongParams == result)
  {
    strcpy( ( char * ) pcWriteBuffer, ( char * ) pcMessageWrongParam );
  }
	// execute the the calibation API
	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);

	return H1DR1_OK;	
}



/*-----------------------------------------------------------*/


/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
