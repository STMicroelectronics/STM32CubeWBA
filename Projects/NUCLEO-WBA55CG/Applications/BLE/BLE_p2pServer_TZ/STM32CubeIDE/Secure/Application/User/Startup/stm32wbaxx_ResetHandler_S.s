/**
  ******************************************************************************
  * File Name          : stm32wbaxx_ResetHandler_S.s
  * Author             : MCD Application Team
  * Description        : STM32WBA5xx Ultra Low Power Devices specific
  *                      Reset handler for Secure project for connectivity
  *                      applications.
                         GCC toolchain.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

  .extern SystemInit
  .extern RestoreFromStandby

  .section .text.Reset_Handler
  .global Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  ldr   sp, =_estack    /* set stack pointer */
/* Call the function allowing to exit from Standby mode*/
  bl RestoreFromStandby
/* Call the clock system initialization function.*/
  bl  SystemInit

/* Copy the data segment initializers from flash to SRAM */
  movs	r1, #0
  b	LoopCopyDataInit

CopyDataInit:
	ldr	r3, =_sidata
	ldr	r3, [r3, r1]
	str	r3, [r0, r1]
	adds	r1, r1, #4

LoopCopyDataInit:
	ldr	r0, =_sdata
	ldr	r3, =_edata
	adds	r2, r0, r1
	cmp	r2, r3
	bcc	CopyDataInit
	ldr	r2, =_sbss
	b	LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
	movs	r3, #0
	str	r3, [r2], #4

LoopFillZerobss:
	ldr	r3, = _ebss
	cmp	r2, r3
	bcc	FillZerobss

/* Call static constructors */
  bl __libc_init_array
/* Call the application's entry point.*/
	bl	main

LoopForever:
    b LoopForever

.size	Reset_Handler, .-Reset_Handler
