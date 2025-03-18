/**
  ******************************************************************************
  * @file    app_menu_cfg.h
  * @author  MCD Application Team
  * @brief   Header for Menu Configuration file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_MENU_CFG_H
#define APP_MENU_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "hap_app.h"
#include "stm_list.h"


/* Defines ------------------------------------------------------------------ */

/* Exported Types ------------------------------------------------------------ */

/* Shared variables --------------------------------------------------------- */

/* Exported Prototypes -------------------------------------------------------*/
/**
 * @brief Initialize and setup the menu
 */
void Menu_Config(void);

/**
 * @brief Set the connecting page as active
 */
void Menu_SetConnectingPage(uint16_t ConnHandle);

/**
 * @brief Set the Linkup Page as active
 */
void Menu_SetLinkupPage(uint16_t ConnHandle);

/**
 * @brief Set the NoStream Page as active
 */
void Menu_SetNoStreamPage(void);

/**
 * @brief Set the Streaming page as active
 * @param pSamplerateText: Samplerate string of the stream
 * @param AudioRole: Audio Role of the stream
 */
void Menu_SetStreamingPage(char* pSamplerateText, Audio_Role_t AudioRole);

/**
 * @brief Set the WaitConnection Page as active
 */
void Menu_SetScanningPage(void);

/**
 * @brief Set the Config Page as active
 */
void Menu_SetConfigPage(void);

/**
 * @brief Set the Startup Page as active
 */
void Menu_SetStartupPage(void);

/**
 * @brief Add a scanned unicast server to the device list
 * @param pAddress: A pointer to the address of the device
 * @param AddressType: The type of address
 * @param pDeviceName: A pointer to the name of the device
 */
void Menu_AddUnicastServer(uint8_t *pAddress, uint8_t AddressType, char *pDeviceName);

/**
 * @brief Set Remote volume value
 */
void Menu_SetRemoteVolume(uint8_t Volume);

/**
 * @brief Set Local volume value
 */
void Menu_SetLocalVolume(uint8_t Volume);

/**
 * @brief Set Current Remote Microphone Mute Value
 */
void Menu_SetRemoteMicMute(uint8_t Mute);

/**
 * @brief Set Call State
 */
void Menu_SetCallState(char *pCallState);

/**
 * @brief Set Preset ID
 */
void Menu_SetPresetID(uint8_t ID);

/**
 * @brief Set Preset Name
 */
void Menu_SetPresetName(char *pPresetName);

/**
 * @brief Set Profiles Linked Up
 */
void Menu_SetProfilesLinked(audio_profile_t Profiles);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_MENU_CFG_H */
