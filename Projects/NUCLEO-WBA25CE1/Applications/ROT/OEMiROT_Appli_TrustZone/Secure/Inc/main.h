/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
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
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif /* __ICCARM__  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"
#include "stm32_board.h"
#include "appli_region_defs.h"

/* Private includes ----------------------------------------------------------*/


/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL(*funcptr)(void);
#endif /* __ICCARM__  */

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

#if defined(OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE)
/* Function pointer type for the secure loader Reset_Handler. */
typedef void (*Loader_ResetHandler_t)(void);

/* Minimal vector table representation for the secure loader. */
typedef struct
{
    uint32_t              loader_msp;   /*!< Initial MSP value for the secure loader      */
    Loader_ResetHandler_t loader_reset; /*!< Reset_Handler entry point of the loader      */
} Loader_VectorTable_t;
#endif /* OEMIROT_OTFDEC_EXTERNAL_FLASH_ENABLE */


/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);


/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
