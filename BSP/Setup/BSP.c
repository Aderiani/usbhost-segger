/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
**********************************************************************
*                                                                    *
*       (c) 2003 - 2021     SEGGER Microcontroller GmbH              *
*                                                                    *
*       www.segger.com     Support: www.segger.com/ticket            *
*                                                                    *
**********************************************************************
*                                                                    *
*       emUSB-Device * USB Device stack for embedded applications    *
*                                                                    *
*                                                                    *
*       Please note:                                                 *
*                                                                    *
*       Knowledge of this file may under no circumstances            *
*       be used to write a similar product.                          *
*                                                                    *
*       Thank you for your fairness !                                *
*                                                                    *
**********************************************************************
*                                                                    *
*       emUSB-Host version: V2.33-r27121                             *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File    : BSP.c
Purpose : BSP for SEGGER emUSB-Host eval board
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "BSP.h"
#include "LPC54605.h"   // Device specific header file, contains CMSIS

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

//
// LEDs
//
#define NUM_LEDS        (5)

#define LED0          (27u)  // PIO0_27, green
#define LED1          (25u)  // PIO0_25, green
#define LED2          (26u)  // PIO0_26, red
#define LED3          (24u)  // PIO0_24, red
#define LED4          (28u)  // PIO0_28, red

#define LED_MASK_ALL  ((1u << LED0) | (1u << LED1) | (1u << LED2) | (1u << LED3) | (1u << LED4))

/*********************************************************************
*
*       Static const data
*
**********************************************************************
*/
static const unsigned char _LEDIdx[] = { LED0, LED1, LED2, LED3, LED4 };

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       BSP_Init()
*/
void BSP_Init(void) {
  int           i;
  int           j;
  unsigned long Mask;

  //
  // Enable clocks
  //
  SYSCON->AHBCLKCTRLSET[0] = (1u << 13)     // IOCON
                           | (1u << 14)     // PIO 0 block
                           | (1u << 15)     // PIO 1 block
                           | (1u << 16)     // PIO 2 block
                           | (1u << 17)     // PIO 3 block
                           ;
  SYSCON->AHBCLKCTRLSET[2] = (1u <<  9)     // PIO 4 block
                           | (1u << 10)     // PIO 5 block
                           ;
  //
  // Switch of all pull-ups
  //
  for (i = 0; i <= 5; i++) {
    for (j = 0; j < 32; j++) {
      if (i != 0 || j < 10 || j > 12) {            // Don't touch the JTAG pins
        IOCON->PIO[i][j] = (1u << 8) | (1u << 9);
      }
    }
  }
  //
  // Set pins to output low
  //
  Mask = ~(  (7u    << 10)    // except JTAG pins
           | (0x1Fu << 24)    // except LED  pins
           | (1u    << 31)    // except ADC0 input
          );
  for (i = 0; i <= 5; i++) {
    GPIO->CLR[i]    = Mask;
    GPIO->DIRSET[i] = Mask;
    Mask            = 0xFFFFFFFFu;
  }

  GPIO->DIR[0] |= LED_MASK_ALL;  // Set LEDs to output low
  GPIO->SET[0]  = LED_MASK_ALL;  // Initially, clear LEDs
}

/*********************************************************************
*
*       BSP_SetLED()
*/
void BSP_SetLED(int Index) {
  if (Index < NUM_LEDS) {
    GPIO->CLR[0] = 1u << _LEDIdx[Index];
  }
}

/*********************************************************************
*
*       BSP_ClrLED()
*/
void BSP_ClrLED(int Index) {
  if (Index < NUM_LEDS) {
    GPIO->SET[0] = 1u << _LEDIdx[Index];
  }
}

/*********************************************************************
*
*       BSP_ToggleLED()
*/
void BSP_ToggleLED(int Index) {
  if (Index < NUM_LEDS) {
    GPIO->NOT[0] = 1u << _LEDIdx[Index];
  }
}

/****** End Of File *************************************************/
