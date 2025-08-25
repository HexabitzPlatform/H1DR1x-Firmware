/*
 * MODBUS Library: ARM STM32 Port (FWLIB 2.0x)
 * Copyright (c) Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * ARM STM32 Port by Niels Andersen, Elcanic A/S <niels.andersen.elcanic@gmail.com>
 *
 * $Id: mbportother.c,v 1.1 2008-12-14 19:33:32 cwalter Exp $
 */
#include "H1DR1.h"
/* ----------------------- System includes ----------------------------------*/

#include <stdlib.h>

/* ----------------------- Platform includes --------------------------------*/
#include "mbport.h"
#include "mbtypes.h"
#include "mbframe.h"
#include "mbutils.h"
#include "portmacro.h"
/* ----------------------- Modbus includes ----------------------------------*/

/* ----------------------- Defines ------------------------------------------*/
#define PORT_INTERRUPT_PRIORITY_MAX     ( 1 )

/* ----------------------- Type definitions ---------------------------------*/

/* ----------------------- Static variables ---------------------------------*/

/* ----------------------- Static functions ---------------------------------*/

/* ----------------------- Start implementation -----------------------------*/
 unsigned long   ulCPSR;
void
assert_failed( uint8_t * file, uint32_t line )
{
    ( void )file;
    ( void )line;
    vMBPAssert(  );
}

void
vMBPAssert( void )
{
    volatile BOOL   bBreakOut = FALSE;

    vMBPEnterCritical(  );
    while( !bBreakOut );
}

void
vMBPEnterCritical( void )
{
	portENTER_CRITICAL(  );
	
}

void
vMBPExitCritical( void )
{
	portEXIT_CRITICAL(  );
 
}

BOOL
bMBPIsWithinException( void )
{
    BOOL bIsWithinException = FALSE;

#if defined( __ICCARM__ ) && ( __ICCARM__ == 1 )
    ulCPSR = __get_CPSR(  );
#endif
    if( ( ulCPSR & ( 0x80 | 0x40 ) ) > 0 )
    {
        bIsWithinException = TRUE;
    }
    return bIsWithinException;
}

/************************ (C) MODIFIED BY HEXABITZ *****END OF FILE****/
