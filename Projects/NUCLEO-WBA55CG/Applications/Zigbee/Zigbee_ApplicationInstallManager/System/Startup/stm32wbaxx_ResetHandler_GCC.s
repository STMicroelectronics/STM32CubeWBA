/**
  ******************************************************************************
  * File Name          : stm32wbaxx_ResetHandler.s
  * Author             : MCD Application Team
  * Description        : STM32WBA5xx Ultra Low Power Devices specific
  *                      Reset handler for connectivity applications.
                         GCC toolchain.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  *
  * Cortex-M version
  *
  ******************************************************************************
  */

  .syntax unified
	.cpu cortex-m33
	.fpu softvfp
	.thumb

  .extern  SystemInit
  .extern  is_boot_from_standby
  .extern  JumpFwApp

/* INIT_BSS macro is used to fill the specified region [start : end] with zeros */
.macro INIT_BSS start, end
  ldr r0, =\start
  ldr r1, =\end
  movs r3, #0
  bl LoopFillZerobss
.endm

/* INIT_DATA macro is used to copy data in the region [start : end] starting from 'src' */
.macro INIT_DATA start, end, src
  ldr r0, =\start
  ldr r1, =\end
  ldr r2, =\src
  movs r3, #0
  bl LoopCopyDataInit
.endm

.section  .text.data_initializers
CopyDataInit:
  ldr r4, [r2, r3]
  str r4, [r0, r3]
  adds r3, r3, #4

LoopCopyDataInit:
  adds r4, r0, r3
  cmp r4, r1
  bcc  CopyDataInit
  bx lr

FillZerobss:
  str  r3, [r0]
  adds r0, r0, #4

LoopFillZerobss:
  cmp r0, r1
  bcc FillZerobss
  bx lr

  .section .text.Reset_Handler
  .global Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0          /* set stack pointer */
/* If we exit from standby mode, restore CPU context and jump to asleep point. */
/* Initialize the Stack Pointer 10 bytes before end of Stack */
/* 10 bytes are the necessary stack spaces to jump directly in the active application */ 
   LDR     SP, =0x20000020
   BL      is_boot_from_standby
   CMP     R0, #1
   BEQ     JumpFwApp
/* In case of reset on the downloaded application */
/* Initialize the Stack Pointer to the address stored at the beginning of the Flash */
   LDR     R0, =0x8000000 
   LDR     SP, [R0]
/* end of specific code section for standby */
/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  INIT_DATA _sdata, _edata, _sidata

/* Zero fill the bss segments. */
  INIT_BSS _sbss, _ebss

/* Call static constructors */
  bl __libc_init_array
/* Call the application s entry point.*/
  bl	main

LoopForever:
  b LoopForever

/* end of specific code section for standby */
  .end
