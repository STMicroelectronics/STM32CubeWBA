/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file    m95p32.h
* @author  MCD Application Team
* @brief   header for m95p32 component on series eeprom
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __M95P32_H
#define __M95P32_H

#ifdef __cplusplus
extern "C" {
#endif

  /* Includes ------------------------------------------------------------------*/
#include "m95p32_conf.h"
#include <stdint.h>

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */


/* Exported types ------------------------------------------------------------*/
/**
* @brief  EEPROM structure definition
*/

/* Exported constants --------------------------------------------------------*/
/** @defgroup M95_Exported_Constants
  * @{
  */

#define MSK_BYTE3                       0xFF0000U
#define MSK_BYTE2                       0xFF00U
#define MSK_BYTE1                       0xFFU
#define SHIFT_16BIT                     16U
#define SHIFT_8BIT                      8U
#define INSTRUCTION_LEN_1_BYTE          1U
#define INSTRUCTION_LEN_2_BYTE          2U
#define INSTRUCTION_LEN_4_BYTE          4U
#define INSTRUCTION_LEN_5_BYTE          5U
#define DUMMY_DATA                      0xFFU
#define M95P32_PAGESIZE                 512U


/* Size of buffers */
#define SIZE610                         610U
#define SIZE512                         512U
/* Exported macro ------------------------------------------------------------*/

#define CMD_ERASE_SECTOR			0x20
#define CMD_ERASE_BLOCK				0xD8
#define CMD_ERASE_CHIP				0XC7
#define CMD_ERASE_PAGE				0xDB
#define CMD_PROG_PAGE				0x0A
#define CMD_WRITE_PAGE				0x02
#define CMD_WREN				0x06
#define CMD_WRDI				0x04
#define	CMD_READ_STATUS_REG			0x05
#define CMD_READ_CONF_SAFETY_REG        	0x15
#define CMD_CLEAR_SAFETY_REG		        0x50
#define CMD_READ_VOLATILE_REG		        0x85
#define CMD_WRITE_VOLATILE_REG		        0x81
#define	CMD_WRITE_STATUS_CONF_REG	        0x01
#define CMD_READ_DATA				0x03
#define CMD_FAST_READ_SINGLE    		0x0B
#define CMD_FAST_READ_DUAL			0x3B
#define CMD_FAST_READ_QUAD			0x6B
#define CMD_READ_ID_PAGE			0x83
#define CMD_FAST_READ_ID_PAGE	        	0x8B
#define CMD_WRITE_ID_PAGE			0x82
#define CMD_DEEP_POWER_DOWN			0xB9
#define CMD_DEEP_POWER_DOWN_RELEASE	        0xAB
#define CMD_READ_JEDEC				0x9F
#define CMD_READ_SFDP				0x5A
#define CMD_ENABLE_RESET			0x66
#define CMD_SOFT_RESET				0x99

/* USER CODE BEGIN EM */
#define SPI_DATA_TIMEOUT                        0xffff

/* USER CODE END EM */

#define SET_BUFEN_BIT                           0x02
#define SET_BUFLD_BIT                           0x01

#define M95_OK                                  0
#define M95_ERROR   	                        -1
#define M95_BUSY    	                        -2
#define M95_TIMEOUT 	                        -3
#define M95_ADDR_OVERFLOW                       -4
#define M95P32_QPSI_TIMEOUT_DEFAULT_VALUE       5000U


typedef int32_t     (*M95_Init_Func)(void);
typedef int32_t     (*M95_DeInit_Func)(void);
typedef int32_t     (*M95_Read_Func) (uint8_t* , uint16_t);
typedef int32_t     (*M95_Write_Func)(uint8_t*, uint16_t);
typedef int32_t     (*M95_SendRecv_Func)( uint8_t *, uint8_t *, uint16_t );
typedef void        (*M95_Delay)(uint32_t );

typedef struct
{
  M95_Init_Func                 Init;
  M95_DeInit_Func               DeInit;
  M95_Read_Func		        Read;
  M95_Write_Func                Write;
  M95_SendRecv_Func             SendRecv;
  M95_Delay                     Delay;
  uint8_t                       Address;
} M95_IO_t;

typedef struct
{
  M95_IO_t        IO;
} M95_Object_t;

#ifdef  USE_SPI
   int32_t Transmit_Data_polling(M95_Object_t *pObj);
#endif

/**
  * @brief  EEPROMEX COMMON driver structure definition
  */
typedef struct
{
  int32_t       (*Init)(M95_Object_t *);
  int32_t       (*WriteEnable)(M95_Object_t *);
  int32_t       (*WriteDisable)(M95_Object_t *);
  int32_t       (*StatusRegRead)(M95_Object_t *, uint8_t *);
  int32_t       (*ReadPage)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastRead)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastDRead)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastQRead)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*WritePage)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*ProgramPage)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*ErasePage)(M95_Object_t *, uint32_t );
  int32_t       (*EraseSector)(M95_Object_t *, uint32_t );
  int32_t       (*EraseBlock)(M95_Object_t *, uint32_t );
  int32_t       (*EraseChip)(M95_Object_t * );
  int32_t       (*ReadID)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastReadID)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*VolRegRead)(M95_Object_t *, uint8_t * );
  int32_t       (*VolRegWrite)(M95_Object_t *, uint8_t );
  int32_t       (*ConfSafetyRegRead)(M95_Object_t * , uint8_t *, uint32_t );
  int32_t       (*StatusConfigRegWrite)(M95_Object_t * , uint8_t *, uint32_t );
  int32_t       (*ClearSafetyFlag)(M95_Object_t *);
  int32_t       (*SFDPRegRead)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*WriteID)(M95_Object_t * , uint8_t *, uint32_t , uint32_t );
  int32_t       (*DeepPowerDown)(M95_Object_t *);
  int32_t       (*DeepPowerDownRel)(M95_Object_t *);
  int32_t       (*JEDECRead)(M95_Object_t * , uint8_t *, uint32_t );
  int32_t       (*EnableReset)(M95_Object_t *);
  int32_t       (*SoftReset)(M95_Object_t *);

} M95P32_CommonDrv_t;



