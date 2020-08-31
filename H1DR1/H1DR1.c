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
uint8_t Slave_add;
uint16_t Reg_address;
uint16_t Nu_regiters;
uint16_t TimeOut;
//xMBMHandle xMBMaster;
unsigned short ReadBuffer[253]={0};
unsigned short WriteBuffer;
TaskHandle_t ModbusHandle = NULL;



/* Private function prototypes -----------------------------------------------*/	 
void ModbusTask(void * argument);
	
//extern void vMBPTimerISR( void );

/* Create CLI commands --------------------------------------------------------*/
//static portBASE_TYPE demoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
//static portBASE_TYPE modeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
//static portBASE_TYPE readCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
//static portBASE_TYPE writeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
//static portBASE_TYPE multiwriteCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );
//static portBASE_TYPE stimeoutCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString );

///* CLI command structure : demo */
//const CLI_Command_Definition_t demoCommandDefinition =
//{
//	( const int8_t * ) "demo", /* The command string to type. */
//	( const int8_t * ) "demo:\r\n Demo bidirectional RS485 communication. Requires two modules\r\n\r\n",
//	demoCommand, /* The function to run. */
//	0 /* no parameter is expected. */
//};
///*-----------------------------------------------------------*/
///* CLI command structure : mode */
//const CLI_Command_Definition_t modeCommandDefinition =
//{
//  ( const int8_t * ) "mode", /* The command string to type. */
//  ( const int8_t * ) "mode:\r\n Set RS485 port mode\r\n\r\n",
//  modeCommand, /* The function to run. */
//  -1 /* Multiparameters are expected. */
//};
///*-----------------------------------------------------------*/
///* CLI command structure : read */
//const CLI_Command_Definition_t readCommandDefinition =
//{
//	( const int8_t * ) "read", /* The command string to type. */
//	( const int8_t * ) "read:\r\n Read from Modbus slave\r\n\r\n",
//	readCommand, /* The function to run. */
//	3 /* Three parameters are expected. */
//};
///*-----------------------------------------------------------*/
///* CLI command structure : write */
//const CLI_Command_Definition_t writeCommandDefinition =
//{
//  ( const int8_t * ) "write", /* The command string to type. */
//  ( const int8_t * ) "write:\r\n Write to Modbus slave\r\n\r\n",
//  writeCommand, /* The function to run. */
//  3 /* Three parameters are expected. */
//};
///*-----------------------------------------------------------*/
///* CLI command structure : multiple write */
//const CLI_Command_Definition_t multiwriteCommandDefinition =
//{
//	( const int8_t * ) "multiwrite", /* The command string to type. */
//	( const int8_t * ) "multiwrite:\r\n Write multuple registers to Modbus slave\r\n\r\n",
//	multiwriteCommand, /* The function to run. */
//	-1 /* Multiparameters are expected. */
//};
///*-----------------------------------------------------------*/
///* CLI command structure : set timeout */
//const CLI_Command_Definition_t stimeoutCommandDefinition =
//{
//  ( const int8_t * ) "stimeout", /* The command string to type. */
//  ( const int8_t * ) "stimeout:\r\n Set Modbus slave timeout\r\n\r\n",
//  stimeoutCommand, /* The function to run. */
//  1 /* One parameter is expected. */
//};

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
//				case (RTU):
//					Br_baud_rate = ( (uint32_t) cMessage[port-1][1+shift] << 24 ) + ( (uint32_t) cMessage[port-1][2+shift] << 16 ) + ( (uint32_t) cMessage[port-1][3+shift] << 8 ) + cMessage[port-1][4+shift];
//					Parity_bit = ( (uint32_t) cMessage[port-1][5+shift] << 24 ) + ( (uint32_t) cMessage[port-1][6+shift] << 16 ) + ( (uint32_t) cMessage[port-1][7+shift] << 8 ) + cMessage[port-1][8+shift];
//					SetupModbusRTU(Br_baud_rate, Parity_bit);
//					break;
//				case (ASCII):
//					Br_baud_rate = ( (uint32_t) cMessage[port-1][1+shift] << 24 ) + ( (uint32_t) cMessage[port-1][2+shift] << 16 ) + ( (uint32_t) cMessage[port-1][3+shift] << 8 ) + cMessage[port-1][4+shift];
//					Parity_bit = ( (uint32_t) cMessage[port-1][5+shift] << 24 ) + ( (uint32_t) cMessage[port-1][6+shift] << 16 ) + ( (uint32_t) cMessage[port-1][7+shift] << 8 ) + cMessage[port-1][8+shift];
//					SetupModbusASCII(Br_baud_rate, Parity_bit);
//					break;
				default :
					break;		
			}
			break;
			
