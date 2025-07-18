/*
 * The Clear BSD License
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright (c) 2016, NXP
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


    .syntax unified


/*********************************************************************
*
*       Function POWER_SetVoltageForFreq
*
*/
    .global POWER_SetVoltageForFreq
    .section ".text.POWER_SetVoltageForFreq", "ax", %progbits
    .thumb
    .thumb_func
    .balign 4
    .type POWER_SetVoltageForFreq, %function
POWER_SetVoltageForFreq:
    ldr        R3, =0x05F5E100
    cmp        R0, R3
    ldr        R3, =0x40020000
    bhi        _POWER_SetVoltageForFreq_2
    movs       R2, #0xB
    movs       R1, #0xC
    str        R2, [R3]
    str        R1, [R3, #4]
    str        R2, [R3, #8]
    str        R2, [R3, #0xC]
_POWER_SetVoltageForFreq_1:
    str        R2, [R3, #0x10]
    str        R2, [R3, #0x14]
    bx         LR
_POWER_SetVoltageForFreq_2:
    ldr        R2, =0x0ABA9500
    cmp        R0, R2
    it         LS
    movls      R1, #0xD
    mov        R2, #0xC
    it         HI
    movhi      R1, #0xF
    str        R1, [R3]
    str        R2, [R3, #4]
    movs       R2, #0xB
    str        R2, [R3, #8]
    str        R1, [R3, #0xC]
    b          _POWER_SetVoltageForFreq_1
    nop
    .pool

/*********************************************************************
*
*       Function POWER_SetPLL
*
*/
    .global POWER_SetPLL
    .section ".text.POWER_SetPLL", "ax", %progbits
    .thumb
    .thumb_func
    .balign 4
    .type POWER_SetPLL, %function
POWER_SetPLL:
    mov        R3, #0x40000000
    mov        R2, #0x4000000
    str        R2, [R3, #0x630]
    ldr        R2, =0x40020000
_POWER_SetPLL_1:
    ldr        R3, [R2, #0x54]
    lsls       R3, R3, #26
    bpl        _POWER_SetPLL_1
    bx         LR
    nop
    .pool


