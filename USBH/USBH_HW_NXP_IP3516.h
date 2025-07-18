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
File        : USBH_HW_NXP_IP3516.h
Purpose     : Header for the NXP IP3516 Host driver
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef USBH_HW_IP3516_H__
#define USBH_HW_IP3516_H__

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include "SEGGER.h"
#include "USBH.h"

#if defined(__cplusplus)
  extern "C" {                 // Make sure we have C-declarations in C++ programs
#endif


/*********************************************************************
*
*       Define configurable
*
**********************************************************************
*/

/*********************************************************************
*
*       Define fixed
*
**********************************************************************
*/

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

/*********************************************************************
*
*       Public functions for external use.
*
**********************************************************************
*/
U32        USBH_LPC54xxx_Add(PTR_ADDR BaseAddr, PTR_ADDR USBRAMAddr, unsigned USBRAMSize);
U32        USBH_LPC55xxx_Add(PTR_ADDR BaseAddr, PTR_ADDR USBRAMAddr, unsigned USBRAMSize);

#if defined(__cplusplus)
  }
#endif

#endif // USBH_HW_IP3516_H__

/*************************** End of file ****************************/
