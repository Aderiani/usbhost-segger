/*********************************************************************
*                   (c) SEGGER Microcontroller GmbH                  *
*                        The Embedded Experts                        *
**********************************************************************

-------------------------- END-OF-HEADER -----------------------------

File    : USBH_FT232_Start.c
Purpose : UART Bridge between FT232 and FT4232H Port C only

Additional information:
  This implementation creates a bridge between:
  - Device A: FT232 single UART device
  - Device B: FT4232H Port C ONLY (ignores ports A, B, D)
  
  FT4232H enumerates as 4 separate devices. We identify Port C
  by checking the interface descriptor.
*/

/*********************************************************************
*
*       #include section
*
**********************************************************************
*/
#include "RTOS.h"
#include "BSP.h"
#include "USBH.h"
#include "USBH_FT232.h"

/*********************************************************************
*
*       Local data definitions
*
**********************************************************************
*/
enum {
  TASK_PRIO_APP = 150,
  TASK_PRIO_USBH_MAIN,
  TASK_PRIO_USBH_ISR
};

//
// Device types for identification
//
typedef enum {
  DEVICE_TYPE_UNKNOWN = 0,
  DEVICE_TYPE_FT232,          // Single port FT232
  DEVICE_TYPE_FT4232H_PORT_A, // Port A of FT4232H
  DEVICE_TYPE_FT4232H_PORT_B, // Port B of FT4232H
  DEVICE_TYPE_FT4232H_PORT_C, // Port C of FT4232H
  DEVICE_TYPE_FT4232H_PORT_D  // Port D of FT4232H
} DEVICE_TYPE;

//
// Device information structure
//
typedef struct {
  USBH_FT232_HANDLE      hDevice;
  USBH_FT232_DEVICE_INFO DeviceInfo;
  DEVICE_TYPE            Type;
  U8                     DevIndex;
  volatile char          IsReady;
  char                   Name[32];
} BRIDGE_DEVICE;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static OS_STACKPTR int     _StackMain[2048/sizeof(int)];
static OS_TASK             _TCBMain;

static OS_STACKPTR int     _StackIsr[1024/sizeof(int)];
static OS_TASK             _TCBIsr;

static USBH_NOTIFICATION_HOOK _Hook;

//
// Bridge devices
//
static BRIDGE_DEVICE _DeviceFT232 = {USBH_FT232_INVALID_HANDLE, {0}, DEVICE_TYPE_UNKNOWN, 0xFF, 0, "FT232"};
static BRIDGE_DEVICE _DeviceFT4232C = {USBH_FT232_INVALID_HANDLE, {0}, DEVICE_TYPE_UNKNOWN, 0xFF, 0, "FT4232H-C"};

//
// Track all FT4232H ports to identify Port C
//
static struct {
  U8 DevIndex;
  U8 InterfaceNum;
  U8 Valid;
} _FT4232H_Ports[4];

static U8 _FT4232H_DeviceCount = 0;

//
// Bridge statistics
//
static struct {
  U32 BytesFT232ToFT4232C;
  U32 BytesFT4232CToFT232;
  U32 ErrorCount;
} _BridgeStats = {0, 0, 0};

/*********************************************************************
*
*       Static Code
*
**********************************************************************
*/

/*********************************************************************
*
*       Static data for FT4232H port tracking
*
**********************************************************************
*/
static struct {
  U8  DeviceCount;
  U8  PortCIndex;
  U16 VendorId;
  U16 ProductId;
} _FT4232H_Info = {0, 0xFF, 0, 0};

/*********************************************************************
*
*       _IdentifyFT4232H_Port
*
*  Function description
*    For FT4232H devices, determines which port based on enumeration order
*    FT4232H always enumerates in order: A, B, C, D
*/
static DEVICE_TYPE _IdentifyFT4232H_Port(U8 DevIndex, USBH_FT232_DEVICE_INFO* pInfo) {
  static U8 LastDevIndex = 0xFF;
  static U8 PortCounter = 0;
  
  //
  // If this is a new FT4232H device (different from last), reset counter
  //
  if (DevIndex != (LastDevIndex + 1)) {
    PortCounter = 0;
  }
  
  LastDevIndex = DevIndex;
  
  //
  // FT4232H enumerates 4 consecutive devices
  //
  DEVICE_TYPE Type = DEVICE_TYPE_FT4232H_PORT_A + PortCounter;
  
  //
  // Track Port C index
  //
  if (PortCounter == 2) {  // Port C is the 3rd port (index 2)
    _FT4232H_Info.PortCIndex = DevIndex;
  }
  
  PortCounter++;
  if (PortCounter >= 4) {
    PortCounter = 0;
  }
  
  return Type;
}

