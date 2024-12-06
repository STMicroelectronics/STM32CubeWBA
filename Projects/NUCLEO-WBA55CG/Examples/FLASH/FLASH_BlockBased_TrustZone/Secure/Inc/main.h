/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    Secure/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for secure main.c module
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined ( __ICCARM__ )
#  define CMSE_NS_CALL  __cmse_nonsecure_call
#  define CMSE_NS_ENTRY __cmse_nonsecure_entry
#else
#  define CMSE_NS_CALL  __attribute((cmse_nonsecure_call))
#  define CMSE_NS_ENTRY __attribute((cmse_nonsecure_entry))
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* Function pointer declaration in non-secure*/
#if defined ( __ICCARM__ )
typedef void (CMSE_NS_CALL *funcptr)(void);
#else
typedef void CMSE_NS_CALL (*funcptr)(void);
#endif

/* typedef for non-secure callback functions */
typedef funcptr funcptr_NS;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void SystemPower_Config(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* Base address of the Flash pages */
#define ADDR_FLASH_PAGE_0     ((uint32_t)0x0C000000) /* Base @ of Page 0, 8 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x0C002000) /* Base @ of Page 1, 8 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x0C004000) /* Base @ of Page 2, 8 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x0C006000) /* Base @ of Page 3, 8 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x0C008000) /* Base @ of Page 4, 8 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x0C00A000) /* Base @ of Page 5, 8 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x0C00C000) /* Base @ of Page 6, 8 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x0C00E000) /* Base @ of Page 7, 8 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x0C010000) /* Base @ of Page 8, 8 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x0C012000) /* Base @ of Page 9, 8 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x0C014000) /* Base @ of Page 10, 8 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x0C016000) /* Base @ of Page 11, 8 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x0C018000) /* Base @ of Page 12, 8 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x0C01A000) /* Base @ of Page 13, 8 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x0C01C000) /* Base @ of Page 14, 8 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x0C01E000) /* Base @ of Page 15, 8 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x0C020000) /* Base @ of Page 16, 8 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x0C022000) /* Base @ of Page 17, 8 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x0C024000) /* Base @ of Page 18, 8 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x0C026000) /* Base @ of Page 19, 8 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0C028000) /* Base @ of Page 20, 8 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0C02A000) /* Base @ of Page 21, 8 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0C02C000) /* Base @ of Page 22, 8 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0C02E000) /* Base @ of Page 23, 8 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0C030000) /* Base @ of Page 24, 8 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0C032000) /* Base @ of Page 25, 8 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0C034000) /* Base @ of Page 26, 8 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0C036000) /* Base @ of Page 27, 8 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0C038000) /* Base @ of Page 28, 8 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0C03A000) /* Base @ of Page 29, 8 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0C03C000) /* Base @ of Page 30, 8 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0C03E000) /* Base @ of Page 31, 8 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x0C040000) /* Base @ of Page 32, 8 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x0C042000) /* Base @ of Page 33, 8 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x0C044000) /* Base @ of Page 34, 8 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x0C046000) /* Base @ of Page 35, 8 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x0C048000) /* Base @ of Page 36, 8 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x0C04A000) /* Base @ of Page 37, 8 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x0C04C000) /* Base @ of Page 38, 8 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x0C04E000) /* Base @ of Page 39, 8 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x0C050000) /* Base @ of Page 40, 8 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x0C052000) /* Base @ of Page 41, 8 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x0C054000) /* Base @ of Page 42, 8 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x0C056000) /* Base @ of Page 43, 8 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x0C058000) /* Base @ of Page 44, 8 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x0C05A000) /* Base @ of Page 45, 8 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x0C05C000) /* Base @ of Page 46, 8 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x0C05E000) /* Base @ of Page 47, 8 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x0C060000) /* Base @ of Page 48, 8 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x0C062000) /* Base @ of Page 49, 8 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x0C064000) /* Base @ of Page 50, 8 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x0C066000) /* Base @ of Page 51, 8 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0C068000) /* Base @ of Page 52, 8 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0C06A000) /* Base @ of Page 53, 8 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0C06C000) /* Base @ of Page 54, 8 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0C06E000) /* Base @ of Page 55, 8 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0C070000) /* Base @ of Page 56, 8 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0C072000) /* Base @ of Page 57, 8 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0C074000) /* Base @ of Page 58, 8 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0C076000) /* Base @ of Page 59, 8 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0C078000) /* Base @ of Page 60, 8 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0C07A000) /* Base @ of Page 61, 8 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0C07C000) /* Base @ of Page 62, 8 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0C07E000) /* Base @ of Page 63, 8 Kbytes */
#define ADDR_FLASH_PAGE_64    ((uint32_t)0x0C080000) /* Base @ of Page 64, 8 Kbytes */
#define ADDR_FLASH_PAGE_65    ((uint32_t)0x0C082000) /* Base @ of Page 65, 8 Kbytes */
#define ADDR_FLASH_PAGE_66    ((uint32_t)0x0C084000) /* Base @ of Page 66, 8 Kbytes */
#define ADDR_FLASH_PAGE_67    ((uint32_t)0x0C086000) /* Base @ of Page 67, 8 Kbytes */
#define ADDR_FLASH_PAGE_68    ((uint32_t)0x0C088000) /* Base @ of Page 68, 8 Kbytes */
#define ADDR_FLASH_PAGE_69    ((uint32_t)0x0C08A000) /* Base @ of Page 69, 8 Kbytes */
#define ADDR_FLASH_PAGE_70    ((uint32_t)0x0C08C000) /* Base @ of Page 70, 8 Kbytes */
#define ADDR_FLASH_PAGE_71    ((uint32_t)0x0C08E000) /* Base @ of Page 71, 8 Kbytes */
#define ADDR_FLASH_PAGE_72    ((uint32_t)0x0C090000) /* Base @ of Page 72, 8 Kbytes */
#define ADDR_FLASH_PAGE_73    ((uint32_t)0x0C092000) /* Base @ of Page 73, 8 Kbytes */
#define ADDR_FLASH_PAGE_74    ((uint32_t)0x0C094000) /* Base @ of Page 74, 8 Kbytes */
#define ADDR_FLASH_PAGE_75    ((uint32_t)0x0C096000) /* Base @ of Page 75, 8 Kbytes */
#define ADDR_FLASH_PAGE_76    ((uint32_t)0x0C098000) /* Base @ of Page 76, 8 Kbytes */
#define ADDR_FLASH_PAGE_77    ((uint32_t)0x0C09A000) /* Base @ of Page 77, 8 Kbytes */
#define ADDR_FLASH_PAGE_78    ((uint32_t)0x0C09C000) /* Base @ of Page 78, 8 Kbytes */
#define ADDR_FLASH_PAGE_79    ((uint32_t)0x0C09E000) /* Base @ of Page 79, 8 Kbytes */
#define ADDR_FLASH_PAGE_80    ((uint32_t)0x0C0A0000) /* Base @ of Page 80, 8 Kbytes */
#define ADDR_FLASH_PAGE_81    ((uint32_t)0x0C0A2000) /* Base @ of Page 81, 8 Kbytes */
#define ADDR_FLASH_PAGE_82    ((uint32_t)0x0C0A4000) /* Base @ of Page 82, 8 Kbytes */
#define ADDR_FLASH_PAGE_83    ((uint32_t)0x0C0A6000) /* Base @ of Page 83, 8 Kbytes */
#define ADDR_FLASH_PAGE_84    ((uint32_t)0x0C0A8000) /* Base @ of Page 84, 8 Kbytes */
#define ADDR_FLASH_PAGE_85    ((uint32_t)0x0C0AA000) /* Base @ of Page 85, 8 Kbytes */
#define ADDR_FLASH_PAGE_86    ((uint32_t)0x0C0AC000) /* Base @ of Page 86, 8 Kbytes */
#define ADDR_FLASH_PAGE_87    ((uint32_t)0x0C0AE000) /* Base @ of Page 87, 8 Kbytes */
#define ADDR_FLASH_PAGE_88    ((uint32_t)0x0C0B0000) /* Base @ of Page 88, 8 Kbytes */
#define ADDR_FLASH_PAGE_89    ((uint32_t)0x0C0B2000) /* Base @ of Page 89, 8 Kbytes */
#define ADDR_FLASH_PAGE_90    ((uint32_t)0x0C0B4000) /* Base @ of Page 90, 8 Kbytes */
#define ADDR_FLASH_PAGE_91    ((uint32_t)0x0C0B6000) /* Base @ of Page 91, 8 Kbytes */
#define ADDR_FLASH_PAGE_92    ((uint32_t)0x0C0B8000) /* Base @ of Page 92, 8 Kbytes */
#define ADDR_FLASH_PAGE_93    ((uint32_t)0x0C0BA000) /* Base @ of Page 93, 8 Kbytes */
#define ADDR_FLASH_PAGE_94    ((uint32_t)0x0C0BC000) /* Base @ of Page 94, 8 Kbytes */
#define ADDR_FLASH_PAGE_95    ((uint32_t)0x0C0BE000) /* Base @ of Page 95, 8 Kbytes */
#define ADDR_FLASH_PAGE_96    ((uint32_t)0x0C0C0000) /* Base @ of Page 96, 8 Kbytes */
#define ADDR_FLASH_PAGE_97    ((uint32_t)0x0C0C2000) /* Base @ of Page 97, 8 Kbytes */
#define ADDR_FLASH_PAGE_98    ((uint32_t)0x0C0C4000) /* Base @ of Page 98, 8 Kbytes */
#define ADDR_FLASH_PAGE_99    ((uint32_t)0x0C0C6000) /* Base @ of Page 99, 8 Kbytes */
#define ADDR_FLASH_PAGE_100   ((uint32_t)0x0C0C8000) /* Base @ of Page 100, 8 Kbytes */
#define ADDR_FLASH_PAGE_101   ((uint32_t)0x0C0CA000) /* Base @ of Page 101, 8 Kbytes */
#define ADDR_FLASH_PAGE_102   ((uint32_t)0x0C0CC000) /* Base @ of Page 102, 8 Kbytes */
#define ADDR_FLASH_PAGE_103   ((uint32_t)0x0C0CE000) /* Base @ of Page 103, 8 Kbytes */
#define ADDR_FLASH_PAGE_104   ((uint32_t)0x0C0D0000) /* Base @ of Page 104, 8 Kbytes */
#define ADDR_FLASH_PAGE_105   ((uint32_t)0x0C0D2000) /* Base @ of Page 105, 8 Kbytes */
#define ADDR_FLASH_PAGE_106   ((uint32_t)0x0C0D4000) /* Base @ of Page 106, 8 Kbytes */
#define ADDR_FLASH_PAGE_107   ((uint32_t)0x0C0D6000) /* Base @ of Page 107, 8 Kbytes */
#define ADDR_FLASH_PAGE_108   ((uint32_t)0x0C0D8000) /* Base @ of Page 108, 8 Kbytes */
#define ADDR_FLASH_PAGE_109   ((uint32_t)0x0C0DA000) /* Base @ of Page 109, 8 Kbytes */
#define ADDR_FLASH_PAGE_110   ((uint32_t)0x0C0DC000) /* Base @ of Page 110, 8 Kbytes */
#define ADDR_FLASH_PAGE_111   ((uint32_t)0x0C0DE000) /* Base @ of Page 111, 8 Kbytes */
#define ADDR_FLASH_PAGE_112   ((uint32_t)0x0C0E0000) /* Base @ of Page 112, 8 Kbytes */
#define ADDR_FLASH_PAGE_113   ((uint32_t)0x0C0E2000) /* Base @ of Page 113, 8 Kbytes */
#define ADDR_FLASH_PAGE_114   ((uint32_t)0x0C0E4000) /* Base @ of Page 114, 8 Kbytes */
#define ADDR_FLASH_PAGE_115   ((uint32_t)0x0C0E6000) /* Base @ of Page 115, 8 Kbytes */
#define ADDR_FLASH_PAGE_116   ((uint32_t)0x0C0E8000) /* Base @ of Page 116, 8 Kbytes */
#define ADDR_FLASH_PAGE_117   ((uint32_t)0x0C0EA000) /* Base @ of Page 117, 8 Kbytes */
#define ADDR_FLASH_PAGE_118   ((uint32_t)0x0C0EC000) /* Base @ of Page 118, 8 Kbytes */
#define ADDR_FLASH_PAGE_119   ((uint32_t)0x0C0EE000) /* Base @ of Page 119, 8 Kbytes */
#define ADDR_FLASH_PAGE_120   ((uint32_t)0x0C0F0000) /* Base @ of Page 120, 8 Kbytes */
#define ADDR_FLASH_PAGE_121   ((uint32_t)0x0C0F2000) /* Base @ of Page 121, 8 Kbytes */
#define ADDR_FLASH_PAGE_122   ((uint32_t)0x0C0F4000) /* Base @ of Page 122, 8 Kbytes */
#define ADDR_FLASH_PAGE_123   ((uint32_t)0x0C0F6000) /* Base @ of Page 123, 8 Kbytes */
#define ADDR_FLASH_PAGE_124   ((uint32_t)0x0C0F8000) /* Base @ of Page 124, 8 Kbytes */
#define ADDR_FLASH_PAGE_125   ((uint32_t)0x0C0FA000) /* Base @ of Page 125, 8 Kbytes */
#define ADDR_FLASH_PAGE_126   ((uint32_t)0x0C0FC000) /* Base @ of Page 126, 8 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0C0FE000) /* Base @ of Page 127, 8 Kbytes */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