//		case (CODE_H1DR1_READ):
//			Slave_add = cMessage[port-1][1+shift];
//			Reg_address = ( (uint16_t) cMessage[port-1][2+shift] << 8 ) + cMessage[port-1][3+shift];
//			Nu_regiters = ( (uint16_t) cMessage[port-1][4+shift] << 8 ) + cMessage[port-1][5+shift];
//			//ReadModbusRegister(Slave_add, Reg_address, Nu_regiters, );
//		break;
//		
//		case (CODE_H1DR1_WRITE):
//			Slave_add = cMessage[port-1][1+shift];
//			Reg_address = ( (uint16_t) cMessage[port-1][2+shift] << 8 ) + cMessage[port-1][3+shift];
//			Nu_regiters = ( (uint16_t) cMessage[port-1][4+shift] << 8 ) + cMessage[port-1][5+shift];
//			//WriteModbusRegister(Slave_add, Reg_address, Data );
//		 break;
//		
//		case (CODE_H1DR1_MULTIWRITE):
//			Slave_add = cMessage[port-1][1+shift];
//			Reg_address = ( (uint16_t) cMessage[port-1][2+shift] << 8 ) + cMessage[port-1][3+shift];
//			Nu_regiters = ( (uint16_t) cMessage[port-1][4+shift] << 8 ) + cMessage[port-1][5+shift];
//			//WriteModbusMultiRegisters(Slave_add, Reg_address, Nu_regiters, );
//		 break;
//		
//		case (CODE_H1DR1_STIMEOUT):
//			TimeOut = ( (uint16_t) cMessage[port-1][1+shift] << 8 ) + cMessage[port-1][2+shift];
//			SetTimeOut(TimeOut);
//		 break;
		
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
//	FreeRTOS_CLIRegisterCommand( &demoCommandDefinition );
//	FreeRTOS_CLIRegisterCommand( &modeCommandDefinition );
//	FreeRTOS_CLIRegisterCommand( &readCommandDefinition );
//	FreeRTOS_CLIRegisterCommand( &writeCommandDefinition );
//	FreeRTOS_CLIRegisterCommand( &multiwriteCommandDefinition );
//	FreeRTOS_CLIRegisterCommand( &stimeoutCommandDefinition );
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
	// Disable MB in case it is running
//	if ( H1DR1_Mode == RTU || H1DR1_Mode == ASCII ) {
//	eMBMClose(xMBMaster);   
//	}
	// Set module mode
	H1DR1_Mode=BRIDGE;
	Src_port=Source_p;
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

/*-----------------------------------------------------------*/

///* --- setup the Modbus mode as RTU
//*/
//Module_Status SetupModbusRTU(uint32_t BaudRate, uint32_t ParityBit)
//{
//	ULONG Pbit = (ULONG) ParityBit;
//	ULONG ulBaudRate = (ULONG) BaudRate;
//	// Reinit Modbus port
//	if (H1DR1_Mode == BRIDGE) {
//		Unbridge(Src_port, P_RS485);
//		}
//	// Set module mode
//	H1DR1_Mode=RTU;
//	// Initialize Modbus port as RTU
//	if ( MB_ENOERR != eMBMSerialInit( &xMBMaster,  MB_RTU, 1, ulBaudRate, Pbit ) ) {
//		return H1DR1_ERROR;}
//	else 	
//		return H1DR1_OK;	
//}

