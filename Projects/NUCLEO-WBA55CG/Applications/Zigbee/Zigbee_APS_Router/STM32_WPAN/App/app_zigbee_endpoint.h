/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_zigbee_endpoint.h
  * Description        : Header for Zigbee Application and it endpoint.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#ifndef APP_ZIGBEE_ENDPOINT_H
#define APP_ZIGBEE_ENDPOINT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "zigbee.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ------------------------------------------------------------*/
#define CLUSTER_NB_MAX                          6u          /* Maximum number of Clusters in this application */

/* USER CODE BEGIN ED */
/* Define Led by their use and not their color */
#ifdef CFG_BSP_ON_NUCLEO
#define   LED_JOIN                              LED_BLUE    /* Led used during Join */
#define   LED_WORK                              LED_RED     /* Led indicate that application work */
#define   LED_OK                                LED_GREEN   /* Led indicate that application is OK */
#endif /* CFG_BSP_ON_NUCLEO */
#ifdef CFG_BSP_ON_DISCOVERY
#ifdef STM32WBA65xx
#define   LED_JOIN                              LED_GREEN   /* Led used during Join */
#define   LED_WORK                              LED_RED     /* Led indicate that application work */
#define   LED_OK                                LED_RED     /* Led indicate that application is OK */  
#else /* STM32WBA65xx */
#define   LED_JOIN                              LED_BLUE    /* Led used during Join */
#define   LED_WORK                              LED_BLUE    /* Led indicate that application work */
#define   LED_OK                                LED_BLUE    /* Led indicate that application is OK */
#endif /* STM32WBA65xx */
#endif /* CFG_BSP_ON_DISCOVERY */
#ifdef CFG_BSP_ON_CEB
#define   LED_JOIN                              LED_BLUE    /* Led used during Join */
#define   LED_WORK                              LED_BLUE    /* Led indicate that application work */
#define   LED_OK                                LED_BLUE    /* Led indicate that application is OK */
#endif /* CFG_BSP_ON_CEB */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
struct ApsInfoT
{
  struct ZbApsFilterT   *pstApsFilter;
  uint32_t              lAsduHandle;
};


/* USER CODE END ET */

/* Exported constants ------------------------------------------------------- */
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables ------------------------------------------------------- */
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes -------------------------------- */
extern void       APP_ZIGBEE_ApplicationInit              ( void );
extern void       APP_ZIGBEE_ApplicationStart             ( void );
extern void       APP_ZIGBEE_PersistenceStartup           ( void );
extern void       APP_ZIGBEE_ConfigEndpoints              ( void );
extern bool       APP_ZIGBEE_ConfigGroupAddr              ( void );

extern void       APP_ZIGBEE_GetStartupConfig             ( struct ZbStartupT * pstConfig );
extern void       APP_ZIGBEE_SetNewDevice                 ( uint16_t iShortAddress, uint64_t dlExtendedAddress, uint8_t cCapability );

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_ZIGBEE_ENDPOINT_H */
