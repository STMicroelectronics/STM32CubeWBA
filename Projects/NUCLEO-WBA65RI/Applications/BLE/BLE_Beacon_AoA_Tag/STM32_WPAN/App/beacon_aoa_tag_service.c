/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    beacon_aoa_tag_service.c
  * @author  GPM WBL Application Team
  * @brief   Bluetooth bracon aoa tag application
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

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "app_ble.h"
#include "cmsis_compiler.h"
#include "ble_core.h"

#include "log_module.h"
//#include "ble.h"
#include "ble_const.h"
#include "beacon_aoa_tag.h"
#include "beacon_aoa_tag_service.h"
#include "app_conf.h"

#include "system_stm32wbaxx.h"
#include "stm32_timer.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef ------------------------------------------------------------*/

typedef struct
{
    uint16_t AdvertisingInterval;/*!< Specifies the desired advertising interval. */
    uint8_t * UuID;              /*!< Specifies the 16-byte UUID to which the beacon belongs. */
    uint8_t * MajorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t * MinorID;           /*!< Specifies the 2-byte beacon identifying information. */
    uint8_t CalibratedTxPower;   /*!< Specifies the power at 1m. */
} Beacon_AoA_Tag_InitTypeDef;
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t enable;
  Adv_Set_t adv_set;  
  uint8_t* data;
  uint16_t data_len;
  uint8_t sid;
  uint16_t property;
  uint16_t interval_min;
  uint16_t interval_max;
  int8_t tx_power;
  uint8_t adv_channels;
  uint8_t address_type;
  uint8_t peer_address_type;
  uint8_t* p_peer_address;
  char username[30];
} Adv_Set_Param_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* Advertising interval for legacy advertising (0.625 ms units) 
  For Beacon AoA Tag this should be set to 100 ms. */
#define LEGACY_ADV_INTERVAL     160  /* 100 ms */
/* Advertising interval for extended advertising (0.625 ms units) */

/* If periodic advertising is used. Reduce advertising rate to save power. */
#define EXT_ADV_INTERVAL        1600 /* 1 s */

/* Advertising interval for periodic advertising (1.25 ms units) */
#define PERIODIC_ADV_INTERVAL   240 /* 300 ms*/

/* PHY used in extended advertising events. One between: HCI_TX_PHY_LE_1M,
  HCI_TX_PHY_LE_2M and HCI_TX_PHY_LE_CODED (HCI_TX_PHY_LE_CODED not possible for direction finding).  */
#define EXT_ADV_PHY HCI_TX_PHY_LE_1M


/* Parameters for Direction Finding */
#define CTE_LENGTH                    20
#define CTE_TYPE                    0x00   /* 0: AoA, 1: AoD with 1 us slots, 2: AoD with 2 us slots */
#define CTE_COUNT                   0x04
/*
The Switching_Pattern_Length and Antenna_IDs[i] parameters are only used when
transmitting an AoD Constant Tone Extension and shall be ignored if CTE_Type
specifies an AoA Constant Tone Extension.
*/
#define SWITCHING_PATTERN_LENGTH    0 /* Ignored for AoA */
#define ANTENNA_IDS                 NULL

#define DATA_SLICE_SIZE                (251)

#define ENABLE                          (1)
#define NUM_SET                         (1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#define ADVERTISING_INTERVAL_INCREMENT (16)
uint8_t beacon_aoa_tag_data[30];
/* USER CODE BEGIN PV */
Adv_Set_Param_t adv_set_param[NUM_SET];

uint8_t a_peeraddr[8];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/