/*-----------------------------------------------------------*/

/* --- setup the Modbus mode as ASCII
*/
//Module_Status SetupModbusASCII(uint32_t BaudRate, uint32_t ParityBit)
//{
//	ULONG Pbit = (ULONG) ParityBit;
//	ULONG ulBaudRate = (ULONG) BaudRate;
//	
//	// Reinit Modbus port
//	if (H1DR1_Mode == BRIDGE) {
//		Unbridge(Src_port, P_RS485);
//		}
//	// Set module mode
//	H1DR1_Mode=ASCII;
//	// Initialize Modbus port as ASCII
//	if ( MB_ENOERR != eMBMSerialInit( &xMBMaster,  MB_ASCII, 1, ulBaudRate, Pbit ) ) { 
//		return H1DR1_ERROR;}
//	else 	
//		return H1DR1_OK;
//}

/*-----------------------------------------------------------*/

/* --- read data from a slave on Modbus port FC: 0x03
*/
//Module_Status ReadModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd, uint8_t NofReg, unsigned short * DataBuffer )
//{
//	unsigned short *Buffer=DataBuffer;

//	if (MB_ENOERR != eMBMReadHoldingRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, Buffer)) { 
//		return H1DR1_ERROR;}
//	else
//		return H1DR1_OK;
//}

/*-----------------------------------------------------------*/

/* --- write data to a slave on Modbus port FC: 0x06
*/
//Module_Status WriteModbusRegister(uint8_t SlaveAdd, uint32_t RegAdd, uint32_t Data)
//{

//		if (MB_ENOERR != eMBMWriteSingleRegister(xMBMaster, SlaveAdd, RegAdd, Data)) {
//		return H1DR1_ERROR;}
//	else
//		return H1DR1_OK;
//}
/*-----------------------------------------------------------*/

/* --- write N data bytes to a slave on Modbus port FC: 0x10
*/
//Module_Status WriteModbusMultiRegisters(uint8_t SlaveAdd, uint32_t RegAdd, uint8_t NofReg, uint16_t *Data)
//{
//	USHORT *InBuffer = (USHORT *)Data;
//	
//		if (MB_ENOERR != eMBMWriteMultipleRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, InBuffer)) {
//		return H1DR1_ERROR;}
//	else
//		return H1DR1_OK;
//}

/*-----------------------------------------------------------*/

/* --- set timeout to a slave on Modbus port
*/
//Module_Status SetTimeOut(uint16_t MiliSeconds)
//{	
//	USHORT timeout = (USHORT) MiliSeconds;
//	
//	if (MB_ENOERR != eMBMSetSlaveTimeout( xMBMaster, timeout )) {
//		return H1DR1_ERROR;}
//	else
//		return H1DR1_OK;
//}


/* -----------------------------------------------------------------------
	|															Commands																 	|
   ----------------------------------------------------------------------- 
*/
//portBASE_TYPE demoCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//	static const int8_t *pcMessage = ( int8_t * ) "Bidirectional RS485 Modbus communication\r\n";
//	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
//	Module_Status result = H1DR1_OK;
//	
//	/* Remove compile time warnings about unused parameters, and check the
//	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//	write buffer length is adequate, so does not check for buffer overflows. */
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );
//	
//	/* Respond to the command */
//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);
//		
//	/* Wait till the end of stream */
//	//while(startMeasurementRanging != STOP_MEASUREMENT_RANGING){};
//	
//	if (result != H1DR1_OK){
//		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
//	}

//	/* clean terminal output */
//	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
//			
//	/* There is no more data to return after this single string, so return
//	pdFALSE. */
//	return pdFALSE;
//}

