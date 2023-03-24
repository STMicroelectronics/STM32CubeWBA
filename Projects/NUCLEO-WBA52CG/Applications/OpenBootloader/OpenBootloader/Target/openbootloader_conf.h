/**
  ******************************************************************************
  * @file    openbootloader_conf.h
  * @author  MCD Application Team
  * @brief   Contains Open Bootloader configuration
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
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef OPENBOOTLOADER_CONF_H
#define OPENBOOTLOADER_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/*--------------------------------- Device ID --------------------------------*/
#define DEVICE_ID_MSB                     0x04U  /* MSB byte of device ID */
#define DEVICE_ID_LSB                     0x92U  /* LSB byte of device ID */

/*--------------------------- Definitions for Memories -----------------------*/
#define FLASH_MEM_SIZE                    (1024U * 1024U)   /* Size of FLASH 1024 kByte */
#define FLASH_START_ADDRESS               0x08000000        /* Start of Flash */
#define FLASH_END_ADDRESS                 (FLASH_START_ADDRESS + FLASH_MEM_SIZE)  /* End of Flash */

#define RAM_SIZE                          (128U * 1024U)    /* Size of RAM 128 kByte */
#define RAM_START_ADDRESS                 0x20000000        /* Start of SRAM */
#define RAM_END_ADDRESS                   (RAM_START_ADDRESS + RAM_SIZE)  /* End of SRAM */

#define ICP_SIZE                          (32U * 1024U)     /* Size of ICP 32 kByte */
#define ICP_START_ADDRESS                 0x0BF88000U       /* System memory start address */
#define ICP_END_ADDRESS                   (ICP_START_ADDRESS + ICP_SIZE)  /* System memory end address */

#define OB1_SIZE                          0x40U             /* Size of OB 64 Byte */
#define OB1_START_ADDRESS                 0x40022040U       /* Option bytes start address */
#define OB1_END_ADDRESS                   (OB1_START_ADDRESS + OB1_SIZE)  /* Option bytes end address */

#define OTP_SIZE                          512U              /* Size of OTP 512 Byte */
#define OTP_START_ADDRESS                 0x0BF90000U       /* OTP start address */
#define OTP_END_ADDRESS                   (OTP_START_ADDRESS + OTP_SIZE)  /* OTP end address */

#define EB_SIZE                           15104U            /* Size of Engi bytes 15104 Byte */
#define EB_START_ADDRESS                  0x0BF90500U       /* Engi bytes start address */
#define EB_END_ADDRESS                    (EB_START_ADDRESS + EB_SIZE)  /* Engi bytes end address */

#define OPENBL_RAM_SIZE                   (7U * 1024U)        /* RAM used by the Open Bootloader 7 kBytes */

#define OPENBL_DEFAULT_MEM                FLASH_START_ADDRESS /* Used for Erase, Write protect and read protect CMDs */

#define RDP_LEVEL_0                       OB_RDP_LEVEL_0
#define RDP_LEVEL_1                       OB_RDP_LEVEL_1
#define RDP_LEVEL_2                       OB_RDP_LEVEL_2

#define AREA_ERROR                        0x0U  /* Error Address Area */
#define FLASH_AREA                        0x1U  /* Flash Address Area */
#define RAM_AREA                          0x2U  /* RAM Address area */
#define OB_AREA                           0x3U  /* Option bytes Address area */
#define OTP_AREA                          0x4U  /* OTP Address area */
#define ICP_AREA                          0x5U  /* System memory area */
#define EB_AREA                           0x6U  /* Engi bytes Address area */

#define FLASH_MASS_ERASE                  0xFFFFU
#define FLASH_BANK1_ERASE                 0xFFFEU
#define FLASH_BANK2_ERASE                 0xFFFDU

#define INTERFACES_SUPPORTED              4U

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPENBOOTLOADER_CONF_H */
