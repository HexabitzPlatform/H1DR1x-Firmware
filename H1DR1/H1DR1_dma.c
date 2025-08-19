/*
 BitzOS (BOS) V0.4.0 - Copyright (C) 2017-2025 Hexabitz
 All rights reserved

 File Name  : H1DR1_dma.c
 Description: Initialize and manage DMA for UART (P1-P6).
 DMA: Setup, start/stop, mode switching for UART RX (Channels 1-6).
 CRC8: Compute CRC8 for message validation.
 */

/* Includes ****************************************************************/
#include "BOS.h"

/* Exported Variables ******************************************************/
DMA_HandleTypeDef *UARTDMAHandler[6];
CRC_HandleTypeDef hcrc;

/***************************************************************************/
/************************ Private function Definitions *********************/
/***************************************************************************/

void DMA_Init(void){

/* DMA controller clock enable */
__DMA1_CLK_ENABLE();
__DMA2_CLK_ENABLE();

/* DMA interrupt init */
/* DMA1_Channel1_IRQn interrupt configuration */
HAL_NVIC_SetPriority(DMA1_Channel1_IRQn,0,0);
HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

/* DMA1_Channel2_3_IRQn interrupt configuration */
HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn,0,0);
HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

/* DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn interrupt configuration */
HAL_NVIC_SetPriority(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn,0,0);
HAL_NVIC_EnableIRQ(DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQn);

}

/***************************************************************************/
/* Setup and start Messaging DMAs */
BOS_Status SetupMessagingRxDMAs(void){
	BOS_Status Status =BOS_OK;

#ifdef _P1
	if(PortStatus[P1] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P1,UARTDMAHandler[UART_DMA_PORT_INDEX_P1]))
			return Status =BOS_ERROR;
	}
#endif

#ifdef _P2
	if(PortStatus[P2] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P2,UARTDMAHandler[UART_DMA_PORT_INDEX_P2]))
			return Status =BOS_ERROR;
	}
#endif

#ifdef _P3	
	if(PortStatus[P3] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P3,UARTDMAHandler[UART_DMA_PORT_INDEX_P3]))
			return Status =BOS_ERROR;
	}
#endif

#ifdef _P4		
	if(PortStatus[P4] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P4,UARTDMAHandler[UART_DMA_PORT_INDEX_P4]))
			return Status =BOS_ERROR;
	}
#endif

#ifdef _P5		
	if(PortStatus[P5] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P5,UARTDMAHandler[UART_DMA_PORT_INDEX_P5]))
			return Status =BOS_ERROR;
	}
#endif

#ifdef _P6
	if(PortStatus[P6] == FREE){
		if(BOS_OK != DMA_MSG_RX_Setup(UART_P6,UARTDMAHandler[UART_DMA_PORT_INDEX_P6]))
			return Status =BOS_ERROR;
	}
#endif

	return Status;
}

/***************************************************************************/
/* Messaging DMA RX setup */
BOS_Status DMA_MSG_RX_Setup(UART_HandleTypeDef *huart,DMA_HandleTypeDef *hDMA){
	BOS_Status Status =BOS_OK;

	if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(huart,(uint8_t* )&UARTRxBuf[GetPort(huart) - 1],MSG_RX_BUF_SIZE))
		return Status =BOS_ERROR;

	__HAL_DMA_DISABLE_IT(hDMA,DMA_IT_HT);

	return Status;
}

/***************************************************************************/
/* Streaming DMA setup */
BOS_Status DMA_STREAM_Setup(UART_HandleTypeDef *huartSrc,UART_HandleTypeDef *huartDst,uint16_t num){
	BOS_Status Status =BOS_OK;
	DMA_HandleTypeDef *hDMA;
	uint8_t port, dstPort;

	port =GetPort(huartSrc);
	dstPort =GetPort(huartDst);
	hDMA =UARTDMAHandler[port - 1];

	/* dstPort = 0 this mean we will receive stream data on RAM memory incoming from a destination module */
	if(dstPort == 0){
		/* set DMA index corresponding to UART to zero, so that the DMA starts writing from the beginning of the specific buffer */
		IndexProcess[GetPort(huartSrc) - 1] =0;
		memset(RawDataBuffer[GetPort(huartSrc) - 1],0,STREAM_BUF_SIZE);
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(huartSrc,RawDataBuffer[GetPort(huartSrc) - 1],num))
			return Status =BOS_ERROR;
		__HAL_DMA_DISABLE_IT(hDMA,DMA_IT_HT);
	}
	else{
		if(HAL_OK != HAL_UARTEx_ReceiveToIdle_DMA(huartSrc,(uint8_t* )(&(huartDst->Instance->TDR)),num))
			return Status =BOS_ERROR;
		__HAL_DMA_DISABLE_IT(hDMA,DMA_IT_HT);
	}

	return Status;
}

