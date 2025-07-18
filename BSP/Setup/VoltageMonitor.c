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
File        : VoltageMonitor.c
Purpose     : Monitor USB voltage on the SEGGER emUSB-Host board
---------------------------END-OF-HEADER------------------------------
*/

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include "LPC54605.h"
#include "RTOS.h"
#include "USBH.h"
#include "BSP.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
//
// Voltage monitoring settings
//
#define VMONITOR_LOW_THRESHOLD            4400u  // Low threshold in mV
#define VMONITOR_INTERVAL                  100u  // Sample interval in us
#define VMONITOR_FAIL_COUNT                  4u  // Number of samples below threshold to trigger system halt


/*********************************************************************
*
*       ADC0_THCMP_IRQHandler
*/
void ADC0_THCMP_IRQHandler(void);
void ADC0_THCMP_IRQHandler(void) {
  static U32 LastEvent = 0;
  static U8  FailCnt = 0;
  U32        Now;

  OS_EnterInterrupt();
  ADC0->FLAGS = (1u << 5);
  Now = OS_GetTime();
  if (Now - LastEvent > 20) {
    FailCnt = 1;
  } else {
    if (++FailCnt >= VMONITOR_FAIL_COUNT) {
      BSP_SetLED(4);
      USBH_PANIC("*** Low voltage, System halted");
      OS_IncDI();
      for (;;) {}
    }
  }
  LastEvent = Now;
  OS_LeaveInterrupt();
}

/*********************************************************************
*
*       StartVoltageMonitor
*/
void StartVoltageMonitor(void);
void StartVoltageMonitor(void) {
  //
  // Power up ADC
  //
  SYSCON->PDRUNCFGCLR[0] = (1u << 10)       // ADC0
                         | (1u <<  9)       // PDEN_VD2_ANA
                         | (1u << 19)       // PDEN_VDDA
                         | (1u << 23)       // PDEN_VREFP
                         ;
  OS_Delay(2);
  //
  // Enable clocks
  //
  SYSCON->AHBCLKCTRLSET[0] = (1u << 13)     // IOCON
                           | (1u << 27)     // ADC0
                           | (1u << 14)     // PIO 0 block
                           ;
  SYSCON->AHBCLKCTRLSET[1] = (1u << 22);    // CTIMER 2
  //
  // Reset ADC + CTIMER2
  //
  SYSCON->PRESETCTRLSET[0] = (1u << 27);
  SYSCON->PRESETCTRLSET[1] = (1u << 22);
  OS_Delay(2);
  SYSCON->PRESETCTRLCLR[0] = (1u << 27);
  SYSCON->PRESETCTRLCLR[1] = (1u << 22);
  //
  // Configure timer
  // Trigger interval = 2000 * (PR+1) * (MR+1) / CPU Clock    (in milliseconds)
  //
  CTIMER2->PR    = 89;                  // Prescaler value
  CTIMER2->MR[3] = VMONITOR_INTERVAL;   // Counter match value
  CTIMER2->MCR   = (1u << 10);          // Reset counter on MATCH3
  CTIMER2->EMR   = (3u << 10);          // Toggle match bit.
  CTIMER2->TCR   = (1u << 1);           // Reset timer
  CTIMER2->TCR   = (1u << 0);           // Enable timer
  //
  // Configure input pin for ADC
  //
  IOCON->PIO[0][31] = (1u << 9);
  //
  // Enable and calibrate ADC
  //
  ADC0->CTRL = (3u << 9)                  // 12-bit resolution
             | 5;                         // system clock / 6 --> 30 MHz
  ADC0->STARTUP = 1;                      // Enable
  OS_Delay(2);
  while ((ADC0->STARTUP & 1u) == 0u) {
  }
  ADC0->CALIB = 1;                        // Start calibration
  while ((ADC0->CALIB & 1u) != 0u) {
  }
  ADC0->STARTUP = 3;                      // Initialize
  while ((ADC0->STARTUP & 2u) != 0u) {
  }
  //
  // Set threshold levels
  //
  ADC0->THR0_LOW  = ((VMONITOR_LOW_THRESHOLD << 12) / 6600u) << 4;
  ADC0->THR0_HIGH = 0xFFF0;
  //
  // Start ADC conversions
  //
  ADC0->SEQ_CTRL[0] = (1u << 5)             // Channel 5
                    | (8u << 12)            // Trigger by CTIMER2 MATCH3
                    | (1u << 31);           // Enable
  //
  // Enable interrupt
  //
  NVIC_SetPriority(ADC0_THCMP_IRQn, (1u << (__NVIC_PRIO_BITS - 1u)));
  NVIC_EnableIRQ(ADC0_THCMP_IRQn);
  ADC0->FLAGS = (1u << 5);
  ADC0->INTEN = (1u << 13);
}

/*************************** End of file ****************************/
