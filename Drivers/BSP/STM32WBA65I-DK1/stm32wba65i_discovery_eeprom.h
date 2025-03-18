/**
******************************************************************************
* @file    stm32wba65i_discovery_eeprom.h
* @author  SRA-SAIL
* @brief   header of STM32WBA65I-DK1 BSP based on m95p32 series eeprom
******************************************************************************
* @attention
*
* Copyright (c) 2023 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H

#ifdef __cplusplus
 extern "C" {
#endif


/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "m95p32.h"
#include "stm32wba65i_discovery_conf.h"

/* Exported macro ------------------------------------------------------------*/
#define BSP_EEPROM_INSTANCES_NBR                   2U
#define BSP_EEPROM_0                               0U

#define EEPROM_PAGESIZE                            4U
#define EEPROMEX_SLAVE_CS_PIN                      GPIO_PIN_0
#define EEPROMEX_SLAVE_CS_PIN_PORT                 GPIOC

#define EEPROMEX_SPI_SLAVE_FOUR_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOC_CLK_ENABLE()

   void EEPROMEX_CTRL_HIGH(void);
   void EEPROMEX_CTRL_LOW(void);

int32_t BSP_EEPROM_Init(uint32_t Instance);
int32_t BSP_EEPROM_WriteEnable(uint32_t Instance);
int32_t BSP_EEPROM_WriteDisable(uint32_t Instance);
int32_t BSP_EEPROM_StatusRegRead(uint32_t Instance, uint8_t * pData);
int32_t BSP_EEPROM_StatusConfigRegWrite(uint32_t Instance, uint8_t * pData, uint16_t Size);
int32_t BSP_EEPROM_WriteBuffer(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,  uint16_t Size);
int32_t BSP_EEPROM_ProgramPage(uint32_t Instance, uint8_t * pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_ReadBuffer(uint32_t Instance, uint8_t *pData, uint32_t TarAddr,  uint16_t Size);
int32_t BSP_EEPROM_FastRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_ErasePage(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseSector(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseBlock(uint32_t Instance, uint32_t TarAddr);
int32_t BSP_EEPROM_EraseChip(uint32_t Instance);
int32_t BSP_EEPROM_ReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_FastReadID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_WriteID(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_DeepPowerDown(uint32_t Instance);
int32_t BSP_EEPROM_DeepPowerDownRel(uint32_t Instance);
int32_t BSP_EEPROM_VolRegRead(uint32_t Instance, uint8_t *pData);
int32_t BSP_EEPROM_VolRegWrite(uint32_t Instance, uint8_t regVal);
int32_t BSP_EEPROM_ConfSafetyRegRead(uint32_t Instance, uint8_t *pData, uint16_t Size);
int32_t BSP_EEPROM_ClearSafetyFlag(uint32_t Instance);
int32_t BSP_EEPROM_SFDPRead(uint32_t Instance, uint8_t *pData, uint32_t TarAddr, uint16_t Size);
int32_t BSP_EEPROM_JEDECRead(uint32_t Instance, uint8_t *pData, uint16_t Size);
int32_t BSP_EEPROM_EnableReset(uint32_t Instance);
int32_t BSP_EEPROM_SoftReset(uint32_t Instance);


/**
 * @}
 */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
  }
#endif
#endif /* __BSP_EEPROM_H */

