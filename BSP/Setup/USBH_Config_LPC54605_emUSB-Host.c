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
File        : USBH_Config_LPC54605_emUSB-Host.c
Purpose     : emUSB Host configuration file for HS and FS controller
              on the SEGGER emUSB-Host board
---------------------------END-OF-HEADER------------------------------
*/

#ifndef USE_HS
  #define USE_HS  1     // Use high-speed controller
#endif
#ifndef USE_FS
  #define USE_FS  1     // Use full-speed controller
#endif

/*********************************************************************
*
*       #include Section
*
**********************************************************************
*/
#include <stdlib.h>
#include "USBH.h"
#if USE_HS
#include "USBH_HW_NXP_IP3516.h"
#endif
#if USE_FS
#include "USBH_HW_OHCI_LPC546xx.h"
#endif
#include "BSP_USB.h"
#include "LPC54605.h"
#include "BSP.h"

/*********************************************************************
*
*       Defines, configurable
*
**********************************************************************
*/
#define ALLOC_SIZE               0x6000

/*********************************************************************
*
*       Defines, non-configurable
*
**********************************************************************
*/
#define USB_USBHS_BASE_ADDR         0x400A3000u
#define USB_USB_RAM_ADDR            0x40100000u
#define USB_USB_RAM_SIZE                0x2000u

#define OHCI_BASE_ADDRESS           0x400A2000u

#define USB_ISR_PRIO  (254)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U32 _aPool[((ALLOC_SIZE) / 4)];

#if USE_HS
  static U32 _IndexHS;
#endif
#if USE_FS
  static U32 _IndexFS;
#endif

//
// Define categories of debug log messages that should be printed.
// For possible categories, see USBH_MCAT_... definitions in USBH.h
//
static const U8 _LogCategories[] = {
  USBH_MCAT_INIT,
  USBH_MCAT_APPLICATION
};

void StartVoltageMonitor(void);

/*********************************************************************
*
*       _InitUSBHw
*
*/
static void _InitUSBHw(void) {
  //
  // Power up PLL stuff
  //
  SYSCON->PDRUNCFGCLR[0] = (1u << 27);      // PDEN_VD4
  SYSCON->PDRUNCFGCLR[0] = (1u <<  9);      // PDEN_VD2_ANA
  SYSCON->PDRUNCFGCLR[1] = (1u <<  3);      // system oscillator
  SYSCON->PDRUNCFGCLR[0] = (1u << 26);      // PDEN_VD3 (POWER_SetPLL())
  //
  // Configure USB PLL to 48 MHz (when crystal oscillator is running at 12 MHz)
  //
  SYSCON->PDRUNCFGSET[1] = (1u <<  1);     // USB PLL off
  SYSCON->USBPLLCTRL = (15u << 0)          // M = 16
                     | (1u  << 8)          // P = 2
                     | (0u  << 10)         // N = 1
                     ;
  SYSCON->PDRUNCFGCLR[1] = (1u <<  1);     // USB PLL on
  //
  // Wait for USB PLL to lock
  //
  USBH_OS_Delay(2);                         // Wait > 72us
  while (SYSCON->USBPLLSTAT == 0) {
  }
  //
  // Power up USB stuff
  //
  SYSCON->PDRUNCFGCLR[0] = (1u << 28);      // PDEN_VD5 (POWER_SetUsbPhy())
#if USE_HS
  SYSCON->PDRUNCFGCLR[0] = (1u << 16);      // USB SRAM
  SYSCON->PDRUNCFGCLR[1] = (1u <<  0);      // USB1 PHY
  //
  // Select USB PLL as clock source and enable clock
  //
  SYSCON->USB1CLKDIV = 0;
  SYSCON->USB1CLKSEL = 2;
  SYSCON->AHBCLKCTRLSET[2] = (1u << 4) | (1u << 6);
  //
  // Reset USB controller
  //
  SYSCON->PRESETCTRLSET[2] = (1u << 4);
  USBH_OS_Delay(2);
  SYSCON->PRESETCTRLCLR[2] = (1u << 4);
  //
  // Enable USB port for host mode
  //
  USBHSH->PORTMODE &= ~(1u << 16);
#endif
#if USE_FS
  //
  // Configure PINs
  //
  SYSCON->AHBCLKCTRLSET[0] = (1u << 13) | (1u << 14);   // Enable IOCON and PIO 0
  IOCON->PIO[0][22] = (1u << 8) | 7u;
  SYSCON->AHBCLKCTRLSET[2] = (1u << 9);     // Enable PIO 4
  IOCON->PIO[4][7] = (1u << 8) | 3u;        // VBUS enable on Pin 7 of PIO4
  //
  // Power up USB stuff
  //
  SYSCON->PDRUNCFGCLR[0] = (1u << 21);      // USB0 PHY
  //
  // Select USB PLL as clock source and enable clock
  //
  SYSCON->USB0CLKDIV = 0;
  SYSCON->USB0CLKSEL = 2;
  SYSCON->AHBCLKCTRLSET[2] = (1u << 16);    // Enable host controller
  SYSCON->AHBCLKCTRLSET[2] = (1u << 17);    // Enable host controller
#endif
}

