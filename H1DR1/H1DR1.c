/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H1DR1.c
 Description: Core source for H1DR1 module, managing RGB LED control.
 Features: CLI commands for LED on/off, color, RGB, toggle, pulse, sweep, dim.
 Peripherals: UART1-6, TIM2-4 (PWM for RGB), low-power modes (stop/standby).
 Flash: Stores topology and command snippets in RO sections.
 Tasks: RGBledTask handles dynamic LED effects (pulse, sweep, dim).
 */
/* Includes ****************************************************************/
#include "BOS.h"
#include "H1DR1.h"
#include "mbm.h"

/* Exported Typedef ********************************************************/
/* Define UART variables */
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* Private Variables *******************************************************/

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

/* Module Parameters */
ModuleParam_t ModuleParam[NUM_MODULE_PARAMS] = { 0 };

/* Private Function Prototypes *********************************************/
void Module_Peripheral_Init(void);
void SetupPortForRemoteBootloaderUpdate(uint8_t port);
uint8_t ClearROtopology(void);
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift);
void MessagingTask(void const *argument);
/* Handler for RTC software timer */
static void RTC_HandleTimer(TimerHandle_t zTimer);

/* Handler for Timeout software timer */
static void TMOUT_HandleTimer(TimerHandle_t sTimer);

/* ISR for Modbus protocol port */
void prvvMBPUSART_RXNE_ISR(void);

/* Local Function Prototypes ***********************************************/

/* Create CLI commands *****************************************************/

/* CLI command structure ***************************************************/
/* CLI command structure : demo */

/***************************************************************************/
/************************ Private function Definitions *********************/
/***************************************************************************/
/* @brief  System Clock Configuration
 *         This function configures the system clock as follows:
 *            - System Clock source            = PLL (HSE)
 *            - SYSCLK(Hz)                     = 64000000
 *            - HCLK(Hz)                       = 64000000
 *            - AHB Prescaler                  = 1
 *            - APB1 Prescaler                 = 1
 *            - HSE Frequency(Hz)              = 8000000
 *            - PLLM                           = 1
 *            - PLLN                           = 16
 *            - PLLP                           = 2
 *            - Flash Latency(WS)              = 2
 *            - Clock Source for UART1,UART2,UART3 = 16MHz (HSI)
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSE; // Enable both HSI and HSE oscillators
	RCC_OscInitStruct.HSEState = RCC_HSE_ON; // Enable HSE (External High-Speed Oscillator)
	RCC_OscInitStruct.HSIState = RCC_HSI_ON; // Enable HSI (Internal High-Speed Oscillator)
	RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1; // No division on HSI
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT; // Default calibration value for HSI
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON; // Enable PLL
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE; // Set PLL source to HSE
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1; // Prescaler for PLL input
	RCC_OscInitStruct.PLL.PLLN = 16; // Multiplication factor for PLL
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; // PLLP division factor
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; // PLLQ division factor
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; // PLLR division factor
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	/** Initializes the CPU, AHB and APB buses clocks */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; // Select PLL as the system clock source
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1; // AHB Prescaler set to 1
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1; // APB1 Prescaler set to 1

	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2); // Configure system clocks with flash latency of 2 WS
}

/***************************************************************************/
/* enable stop mode regarding only UART1 , UART2 , and UART3 */
BOS_Status EnableStopModebyUARTx(uint8_t port) {

	UART_WakeUpTypeDef WakeUpSelection;
	UART_HandleTypeDef *huart = GetUart(port);

	if ((huart->Instance == USART1) || (huart->Instance == USART2) || (huart->Instance == USART3)) {

		/* make sure that no UART transfer is on-going */
		while (__HAL_UART_GET_FLAG(huart, USART_ISR_BUSY) == SET);

		/* make sure that UART is ready to receive */
		while (__HAL_UART_GET_FLAG(huart, USART_ISR_REACK) == RESET);

		/* set the wake-up event:
		 * specify wake-up on start-bit detection */
		WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;
		HAL_UARTEx_StopModeWakeUpSourceConfig(huart, WakeUpSelection);

		/* Enable the UART Wake UP from stop mode Interrupt */
		__HAL_UART_ENABLE_IT(huart, UART_IT_WUF);

		/* enable MCU wake-up by LPUART */
		HAL_UARTEx_EnableStopMode(huart);

		/* enter STOP mode */
		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	} else
		return BOS_ERROR;

}