/*-----------------------------------------------------------*/

//static portBASE_TYPE modeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//  static const int8_t *pcMessage = ( int8_t * ) "RS485 port mode has been set!\r\n";
//	static const int8_t *pcMessageWrongParam = ( int8_t * ) "Wrong parameter!\r\n";
//	Module_Status result = H1DR1_OK;
//  int8_t *pcParameterString1;
//	int8_t *pcParameterString2;
//	int8_t *pcParameterString3;
//  portBASE_TYPE xParameterStringLength1 = 0;
//	portBASE_TYPE xParameterStringLength2 = 0;
//	portBASE_TYPE xParameterStringLength3 = 0;
//	uint8_t Pb;

//  /* Remove compile time warnings about unused parameters, and check the
//  write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//  write buffer length is adequate, so does not check for buffer overflows. */
//  ( void ) xWriteBufferLen;
//  configASSERT( pcWriteBuffer );

//  /* 1st parameter for RS485 port mode */
//  pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
//	/* 2nd parameter for parity bit or src port in case of BRIDGE mode */
//  pcParameterString2 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 2, &xParameterStringLength2);
//	/* 3rd parameter for baud rate */
//  pcParameterString3 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 3, &xParameterStringLength3);
//	
//	/* Respond to the command */
//	if (pcParameterString2[0] == 'p' && !strncmp((const char *)pcParameterString1, "bridge", 6)) 
//	{
//		Src_port = ( uint8_t ) atol( ( char * ) pcParameterString2+1 );
//		Br_baud_rate = ( uint32_t ) atol( ( char * ) pcParameterString3 );
//		if (Br_baud_rate<=921600 && Br_baud_rate>=300)
//		{
//		SetupBridgeMode(Src_port,Br_baud_rate);
//		}
//		else result=H1DR1_ERR_WrongParams;	
//	}
//	else if (NULL != pcParameterString2 && !strncmp((const char *)pcParameterString1, "rtu", 3))
//	{
//		Br_baud_rate = ( uint32_t ) atol( ( char * ) pcParameterString3 );
//		if (Br_baud_rate<=115200 && Br_baud_rate>=2400)
//		{
//			Pb = ( uint8_t ) atol( ( char * ) pcParameterString2 );
//			switch (Pb){
//				case 0: 
//					Parity_bit = UART_PARITY_NONE;
//					break;
//				case 1:
//					Parity_bit = UART_PARITY_ODD;
//					break;
//				case 2:
//					Parity_bit = UART_PARITY_EVEN;
//					break;
//				default: 
//					result=H1DR1_ERR_WrongParams;
//					break;
//			}	
//			SetupModbusRTU(Br_baud_rate, Parity_bit);
//		}
//		else result=H1DR1_ERR_WrongParams;
//	}
//	else if (!strncmp((const char *)pcParameterString1, "ascii", 5))
//	{
//		Br_baud_rate = ( uint32_t ) atol( ( char * ) pcParameterString3 );
//		if (Br_baud_rate<=115200 && Br_baud_rate>=2400)
//		{
//			Pb = ( uint8_t ) atol( ( char * ) pcParameterString2 );
//			switch (Pb){
//				case 0: 
//					Parity_bit = UART_PARITY_NONE;
//					break;
//				case 1:
//					Parity_bit = UART_PARITY_ODD;
//					break;
//				case 2:
//					Parity_bit = UART_PARITY_EVEN;
//					break;
//				default: 
//					result=H1DR1_ERR_WrongParams;
//					break;
//			}
//			SetupModbusASCII(Br_baud_rate, Parity_bit);
//		}
//	}
//	else result=H1DR1_ERR_WrongParams;
//	
//  if (H1DR1_ERR_WrongParams == result)
//  {
//    strcpy( ( char * ) pcWriteBuffer, ( char * ) pcMessageWrongParam );
//  }

//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);

//	return H1DR1_OK;	
//}

/*-----------------------------------------------------------*/

