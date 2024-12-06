/**
  ******************************************************************************
  * @file    app_phy_cli.h
  * @author  MCD Application Team
  * @brief   Header for the macros of PHY CLI application
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PHY_CLI_H
#define PHY_CLI_H

/* Includes ------------------------------------------------------------------*/

#define APP_READ32_REG(base_addr) (*(volatile uint32_t *)(base_addr))
#define APP_WRITE32_REG(base_addr, data)  (*(volatile uint32_t *)(base_addr) = (data))

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Debug purpose */
#if (CFG_DEBUG_IAR_PRINT ==0)
#define PHY_VALID_DBG(...)                                        \
{                                                                 \
  UTIL_ADV_TRACE_COND_FSend(VLEVEL_H, ~0x0, 0, __VA_ARGS__);      \
}
/* Error codes */
#define PHY_CLI_ERROR(...)                                      \
{                                                               \
  UTIL_ADV_TRACE_COND_FSend(VLEVEL_L, ~0x0, 0, __VA_ARGS__);    \
}
/* Mandatory prints (e.g. RX results) */
#define PHY_CLI_PRINT(...)                                      \
{                                                               \
  app_phy_cli_print(__VA_ARGS__);                               \
}
#else
#define PHY_CLI_DBG(...)                                        \
{                                                               \
  APP_DBG(__VA_ARGS__);      \
}
/* Error codes */
#define PHY_CLI_ERROR(...)                                      \
{                                                               \
  APP_DBG(__VA_ARGS__);      \
}
/* Mandatory prints (e.g. RX results) */
#define PHY_CLI_PRINT(...)                                      \
{                                                               \
  APP_DBG(__VA_ARGS__); \
}
#endif

//CONFIGURATION
#define RESTART_RADIO_BEFORE_RX         1

/* Exported functions ------------------------------------------------------- */
void APP_PHY_CLI_Init(void);
void app_phy_cli_print(const char *aFormat, ...);

#endif /* PHY_CLI_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
