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
File        : FS_Conf.h
Purpose     : File system configuration
---------------------------END-OF-HEADER------------------------------
*/

#ifndef FS_CONF_H
#define FS_CONF_H

#ifdef DEBUG
 #if (DEBUG)
   #define FS_DEBUG_LEVEL                     5
 #endif
#endif

#define FS_OS_LOCKING          (1)
#define FS_SUPPORT_JOURNAL     (1)
#define FS_SUPPORT_ENCRYPTION  (0)


#ifdef __ICCARM__
  #if __ICCARM__
    #if ((__TID__ >> 4) & 0x0F) < 6   // For any ARM CPU core < v7, we will use optimized routines
      #include "SEGGER.h"
      #define FS_MEMCPY(pDest, pSrc, NumBytes) SEGGER_ARM_memcpy(pDest, pSrc, NumBytes)    // Speed optimization: Our memcpy is much faster!
    #endif
  #endif
#endif

#endif  /* Avoid multiple inclusion */


/*************************** End of file ****************************/