/*********************************************************************
*
*       _IdentifyDevice
*
*  Function description
*    Identifies the device type and port
*/
static DEVICE_TYPE _IdentifyDevice(USBH_FT232_DEVICE_INFO* pInfo, U8 DevIndex) {
  static U16 LastUnknownPID = 0;
  static U8 UnknownPIDCount = 0;
  
  //
  // Check Product ID
  //
  switch (pInfo->ProductId) {
    case 0x6011:  // FT4232H standard PID
    case 0x6043:  // FT4232H variant (custom PID)
      return _IdentifyFT4232H_Port(DevIndex, pInfo);
    
    case 0x6001:  // FT232R/FT232B
    case 0x6010:  // FT2232H/FT2232D
    case 0x6014:  // FT232H
      return DEVICE_TYPE_FT232;
    
    default:
      //
      // Unknown PID - might be a custom FT4232H
      // If we see 4 consecutive devices with the same unknown PID,
      // assume it's an FT4232H
      //
      if (pInfo->ProductId == LastUnknownPID || LastUnknownPID == 0) {
        LastUnknownPID = pInfo->ProductId;
        UnknownPIDCount++;
        
        if (UnknownPIDCount <= 4) {
          USBH_Logf_Application("Unknown FTDI PID 0x%04X - device %d of possible FT4232H", 
                                pInfo->ProductId, UnknownPIDCount);
          return _IdentifyFT4232H_Port(DevIndex, pInfo);
        }
      } else {
        // Different PID, reset counter
        LastUnknownPID = pInfo->ProductId;
        UnknownPIDCount = 1;
      }
      
      USBH_Logf_Application("Unknown FTDI device: PID=0x%04X", pInfo->ProductId);
      return DEVICE_TYPE_FT232;  // Assume single port
  }
}

/*********************************************************************
*
*       _OpenDevice
*
*  Function description
*    Opens and configures an FTDI device
*/
static int _OpenDevice(BRIDGE_DEVICE* pDev) {
  USBH_STATUS Status;
  
  pDev->hDevice = USBH_FT232_Open(pDev->DevIndex);
  
  if (pDev->hDevice == USBH_FT232_INVALID_HANDLE) {
    USBH_Logf_Application("Failed to open %s", pDev->Name);
    return -1;
  }
  
  //
  // Configure the device for UART communication
  //
  USBH_FT232_AllowShortRead(pDev->hDevice, 1);
  USBH_FT232_SetTimeouts(pDev->hDevice, 10, 1000);  // 10ms read timeout for fast bridging
  
  Status = USBH_FT232_SetBaudRate(pDev->hDevice, USBH_FT232_BAUD_115200);
  if (Status != USBH_STATUS_SUCCESS) {
    USBH_Logf_Application("Failed to set baudrate for %s", pDev->Name);
    USBH_FT232_Close(pDev->hDevice);
    pDev->hDevice = USBH_FT232_INVALID_HANDLE;
    return -1;
  }
  
  USBH_FT232_SetDataCharacteristics(pDev->hDevice, USBH_FT232_BITS_8, USBH_FT232_STOP_BITS_1, USBH_FT232_PARITY_NONE);
  USBH_FT232_SetFlowControl(pDev->hDevice, USBH_FT232_FLOW_NONE, 0, 0);
  
  //
  // Clear buffers
  //
  USBH_FT232_Purge(pDev->hDevice, USBH_FT232_PURGE_RX | USBH_FT232_PURGE_TX);
  
  USBH_Logf_Application("%s configured successfully", pDev->Name);
  return 0;
}

