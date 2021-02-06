#ifndef PORTMACRO_H
#define PORTMACRO_H

/* System include files */
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-----------------------------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *---------------------------------------------------------------------------*/

/* Type definitions. */
#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if( configUSE_16_BIT_TICKS == 1 )
    typedef uint16_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffff
#else
    typedef uint32_t TickType_t;
    #define portMAX_DELAY ( TickType_t ) 0xffffffffUL

    /* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
    not need to be guarded with a critical section. */
    #define portTICK_TYPE_IS_ATOMIC 1
#endif
/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portBYTE_ALIGNMENT 2
#define portSTACK_GROWTH -1
#define portTICK_PERIOD_MS ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
/*-----------------------------------------------------------*/

/* Critical section management. */
#define portDISABLE_INTERRUPTS()                                            \
{                                                                           \
uint16_t priority = (uint16_t)(configMAX_SYSCALL_INTERRUPT_PRIORITY << 8);  \
                                                                            \
    /* Mask interrupts at and below the kernel interrupt priority. */       \
    asm volatile(                                                           \
        "move.w %%sr, %%d1                   \n\t"                          \
        "andi.w #0xF8FF, %%d1                \n\t"                          \
        "move.w %[priority], %%d0            \n\t"                          \
        "or.w   %%d0, %%d1                   \n\t"                          \
        "move.w %%d1, %%sr                   \n\t"                          \
        :                                                                   \
        :[priority]"rm"(priority)                                           \
        :"%d0", "%d1"                                                       \
    );                                                                      \
}

#define portENABLE_INTERRUPTS()                                             \
{                                                                           \
    /* Unmask all interrupts. */                                            \
    asm volatile(                                                           \
        "move.w %%sr, %%d0                   \n\t"                          \
        "andi.w #0xF8FF, %%d0                \n\t"                          \
        "move.w %%d0, %%sr                   \n\t"                          \
        ::                                                                  \
        :"%d0"                                                              \
    );                                                                      \
}

extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
#define portENTER_CRITICAL() vPortEnterCritical()
#define portEXIT_CRITICAL() vPortExitCritical()
/*-----------------------------------------------------------*/

/* Task utilities. */
#define portYIELD()                                                         \
{                                                                           \
    asm volatile("trap #15");                                               \
}

#define portNOP() asm volatile("nop")
/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters ) __attribute__((noreturn))
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )
/*-----------------------------------------------------------*/

/* Required by the kernel aware debugger. */
#ifdef __DEBUG
    #define portREMOVE_STATIC_QUALIFIER
#endif

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */
