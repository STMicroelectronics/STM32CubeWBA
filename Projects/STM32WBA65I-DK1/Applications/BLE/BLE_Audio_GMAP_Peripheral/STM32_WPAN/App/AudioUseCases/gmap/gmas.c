/**
  ******************************************************************************
  * @file    gmas.c
  * @author  MCD Application Team
  * @brief   This file contains Gaming Audio Service
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

/* Includes ------------------------------------------------------------------*/
#include "gmas.h"
#include "gmap_log.h"
#include "ble_gatt_aci.h"
#include "svc_ctl.h"
#include "gmap_config.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private defines -----------------------------------------------------------*/
#if (BLE_CFG_GMAP_UGG_ROLE == 1)
#define NUM_GMAS_UGG_CHARACTERISTIC_RECORDS (2u)
#else /* (BLE_CFG_GMAP_UGG_ROLE == 1) */
#define NUM_GMAS_UGG_CHARACTERISTIC_RECORDS (0u)
#endif /* (BLE_CFG_GMAP_UGG_ROLE == 1) */

#if (BLE_CFG_GMAP_UGT_ROLE == 1)
#define NUM_GMAS_UGT_CHARACTERISTIC_RECORDS (2u)
#else /* (BLE_CFG_GMAP_UGT_ROLE == 1) */
#define NUM_GMAS_UGT_CHARACTERISTIC_RECORDS (0u)
#endif /* (BLE_CFG_GMAP_UGT_ROLE == 1) */

#if (BLE_CFG_GMAP_BGS_ROLE == 1)
#define NUM_GMAS_BGS_CHARACTERISTIC_RECORDS (2u)
#else /* (BLE_CFG_GMAP_BGS_ROLE == 1) */
#define NUM_GMAS_BGS_CHARACTERISTIC_RECORDS (0u)
#endif /* (BLE_CFG_GMAP_BGS_ROLE == 1) */

#if (BLE_CFG_GMAP_BGR_ROLE == 1)
#define NUM_GMAS_BGR_CHARACTERISTIC_RECORDS (2u)
#else /* (BLE_CFG_GMAP_BGR_ROLE == 1) */
#define NUM_GMAS_BGR_CHARACTERISTIC_RECORDS (0u)
#endif /* (BLE_CFG_GMAP_BGR_ROLE == 1) */

/*
 * GMAP_NUM_GATT_ATTRIBUTES: number of GATT attributes required for Gaming Audio Profile
 */
#define NUM_GMAS_CHARACTERISTIC_RECORDS    (2u + NUM_GMAS_UGG_CHARACTERISTIC_RECORDS \
                                            + NUM_GMAS_UGT_CHARACTERISTIC_RECORDS \
                                            + NUM_GMAS_BGS_CHARACTERISTIC_RECORDS \
                                            + NUM_GMAS_BGR_CHARACTERISTIC_RECORDS)
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
static tBleStatus GMAS_SetCharacteristic(uint16_t ServiceHandle, uint16_t CharHandle, uint8_t *pData, uint16_t DataLen);
/* Functions Definition ------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
/**
  * @brief  Initialize the Gaming Audio Service
  */