/*********************************************************************
*
*       _ISR0
*
*/
#if USE_FS
static void _ISR0(void) {
  USBH_ServiceISR(_IndexFS);
}
#endif

/*********************************************************************
*
*       _ISR1
*
*/
#if USE_HS
static void _ISR1(void) {
  USBH_ServiceISR(_IndexHS);
}
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/*********************************************************************
*
*       DelayHalfUs
*
*  Function description
*    Delay for Time * 0.5 us
*/
void DelayHalfUs(I32 Time);
void DelayHalfUs(I32 Time) {
  I32 tStart;
  I32 WaitCycles;
  volatile I32 *pCycCounter = (volatile I32 *)0xE0001004;     // Cortex-M cycle counter

  tStart = *pCycCounter;
  WaitCycles = Time *= 90;                   // Multiply with half the CPU clock frequency in MHz
  while (*pCycCounter - tStart < WaitCycles) {
  }
}

/*********************************************************************
*
*       USBH_X_Config
*
*  Function description
*/
void USBH_X_Config(void) {
  USBH_AssignMemory(&_aPool[0], ALLOC_SIZE);    // Assigning memory should be the first thing
  USBH_ConfigSupportExternalHubs (1);           // Default values: The hub module is disabled, this is done to save memory.
  // USBH_ConfigPowerOnGoodTime     (300);         // Default values: 300 ms wait time before the host starts communicating with a device.
  //
  // Define log and warn filter
  // Note: The terminal I/O emulation affects the timing
  // of your communication, since the debugger stops the target
  // for every terminal I/O unless you use RTT!
  //
  USBH_ConfigMsgFilter(USBH_WARN_FILTER_SET_ALL, 0, NULL);           // Output all warnings.
  USBH_ConfigMsgFilter(USBH_LOG_FILTER_SET, sizeof(_LogCategories), _LogCategories);
  StartVoltageMonitor();
  _InitUSBHw();
#if USE_HS
  _IndexHS = USBH_LPC54xxx_Add(USB_USBHS_BASE_ADDR, USB_USB_RAM_ADDR, USB_USB_RAM_SIZE);
  BSP_USBH_InstallISR_Ex((int)USB1_IRQn, _ISR1, USB_ISR_PRIO);
#endif
#if USE_FS
  _IndexFS = USBH_OHCI_LPC546_Add((void * )OHCI_BASE_ADDRESS);
  USBH_ConfigTransferBufferSize(_IndexFS, 1024);
  BSP_USBH_InstallISR_Ex((int)USB0_IRQn, _ISR0, USB_ISR_PRIO);
#endif
}

/*************************** End of file ****************************/