static tBleStatus Beacon_AoA_Tag_Init(Beacon_AoA_Tag_InitTypeDef *Beacon_AoA_Tag_Init)
{
  uint8_t i;
  uint16_t adv_length_max;  
  uint8_t adv_set_count_supported;
  
  tBleStatus ret = BLE_STATUS_SUCCESS;
  uint16_t AdvertisingInterval = (Beacon_AoA_Tag_Init->AdvertisingInterval * ADVERTISING_INTERVAL_INCREMENT / 10);
  
  beacon_aoa_tag_data[0] = 2;                                                                       /*< Length. */
  beacon_aoa_tag_data[1] = AD_TYPE_FLAGS;                                                           /*< Flags data type value. */
  beacon_aoa_tag_data[2] = (FLAG_BIT_LE_GENERAL_DISCOVERABLE_MODE | FLAG_BIT_BR_EDR_NOT_SUPPORTED); /*< BLE general discoverable, without BR/EDR support. */
  beacon_aoa_tag_data[3] = 26;                                                                      /*< Length. */
  beacon_aoa_tag_data[4] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;                                      /*< Manufacturer Specific Data data type value. */
  beacon_aoa_tag_data[5] = 0x4C;
  beacon_aoa_tag_data[6] = 0x00;
  beacon_aoa_tag_data[7] = 0x02;
  beacon_aoa_tag_data[8] = 0x15;                                                                    /*< 32-bit Manufacturer Data. */
  memcpy(&beacon_aoa_tag_data[9], Beacon_AoA_Tag_Init->UuID, 16);
  beacon_aoa_tag_data[25] = Beacon_AoA_Tag_Init->MajorID[0];                                        /*< 2-byte Major. */
  beacon_aoa_tag_data[26] = Beacon_AoA_Tag_Init->MajorID[1];                                        
  beacon_aoa_tag_data[27] = Beacon_AoA_Tag_Init->MinorID[0];                                        /*< 2-byte Minor. */
  beacon_aoa_tag_data[28] = Beacon_AoA_Tag_Init->MinorID[1];                                        
  beacon_aoa_tag_data[29] = Beacon_AoA_Tag_Init->CalibratedTxPower;                                 /*< Ranging data. */


  LOG_INFO_APP("--\nBegin to configure and start advertising sets defined in the application : \n" );

  ret = hci_le_read_number_of_supported_advertising_sets(&adv_set_count_supported);
  if (ret == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Number of advertising sets supported : %d\n" ,adv_set_count_supported);
  }
  else
  {
    LOG_INFO_APP("Error in hci_le_read_number_of_supported_advertising_sets(), 0x%02X\n", ret);
    return ret;
  }
    
  ret = hci_le_read_maximum_advertising_data_length(&adv_length_max);
  if (ret == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Advertising data length max          : %d\n" ,adv_length_max);
  }
  else
  {
    LOG_INFO_APP("Error in hci_le_read_maximum_advertising_data_length(), 0x%02X\n", ret);
    return ret;
  }
  
  i = 0;
  adv_set_param[i].enable = ENABLE;
  adv_set_param[i].data = NULL;
  adv_set_param[i].data_len = 200;
  strcpy(adv_set_param[i].username, "ext_noscan_noconn");
  adv_set_param[i].sid = i;
  adv_set_param[i].interval_min = AdvertisingInterval;
  adv_set_param[i].interval_max = AdvertisingInterval;
  adv_set_param[i].tx_power = 0x00;
  adv_set_param[i].adv_channels = ADV_CH_37 | ADV_CH_38 | ADV_CH_39;
  adv_set_param[i].property = 0x00; /* Extended NO SCANNABLE - NO CONNECTABLE */
  adv_set_param[i].address_type = GAP_PUBLIC_ADDR;
  adv_set_param[i].peer_address_type = GAP_PUBLIC_ADDR;
  adv_set_param[i].p_peer_address = &a_peeraddr[0];
  adv_set_param[i].adv_set.Advertising_Handle = i; 
  adv_set_param[i].adv_set.Duration = 0;
  adv_set_param[i].adv_set.Max_Extended_Advertising_Events = 0;  
  
#if 0
  /* This command is used to set the extended advertising configuration for one advertising set. */
  /* For STM32WBA, this command is deprecated: it is better to use ACI_GAP_ADV_SET_CONFIGURATION_V2. */
  ret = aci_gap_adv_set_configuration(0,
                                        adv_set_param[i].adv_set.Advertising_Handle,
                                        adv_set_param[i].property,
                                        adv_set_param[i].interval_min,
                                        adv_set_param[i].interval_max,
                                        adv_set_param[i].adv_channels,
                                        adv_set_param[i].address_type,
                                        adv_set_param[i].peer_address_type,
                                        adv_set_param[i].p_peer_address,
                                        NO_WHITE_LIST_USE,
                                        (uint8_t)adv_set_param[i].tx_power,
                                        0x00,
                                        0x01,
                                        adv_set_param[i].sid,
                                        0x01);
  if (ret == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Success: aci_gap_adv_set_configuration\n");
  }
  else
  {
    LOG_INFO_APP("  Fail   : aci_gap_adv_set_configuration, result: 0x%02X\n", ret);
  }
#else
  /* This command is used to set the extended advertising configuration for one advertising set. */
  ret = aci_gap_adv_set_configuration_v2(0,
                                        adv_set_param[i].adv_set.Advertising_Handle,
                                        adv_set_param[i].property,
                                        adv_set_param[i].interval_min,
                                        adv_set_param[i].interval_max,
                                        adv_set_param[i].adv_channels,
                                        adv_set_param[i].address_type,
                                        adv_set_param[i].peer_address_type,
                                        adv_set_param[i].p_peer_address,
                                        NO_WHITE_LIST_USE,
                                        (uint8_t)adv_set_param[i].tx_power,
                                        0x01, /* Primary_Adv_PHY */
                                        0x00, /* Secondary_Adv_Max_Skip */
                                        0x02, /* Secondary_Adv_PHY */
                                        adv_set_param[i].sid,
                                        0x00, /* Scan_Req_Notification_Enable */
                                        0x00, /* Primary_Adv_PHY_Options */
                                        0x00  /* Secondary_Adv_PHY_Options */);
  if (ret == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Success: aci_gap_adv_set_configuration_v2\n");
  }
  else
  {
    LOG_INFO_APP("  Fail   : aci_gap_adv_set_configuration_v2, result: 0x%02X\n", ret);
  }
#endif

    
  /* This command is used to set the data used in extended advertising PDUs that have a data field. */
  ret = aci_gap_adv_set_adv_data(adv_set_param[i].adv_set.Advertising_Handle,
                                 HCI_SET_ADV_DATA_OPERATION_COMPLETE, 
                                 0x00, /* Fragment_Preference: The Controller may fragment all data */
                                 sizeof(beacon_aoa_tag_data), /* Advertising_Data_Length */
                                 beacon_aoa_tag_data); /* Advertising_Data */
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in aci_gap_adv_set_adv_data() 0x%02x\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success   : aci_gap_adv_set_adv_data\n");
  }
  

  /* Configure periodic advertising */
  ret = hci_le_set_periodic_advertising_parameters(adv_set_param[i].adv_set.Advertising_Handle, 
                                                   PERIODIC_ADV_INTERVAL, 
                                                   PERIODIC_ADV_INTERVAL, 
                                                   0);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in hci_le_set_periodic_advertising_parameters() 0x%02x\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success   : hci_le_set_periodic_advertising_parameters\n");
  }
  
  
  ret = hci_le_set_periodic_advertising_data(adv_set_param[i].adv_set.Advertising_Handle, 
                                             0x03,
                                             sizeof(beacon_aoa_tag_data), /* Advertising_Data_Length */
                                             beacon_aoa_tag_data); /* Advertising_Data */
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in hci_le_set_periodic_advertising_data() 0x%02x\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success   : hci_le_set_periodic_advertising_data\n");
  }
   
  /* Set the connectionless CTE transmit parameters */
  ret = hci_le_set_connectionless_cte_transmit_parameters(adv_set_param[i].adv_set.Advertising_Handle, CTE_LENGTH, CTE_TYPE, CTE_COUNT, SWITCHING_PATTERN_LENGTH, ANTENNA_IDS);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in hci_le_set_connectionless_cte_transmit_parameters() 0x%02X\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success: hci_le_set_connectionless_cte_transmit_parameters\n");
  }
  

  ret = hci_le_set_connectionless_cte_transmit_enable(adv_set_param[i].adv_set.Advertising_Handle, ENABLE);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in hci_le_set_connectionless_cte_transmit_enable() 0x%02X\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success: hci_le_set_connectionless_cte_transmit_enable\n");
    LOG_INFO_APP("CTE configured\n");
  }
  

 
  /* Enable periodic advertising */
  ret = hci_le_set_periodic_advertising_enable(ENABLE, adv_set_param[i].adv_set.Advertising_Handle);
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("Error in hci_le_set_periodic_advertising_enable() 0x%02X\r\n", ret);
    return ret;
  }
  else
  {
    LOG_INFO_APP("  Success: hci_le_set_periodic_advertising_enable\n");
  }
            
  LOG_INFO_APP("Periodic advertising configured\n");
  
   
  ret = aci_gap_adv_set_enable(ENABLE, NUM_SET, &adv_set_param[i].adv_set);
  if (ret == BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Success: aci_gap_adv_set_enable\n");
  }
  else
  {
    LOG_INFO_APP("  Fail   : aci_gap_adv_set_enable, result: 0x%02X\n", ret);
  }
  
  return ret;
}

void Beacon_AoA_Tag_Process(void)
{

  /* Default beacon aoa tag data */
  uint8_t UuID[]    = { UUID };
  uint8_t MajorID[] = { MAJOR_ID };
  uint8_t MinorID[] = { MINOR_ID };

  Beacon_AoA_Tag_InitTypeDef Beacon_AoA_Tag_InitStruct =
  {
    .AdvertisingInterval = ADVERTISING_INTERVAL_IN_MS,
    .UuID                = UuID,
    .MajorID             = MajorID,
    .MinorID             = MinorID,
    .CalibratedTxPower   = CALIBRATED_TX_POWER_AT_1_M
  };

  Beacon_AoA_Tag_Init(&Beacon_AoA_Tag_InitStruct);
}
/* USER CODE BEGIN FD */

/* USER CODE END FD */