tBleStatus GMAS_InitService(GMAS_ServiceContext_t *pSrvContext)
{
  uint16_t uuid;
  uint8_t num_char_records = NUM_GMAS_CHARACTERISTIC_RECORDS;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;

  memset(pSrvContext, 0, sizeof(GMAS_ServiceContext_t) );
  /**
  *  Add Gaming Audio Service
  */
  uuid = GAMING_AUDIO_SERVICE_UUID;
  hciCmdResult = aci_gatt_add_service(UUID_TYPE_16,
                                       (Service_UUID_t *) &uuid,
                                       PRIMARY_SERVICE,
                                       (1+num_char_records),
                                       &(pSrvContext->ServiceHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    pSrvContext->ServiceEndHandle = pSrvContext->ServiceHandle+num_char_records;
    BLE_DBG_GMAS_MSG("Gaming Audio Service (GMAS) is added Successfully %04X (Max_Attribute_Records : %d)\n",
                    pSrvContext->ServiceHandle,
                    (1+num_char_records));
  }
  else
  {
    BLE_DBG_GMAS_MSG("FAILED to add Gaming Audio Service (GMAS), Error: %02X !!\n",hciCmdResult);
  }
  return hciCmdResult;
}

tBleStatus GMAS_InitCharacteristics(GMAS_ServiceContext_t *pSrvContext,
                                    GMAP_Role_t GMAPRoles,
                                    UGGFeatures_t UGGFeatures,
                                    UGTFeatures_t UGTFeatures,
                                    BGSFeatures_t BGSFeatures,
                                    BGRFeatures_t BGRFeatures)
{
  uint16_t uuid;
  tBleStatus hciCmdResult = BLE_STATUS_SUCCESS;
  /*
  *  Add GMAP Role Characteristic
  */
  uuid = GMAP_ROLE_UUID;
  hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                  UUID_TYPE_16,
                                  (Char_UUID_t *) &uuid ,
                                  1u,
                                  (CHAR_PROP_READ),
                                  ATTR_PERMISSION_ENCRY_READ,
                                  0,
                                  10,
                                  0,
                                  &(pSrvContext->GMAPRoleHandle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_GMAS_MSG("GMAP Role Characteristic added Successfully %04X\n", pSrvContext->GMAPRoleHandle);

    hciCmdResult = GMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                         pSrvContext->GMAPRoleHandle,
                                         (uint8_t*) &GMAPRoles,
                                         1);
    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAS_MSG("GMAP Role Characteristic (%04X) Value updated Successfully\n",
                      pSrvContext->GMAPRoleHandle);
    }
    else
    {
      BLE_DBG_GMAS_MSG("FAILED to update GMAP Role Characteristic (%04X) Value, Error: %02X !!\n",
                      pSrvContext->GMAPRoleHandle,
                      hciCmdResult);
    }
  }
  else
  {
    BLE_DBG_GMAS_MSG("FAILED to add GMAP Role Characteristic, Error: %02X !!\n", hciCmdResult);
  }

#if (BLE_CFG_GMAP_UGG_ROLE == 1)
  if (GMAPRoles & GMAP_ROLE_UNICAST_GAME_GATEWAY)
  {
    /*
    *  Add UGG Features Characteristic
    */
    uuid = UGG_FEATURES_UUID;
    hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                    UUID_TYPE_16,
                                    (Char_UUID_t *) &uuid ,
                                    1u,
                                    (CHAR_PROP_READ),
                                    ATTR_PERMISSION_ENCRY_READ,
                                    0,
                                    10,
                                    0,
                                    &(pSrvContext->UGGFeaturesHandle));

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAS_MSG("UGG Features Characteristic added Successfully %04X\n", pSrvContext->UGGFeaturesHandle);

      hciCmdResult = GMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                           pSrvContext->UGGFeaturesHandle,
                                           (uint8_t*) &UGGFeatures,
                                           1);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_GMAS_MSG("UGG Features Characteristic (%04X) Value updated Successfully\n",
                        pSrvContext->UGGFeaturesHandle);
      }
      else
      {
        BLE_DBG_GMAS_MSG("FAILED to update UGG Features Characteristic (%04X) Value, Error: %02X !!\n",
                        pSrvContext->UGGFeaturesHandle,
                        hciCmdResult);
      }
    }
    else
    {
      BLE_DBG_GMAS_MSG("FAILED to add UGG Features Characteristic, Error: %02X !!\n", hciCmdResult);
    }
  }
#endif /* (BLE_CFG_GMAP_UGG_ROLE == 1) */

#if (BLE_CFG_GMAP_UGT_ROLE == 1)
  if (GMAPRoles & GMAP_ROLE_UNICAST_GAME_TERMINAL)
  {
    /*
    *  Add UGT Features Characteristic
    */
    uuid = UGT_FEATURES_UUID;
    hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                    UUID_TYPE_16,
                                    (Char_UUID_t *) &uuid ,
                                    1u,
                                    (CHAR_PROP_READ),
                                    ATTR_PERMISSION_ENCRY_READ,
                                    0,
                                    10,
                                    0,
                                    &(pSrvContext->UGTFeaturesHandle));

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAS_MSG("UGT Features Characteristic added Successfully %04X\n", pSrvContext->UGTFeaturesHandle);

      hciCmdResult = GMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                           pSrvContext->UGTFeaturesHandle,
                                           (uint8_t*) &UGTFeatures,
                                           1);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_GMAS_MSG("UGT Features Characteristic (%04X) Value updated Successfully\n",
                        pSrvContext->UGTFeaturesHandle);
      }
      else
      {
        BLE_DBG_GMAS_MSG("FAILED to update UGT Features Characteristic (%04X) Value, Error: %02X !!\n",
                        pSrvContext->UGTFeaturesHandle,
                        hciCmdResult);
      }
    }
    else
    {
      BLE_DBG_GMAS_MSG("FAILED to add UGT Features Characteristic, Error: %02X !!\n", hciCmdResult);
    }
  }