//portBASE_TYPE readCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//	static const int8_t *pcMessage = ( int8_t * ) "Data has been read from a slave\r\n";
//	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
//	Module_Status result = H1DR1_OK;
//  int8_t *pcParameterString1;
//	int8_t *pcParameterString2;
//	int8_t *pcParameterString3;
//  portBASE_TYPE xParameterStringLength1 = 0;
//	portBASE_TYPE xParameterStringLength2 = 0;
//	portBASE_TYPE xParameterStringLength3 = 0;
//	
//	/* Remove compile time warnings about unused parameters, and check the
//	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//	write buffer length is adequate, so does not check for buffer overflows. */
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );
//	
//	/* 1st parameter for slave address */
//  pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
//	/* 2nd parameter for first register address */
//  pcParameterString2 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 2, &xParameterStringLength2);
//	/* 3rd parameter for number of regsiters */
//  pcParameterString3 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 3, &xParameterStringLength3);
//	
//	/* Respond to the command */
//	Slave_add = ( uint8_t ) atol( ( char * ) pcParameterString1 );
//	Reg_address = ( uint16_t ) atol( ( char * ) pcParameterString2 );
//	Nu_regiters = ( uint16_t ) atol( ( char * ) pcParameterString3 );
//	
//	if ( H1DR1_ERROR != ReadModbusRegister(Slave_add, Reg_address, Nu_regiters, ReadBuffer) ){
//		result=H1DR1_OK;
//	}
//	else result=H1DR1_ERR_WrongParams;
//	
//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);
//	/* Wait till the end of stream */
//	//while(startMeasurementRanging != STOP_MEASUREMENT_RANGING){};
//	
//	if (result != H1DR1_OK){
//		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
//	}

//	/* clean terminal output */
//	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
//			
//	/* There is no more data to return after this single string, so return
//	pdFALSE. */
//	return pdFALSE;
//}

///*-----------------------------------------------------------*/

//portBASE_TYPE writeCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//	static const int8_t *pcMessage = ( int8_t * ) "Data has been sent to a slave\r\n";
//	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
//	Module_Status result = H1DR1_OK;
//  int8_t *pcParameterString1;
//	int8_t *pcParameterString2;
//	int8_t *pcParameterString3;
//  portBASE_TYPE xParameterStringLength1 = 0;
//	portBASE_TYPE xParameterStringLength2 = 0;
//	portBASE_TYPE xParameterStringLength3 = 0;
//	uint8_t databyte;
//	
//	/* Remove compile time warnings about unused parameters, and check the
//	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//	write buffer length is adequate, so does not check for buffer overflows. */
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );
//	
//	/* 1st parameter for slave address */
//  pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
//	/* 2nd parameter for first register address */
//  pcParameterString2 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 2, &xParameterStringLength2);
//	/* 3rd parameter for number of regsiters */
//  pcParameterString3 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 3, &xParameterStringLength3);
//	
//	/* Respond to the command */
//	Slave_add = ( uint8_t ) atol( ( char * ) pcParameterString1 );
//	Reg_address = ( uint16_t ) atol( ( char * ) pcParameterString2 );
//	databyte = ( uint8_t ) atol( ( char * ) pcParameterString3 );
//	
//	if ( H1DR1_ERROR != WriteModbusRegister( Slave_add, Reg_address, databyte) ){
//		result=H1DR1_OK;
//	}
//	else result=H1DR1_ERR_WrongParams;
//	
//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);
//	/* Wait till the end of stream */
//	//while(startMeasurementRanging != STOP_MEASUREMENT_RANGING){};
//	
//	if (result != H1DR1_OK){
//		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
//	}

//	/* clean terminal output */
//	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
//			
//	/* There is no more data to return after this single string, so return
//	pdFALSE. */
//	return pdFALSE;
//}

///*-----------------------------------------------------------*/

