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
File        : USBH_CP210X.h
Purpose     : API of the USB host stack
-------------------------- END-OF-HEADER -----------------------------
*/

#ifndef USBH_CP210X_H_
#define USBH_CP210X_H_

#include "USBH.h"
#include "SEGGER.h"

#if defined(__cplusplus)
  extern "C" {                 // Make sure we have C-declarations in C++ programs
#endif

/*********************************************************************
*
*       Defines, fixed
*
**********************************************************************
*/
#define USBH_CP210X_INVALID_HANDLE  NULL

//
// Baud Rates
//
#define USBH_CP210X_BAUD_300         300
#define USBH_CP210X_BAUD_600         600
#define USBH_CP210X_BAUD_1200       1200
#define USBH_CP210X_BAUD_2400       2400
#define USBH_CP210X_BAUD_4800       4800
#define USBH_CP210X_BAUD_9600       9600
#define USBH_CP210X_BAUD_14400     14400
#define USBH_CP210X_BAUD_19200     19200
#define USBH_CP210X_BAUD_38400     38400
#define USBH_CP210X_BAUD_57600     57600
#define USBH_CP210X_BAUD_115200   115200
#define USBH_CP210X_BAUD_230400   230400
#define USBH_CP210X_BAUD_460800   460800
#define USBH_CP210X_BAUD_921600   921600

//
// Word Lengths
//
#define USBH_CP210X_BITS_8             8u
#define USBH_CP210X_BITS_7             7u
#define USBH_CP210X_BITS_6             6u
#define USBH_CP210X_BITS_5             5u

//
// Stop Bits
//
#define USBH_CP210X_STOP_BITS_1        0u
#define USBH_CP210X_STOP_BITS_1_5      1u // 1.5 stop bits
#define USBH_CP210X_STOP_BITS_2        2u

//
// Parity
//
#define USBH_CP210X_PARITY_NONE        0u
#define USBH_CP210X_PARITY_ODD         1u
#define USBH_CP210X_PARITY_EVEN        2u
#define USBH_CP210X_PARITY_MARK        3u
#define USBH_CP210X_PARITY_SPACE       4u

//
// Purge rx and tx buffers.
// According to CP210x doc there are two bits for each direction:
// bit 0: Clear the transmit queue.
// bit 1: Clear the receive queue.
// bit 2: Clear the transmit queue.
// bit 3: Clear the receive queue.
//
#define USBH_CP210X_PURGE_RX         0x0Au
#define USBH_CP210X_PURGE_TX         0x05u

/*********************************************************************
*
*       USBH_CP210X_HANDLE
*/
typedef struct _USBH_CP210X_INST * USBH_CP210X_HANDLE;


/*********************************************************************
*
*       USBH_CP210X_DEVICE_INFO
*
*  Description
*    Contains information about an CP210X device.
*/
typedef struct {
  U16         VendorId;             // The Vendor ID of the device.
  U16         ProductId;            // The Product ID of the device.
  USBH_SPEED  Speed;                // The USB speed of the device, see USBH_SPEED.
  U8          InterfaceNo;          // Interface index (from USB descriptor).
  U8          Class;                // The Class value field of the interface
  U8          SubClass;             // The SubClass value field of the interface
  U8          Protocol;             // The Protocol value field of the interface
  USBH_INTERFACE_ID InterfaceID;    // ID of the interface.
} USBH_CP210X_DEVICE_INFO;

typedef void USBH_CP210X_USER_FUNC(void * pContext);

USBH_STATUS           USBH_CP210X_Init                    (void);
void                  USBH_CP210X_Exit                    (void);
USBH_STATUS           USBH_CP210X_AddNotification         (USBH_NOTIFICATION_HOOK * pHook, USBH_NOTIFICATION_FUNC * pfNotification, void * pContext);
USBH_STATUS           USBH_CP210X_RemoveNotification      (const USBH_NOTIFICATION_HOOK * pHook);
USBH_CP210X_HANDLE    USBH_CP210X_Open                    (unsigned Index);
void                  USBH_CP210X_Close                   (USBH_CP210X_HANDLE hDevice);
USBH_STATUS           USBH_CP210X_GetDeviceInfo           (USBH_CP210X_HANDLE hDevice, USBH_CP210X_DEVICE_INFO * pDevInfo);
USBH_STATUS           USBH_CP210X_AllowShortRead          (USBH_CP210X_HANDLE hDevice, U8 AllowShortRead);
USBH_STATUS           USBH_CP210X_SetBaudRate             (USBH_CP210X_HANDLE hDevice, U32 BaudRate);
USBH_STATUS           USBH_CP210X_Read                    (USBH_CP210X_HANDLE hDevice,       U8 * pData, U32 NumBytes, U32 * pNumBytesRead, U32 Timeout);
USBH_STATUS           USBH_CP210X_Write                   (USBH_CP210X_HANDLE hDevice, const U8 * pData, U32 NumBytes, U32 * pNumBytesWritten, U32 Timeout);
USBH_STATUS           USBH_CP210X_SetDataCharacteristics  (USBH_CP210X_HANDLE hDevice, U8  Length, U8 StopBits,  U8 Parity);
USBH_STATUS           USBH_CP210X_SetModemHandshaking     (USBH_CP210X_HANDLE hDevice, U8 Mask, U8 DTR, U8 RTS);
USBH_STATUS           USBH_CP210X_GetModemStatus          (USBH_CP210X_HANDLE hDevice, U8 * pModemStatus);
USBH_STATUS           USBH_CP210X_Purge                   (USBH_CP210X_HANDLE hDevice, U8 Mask);
USBH_STATUS           USBH_CP210X_SetBreak                (USBH_CP210X_HANDLE hDevice, U8 OnOff);
USBH_INTERFACE_HANDLE USBH_CP210X_GetInterfaceHandle      (USBH_CP210X_HANDLE hDevice);

#if defined(__cplusplus)
  }
#endif

#endif // USBH_CP210X_H_

/*************************** End of file ****************************/