#endif /* (BLE_CFG_GMAP_UGT_ROLE == 1) */

#if (BLE_CFG_GMAP_BGS_ROLE == 1)
  if (GMAPRoles & GMAP_ROLE_BROADCAST_GAME_SENDER)
  {
    /*
    *  Add BGS Features Characteristic
    */
    uuid = BGS_FEATURES_UUID;
    hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                    UUID_TYPE_16,
                                    (Char_UUID_t *) &uuid ,
                                    1u,
                                    (CHAR_PROP_READ),
                                    ATTR_PERMISSION_ENCRY_READ,
                                    0,
                                    10,
                                    0,
                                    &(pSrvContext->BGSFeaturesHandle));

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAS_MSG("BGS Features Characteristic added Successfully %04X\n", pSrvContext->BGSFeaturesHandle);

      hciCmdResult = GMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                           pSrvContext->BGSFeaturesHandle,
                                           (uint8_t*) &BGSFeatures,
                                           1);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_GMAS_MSG("BGS Features Characteristic (%04X) Value updated Successfully\n",
                        pSrvContext->BGSFeaturesHandle);
      }
      else
      {
        BLE_DBG_GMAS_MSG("FAILED to update BGS Features Characteristic (%04X) Value, Error: %02X !!\n",
                        pSrvContext->BGSFeaturesHandle,
                        hciCmdResult);
      }
    }
    else
    {
      BLE_DBG_GMAS_MSG("FAILED to add BGS Features Characteristic, Error: %02X !!\n", hciCmdResult);
    }
  }
#endif /* (BLE_CFG_GMAP_BGS_ROLE == 1) */

#if (BLE_CFG_GMAP_BGR_ROLE == 1)
  if (GMAPRoles & GMAP_ROLE_BROADCAST_GAME_RECEIVER)
  {
    /*
    *  Add BGR Features Characteristic
    */
    uuid = BGR_FEATURES_UUID;
    hciCmdResult = aci_gatt_add_char(pSrvContext->ServiceHandle,
                                    UUID_TYPE_16,
                                    (Char_UUID_t *) &uuid ,
                                    1u,
                                    (CHAR_PROP_READ),
                                    ATTR_PERMISSION_ENCRY_READ,
                                    0,
                                    10,
                                    0,
                                    &(pSrvContext->BGRFeaturesHandle));

    if (hciCmdResult == BLE_STATUS_SUCCESS)
    {
      BLE_DBG_GMAS_MSG("BGR Features Characteristic added Successfully %04X\n", pSrvContext->BGRFeaturesHandle);

      hciCmdResult = GMAS_SetCharacteristic(pSrvContext->ServiceHandle,
                                           pSrvContext->BGRFeaturesHandle,
                                           (uint8_t*) &BGRFeatures,
                                           1);
      if (hciCmdResult == BLE_STATUS_SUCCESS)
      {
        BLE_DBG_GMAS_MSG("BGR Features Characteristic (%04X) Value updated Successfully\n",
                        pSrvContext->BGRFeaturesHandle);
      }
      else
      {
        BLE_DBG_GMAS_MSG("FAILED to update BGR Features Characteristic (%04X) Value, Error: %02X !!\n",
                        pSrvContext->BGRFeaturesHandle,
                        hciCmdResult);
      }
    }
    else
    {
      BLE_DBG_GMAS_MSG("FAILED to add BGR Features Characteristic, Error: %02X !!\n", hciCmdResult);
    }
  }
#endif /* (BLE_CFG_GMAP_BGR_ROLE == 1) */

  return hciCmdResult;
}

/* Private functions ----------------------------------------------------------*/

static tBleStatus GMAS_SetCharacteristic(uint16_t ServiceHandle, uint16_t CharHandle, uint8_t *pData,uint16_t DataLen)
{
 tBleStatus hciCmdResult;

  hciCmdResult = aci_gatt_update_char_value_ext(0x0000,
                                                ServiceHandle,
                                                CharHandle,
                                                0x01,                           /*Notification*/
                                                DataLen,                        /* Char_Length*/
                                                0,                              /* charValOffset */
                                                DataLen,                        /* charValueLen */
                                                (uint8_t *) &pData[0]);

  return hciCmdResult;
}