/***************************************************************************/
/* Stop (Stream or message) DMA */
BOS_Status StopDMA(uint8_t port){
	BOS_Status Status =BOS_OK;
	DMA_HandleTypeDef *hDMA;
	UART_HandleTypeDef *huartSrc;

	huartSrc =GetUart(port);
	hDMA =UARTDMAHandler[port - 1];

	if(HAL_OK != HAL_UART_DMAStop(huartSrc))
		return Status =BOS_ERROR;

	hDMA->Instance->CNDTR =0;
	if(PortStatus[port] == STREAM){
		dmaStreamCount[port - 1] =0;
		dmaStreamTotal[port - 1] =0;
	}

	return Status;
}

/***************************************************************************/
/* Switch messaging DMA channels to streaming */
BOS_Status SwitchMsgDMAToStream(uint8_t port){
	BOS_Status Status =BOS_OK;
	UART_HandleTypeDef *huartSrc;

	if(BOS_OK != StopDMA(port))
		return Status =BOS_ERROR;

	huartSrc =GetUart(port);
	HAL_UART_MspInit(huartSrc);

	return Status;
}

/***************************************************************************/
/* Switch streaming DMA channel to messaging */
BOS_Status SwitchStreamDMAToMsg(uint8_t port){
	BOS_Status Status =BOS_OK;
	UART_HandleTypeDef *huartSrc;

	if(BOS_OK != StopDMA(port))
		return Status =BOS_ERROR;

	huartSrc =GetUart(port);
	/* Initialize a messaging DMA using same channels */
	HAL_UART_MspInit(huartSrc);
	/* change port status */
	PortStatus[GetPort(UARTDMAHandler[port - 1]->Parent)] =FREE;

	dmaStreamDst[port - 1] =0;
	IndexProcess[port - 1] =0;
	/* Read this port again in messaging mode */
	if(BOS_OK != DMA_MSG_RX_Setup(GetUart(port),UARTDMAHandler[port - 1]))
		return Status =BOS_ERROR;

	return Status;
}
/***************************************************************************/
/* CRC16 Modbus Init function */
void CRC_Init(void) {
    hcrc.Instance = CRC;
    hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
    hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE; // سنحدد polynomial يدوي
    hcrc.Init.GeneratingPolynomial = 0x8005; // Polynomial CRC16 Modbus
    hcrc.Init.CRCLength = CRC_POLYLENGTH_16B;
    hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_BYTE; // عكس البايتات ليتوافق مع Modbus
    hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_ENABLE; // عكس الخرج أيضا
    hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES; // نمرر البيانات كـ bytes
    HAL_CRC_Init(&hcrc);
}

/***************************************************************************/
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc) {
    if(hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_ENABLE();
    }
}

/***************************************************************************/
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc) {
    if(hcrc->Instance == CRC) {
        __HAL_RCC_CRC_CLK_DISABLE();
    }
}

/***************************************************************************/
/* Calculate CRC16 Modbus for a data buffer */
uint16_t CalculateCRC16(uint8_t *pBuffer, uint16_t size) {
    if (pBuffer == NULL || size == 0) {
        return 0;
    }

    /* HAL_CRC_Calculate expects data in uint32_t words, so we handle packing manually */
    uint32_t temp_buffer[64]; // حجم مؤقت، يمكن تكبيره إذا لزم
    uint16_t i;

    // تعبئة buffer مؤقت بكلمات 32 بت
    for (i = 0; i < size; i++) {
        ((uint8_t*)temp_buffer)[i] = pBuffer[i];
    }

    return (uint16_t)HAL_CRC_Calculate(&hcrc, temp_buffer, (size + 3) / 4);
}

/***************************************************************************/
/***************** (C) COPYRIGHT HEXABITZ ***** END OF FILE ****************/
