
/**
  ******************************************************************************
  * @file    zdd_ble_interface.h
  * @author  MCD Application Team
  * @brief   Defines and structure used for ZigbeeDirect to be shared between 
  *          the application and the ZigbeeDirect stack library.
  *
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef ZDD_BLE_INTERFACE_H
#define ZDD_BLE_INTERFACE_H

#ifdef __cplusplus
extern "C"{
#endif

/* Device information service */
typedef enum
{
  DIS_MANS,
  DIS_SYID,
  DIS_MONS,
  DIS_SNS,
  DIS_HRS,
  DIS_FRS,
  DIS_CHAROPCODE_LAST
} DIS_CharOpcode_t;

typedef enum
{
  DIS_MANS_READ_EVT,
  DIS_SYID_READ_EVT,
  DIS_MONS_READ_EVT,
  DIS_SNS_READ_EVT,
  DIS_HRS_READ_EVT,
  DIS_FRS_READ_EVT,
  DIS_BOOT_REQUEST_EVT
} DIS_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

} DIS_Data_t;

extern tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData);

/* tunneling service */
typedef enum
{
  TUNNELING_TUNNZDTSNPDU,
  TUNNELING_CHAROPCODE_LAST
} TUNNELING_CharOpcode_t;

typedef enum
{
  TUNNELING_TUNNZDTSNPDU_WRITE_EVT,
  TUNNELING_TUNNZDTSNPDU_INDICATE_ENABLED_EVT,
  TUNNELING_TUNNZDTSNPDU_INDICATE_DISABLED_EVT,
  TUNNELING_BOOT_REQUEST_EVT
} TUNNELING_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

} TUNNELING_Data_t;

extern tBleStatus TUNNELING_UpdateValue(TUNNELING_CharOpcode_t CharOpcode, TUNNELING_Data_t *pData);

/* security service */
typedef enum
{
  ZDDSECURITY_SECURITY25519AES,
  ZDDSECURITY_SECURITY25519SHA,
  ZDDSECURITY_P256SHA,
  ZDDSECURITY_CHAROPCODE_LAST
} ZDDSECURITY_CharOpcode_t;

typedef enum
{
  ZDDSECURITY_SECURITY25519AES_WRITE_EVT,
  ZDDSECURITY_SECURITY25519AES_INDICATE_ENABLED_EVT,
  ZDDSECURITY_SECURITY25519AES_INDICATE_DISABLED_EVT,
  ZDDSECURITY_SECURITY25519SHA_WRITE_EVT,
  ZDDSECURITY_SECURITY25519SHA_INDICATE_ENABLED_EVT,
  ZDDSECURITY_SECURITY25519SHA_INDICATE_DISABLED_EVT,
  ZDDSECURITY_P256SHA_WRITE_EVT,
  ZDDSECURITY_P256SHA_INDICATE_ENABLED_EVT,
  ZDDSECURITY_P256SHA_INDICATE_DISABLED_EVT,
  ZDDSECURITY_BOOT_REQUEST_EVT
} ZDDSECURITY_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

} ZDDSECURITY_Data_t;

extern tBleStatus ZDDSECURITY_UpdateValue(ZDDSECURITY_CharOpcode_t CharOpcode, ZDDSECURITY_Data_t *pData);

/* Commissioning service */
typedef enum
{
  ZIGBEEDIRECTCOMM_FORMNETWORK,
  ZIGBEEDIRECTCOMM_JOINNETWORK,
  ZIGBEEDIRECTCOMM_PERMITJOIN,
  ZIGBEEDIRECTCOMM_LEAVENETWORK,
  ZIGBEEDIRECTCOMM_COMMSTATUS,
  ZIGBEEDIRECTCOMM_MANAGEJOINER,
  ZIGBEEDIRECTCOMM_COMMIDENTITY,
  ZIGBEEDIRECTCOMM_FINDBIND,
  ZIGBEEDIRECTCOMM_CHAROPCODE_LAST
} ZIGBEEDIRECTCOMM_CharOpcode_t;

typedef enum
{
  ZIGBEEDIRECTCOMM_FORMNETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_JOINNETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_PERMITJOIN_WRITE_EVT,
  ZIGBEEDIRECTCOMM_LEAVENETWORK_WRITE_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_READ_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_ENABLED_EVT,
  ZIGBEEDIRECTCOMM_COMMSTATUS_NOTIFY_DISABLED_EVT,
  ZIGBEEDIRECTCOMM_MANAGEJOINER_WRITE_EVT,
  ZIGBEEDIRECTCOMM_COMMIDENTITY_READ_EVT,
  ZIGBEEDIRECTCOMM_COMMIDENTITY_WRITE_EVT,
  ZIGBEEDIRECTCOMM_FINDBIND_WRITE_EVT,
  ZIGBEEDIRECTCOMM_BOOT_REQUEST_EVT
} ZIGBEEDIRECTCOMM_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service2_Data_t */

  /* USER CODE END Service2_Data_t */
} ZIGBEEDIRECTCOMM_Data_t;
 
extern tBleStatus ZIGBEEDIRECTCOMM_UpdateValue(ZIGBEEDIRECTCOMM_CharOpcode_t CharOpcode, ZIGBEEDIRECTCOMM_Data_t *pData);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*ZDD_BLE_INTERFACE_H */
