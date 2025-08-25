/*
 * MODBUS Library: Skeleton port
 * Modified to use configurable UART macros
 */

#include "H1DR1.h"
#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "mbtypes.h"
#include "mbportlayer.h"
#include "mbframe.h"
#include "mbutils.h"
#include "Port.h"

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
UBYTE Byte;

/* ----------------------- Static functions ---------------------------------*/
void     prvvMBPUSART_TC_ISR( void );
void     prvvMBPUSART_RXNE_ISR( void );

/* ----------------------- Start implementation -----------------------------*/

eMBErrorCode
eMBPSerialInit( xMBPSerialHandle * pxSerialHdl, UCHAR ucPort, ULONG ulBaudRate,
                UCHAR ucDataBits, eMBSerialParity eParity, UCHAR ucStopBits, xMBHandle xMBMHdl)
{
    eMBErrorCode    eStatus = MB_ENOERR;
    UBYTE           ubIdx;

    MBP_ENTER_CRITICAL_SECTION(  );
    if( !bIsInitalized )
    {
        for( ubIdx = 0; ubIdx < MB_UTILS_NARRSIZE( xSerialHdls ); ubIdx++ )
        {
            HDL_RESET( &xSerialHdls[ubIdx] );
        }

        MB_UART_INIT_FN();
        RS485_TX_DISABLE();
        bIsInitalized = TRUE;
    }

    if( ( MB_HDL_INVALID == xMBMHdl ) || ( NULL == pxSerialHdl ) )
    {
        eStatus = MB_EINVAL;
    }
    else
    {
        /* check baud, stopbits, parity... */
        if( ( ulBaudRate < UART_BAUDRATE_MIN ) || ( ulBaudRate > UART_BAUDRATE_MAX ) )
        {
            eStatus = MB_EINVAL;
        }

        if( eStatus != MB_EINVAL )
        {
            if( IDX_INVALID == xSerialHdls[UART_IDX].ubIdx )
            {
                /* init uart */
                MB_UART_INIT_FN();

                /* disable interrupts first */
                __HAL_UART_DISABLE_IT(&MB_UART_HANDLE, UART_IT_RXNE);
                __HAL_UART_DISABLE_IT(&MB_UART_HANDLE, UART_IT_TC);

                UBYTE ubUDR = 0xFF;
                HAL_UART_Receive_IT(&MB_UART_HANDLE, &ubUDR, sizeof(ubUDR));

                *pxSerialHdl = &xSerialHdls[UART_IDX];
                xSerialHdls[UART_IDX].ubIdx = UART_IDX;
                xSerialHdls[UART_IDX].xMBMHdl = xMBMHdl;

                eStatus = MB_ENOERR;
            }
            else
            {
                eStatus = MB_ENORES;
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
        if( ( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN ) && ( NULL == pxSerialIntHdl->pvMBMReceiveFN ) )
        {
            HAL_UART_DeInit(&MB_UART_HANDLE);
            RS485_TX_DISABLE();
            HDL_RESET( pxSerialIntHdl );
            eStatus = MB_ENOERR;
        }
        else
        {
            eStatus = MB_EIO;
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

    MBP_ENTER_CRITICAL_SECTION(  );
    if( MB_IS_VALID_HDL( pxSerialIntHdl, xSerialHdls ) )
    {
        eStatus = MB_ENOERR;
        if( NULL != pbMBMTransmitterEmptyFN )
        {
            MBP_ASSERT( NULL == pxSerialIntHdl->pbMBMTransmitterEmptyFN );
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = pbMBMTransmitterEmptyFN;

            RS485_TX_ENABLE();

            BOOL bHasMoreData = TRUE;
            UBYTE ubTxByte[MBM_SER_PDU_SIZE_MAX]={0};
            UBYTE FrameCnt=0;

            while (bHasMoreData){
                bHasMoreData = pxSerialIntHdl->pbMBMTransmitterEmptyFN( pxSerialIntHdl->xMBMHdl, &ubTxByte[FrameCnt] );
                FrameCnt++;
            }
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = NULL;

            HAL_UART_Transmit_IT( &MB_UART_HANDLE, ubTxByte, FrameCnt-1 );
        }
        else
        {
            pxSerialIntHdl->pbMBMTransmitterEmptyFN = NULL;
            __HAL_UART_DISABLE_IT(&MB_UART_HANDLE, UART_IT_TXE);
            __HAL_UART_ENABLE_IT(&MB_UART_HANDLE, UART_IT_TC);
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
            __HAL_UART_ENABLE_IT(&MB_UART_HANDLE, UART_IT_RXNE);
        }
        else
        {
            pxSerialIntHdl->pvMBMReceiveFN = NULL;
            __HAL_UART_DISABLE_IT(&MB_UART_HANDLE, UART_IT_RXNE);
        }
    }
    MBP_EXIT_CRITICAL_SECTION(  );
    return eStatus;
}

/* USART Transmit Complete interrupt */
void prvvMBPUSART_TC_ISR( void )
{
    RS485_TX_DISABLE();
    __HAL_UART_DISABLE_IT(&MB_UART_HANDLE, UART_IT_TC);
}

/* USART Receive interrupt */
void prvvMBPUSART_RXNE_ISR( void )
{
    UBYTE ubUDR;
    FlagStatus fs = RESET;

    if( __HAL_UART_GET_FLAG(&MB_UART_HANDLE, UART_FLAG_ORE) ) fs |= 1;
    if( __HAL_UART_GET_FLAG(&MB_UART_HANDLE, UART_FLAG_NE) ) fs |= 2;
    if( __HAL_UART_GET_FLAG(&MB_UART_HANDLE, UART_FLAG_FE) ) fs |= 4;
    if( __HAL_UART_GET_FLAG(&MB_UART_HANDLE, UART_FLAG_PE) ) fs |= 8;

    ubUDR = (0xFF & (MB_UART_INSTANCE->RDR));

    if( fs == RESET )
    {
        MBP_ASSERT( IDX_INVALID != xSerialHdls[UART_IDX].ubIdx );
        if( NULL != xSerialHdls[UART_IDX].pvMBMReceiveFN )
        {
            xSerialHdls[UART_IDX].pvMBMReceiveFN( xSerialHdls[UART_IDX].xMBMHdl, ubUDR );
        }
    }

    HAL_UART_Receive_IT(&MB_UART_HANDLE, &ubUDR,sizeof(ubUDR));
}

