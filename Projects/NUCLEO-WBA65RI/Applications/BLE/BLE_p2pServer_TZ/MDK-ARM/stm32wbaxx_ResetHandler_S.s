;********************************************************************************
;* File Name          : stm32wbaxx_ResetHandler_S.s
;* Author             : MCD Application Team
;* Description        : STM32WBA6xx Ultra Low Power Devices specific
;                       Reset handler for Secure projet for connectivity
;                       applications.
;                       MDK-ARM toolchain.
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
                 PRESERVE8
                 THUMB
                 AREA    |.text|, CODE, READONLY

Reset_Handler   PROC
                EXPORT  Reset_Handler
                IMPORT  SystemInit
                IMPORT  __main
                IMPORT  RestoreFromStandby
                LDR     R0, =RestoreFromStandby
                BLX     R0
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP
                END