//portBASE_TYPE multiwriteCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//	static const int8_t *pcMessage = ( int8_t * ) "Multiple registers data has been sent to a slave\r\n";
//	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
//	Module_Status result = H1DR1_OK;
//  int8_t *pcParameterString1;
//	int8_t *pcParameterString2;
//	int8_t *pcParameterString3;
//  portBASE_TYPE xParameterStringLength1 = 0;
//	portBASE_TYPE xParameterStringLength2 = 0;
//	portBASE_TYPE xParameterStringLength3 = 0;
////	portBASE_TYPE xParameterStringLength4 = 0;
//	
//	/* Remove compile time warnings about unused parameters, and check the
//	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//	write buffer length is adequate, so does not check for buffer overflows. */
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );
//	
//	/* 1st parameter for slave address */
//  pcParameterString1 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
//	/* 2nd parameter for first register address */
//  pcParameterString2 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 2, &xParameterStringLength2);
//	/* 3rd parameter for number of regsiters */
//  pcParameterString3 = ( int8_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 3, &xParameterStringLength3);
//	
//	/* Respond to the command */
//	Slave_add = ( uint8_t ) atol( ( char * ) pcParameterString1 );
//	Reg_address = ( uint16_t ) atol( ( char * ) pcParameterString2 );
//	Nu_regiters = ( uint16_t ) atol( ( char * ) pcParameterString3 );
//	
//	/*if ( H1DR1_ERROR != WriteModbusMultiRegisters(Slave_add, Reg_address, Nu_regiters, Buffer) ){
//		result=H1DR1_OK;
//	}
//	else result=H1DR1_ERR_WrongParams;*/
//	
//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);
//	/* Wait till the end of stream */
//	//while(startMeasurementRanging != STOP_MEASUREMENT_RANGING){};
//	
//	if (result != H1DR1_OK){
//		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
//	}

//	/* clean terminal output */
//	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
//			
//	/* There is no more data to return after this single string, so return
//	pdFALSE. */
//	return pdFALSE;
//}


///*-----------------------------------------------------------*/

//portBASE_TYPE stimeoutCommand( int8_t *pcWriteBuffer, size_t xWriteBufferLen, const int8_t *pcCommandString )
//{
//	static const int8_t *pcMessage = ( int8_t * ) "Response timeout has been set\r\n";
//	static const int8_t *pcMessageError = ( int8_t * ) "Wrong parameter\r\n";
//	Module_Status result = H1DR1_OK;
//  uint16_t *pcParameterString1;
//  portBASE_TYPE xParameterStringLength1 = 0;
//	uint16_t timeout;
//	
//	/* Remove compile time warnings about unused parameters, and check the
//	write buffer is not NULL.  NOTE - for simplicity, this example assumes the
//	write buffer length is adequate, so does not check for buffer overflows. */
//	( void ) pcCommandString;
//	( void ) xWriteBufferLen;
//	configASSERT( pcWriteBuffer );
//	
//	/* 1st parameter for timeout in miliseconds */
//  pcParameterString1 = ( uint16_t * ) FreeRTOS_CLIGetParameter (pcCommandString, 1, &xParameterStringLength1);
//	
//	/* Respond to the command */
//	timeout = ( uint8_t ) atol( ( char * ) pcParameterString1 );
//	
//	if ( H1DR1_ERROR != SetTimeOut(timeout) ){
//		result=H1DR1_OK;
//	}
//	else result=H1DR1_ERR_WrongParams;
//	
//	if (result != H1DR1_OK){
//		strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessageError);
//	}
//	
//	strcpy(( char * ) pcWriteBuffer, ( char * ) pcMessage);

//	/* clean terminal output */
//	memset((char *) pcWriteBuffer, 0, strlen((char *)pcWriteBuffer));
//			
//	/* There is no more data to return after this single string, so return
//	pdFALSE. */
//	return pdFALSE;
//}

/************************ (C) COPYRIGHT HEXABITZ *****END OF FILE****/