/***************************************************************************/
/* Enable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status EnableStandbyModebyWakeupPinx(WakeupPins_t wakeupPins) {

	/* Clear the WUF FLAG */
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);

	/* Enable the WAKEUP PIN */
	switch (wakeupPins) {

	case PA0_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
		break;

	case PA2_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
		break;

	case PB5_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
		break;

	case PC13_PIN:
		HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
		break;

	case NRST_PIN:
		/* do no thing*/
		break;
	}

	/* Enable SRAM content retention in Standby mode */
	HAL_PWREx_EnableSRAMRetention();

	/* Finally enter the standby mode */
	HAL_PWR_EnterSTANDBYMode();

	return BOS_OK;
}

/***************************************************************************/
/* Disable standby mode regarding wake-up pins:
 * WKUP1: PA0  pin
 * WKUP4: PA2  pin
 * WKUP6: PB5  pin
 * WKUP2: PC13 pin
 * NRST pin
 *  */
BOS_Status DisableStandbyModeWakeupPinx(WakeupPins_t wakeupPins) {

	/* The standby wake-up is same as a system RESET:
	 * The entire code runs from the beginning just as if it was a RESET.
	 * The only difference between a reset and a STANDBY wake-up is that, when the MCU wakes-up,
	 * The SBF status flag in the PWR power control/status register (PWR_CSR) is set */
	if (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET) {
		/* clear the flag */
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

		/* Disable  Wake-up Pinx */
		switch (wakeupPins) {

		case PA0_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); /* PA0 */
			break;

		case PA2_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN4); /* PA2 */
			break;

		case PB5_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN6); /* PB5 */
			break;

		case PC13_PIN:
			HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2); /* PC13 */
			break;

		case NRST_PIN:
			/* do no thing*/
			break;
		}

		IND_blink(1000);

	} else
		return BOS_OK;

}

/***************************************************************************/
/* Save Command Topology in Flash RO */
uint8_t SaveTopologyToRO(void) {

	HAL_StatusTypeDef flashStatus = HAL_OK;

	/* flashAdd is initialized with 8 because the first memory room in topology page
	 * is reserved for module's ID */
	uint16_t flashAdd = 8;
	uint16_t temp = 0;

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();

	/* Erase Topology page */
	FLASH_PageErase(FLASH_BANK_2, TOPOLOGY_PAGE_NUM);

	/* Wait for an Erase operation to complete */
	flashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);

	if (flashStatus != HAL_OK) {
		/* return FLASH error code */
		return pFlash.ErrorCode;
	}

	else {
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}

	/* Save module's ID and topology */
	if (myID) {

		/* Save module's ID */
		temp = (uint16_t) (N << 8) + myID;

		/* Save module's ID in Flash memory */
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, TOPOLOGY_START_ADDRESS, temp);

		/* Wait for a Write operation to complete */
		flashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);

		if (flashStatus != HAL_OK) {
			/* return FLASH error code */
			return pFlash.ErrorCode;
		}

		else {
			/* If the program operation is completed, disable the PG Bit */
			CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
		}

		/* Save topology */
		for (uint8_t row = 1; row <= N; row++) {
			for (uint8_t column = 0; column <= MAX_NUM_OF_PORTS; column++) {
				/* Check the module serial number
				 * Note: there isn't a module has serial number 0
				 */
				if (Array[row - 1][0]) {
					/* Save each element in topology Array in Flash memory */
					HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, TOPOLOGY_START_ADDRESS + flashAdd,
							Array[row - 1][column]);
					/* Wait for a Write operation to complete */
					flashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);
					if (flashStatus != HAL_OK) {
						/* return FLASH error code */
						return pFlash.ErrorCode;
					} else {
						/* If the program operation is completed, disable the PG Bit */
						CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
						/* update new flash memory address */
						flashAdd += 8;
					}
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Save Command Snippets in Flash RO */
uint8_t SaveSnippetsToRO(void) {
	HAL_StatusTypeDef FlashStatus = HAL_OK;
	uint8_t snipBuffer[sizeof(Snippet_t) + 1] = { 0 };

	/* Unlock the FLASH control register access */
	HAL_FLASH_Unlock();
	/* Erase Snippets page */
	FLASH_PageErase(FLASH_BANK_2, SNIPPETS_PAGE_NUM);
	/* Wait for an Erase operation to complete */
	FlashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);

	if (FlashStatus != HAL_OK) {
		/* return FLASH error code */
		return pFlash.ErrorCode;
	} else {
		/* Operation is completed, disable the PER Bit */
		CLEAR_BIT(FLASH->CR, FLASH_CR_PER);
	}

	/* Save Command Snippets */
	int currentAdd = SNIPPETS_START_ADDRESS;
	for (uint8_t index = 0; index < NumOfRecordedSnippets; index++) {
		/* Check if Snippet condition is true or false */
		if (Snippets[index].Condition.ConditionType) {
			/* A marker to separate Snippets */
			snipBuffer[0] = 0xFE;
			memcpy((uint32_t*) &snipBuffer[1], (uint8_t*) &Snippets[index], sizeof(Snippet_t));
			/* Copy the snippet struct buffer (20 x NumOfRecordedSnippets). Note this is assuming sizeof(Snippet_t) is even */
			for (uint8_t j = 0; j < (sizeof(Snippet_t) / 4); j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, currentAdd, *(uint64_t*) &snipBuffer[j * 8]);
				FlashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 8;
				}
			}
			/* Copy the snippet commands buffer. Always an even number. Note the string termination char might be skipped */
			for (uint8_t j = 0; j < ((strlen(Snippets[index].CMD) + 1) / 4); j++) {
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, currentAdd, *(uint64_t*) (Snippets[index].CMD + j * 4));
				FlashStatus = FLASH_WaitForLastOperation((uint32_t) HAL_FLASH_TIMEOUT_VALUE);
				if (FlashStatus != HAL_OK) {
					return pFlash.ErrorCode;
				} else {
					/* If the program operation is completed, disable the PG Bit */
					CLEAR_BIT(FLASH->CR, FLASH_CR_PG);
					currentAdd += 8;
				}
			}
		}
	}
	/* Lock the FLASH control register access */
	HAL_FLASH_Lock();
}