/*********************************************************************
*
*       _BridgeData
*
*  Function description
*    Bridges data from source device to destination device
*/
static void _BridgeData(BRIDGE_DEVICE* pSrc, BRIDGE_DEVICE* pDst, U32* pByteCounter) {
  U8 Buffer[256];
  U32 NumBytesRead, NumBytesWritten;
  USBH_STATUS Status;
  
  //
  // Read from source
  //
  Status = USBH_FT232_Read(pSrc->hDevice, Buffer, sizeof(Buffer), &NumBytesRead);
  
  if (Status == USBH_STATUS_SUCCESS && NumBytesRead > 0) {
    //
    // Forward to destination
    //
    Status = USBH_FT232_Write(pDst->hDevice, Buffer, NumBytesRead, &NumBytesWritten);
    
    if (Status == USBH_STATUS_SUCCESS) {
      *pByteCounter += NumBytesWritten;
      
      // Optional: Log the bridged data (disable in production for performance)
      #ifdef DEBUG_BRIDGE_DATA
      Buffer[NumBytesRead] = '\0';
      USBH_Logf_Application("%s->%s: %s", pSrc->Name, pDst->Name, Buffer);
      #endif
    } else if (Status != USBH_STATUS_TIMEOUT) {
      _BridgeStats.ErrorCount++;
      USBH_Logf_Application("Write error %s->%s: %d", pSrc->Name, pDst->Name, Status);
    }
  } else if (Status != USBH_STATUS_TIMEOUT && Status != USBH_STATUS_SUCCESS) {
    _BridgeStats.ErrorCount++;
    USBH_Logf_Application("Read error from %s: %d", pSrc->Name, Status);
  }
}

/*********************************************************************
*
*       _RunBridge
*
*  Function description
*    Main bridge loop - forwards data between two devices
*/
static void _RunBridge(void) {
  U32 LastStatsTime = 0;
  
  USBH_Logf_Application("========================================");
  USBH_Logf_Application("UART Bridge Started");
  USBH_Logf_Application("%s <-> %s", _DeviceFT232.Name, _DeviceFT4232C.Name);
  USBH_Logf_Application("========================================");
  
  //
  // Main bridge loop
  //
  while (_DeviceFT232.IsReady && _DeviceFT4232C.IsReady) {
    //
    // Bridge data in both directions
    //
    _BridgeData(&_DeviceFT232, &_DeviceFT4232C, &_BridgeStats.BytesFT232ToFT4232C);
    _BridgeData(&_DeviceFT4232C, &_DeviceFT232, &_BridgeStats.BytesFT4232CToFT232);
    
    //
    // Print statistics every 10 seconds
    //
    if ((OS_GetTime32() - LastStatsTime) >= 10000) {
      USBH_Logf_Application("Bridge Stats: FT232->FT4232C: %u bytes, FT4232C->FT232: %u bytes, Errors: %u",
                            _BridgeStats.BytesFT232ToFT4232C,
                            _BridgeStats.BytesFT4232CToFT232,
                            _BridgeStats.ErrorCount);
      LastStatsTime = OS_GetTime32();
    }
    
    //
    // Small delay to prevent CPU overload
    //
    OS_Delay(1);
  }
  
  USBH_Logf_Application("Bridge stopped - device disconnected");
}

/*********************************************************************
*
*       _OnDevReady
*
*  Function description
*    Called when both devices are connected and ready
*/
static void _OnDevReady(void) {
  //
  // Open and configure FT232
  //
  if (_OpenDevice(&_DeviceFT232) != 0) {
    return;
  }
  
  //
  // Open and configure FT4232H Port C
  //
  if (_OpenDevice(&_DeviceFT4232C) != 0) {
    USBH_FT232_Close(_DeviceFT232.hDevice);
    _DeviceFT232.hDevice = USBH_FT232_INVALID_HANDLE;
    return;
  }
  
  //
  // Start bridging
  //
  _RunBridge();
  
  //
  // Cleanup
  //
  if (_DeviceFT232.hDevice != USBH_FT232_INVALID_HANDLE) {
    USBH_FT232_Close(_DeviceFT232.hDevice);
    _DeviceFT232.hDevice = USBH_FT232_INVALID_HANDLE;
  }
  
  if (_DeviceFT4232C.hDevice != USBH_FT232_INVALID_HANDLE) {
    USBH_FT232_Close(_DeviceFT4232C.hDevice);
    _DeviceFT4232C.hDevice = USBH_FT232_INVALID_HANDLE;
  }
}

