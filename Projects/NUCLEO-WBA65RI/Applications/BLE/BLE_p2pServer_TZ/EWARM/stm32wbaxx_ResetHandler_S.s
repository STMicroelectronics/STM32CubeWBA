;********************************************************************************
;* File Name          : stm32wbaxx_ResetHandler_S.s
;* Author             : MCD Application Team
;* Description        : STM32WBA5xx Ultra Low Power Devices specific
;                       Reset handler for Secure project for connectivity 
;                       applications.
;                       EWARM toolchain.
;********************************************************************************
;* @attention
;*
;* Copyright (c) 2024 STMicroelectronics.
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
        EXTERN  RestoreFromStandby

        PUBLIC Reset_Handler
        SECTION .text:CODE:NOROOT:REORDER(2)
Reset_Handler
        LDR     R0, =RestoreFromStandby
        BLX     R0
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0
        END