/***************************************************************************/
/* Clear Array topology in SRAM and Flash RO */
uint8_t ClearROtopology(void) {
	/* Clear the Array */
	memset(Array, 0, sizeof(Array));
	N = 1;
	myID = 0;

	return SaveTopologyToRO();
}

/***************************************************************************/
/* Trigger ST factory bootloader update for a remote module */
void RemoteBootloaderUpdate(uint8_t src, uint8_t dst, uint8_t inport, uint8_t outport) {

	uint8_t myOutport = 0, lastModule = 0;
	int8_t *pcOutputString;

	/* 1. Get Route to destination module */
	myOutport = FindRoute(myID, dst);
	if (outport && dst == myID) { /* This is a 'via port' update and I'm the last module */
		myOutport = outport;
		lastModule = myID;
	} else if (outport == 0) { /* This is a remote update */
		if (NumberOfHops(dst)== 1)
		lastModule = myID;
		else
		lastModule = Route[NumberOfHops(dst)-1]; /* previous module = Route[Number of hops - 1] */
	}

	/* 2. If this is the source of the message, show status on the CLI */
	if (src == myID) {
		/* Obtain the address of the output buffer.  Note there is no mutual
		 * exclusion on this buffer as it is assumed only one command console
		 * interface will be used at any one time. */
		pcOutputString = FreeRTOS_CLIGetOutputBuffer();

		if (outport == 0)		// This is a remote module update
			sprintf((char*) pcOutputString, pcRemoteBootloaderUpdateMessage, dst);
		else
			// This is a 'via port' remote update
			sprintf((char*) pcOutputString, pcRemoteBootloaderUpdateViaPortMessage, dst, outport);

		strcat((char*) pcOutputString, pcRemoteBootloaderUpdateWarningMessage);
		writePxITMutex(inport, (char*) pcOutputString, strlen((char*) pcOutputString), cmd50ms);
		Delay_ms(100);
	}

	/* 3. Setup my inport and outport for bootloader update */
	SetupPortForRemoteBootloaderUpdate(inport);
	SetupPortForRemoteBootloaderUpdate(myOutport);

	/* 5. Build a DMA stream between my inport and outport */
	StartScastDMAStream(inport, myID, myOutport, myID, BIDIRECTIONAL, 0xFFFFFFFF, 0xFFFFFFFF, false);
}

/***************************************************************************/
/* Setup a port for remote ST factory bootloader update:
 * Set baudrate to 57600
 * Enable even parity
 * Set datasize to 9 bits
 */
