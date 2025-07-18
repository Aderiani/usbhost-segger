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
File        : FS_ConfigIO_USBH.c
Purpose     : I/O configuration routines for file system
---------------------------END-OF-HEADER------------------------------
*/

#include "USBH.h"
#include "FS.h"


/*********************************************************************
*
*       FS_X_Panic
*
*  Function description
*    Referred in debug builds of the file system only and
*    called only in case of fatal, unrecoverable errors.
*/
void FS_X_Panic(int ErrorCode) {
  while (ErrorCode) {
  }
}

/*********************************************************************
*
*      Logging: OS dependent
*
*/
void FS_X_Log(const char * s) {
#if USBH_SUPPORT_LOG
  USBH_Logf(USBH_MCAT_INIT, "%s", s);
#else
  USBH_USE_PARA(s);
#endif
}

void FS_X_Warn(const char * s) {
#if USBH_SUPPORT_WARN
  USBH_Warnf(USBH_MCAT_INIT, "FS warning: %s", s);
#else
  USBH_USE_PARA(s);
#endif
}

void FS_X_ErrorOut(const char * s) {
#if USBH_SUPPORT_WARN
  USBH_Warnf(USBH_MCAT_INIT, "FS error: %s", s);
#else
  USBH_USE_PARA(s);
#endif
}

/*************************** End of file ****************************/