/*********************************************************************
*
*       _cbOnAddRemoveDevice
*
*  Function description
*    Callback when a device is added or removed
*/
static void _cbOnAddRemoveDevice(void * pContext, U8 DevIndex, USBH_DEVICE_EVENT Event) {
  USBH_FT232_HANDLE hTemp;
  USBH_FT232_DEVICE_INFO TempInfo;
  DEVICE_TYPE DeviceType;
  
  (void)pContext;
  
  switch (Event) {
  case USBH_DEVICE_EVENT_ADD:
    USBH_Logf_Application("**** Device added [%d]", DevIndex);
    
    //
    // Get device info to identify it
    //
    hTemp = USBH_FT232_Open(DevIndex);
    if (hTemp != USBH_FT232_INVALID_HANDLE) {
      USBH_FT232_GetDeviceInfo(hTemp, &TempInfo);
      USBH_FT232_Close(hTemp);
      
      DeviceType = _IdentifyDevice(&TempInfo, DevIndex);
      
      //
      // Handle based on device type
      //
      if (DeviceType == DEVICE_TYPE_FT232) {
        //
        // Single port FT232
        //
        if (!_DeviceFT232.IsReady) {
          _DeviceFT232.DevIndex = DevIndex;
          _DeviceFT232.DeviceInfo = TempInfo;
          _DeviceFT232.Type = DeviceType;
          _DeviceFT232.IsReady = 1;
          USBH_Logf_Application("FT232 connected and ready");
        } else {
          USBH_Logf_Application("FT232 already connected, ignoring new device");
        }
      } else if (DeviceType == DEVICE_TYPE_FT4232H_PORT_C) {
        //
        // FT4232H Port C
        //
        if (!_DeviceFT4232C.IsReady) {
          _DeviceFT4232C.DevIndex = DevIndex;
          _DeviceFT4232C.DeviceInfo = TempInfo;
          _DeviceFT4232C.Type = DeviceType;
          _DeviceFT4232C.IsReady = 1;
          USBH_Logf_Application("FT4232H Port C connected and ready");
        } else {
          USBH_Logf_Application("FT4232H Port C already connected, ignoring");
        }
      } else if (DeviceType >= DEVICE_TYPE_FT4232H_PORT_A && DeviceType <= DEVICE_TYPE_FT4232H_PORT_D) {
        //
        // Other FT4232H ports - log which port
        //
        char PortChar = 'A' + (DeviceType - DEVICE_TYPE_FT4232H_PORT_A);
        USBH_Logf_Application("FT4232H Port %c detected, ignoring (only using Port C)", PortChar);
      }
    }
    break;
    
  case USBH_DEVICE_EVENT_REMOVE:
    USBH_Logf_Application("**** Device removed [%d]", DevIndex);
    
    //
    // Find which device was removed
    //
    if (_DeviceFT232.DevIndex == DevIndex) {
      _DeviceFT232.IsReady = 0;
      _DeviceFT232.DevIndex = 0xFF;
      USBH_Logf_Application("FT232 disconnected");
    } else if (_DeviceFT4232C.DevIndex == DevIndex) {
      _DeviceFT4232C.IsReady = 0;
      _DeviceFT4232C.DevIndex = 0xFF;
      USBH_Logf_Application("FT4232H Port C disconnected");
    }
    break;
    
  default:
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
#ifdef __cplusplus
extern "C" {     /* Make sure we have C-declarations in C++ programs */
#endif
void MainTask(void);
#ifdef __cplusplus
}
#endif
void MainTask(void) {
  USBH_Init();
  OS_SetPriority(OS_GetTaskID(), TASK_PRIO_APP);
  OS_CREATETASK(&_TCBMain, "USBH_Task", USBH_Task, TASK_PRIO_USBH_MAIN, _StackMain);
  OS_CREATETASK(&_TCBIsr, "USBH_isr", USBH_ISRTask, TASK_PRIO_USBH_ISR, _StackIsr);
  
  USBH_FT232_Init();
  USBH_FT232_AddNotification(&_Hook, _cbOnAddRemoveDevice, NULL);
  
  USBH_Logf_Application("UART Bridge waiting for devices...");
  USBH_Logf_Application("Looking for: FT232 and FT4232H Port C");
  
  //
  // Main loop - wait for both specific devices to be connected
  //
  while (1) {
    BSP_ToggleLED(1);
    
    if (_DeviceFT232.IsReady && _DeviceFT4232C.IsReady) {
      USBH_Logf_Application("Both devices connected - starting bridge");
      _OnDevReady();
      
      //
      // Reset device states after bridge ends
      //
      _DeviceFT232.IsReady = 0;
      _DeviceFT4232C.IsReady = 0;
      _DeviceFT232.DevIndex = 0xFF;
      _DeviceFT4232C.DevIndex = 0xFF;
      
      //
      // Reset statistics
      //
      _BridgeStats.BytesFT232ToFT4232C = 0;
      _BridgeStats.BytesFT4232CToFT232 = 0;
      _BridgeStats.ErrorCount = 0;
    }
    
    OS_Delay(100);
  }
}

/*************************** End of file ****************************/