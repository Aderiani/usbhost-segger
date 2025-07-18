/*
** ###################################################################
**     Processors:          LPC54608J512BD208
**                          LPC54608J512ET180
**
**     Compilers:           Keil ARM C/C++ Compiler
**                          GNU C Compiler
**                          IAR ANSI C/C++ Compiler for ARM
**
**     Reference manual:    LPC546xx User manual Rev.1.9  5 June 2017
**     Version:             rev. 1.2, 2017-06-08
**     Build:               b180115
**
**     Abstract:
**         Provides a system configuration function and a global variable that
**         contains the system frequency. It configures the device and initializes
**         the oscillator (PLL) that is part of the microcontroller device.
**
**     The Clear BSD License
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2018 NXP
**     All rights reserved.
**
**     Redistribution and use in source and binary forms, with or without
**     modification, are permitted (subject to the limitations in the
**     disclaimer below) provided that the following conditions are met:
**
**     * Redistributions of source code must retain the above copyright
**       notice, this list of conditions and the following disclaimer.
**
**     * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**
**     * Neither the name of the copyright holder nor the names of its
**       contributors may be used to endorse or promote products derived from
**       this software without specific prior written permission.
**
**     NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
**     GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT
**     HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
**     WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
**     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
**     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
**     LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
**     CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
**     SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
**     BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
**     WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
**     OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
**     IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**     http:                 www.nxp.com
**     mail:                 support@nxp.com
**
**     Revisions:
**     - rev. 1.0 (2016-08-12)
**         Initial version.
**     - rev. 1.1 (2016-11-25)
**         Update CANFD and Classic CAN register.
**         Add MAC TIMERSTAMP registers.
**     - rev. 1.2 (2017-06-08)
**         Remove RTC_CTRL_RTC_OSC_BYPASS.
**         SYSCON_ARMTRCLKDIV rename to SYSCON_ARMTRACECLKDIV.
**         Remove RESET and HALT from SYSCON_AHBCLKDIV.
**
** ###################################################################
*/

/*!
 * @file LPC54605
 * @version 1.2
 * @date 2017-06-08
 * @brief Device specific configuration file for LPC54608 (implementation file)
 *
 * Provides a system configuration function and a global variable that contains
 * the system frequency. It configures the device and initializes the oscillator
 * (PLL) that is part of the microcontroller device.
 */

#include <stdint.h>
#include "LPC54605.h"

extern void *_vectors;

void POWER_SetPLL(void);
void POWER_SetVoltageForFreq(uint32_t freq);

/* ----------------------------------------------------------------------------
   -- Core clock
   ---------------------------------------------------------------------------- */

uint32_t SystemCoreClock = DEFAULT_SYSTEM_CLOCK;

/* ----------------------------------------------------------------------------
   -- SystemInit()
   ---------------------------------------------------------------------------- */

void SystemInit (void) {
  uint32_t tmp;

#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

  SCB->VTOR = (uint32_t)&_vectors;
/* Optionally enable RAM banks that may be off by default at reset */
#if !defined(DONT_ENABLE_DISABLED_RAMBANKS)
  SYSCON->AHBCLKCTRLSET[0] = SYSCON_AHBCLKCTRL_SRAM1_MASK | SYSCON_AHBCLKCTRL_SRAM2_MASK | SYSCON_AHBCLKCTRL_SRAM3_MASK;
#endif

  /*!< Set up the clock sources */
  /*!< Set up FRO */
  SYSCON->PDRUNCFGCLR[0] = (1u << 4);                    /*!< Ensure FRO is on  */
  SYSCON->MAINCLKSELA = 0;
  SYSCON->MAINCLKSELB = 0;                               /*!< Switch to FRO 12MHz first to ensure we can change voltage without accidentally
                                                              being below the voltage for current speed */
//  POWER_SetVoltageForFreq(12000000U);                    /*!< Set voltage for the one of the fastest clock outputs: System clock output */
  tmp = SYSCON->FLASHCFG & ~(SYSCON_FLASHCFG_FLASHTIM_MASK);
  SYSCON->FLASHCFG = tmp | (5u << SYSCON_FLASHCFG_FLASHTIM_SHIFT);    /*!< Set FLASH wait states for core */

  SYSCON->SYSPLLCLKSEL = 1;                        /*!< Set sys pll clock source from external crystal */

  /* Turn on the ext clock  */
  SYSCON->PDRUNCFGCLR[0] = (1u <<  9);      // PDEN_VD2_ANA
  SYSCON->PDRUNCFGCLR[1] = (1u <<  3);      // system oscillator
  /* Enable power VD3 for PLLs */
  SYSCON->PDRUNCFGCLR[1] = (1u <<  3);      // system oscillator
  POWER_SetPLL();
  /* Power off PLL during setup changes */
  SYSCON->PDRUNCFGSET[0] = (1u << 22);

  /* Write PLL setup data */
  SYSCON->SYSPLLCTRL = SYSCON_SYSPLLCTRL_SELI(32U) | SYSCON_SYSPLLCTRL_SELP(16U) | SYSCON_SYSPLLCTRL_SELR(0U);
  SYSCON->SYSPLLNDEC = SYSCON_SYSPLLNDEC_NDEC(770U);
  SYSCON->SYSPLLNDEC = SYSCON_SYSPLLNDEC_NDEC(770U) | (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */
  SYSCON->SYSPLLPDEC = SYSCON_SYSPLLPDEC_PDEC(98U);
  SYSCON->SYSPLLPDEC = SYSCON_SYSPLLPDEC_PDEC(98U) | (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */
  SYSCON->SYSPLLMDEC = SYSCON_SYSPLLMDEC_MDEC(8191U);
  SYSCON->SYSPLLMDEC = SYSCON_SYSPLLMDEC_MDEC(8191U) | (1U << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */

  /* Enable peripheral states by setting low */
  SYSCON->PDRUNCFGCLR[0] = (1u << 22);
  while ((SYSCON->SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK_MASK) == 0) {
  }

  POWER_SetVoltageForFreq(180000000U);             /*!< Set voltage for the one of the fastest clock outputs: System clock output */
  tmp = SYSCON->FLASHCFG & ~(SYSCON_FLASHCFG_FLASHTIM_MASK);
  SYSCON->FLASHCFG = tmp | (8u << SYSCON_FLASHCFG_FLASHTIM_SHIFT);    /*!< Set FLASH wait states for core */
  SYSCON->MAINCLKSELB = 2;                         /*!< Switch System clock to SYS PLL 180MHz */
}

/* ----------------------------------------------------------------------------
   -- SystemCoreClockUpdate()
   ---------------------------------------------------------------------------- */

void SystemCoreClockUpdate (void) {
  SystemCoreClock = 180000000U;
}