/**
  * @brief  M95 driver structure definition
  */
typedef struct
{
  int32_t       (*Init)(M95_Object_t *);
  int32_t       (*WriteEnable)(M95_Object_t *);
  int32_t       (*WriteDisable)(M95_Object_t *);
  int32_t       (*StatusRegRead)(M95_Object_t *, uint8_t *);
  int32_t       (*ReadPage)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastRead)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastDRead)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastQRead)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*WritePage)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*ProgramPage)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*ErasePage)(M95_Object_t *, uint32_t );
  int32_t       (*EraseSector)(M95_Object_t *, uint32_t );
  int32_t       (*EraseBlock)(M95_Object_t *, uint32_t );
  int32_t       (*EraseChip)(M95_Object_t * );
  int32_t       (*ReadID)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*FastReadID)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*VolRegRead)(M95_Object_t *, uint8_t * );
  int32_t       (*VolRegWrite)(M95_Object_t *, uint8_t );
  int32_t       (*ConfSafetyRegRead)(M95_Object_t *, uint8_t *, uint32_t );
  int32_t       (*StatusConfigRegWrite)(M95_Object_t *, uint8_t *, uint32_t );
  int32_t       (*ClearSafetyFlag)(M95_Object_t *);
  int32_t       (*SFDPRegRead)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*WriteID)(M95_Object_t *, uint8_t *, uint32_t , uint32_t );
  int32_t       (*DeepPowerDown)(M95_Object_t *);
  int32_t       (*DeepPowerDownRel)(M95_Object_t *);
  int32_t       (*JEDECRead)(M95_Object_t *, uint8_t *, uint32_t );
  int32_t       (*EnableReset)(M95_Object_t *);
  int32_t       (*SoftReset)(M95_Object_t *);

} M95P32_Drv_t;

extern M95P32_Drv_t M95P32_spi_Drv;