void SetupPortForRemoteBootloaderUpdate(uint8_t port){

	UART_HandleTypeDef *huart =GetUart(port);
	HAL_UART_DeInit(huart);
	huart->Init.Parity = UART_PARITY_EVEN;
	huart->Init.WordLength = UART_WORDLENGTH_9B;
	HAL_UART_Init(huart);

	/* The CLI port RXNE interrupt might be disabled so enable here again to be sure */
	__HAL_UART_ENABLE_IT(huart,UART_IT_RXNE);

}

/***************************************************************************/
/*
 * @brief: Task for handling messaging.
 * @param: argument - pointer to the task argument.
 * @retval: None
 */
void MessagingTask(void const *argument) {

	vTaskDelete(NULL);
}

/***************************************************************************/
/* H1DR1 module initialization */
void Module_Peripheral_Init(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Array ports */
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_USART4_UART_Init();
	MX_USART5_UART_Init();
	MX_USART6_UART_Init();

	/* Circulating DMA Channels ON All Module */
	for (int i = 1; i <= NUM_OF_PORTS; i++) {
		if (GetUart(i) == &huart1) {
			dmaIndex[i - 1] = &(DMA1_Channel1->CNDTR);
		} else if (GetUart(i) == &huart2) {
			dmaIndex[i - 1] = &(DMA1_Channel2->CNDTR);
		} else if (GetUart(i) == &huart3) {
			dmaIndex[i - 1] = &(DMA1_Channel3->CNDTR);
		} else if (GetUart(i) == &huart4) {
			dmaIndex[i - 1] = &(DMA1_Channel4->CNDTR);
		} else if (GetUart(i) == &huart5) {
			dmaIndex[i - 1] = &(DMA1_Channel5->CNDTR);
		} else if (GetUart(i) == &huart6) {
			dmaIndex[i - 1] = &(DMA1_Channel6->CNDTR);
		}
	}

}

/***************************************************************************/
/* H1DR1 message processing task */
Module_Status Module_MessagingTask(uint16_t code, uint8_t port, uint8_t src, uint8_t dst, uint8_t shift) {
	Module_Status result = H1DR1_OK;


	switch (code) {

	default:
		result = H1DR1_ERR_UNKNOWNMESSAGE;
		break;
	}

	return result;
}
/***************************************************************************/
/* Get the port for a given UART */
uint8_t GetPort(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART4)
		return P1;
	else if (huart->Instance == USART2)
		return P2;
	else if (huart->Instance == USART6)
		return P3;
	else if (huart->Instance == USART1)
		return P4;
	else if (huart->Instance == USART5)
		return P5;
	else if (huart->Instance == USART6)
		return P6;

	return 0;
}

/***************************************************************************/
/* Register this module CLI Commands */
void RegisterModuleCLICommands(void) {

}

/***************************************************************************/
/* Samples a module parameter value based on parameter index.
 * paramIndex: Index of the parameter (1-based index).
 * value: Pointer to store the sampled float value.
 */
Module_Status GetModuleParameter(uint8_t paramIndex, float *value) {
	Module_Status status = BOS_OK;

	switch (paramIndex) {

	/* Invalid parameter index */
	default:
		status = BOS_ERR_WrongParam;
		break;
	}

	return status;
}
/***************************************************************************/
/****************************** Local Functions ****************************/
/***************************************************************************/
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
/***************************************************************************/
/***************************** General Functions ***************************/
/***************************************************************************/
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
	/* Initialize Modbus port as RTU */
	if (MB_ENOERR != eMBMSerialInit(&xMBMaster, MB_RTU, 1, ulBaudRate, Pbit)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}
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
	/* Initialize Modbus port as ASCII */
	if (MB_ENOERR != eMBMSerialInit(&xMBMaster, MB_ASCII, 1, ulBaudRate, Pbit)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}
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

	if (MB_ENOERR != eMBMReadHoldingRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, Buffer)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}
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
	if (MB_ENOERR != eMBMWriteSingleRegister(xMBMaster, SlaveAdd, RegAdd, Data)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}
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
	if (MB_ENOERR != eMBMWriteMultipleRegisters(xMBMaster, SlaveAdd, RegAdd, NofReg, InBuffer)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}
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
// */
Module_Status SetTimeOut(uint16_t MiliSeconds) {
	Module_Status Status;
	USHORT timeout = (USHORT) MiliSeconds;
	if (MB_ENOERR != eMBMSetSlaveTimeout(xMBMaster, timeout)) {
		Status = H1DR1_ERROR;
	} else {
		Status = H1DR1_OK;
	}

	return Status;
}

/***************************************************************************/
/********************************* Commands ********************************/
/***************************************************************************/

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
