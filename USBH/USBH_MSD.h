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
File        : USBH_MSD.h
Purpose     : MSD API of the USB host stack
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef USBH_MSD_H_
#define USBH_MSD_H_

#include "SEGGER.h"
#include "USBH.h"

#if defined(__cplusplus)
  extern "C" {                 // Make sure we have C-declarations in C++ programs
#endif

#ifndef USBH_USE_LEGACY_MSD
  #define USBH_USE_LEGACY_MSD   0
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define  USBH_MSD_MAX_UNITS              32u // Limited to 32 because USBH_MSD_GetUnits returns a U32 bit mask.

/*********************************************************************
*
*       USBH_MSD
*
*  Description
*    Function parameter for the user callback function USBH_MSD_LUN_NOTIFICATION_FUNC
*/
typedef enum {
  USBH_MSD_EVENT_ADD,
  USBH_MSD_EVENT_REMOVE,
  USBH_MSD_EVENT_ERROR
} USBH_MSD_EVENT;

/*********************************************************************
*
*       USBH_MSD_UNIT_INFO
*
*  Description
*    Contains logical unit information.
*/
typedef struct {
  U32   TotalSectors;     // Contains the number of total sectors available on the LUN.
  U16   BytesPerSector;   // Contains the number of bytes per sector.
  int   WriteProtectFlag; // Nonzero if the device is write protected.
  U16   VendorId;         // USB Vendor ID.
  U16   ProductId;        // USB Product ID.
  char  acVendorName[9];  // Vendor identification string.
  char  acProductName[17];// Product identification string.
  char  acRevision[5];    // Revision string.
} USBH_MSD_UNIT_INFO;

/*********************************************************************
*
*       USBH_MSD_AHEAD_BUFFER
*
*  Description
*    Structure describing the read-ahead-cache buffer.
*/
typedef struct {
  U8 * pBuffer; // Pointer to a buffer.
  U32  Size;    // Size of the buffer in bytes.
} USBH_MSD_AHEAD_BUFFER;


/*********************************************************************
*
*       USBH_MSD_LUN_NOTIFICATION_FUNC
*
*  Description
*    This callback function is called when a logical unit is either
*    added or removed. To get detailed information USBH_MSD_GetStatus()
*    has to be called. The LUN indexes must be used to get access to
*    a specified unit of the device.
*
*  Parameters
*    pContext:  Pointer to a context that was set by the user when the USBH_MSD_Init() was called.
*    DevIndex:  Zero based index of the device that was attached or removed.
*    Event:     Gives information about the event that has occurred.
*               The following events are currently available:
*               * USBH_MSD_EVENT_ADD A device was attached.
*               * USBH_MSD_EVENT_REMOVE A device was removed.
*               * USBH_MSD_EVENT_ERROR A new device could not be added because of errors.
*/
typedef void USBH_MSD_LUN_NOTIFICATION_FUNC(void * pContext, U8 DevIndex, USBH_MSD_EVENT Event);

int         USBH_MSD_Init           (USBH_MSD_LUN_NOTIFICATION_FUNC * pfLunNotification, void * pContext);
void        USBH_MSD_Exit           (void);
USBH_STATUS USBH_MSD_ReadSectors    (U8 Unit, U32 SectorAddress, U32 NumSectors, U8 * pBuffer);
USBH_STATUS USBH_MSD_WriteSectors   (U8 Unit, U32 SectorAddress, U32 NumSectors, const U8 * pBuffer);
USBH_STATUS USBH_MSD_GetUnitInfo    (U8 Unit, USBH_MSD_UNIT_INFO * pInfo);
USBH_STATUS USBH_MSD_GetStatus      (U8 Unit);
void        USBH_MSD_UseAheadCache  (int OnOff);
void        USBH_MSD_SetAheadBuffer (const USBH_MSD_AHEAD_BUFFER * pAheadBuf);
USBH_STATUS USBH_MSD_GetPortInfo    (U8 Unit, USBH_PORT_INFO * pPortInfo);
USBH_STATUS USBH_MSD_GetUnits       (U8 DevIndex, U32 *pUnitMask);
void        USBH_MSD_SetNotification(USBH_MSD_LUN_NOTIFICATION_FUNC * pfLunNotification, void * pContext);
#if defined(__cplusplus)
  }
#endif

#endif

/*************************** End of file ****************************/