int32_t M95P32_spi_Init( M95_Object_t *pObj );
int32_t M95P32_spi_DeInit( M95_Object_t *pObj );
int32_t M95P32_RegisterBusIO(M95_Object_t *pObj, M95_IO_t *pIO);
int32_t WRITE_ENABLE(M95_Object_t *pObj);
int32_t WRITE_DISABLE(M95_Object_t *pObj);
int32_t Read_StatusReg(M95_Object_t *pObj, uint8_t *pData);
int32_t Write_StatusConfigReg(M95_Object_t *pObj, uint8_t *pData, uint32_t Size);
int32_t Single_Read(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t FAST_Read(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr,uint32_t Size);
int32_t FAST_DRead(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t FAST_QRead(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Page_Write(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Page_Prog(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Page_Erase(M95_Object_t *pObj, uint32_t Add);
int32_t Sector_Erase(M95_Object_t *pObj, uint32_t Add);
int32_t Block_Erase(M95_Object_t *pObj, uint32_t Add);
int32_t Chip_Erase(M95_Object_t *pObj);
int32_t Read_ID(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t FAST_Read_ID(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Write_ID(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Deep_Power_Down(M95_Object_t *pObj);
int32_t Deep_Power_Down_Release(M95_Object_t *pObj);
int32_t Read_JEDEC(M95_Object_t *pObj, uint8_t *pData, uint32_t Size);
int32_t ReadConfigReg(M95_Object_t *pObj, uint8_t *pData, uint32_t Size);
int32_t ReadVolatileReg(M95_Object_t *pObj, uint8_t *pData);
int32_t WriteVolatileRegister(M95_Object_t *pObj, uint8_t regVal);
int32_t ClearSafetyFlag(M95_Object_t *pObj);
int32_t Read_SFDP(M95_Object_t *pObj, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
int32_t Reset_Enable(M95_Object_t *pObj);
int32_t Soft_Reset(M95_Object_t *pObj);


/** @addtogroup M95xx_Private_Functions
  * @{
  */
#ifdef USE_QUADSPI
  HAL_StatusTypeDef QSPI_WriteEnable(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_WriteDisable(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_ReadSatusReg(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData);
  HAL_StatusTypeDef QSPI_WriteStatusConfigRegister(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t nb_data);
  HAL_StatusTypeDef QSPI_Read(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
  HAL_StatusTypeDef QSPI_FAST_Read(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
  HAL_StatusTypeDef QSPI_DRead(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
  HAL_StatusTypeDef QSPI_QRead(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
  HAL_StatusTypeDef QSPI_Write(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData, uint32_t TarAddr, uint32_t Size);
  HAL_StatusTypeDef QSPI_Prog(QSPI_HandleTypeDef *local_hqspi,uint32_t add,uint32_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_PageErase(QSPI_HandleTypeDef *local_hqspi,uint32_t Add);
  HAL_StatusTypeDef QSPI_SectorErase(QSPI_HandleTypeDef *local_hqspi,uint32_t Add);
  HAL_StatusTypeDef QSPI_BlockErase(QSPI_HandleTypeDef *local_hqspi,uint32_t Add);
  HAL_StatusTypeDef QSPI_ChipErase(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_Read_ID(QSPI_HandleTypeDef *local_hqspi,uint32_t add,uint32_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_FAST_Read_ID(QSPI_HandleTypeDef *local_hqspi,uint32_t add,uint32_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_Write_ID(QSPI_HandleTypeDef *local_hqspi,uint32_t add,uint32_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_Deep_Power_Down(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_Deep_Power_Down_Release(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_Read_JEDEC(QSPI_HandleTypeDef *local_hqspi,uint8_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_ReadConfigReg(QSPI_HandleTypeDef *local_hqspi,uint8_t nb_reg,uint8_t *pData);
  HAL_StatusTypeDef QSPI_ReadVolatileReg(QSPI_HandleTypeDef *local_hqspi, uint8_t *pData);
  HAL_StatusTypeDef QSPI_WriteVolatileRegister(QSPI_HandleTypeDef *local_hqspi,uint8_t regVal);
  HAL_StatusTypeDef QSPI_ClearSafetyFlag(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_Read_SFDP(QSPI_HandleTypeDef *local_hqspi,uint32_t add,uint32_t nb_data,uint8_t *pData);
  HAL_StatusTypeDef QSPI_Reset_Enable(QSPI_HandleTypeDef *local_hqspi);
  HAL_StatusTypeDef QSPI_Soft_Reset(QSPI_HandleTypeDef *local_hqspi);
#endif

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
#endif /* __M95P32_H */
