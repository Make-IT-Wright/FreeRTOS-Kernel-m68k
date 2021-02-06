/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#ifndef configM68K_INCLUDE_FORMAT_WORD
#error "Must define configM68K_INCLUDE_FORMAT_WORD for this port"
#endif

/* Used to keep track of the number of nested calls to taskENTER_CRITICAL().  This
will be set to 0 prior to the first task being started. */
static uint32_t ulCriticalNesting = 0x9999UL;

/*-----------------------------------------------------------*/

StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
{
    /* Param will be offset +8 from SP after context restore */
    pxTopOfStack--;
    *pxTopOfStack = (uint32_t)pvParameters;

    pxTopOfStack--;
    *pxTopOfStack = 0xDEADBEEF;

#if (configM68K_INCLUDE_FORMAT_WORD == 1)
    /* Format word - word size */
    pxTopOfStack = (StackType_t *)((uint16_t *)pxTopOfStack - 1);
    *(uint16_t *)pxTopOfStack = 0;
#endif

    /* Return address */
    pxTopOfStack--;
    *pxTopOfStack = (StackType_t)pxCode;

    /* Initial Status Register value - supervisor mode - word size */
    pxTopOfStack = (StackType_t *)((uint16_t *)pxTopOfStack - 1);
    *(uint16_t *)pxTopOfStack = 0x2000;

    /* 15 more longs for D0-A6(FP), A7(SP) stored in TCB */
    pxTopOfStack -= 15;

    return pxTopOfStack;
}
/*-----------------------------------------------------------*/

__attribute__(( weak )) void
vApplicationSetupInterrupts( void )
{
    /* Your interrupt enable code here */

    portENABLE_INTERRUPTS();
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler( void )
{
extern void vPortStartFirstTask( void );

    ulCriticalNesting = 0UL;

    /* Configure the interrupts used by this port. */
    vApplicationSetupInterrupts();

    /* Start the first task executing. */
    vPortStartFirstTask();

    return pdFALSE;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
    /* Not implemented in ports where there is nothing to return to.
     * Artificially force an assert. */
    configASSERT( uxInterruptNesting == 1000UL );
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
    if( ulCriticalNesting == 0UL ) {
        portDISABLE_INTERRUPTS();
    }

    ulCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
    ulCriticalNesting--;

    if( ulCriticalNesting == 0 ) {
        portENABLE_INTERRUPTS();
    }
}
/*-----------------------------------------------------------*/

void vPortIncrementTick( void )
{
    portDISABLE_INTERRUPTS();
    {
        if( xTaskIncrementTick() != pdFALSE ) {
            asm volatile("trap #15");
        }
    }
    portENABLE_INTERRUPTS();
}
