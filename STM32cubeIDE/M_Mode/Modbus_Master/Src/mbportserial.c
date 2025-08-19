/* 
 * MODBUS Library: Skeleton port
 * Copyright (c) 2008 Christian Walter <wolti@sil.at>
 * All rights reserved.
 *
 * $Id: mbportserial.c,v 1.1 2008-04-06 07:46:23 cwalter Exp $
 */


#include "H1DR1.h"
/* ----------------------- System includes ----------------------------------*/

#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "mbtypes.h"
#include "mbportlayer.h"
#include "mbframe.h"
#include "mbutils.h"

/* ----------------------- Defines ------------------------------------------*/
#define IDX_INVALID				( 255 )
#define UART_BAUDRATE_MIN		( 2400 )
#define UART_BAUDRATE_MAX		( 115200 )


#define UART_3_ENABLED          ( 1 )   /*!< Set this to 1 to enable USART1 */

#if ( UART_3_ENABLED == 1 )
#define UART_3_PORT             ( MB_UART_3 )
#define UART_3_IDX              ( 0 )
#define NUARTS                  ( 1 )
#endif

#define RS_485_UART_3_INIT(  )	\
do { \
} while( 0 )

extern UART_HandleTypeDef huart3;

#define RS_485_UART_3_ENABLE_TX()	 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)

#define RS_485_UART_3_DISABLE_TX(  ) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)

/* ----------------------- Defines ------------------------------------------*/
/* ----------------------- Defines (Internal - Don't change) ----------------*/
#define HDL_RESET( x ) do { \
	( x )->ubIdx = IDX_INVALID; \
	( x )->pbMBMTransmitterEmptyFN = NULL; \
	( x )->pvMBMReceiveFN = NULL; \
	( x )->xMBMHdl = MB_HDL_INVALID; \
} while( 0 );

/* ----------------------- Type definitions ---------------------------------*/
typedef struct
{
    UBYTE           ubIdx;
    pbMBPSerialTransmitterEmptyAPIV1CB pbMBMTransmitterEmptyFN;
    pvMBPSerialReceiverAPIV1CB pvMBMReceiveFN;
    xMBHandle       xMBMHdl;
} xSerialHandle;

/* ----------------------- Static variables ---------------------------------*/
STATIC xSerialHandle xSerialHdls[NUARTS];
STATIC BOOL     bIsInitalized = FALSE;
#define MBM_SER_PDU_SIZE_MAX            ( 256 ) /*!< Maximum size of a MODBUS RTU frame. */
UBYTE Byte;

/* ----------------------- Static functions ---------------------------------*/
void     prvvMBPUSART1_TC_ISR( void );  
void     prvvMBPUSART1_RXNE_ISR( void );  
extern void MX_USART1_UART_Init(void);

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UBYTE           ubIdx;

    /* Added by Hexabitz Modbus firmware */
		uint32_t BaudRate;
		uint32_t DataBits;
		uint32_t Parity;
		uint32_t StopBit;
		int x;
		UNUSED(x);
		UNUSED(BaudRate);
		UNUSED(DataBits);
		UNUSED(Parity);
		UNUSED(StopBit);
		UBYTE           ubUDR;
		ubUDR =0xFF;
		/* End of addition */

    MBP_ENTER_CRITICAL_SECTION(  );
    if( !bIsInitalized )
    {
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
        {
            HDL_RESET( &xSerialHdls[ubIdx] );
        }
#if UART_3_ENABLED == 1
        RS_485_UART_3_INIT(  );
        RS_485_UART_3_DISABLE_TX(  );
#endif
        bIsInitalized = TRUE;
    }

    if( ( MB_HDL_INVALID == xMBMHdl ) || ( NULL == pxSerialHdl ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        eStatus = MB_ENORES;

        /* Setup baudrate */
        if( ( ulBaudRate > UART_BAUDRATE_MIN ) && ( ulBaudRate < UART_BAUDRATE_MAX ) )
        {
            //BaudRate = ulBaudRate;
        }
        else
        {
            eStatus = MB_EINVAL;
        }

        /* Setup stopbits */
        switch ( ucStopBits )
        {
        case 1:
            StopBit = UART_STOPBITS_1;
            break;
        case 2:
            StopBit = UART_STOPBITS_2;
            break;
        default:
            eStatus = MB_EINVAL;
            break;
        }

        /* For STM32 parity is placed on bit 9 (MSB)
         * Select correct number of databits */
        if( ucDataBits == 8 )
        {
            if( eParity != MB_PAR_NONE )
            {
                DataBits = UART_WORDLENGTH_9B;
            }
            else
            {
                DataBits = UART_WORDLENGTH_8B;
            }
        }
        else
        {
            if( eParity != MB_PAR_NONE )
            {
                DataBits = UART_WORDLENGTH_8B;
            }
            else
            {
                eStatus = MB_EINVAL;
            }
        }

        /* Setup parity */
        switch ( eParity )
        {
        case MB_PAR_NONE:
            Parity = UART_PARITY_NONE;
            break;
        case MB_PAR_ODD:
            Parity = UART_PARITY_ODD;
            break;
        case MB_PAR_EVEN:
            Parity = UART_PARITY_EVEN;
            break;
        default:
            eStatus = MB_EINVAL;
            break;
        }

        if( eStatus != MB_EINVAL )
        {
            switch ( ucPort )
            {
#if UART_3_ENABLED == 1
            case UART_3_PORT:
                if( IDX_INVALID == xSerialHdls[UART_3_IDX].ubIdx )
                {
                    /* Configure  USART1 */
                    //MB_PORT_Init(BaudRate, DataBits, Parity, StopBit);
										MX_USART3_UART_Init();
									
                    /* Disable receive and transmit interrupts from the beginning */
                    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
										__HAL_UART_DISABLE_IT(&huart3, UART_IT_TC);
									x=sizeof(ubUDR);
										HAL_UART_Receive_IT(&huart3, &ubUDR, sizeof(ubUDR));
//									HAL_StatusTypeDef	 wq=HAL_UART_Receive_DMA(&huart3, &ubUDR,  sizeof(ubUDR));
                    /* Setup handle to uart */
                    *pxSerialHdl = &xSerialHdls[UART_3_IDX];
                    xSerialHdls[UART_3_IDX].ubIdx = UART_3_IDX;
                    xSerialHdls[UART_3_IDX].xMBMHdl = xMBMHdl;

                    /* Everything is ok */
                    eStatus = MB_ENOERR;
                }
                else
                {
                    eStatus = MB_ENORES;
                }
                break;
#endif
            default:
                break;
            }
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialClose( xMBPSerialHandle xSerialHdl )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        switch ( pxSerialIntHdl->ubIdx )
        {
#if UART_3_ENABLED == 1
        case UART_3_IDX:
            if( ( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN ) && ( NULL == pxSerialIntHdl->pvMBMReceiveFN ) )
            {
                /* Close USART 1 */
                HAL_UART_DeInit(&huart3);
                /* Force RS485 back to receive mode */
                RS_485_UART_3_DISABLE_TX(  );
                /* Reset handle */
                HDL_RESET( pxSerialIntHdl );
                /* No error */
                eStatus = MB_ENOERR;
            }
            else
            {
                eStatus = MB_EIO;
            }
            break;
#endif
        default:
            MBP_ASSERT( 0 );
            break;
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialTxEnable( xMBPSerialHandle xSerialHdl, pbMBPSerialTransmitterEmptyCB pbMBMTransmitterEmptyFN )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;
	
		/* Next lines added for copying frame packet to buffer */
		MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_3_IDX].ubIdx );
    BOOL      bHasMoreData = TRUE;      
    UBYTE     ubTxByte[MBM_SER_PDU_SIZE_MAX]={0};
		UBYTE 		FrameCnt=0;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pbMBMTransmitterEmptyFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN );
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = pbMBMTransmitterEmptyFN;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_3_ENABLED == 1
            case UART_3_IDX:
                /* RS485 transmit mode */
                RS_485_UART_3_ENABLE_TX(  );
                /* Copy the frame to buffer */
								if( NULL != xSerialHdls[UART_3_IDX].pbMBMTransmitterEmptyFN )
									{
										while (bHasMoreData){
											bHasMoreData = xSerialHdls[UART_3_IDX].pbMBMTransmitterEmptyFN( xSerialHdls[UART_3_IDX].xMBMHdl, &ubTxByte[FrameCnt] );
											FrameCnt++;
										}
										xSerialHdls[UART_3_IDX].pbMBMTransmitterEmptyFN = NULL;
									}
								/* Send the buffer once */
					HAL_UART_Transmit_IT( &huart3, ubTxByte, FrameCnt-1 );
							//	HAL_UART_Transmit_DMA(&huart3, ubTxByte, FrameCnt-1);
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }

        }
        else
        {
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = NULL;
            /* The transmitter is disable when the last frame has been sent.
             * This is necessary for RS485 with a half-duplex bus.
             */
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_3_ENABLED == 1
            case UART_3_IDX:
                /* Disable transmit register empty interrupt */
								__HAL_UART_DISABLE_IT(&huart3, UART_IT_TXE);
                /* Enable transmit complete interrupt */
								__HAL_UART_ENABLE_IT(&huart3, UART_IT_TC);
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

eMBErrorCode
eMBPSerialRxEnable( xMBPSerialHandle xSerialHdl, pvMBPSerialReceiverCB pvMBMReceiveFN )
{
    eMBErrorCode    eStatus = MB_EINVAL;
    xSerialHandle  *pxSerialIntHdl = xSerialHdl;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pvMBMReceiveFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pvMBMReceiveFN );
            pxSerialIntHdl->pvMBMReceiveFN = pvMBMReceiveFN;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_3_ENABLED == 1
            case UART_3_IDX:
                /* Enable USART 1 receive interrupt */
                __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
        else
        {
            pxSerialIntHdl->pvMBMReceiveFN = NULL;
            switch ( pxSerialIntHdl->ubIdx )
            {
#if UART_3_ENABLED == 1
            case UART_3_IDX:
                /* Disable USART 1 receive interrupt */
                //__HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
								
                break;
#endif
            default:
                MBP_ASSERT( 0 );
            }
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/* USART 1 Transmit Complete interrupt */
void
prvvMBPUSART1_TC_ISR( void )
{	
  /* Back to receive mode */
  RS_485_UART_3_DISABLE_TX(  );
  /* Transmission complete. Disable interrupt */
  __HAL_UART_DISABLE_IT(&huart3, UART_IT_TC);
}
uint8_t jk[20];
/* USART 1 Receive interrupt */
void
prvvMBPUSART1_RXNE_ISR( void )
{
UBYTE           ubUDR;
    FlagStatus      fs;
	int x;
	UNUSED(x);
    /* Read current flagstatus */
    fs = RESET;
    if( __HAL_UART_GET_FLAG(&huart3, UART_FLAG_ORE) )
    {
        fs |= 1;
    }
    if( __HAL_UART_GET_FLAG(&huart3, UART_FLAG_NE) )
    {
        fs |= 2;
    }
    if( __HAL_UART_GET_FLAG(&huart3, UART_FLAG_FE) )
    {
        fs |= 4;
    }
    if( __HAL_UART_GET_FLAG(&huart3, UART_FLAG_PE) )
    {
        fs |= 8;
    }

    /* Receive byte from USART1 */
		
		ubUDR = (0xFF & (USART1->RDR));

    /* Send data to modbus functions
     * if no error */
    if( fs == RESET )
    {
        /* Pass received data on to modbuslib */
        MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_3_IDX].ubIdx );
        if( NULL != xSerialHdls[UART_3_IDX].pvMBMReceiveFN )
        {
            xSerialHdls[UART_3_IDX].pvMBMReceiveFN( xSerialHdls[UART_3_IDX].xMBMHdl, ubUDR );
        }
    }
	x=sizeof(ubUDR);
HAL_UART_Receive_IT(&huart3, &ubUDR,sizeof(ubUDR));
//			HAL_UART_Receive_DMA(&huart3, &ubUDR, 5);
		//__HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
		
}


/************************ (C) MODIFIED BY HEXABITZ *****END OF FILE****/
