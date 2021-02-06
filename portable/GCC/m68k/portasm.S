    .extern vPortIncrementTick
    .extern vTaskSwitchContext
    .extern pxCurrentTCB

    .global vPortStartFirstTask
    .global TRAP14
    .global TRAP15

    .data

/* ulContextSwitchNesting is used to keep track of whether a tasks context has
 * been saved or not, e.g. to prevent double save/restore during a tick which
 * causes a yield. */
ulContextSwitchNesting: .word

    .text

.macro portSAVE_CONTEXT

    /* If ulContextSwitchNesting is not zero, context has already been saved
     * for the current task, so should not be saved again */
    tst.l   ulContextSwitchNesting
    bne     0f

    /* Save context */
    move.l  #1, ulContextSwitchNesting

    movem.l %d0-%fp, -(%sp)
    move.l  pxCurrentTCB, %a0
    move.l  %sp, (%a0)

0:

    .endm

.macro portRESTORE_CONTEXT

    /* If ulContextSwitchNesting is zero, context has already been restored
     * for the current task, so should not be restored again */
    tst.l   ulContextSwitchNesting
    beq     0f

    /* Restore context */
    clr.l   ulContextSwitchNesting

    move.l  pxCurrentTCB, %a0
    move.l  (%a0), %sp
    movem.l (%sp)+, %d0-%fp

0:

    .endm

/********************************************************************/

vPortStartFirstTask:

    /* Setup ulContextSwitchNesting to permit a context restore */
    move.l  #1, ulContextSwitchNesting

    portRESTORE_CONTEXT

    rte

/********************************************************************/

/* Tick */
TRAP14:

    portSAVE_CONTEXT

    jsr     vPortIncrementTick

    portRESTORE_CONTEXT

    rte

/********************************************************************/

/* Task yield */
TRAP15:

    portSAVE_CONTEXT

    jsr     vTaskSwitchContext

    portRESTORE_CONTEXT

    rte

    .end
