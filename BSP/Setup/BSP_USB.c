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
File    : BSP_USB.c
Purpose : BSP for LPC54605_emUSB_Host board
          Functions for USB controllers
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "BSP_USB.h"
#include "RTOS.h"
#include "LPC54605.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

/*********************************************************************
*
*       Types
*
**********************************************************************
*/
typedef void (ISR_HANDLER)(void);

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static ISR_HANDLER * _pfUSB0ISRHandler;
static ISR_HANDLER * _pfUSB1ISRHandler;

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
#ifdef __cplusplus
extern "C" {
#endif
  void USB0_IRQHandler(void);
  void USB1_IRQHandler(void);
#ifdef __cplusplus
}
#endif

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/
/*********************************************************************
*
*       USB0_IRQHandler
*/
void USB0_IRQHandler(void) {
  OS_EnterInterrupt();
  if (_pfUSB0ISRHandler) {
    _pfUSB0ISRHandler();
  }
  OS_LeaveInterrupt();
}

/*********************************************************************
*
*       USB1_IRQHandler
*/
void USB1_IRQHandler(void) {
  OS_EnterInterrupt();
  if (_pfUSB1ISRHandler) {
    _pfUSB1ISRHandler();
  }
  OS_LeaveInterrupt();
}

/*********************************************************************
*
*       BSP_USB_InstallISR_Ex
*/
void BSP_USB_InstallISR_Ex(int ISRIndex, void (*pfISR)(void), int Prio) {
  (void)Prio;
  if (ISRIndex == USB0_IRQn) {
    _pfUSB0ISRHandler = pfISR;
  }
  if (ISRIndex == USB1_IRQn) {
    _pfUSB1ISRHandler = pfISR;
  }
  NVIC_SetPriority((IRQn_Type)ISRIndex, (1u << __NVIC_PRIO_BITS) - 2u);
  NVIC_EnableIRQ((IRQn_Type)ISRIndex);
}

/*********************************************************************
*
*       BSP_USBH_InstallISR_Ex
*/
void BSP_USBH_InstallISR_Ex(int ISRIndex, void (*pfISR)(void), int Prio) {
  (void)Prio;
  if (ISRIndex == USB0_IRQn) {
    _pfUSB0ISRHandler = pfISR;
  }
  if (ISRIndex == USB1_IRQn) {
    _pfUSB1ISRHandler = pfISR;
  }
  NVIC_SetPriority((IRQn_Type)ISRIndex, (1u << __NVIC_PRIO_BITS) - 2u);
  NVIC_EnableIRQ((IRQn_Type)ISRIndex);
}

/*********************************************************************
*
*       BSP_USB_Init
*/
void BSP_USB_Init(void) {
}

/****** End Of File *************************************************/
