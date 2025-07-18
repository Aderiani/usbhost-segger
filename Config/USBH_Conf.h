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
File    : USBH_Conf.h
Purpose : Config file. Modify to reflect your configuration
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef USBH_CONF_H      // Avoid multiple inclusion
#define USBH_CONF_H


#if defined(DEBUG) && !defined(USBH_DEBUG)
  #if DEBUG
    #define USBH_DEBUG   2  // Debug level 1: Support "Panic" checks, 2: Support warn & log
  #endif
#endif

#ifndef    USBH_SUPPORT_ISO_TRANSFER
  #define  USBH_SUPPORT_ISO_TRANSFER    1
#endif


#endif // Avoid multiple inclusion

/*************************** End of file ****************************/
