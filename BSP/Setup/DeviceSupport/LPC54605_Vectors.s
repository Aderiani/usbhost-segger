/*********************************************************************
*                     SEGGER Microcontroller GmbH                    *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 1995 - 2021 SEGGER Microcontroller GmbH                  *
*                                                                    *
*       Internet: segger.com  Support: support_embos@segger.com      *
*                                                                    *
**********************************************************************
*                                                                    *
*       embOS * Real time operating system for microcontrollers      *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product or a real-time            *
*       operating system for in-house use.                           *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       OS version: V5.14.0.0                                        *
*                                                                    *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File      : LPC54605_Vectors.s
Purpose   : Exception and interrupt vectors for LPC54605 devices.

Additional information:
  Preprocessor Definitions
    __NO_EXTERNAL_INTERRUPTS
      If defined,
        the vector table will contain only the internal exceptions
        and interrupts.
    __VECTORS_IN_RAM
      If defined,
        an area of RAM, large enough to store the vector table, 
        will be reserved.

    __OPTIMIZATION_SMALL
      If defined,
        all weak definitions of interrupt handlers will share the 
        same implementation.
      If not defined,
        all weak definitions of interrupt handlers will be defined
        with their own implementation.
*/
        .syntax unified

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/

//
// Directly place a vector (word) in the vector table
//
.macro VECTOR Name=
        .section .vectors, "ax"
        .code 16
        .word \Name
.endm

//
// Declare an exception handler with a weak definition
//
.macro EXC_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
.endm

//
// Declare an interrupt handler with a weak definition
//
.macro ISR_HANDLER Name=
        //
        // Insert vector in vector table
        //
        .section .vectors, "ax"
        .word \Name
        //
        // Insert dummy handler in init section
        //
#if defined(__OPTIMIZATION_SMALL)
        .section .init, "ax"
        .weak \Name
        .thumb_set \Name,Dummy_Handler
#else
        .section .init.\Name, "ax"
        .thumb_func
        .weak \Name
        .balign 2
\Name:
        1: b 1b   // Endless loop
#endif
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED
        .section .vectors, "ax"
        .word 0
.endm

//
// Place a reserved vector in vector table
//
.macro ISR_RESERVED_DUMMY
        .section .vectors, "ax"
        .word Dummy_Handler
.endm

/*********************************************************************
*
*       Externals
*
**********************************************************************
*/
        .extern __stack_end__
        .extern Reset_Handler
        .extern HardFault_Handler

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*  Setup of the vector table and weak definition of interrupt handlers
*
*/
        .section .vectors, "ax"
        .code 16
        .balign 512
        .global _vectors
_vectors:
        //
        // Internal exceptions and interrupts
        //
        VECTOR __stack_end__
        VECTOR Reset_Handler
        EXC_HANDLER NMI_Handler
        VECTOR HardFault_Handler
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER SVC_Handler
        ISR_RESERVED
        ISR_RESERVED
        EXC_HANDLER PendSV_Handler
        EXC_HANDLER SysTick_Handler
        //
        // External interrupts
        //
#ifndef __NO_EXTERNAL_INTERRUPTS
        ISR_HANDLER WDT_BOD_IRQHandler
        ISR_HANDLER DMA0_IRQHandler
        ISR_HANDLER GINT0_IRQHandler
        ISR_HANDLER GINT1_IRQHandler
        ISR_HANDLER PIN_INT0_IRQHandler
        ISR_HANDLER PIN_INT1_IRQHandler
        ISR_HANDLER PIN_INT2_IRQHandler
        ISR_HANDLER PIN_INT3_IRQHandler
        ISR_HANDLER UTICK0_IRQHandler
        ISR_HANDLER MRT0_IRQHandler
        ISR_HANDLER CTIMER0_IRQHandler
        ISR_HANDLER CTIMER1_IRQHandler
        ISR_HANDLER SCT0_IRQHandler
        ISR_HANDLER CTIMER3_IRQHandler
        ISR_HANDLER FLEXCOMM0_IRQHandler
        ISR_HANDLER FLEXCOMM1_IRQHandler
        ISR_HANDLER FLEXCOMM2_IRQHandler
        ISR_HANDLER FLEXCOMM3_IRQHandler
        ISR_HANDLER FLEXCOMM4_IRQHandler
        ISR_HANDLER FLEXCOMM5_IRQHandler
        ISR_HANDLER FLEXCOMM6_IRQHandler
        ISR_HANDLER FLEXCOMM7_IRQHandler
        ISR_HANDLER ADC0_SEQA_IRQHandler
        ISR_HANDLER ADC0_SEQB_IRQHandler
        ISR_HANDLER ADC0_THCMP_IRQHandler
        ISR_HANDLER DMIC0_IRQHandler
        ISR_HANDLER HWVAD0_IRQHandler
        ISR_HANDLER USB0_NEEDCLK_IRQHandler
        ISR_HANDLER USB0_IRQHandler
        ISR_HANDLER RTC_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER PIN_INT4_IRQHandler
        ISR_HANDLER PIN_INT5_IRQHandler
        ISR_HANDLER PIN_INT6_IRQHandler
        ISR_HANDLER PIN_INT7_IRQHandler
        ISR_HANDLER CTIMER2_IRQHandler
        ISR_HANDLER CTIMER4_IRQHandler
        ISR_HANDLER RIT_IRQHandler
        ISR_HANDLER SPIFI0_IRQHandler
        ISR_HANDLER FLEXCOMM8_IRQHandler
        ISR_HANDLER FLEXCOMM9_IRQHandler
        ISR_HANDLER SDIO_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER USB1_IRQHandler
        ISR_HANDLER USB1_NEEDCLK_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER EEPROM_IRQHandler
        ISR_RESERVED_DUMMY
        ISR_RESERVED_DUMMY
        ISR_HANDLER SMARTCARD0_IRQHandler
        ISR_HANDLER SMARTCARD1_IRQHandler
#endif
        //
        .section .vectors, "ax"
_vectors_end:

#ifdef __VECTORS_IN_RAM
        //
        // Reserve space with the size of the vector table
        // in the designated RAM section.
        //
        .section .vectors_ram, "ax"
        .balign 512
        .global _vectors_ram

_vectors_ram:
        .space _vectors_end - _vectors, 0
#endif

/*********************************************************************
*
*  Dummy handler to be used for reserved interrupt vectors
*  and weak implementation of interrupts.
*
*/
        .section .init.Dummy_Handler, "ax"
        .thumb_func
        .weak Dummy_Handler
        .balign 2
Dummy_Handler:
        1: b 1b   // Endless loop


/*************************** End of file ****************************/
