/*
 * MODBUS Library: ARM STM32 Port (FWLIB 2.0x)
 * Copyright (c) Christian Walter <cwalter@embedded-solutions.at>
 * All rights reserved.
 *
 * ARM STM32 Port by Niels Andersen, Elcanic A/S <niels.andersen.elcanic@gmail.com>
 *
 * $Id: mbport.h,v 1.1 2008-12-21 13:50:51 cwalter Exp $
 */

#ifndef _MBM_PORT_H
#define _MBM_PORT_H


#include "H1DR1.h"




#include <assert.h>


#ifdef __cplusplus
extern          "C"
{
#endif

/* ----------------------- Defines ------------------------------------------*/

#define INLINE                              inline
#define STATIC                              static

#define PR_BEGIN_EXTERN_C                   extern "C" {
#define	PR_END_EXTERN_C                     }

#define MBP_ASSERT( x )                     \
    ( ( x ) ? ( void )0 : vMBPAssert( ) )

#define MBP_ENTER_CRITICAL_SECTION()   taskENTER_CRITICAL()
#define MBP_EXIT_CRITICAL_SECTION()    taskEXIT_CRITICAL()

#ifndef TRUE
#define TRUE                                ( BOOL )1
#endif

#ifndef FALSE
#define FALSE                               ( BOOL )0
#endif

#define MBP_EVENTHDL_INVALID                NULL
#define MBP_TIMERHDL_INVALID                NULL
#define MBP_SERIALHDL_INVALID               NULL
#define MBP_TCPHDL_INVALID                  NULL
#define MBP_TCPHDL_CLIENT_INVALID           NULL

#define MB_UART_3                      1
#define MB_UART_2                       2

/* ----------------------- Function prototypes ------------------------------*/


/* ----------------------- Type definitions ---------------------------------*/
typedef void      *xMBPEventHandle;
typedef void      *xMBPTimerHandle;
typedef void      *xMBPSerialHandle;
typedef void      *xMBPTCPHandle;
typedef void      *xMBPTCPClientHandle;

typedef char      BOOL;

typedef char      BYTE;
typedef unsigned char UBYTE;

typedef unsigned char UCHAR;
typedef char      CHAR;

typedef unsigned short USHORT;
typedef short     SHORT;

typedef unsigned long ULONG;
typedef long      LONG;

/* ----------------------- Function prototypes ------------------------------*/
void              vMBPEnterCritical( void );
void              vMBPExitCritical( void );
void              vMBPAssert( void );

#ifdef __cplusplus
}
#endif


#endif


#include <assert.h>

#include "stm32g0xx_hal.h"  /* STM32 FW library */

#include "H1DR1.h"

#ifdef _cplusplus
extern          "C"
{


/* ----------------------- Defines ------------------------------------------*/

#define INLINE
#define STATIC                         static

#define PR_BEGIN_EXTERN_C              extern "C" {
#define	PR_END_EXTERN_C                }

#define MBP_ASSERT( x )                ( ( x ) ? ( void ) 0 : vMBPAssert(  ) )

#define MBP_ENTER_CRITICAL_SECTION( )  vMBPEnterCritical( )
#define MBP_EXIT_CRITICAL_SECTION( )   vMBPExitCritical( )

#ifndef TRUE
#define TRUE                           ( BOOL )1
#endif

#ifndef FALSE
#define FALSE                          ( BOOL )0
#endif

#define MB_PREEMP_PRIORITY              ( 0 )
#define MBP_EVENTHDL_INVALID            NULL
#define MBP_TIMERHDL_INVALID            NULL
#define MBP_SERIALHDL_INVALID           NULL
#define MBP_TCPHDL_INVALID              NULL

#define MB_UART_1                       1
#define MB_UART_2                       2

/* ----------------------- Type definitions ---------------------------------*/
typedef void       *xMBPEventHandle;
typedef void       *xMBPTimerHandle;
typedef void       *xMBPSerialHandle;
typedef void       *xMBPTCPHandle;
typedef void       *xMBPTCPClientHandle;

typedef char        BOOL;

typedef char        BYTE;
typedef unsigned char UBYTE;

typedef unsigned char UCHAR;
typedef char        CHAR;

typedef unsigned short USHORT;
typedef short       SHORT;

typedef unsigned long ULONG;
typedef long        LONG;

typedef enum
{
    MBP_DEBUGPIN_0 = 0,
    MBP_DEBUGPIN_1 = 1
} eMBPDebugPin;
/* ----------------------- Function prototypes ------------------------------*/
void                vMBPAssert( void );
void                vMBPEnterCritical( void );
void                vMBPExitCritical( void );
void                vMBPSetDebugPin( eMBPDebugPin ePinName, BOOL bTurnOn );
#ifdef _cplusplus
}
#endif

#endif
