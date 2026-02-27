;********************************************************************************
;* File Name          : stm32wbaxx_ResetHandler.s
;* Author             : MCD Application Team
;* Description        : STM32WBA5xx Ultra Low Power Devices specific
;                       Reset handler for connectivity applications.
;                       EWARM toolchain.
;********************************************************************************
;* @attention
;*
;* Copyright (c) 2022 STMicroelectronics.
;* All rights reserved.
;*
;* This software is licensed under terms that can be found in the LICENSE file
;* in the root directory of this software component.
;* If no LICENSE file comes with this software, it is provided AS-IS.
;*
;*******************************************************************************
;
; Cortex-M version
;
        EXTERN  __iar_program_start
        EXTERN  SystemInit
        EXTERN  is_boot_from_standby
        EXTERN  JumpFwApp

        PUBLIC Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
/* If we exit from standby mode, restore CPU context and jump to asleep point. */
/* Initialize the Stack Pointer 10 bytes before end of Stack */
/* 10 bytes are the necessary stack spaces to jump directly in the active application */ 
        LDR     SP, =0x20000020
        BL      is_boot_from_standby
        CMP     R0, #1
        BEQ     JumpFwApp
/* end of specific code section for standby */
/* In case of reset on the downloaded application */
/* Initialize the Stack Pointer to the address stored at the beginning of the Flash */
        LDR     R0, =0x8000000 
        LDR     SP, [R0]
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        END
