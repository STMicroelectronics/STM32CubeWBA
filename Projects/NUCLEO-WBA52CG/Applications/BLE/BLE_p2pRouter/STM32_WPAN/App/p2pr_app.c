/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    STM32_WPAN
  * @author  MCD Application Team
  * @brief   STM32_WPAN application definition.
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
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "p2pr_app.h"
#include "p2pr.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32wbaxx_nucleo.h"
#include "gatt_client_app.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Notiffwd_NOTIFICATION_OFF,
  Notiffwd_NOTIFICATION_ON,
  Devinfo_NOTIFICATION_OFF,
  Devinfo_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  P2PR_APP_SENDINFORMATION_LAST
} P2PR_APP_SendInformation_t;

typedef struct
{
  P2PR_APP_SendInformation_t     Notiffwd_Notification_Status;
  P2PR_APP_SendInformation_t     Devinfo_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  uint8_t P2PR_device_status[P2P_DEVICE_COUNT_MAX * 2];
  uint8_t P2PR_device_connHdl[P2P_DEVICE_COUNT_MAX * 2];
  uint8_t a_P2PR_device_bd_addr[P2P_DEVICE_COUNT_MAX * 2][BD_ADDR_SIZE];
  uint8_t a_P2PR_device_char_write_level[P2P_DEVICE_COUNT_MAX * 2];
  uint8_t a_P2PR_device_char_notif_level[P2P_DEVICE_COUNT_MAX * 2];
  uint8_t a_P2PR_device_name[P2P_DEVICE_COUNT_MAX * 2][32];
  uint8_t a_P2PR_device_name_len[P2P_DEVICE_COUNT_MAX * 2];
  
  uint8_t P2PR_writeVal[2];
  uint8_t P2PR_writeValLen;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} P2PR_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static P2PR_APP_Context_t P2PR_APP_Context;

uint8_t a_P2PR_UpdateCharData[247];

/* USER CODE BEGIN PV */
static uint16_t last_connHdl;
static uint8_t notifDevInfoTable_index;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void P2PR_Notiffwd_SendNotification(void);
static void P2PR_Devinfo_SendNotification(void);

/* USER CODE BEGIN PFP */
static void P2PR_Connect_Request(void);
static uint8_t P2PR_notifDevInfo(uint8_t dev_idx);
static void P2PR_notifDevInfoTable(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void P2PR_Notification(P2PR_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */
  uint8_t dev_idx, loop;
  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case P2PR_WRITEFWD_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case P2PR_WRITEFWD_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      dev_idx = p_Notification->DataTransfered.p_Payload[0];

      if (dev_idx < (P2P_DEVICE_COUNT_MAX * 2))
      {
        P2PR_APP_Context.P2PR_writeVal[0] = p_Notification->DataTransfered.p_Payload[0];
        P2PR_APP_Context.P2PR_writeVal[1] = p_Notification->DataTransfered.p_Payload[1];
        P2PR_APP_Context.P2PR_writeValLen = p_Notification->DataTransfered.Length;
        UTIL_SEQ_SetTask( 1u << CFG_TASK_FORWARD_WRITE_ID, CFG_SCH_PRIO_0);
        
        P2PR_APP_Context.a_P2PR_device_char_write_level[dev_idx] = p_Notification->DataTransfered.p_Payload[1];
        P2PR_notifDevInfo(dev_idx);
      }
      else if ( dev_idx == 0xFF)
      {        
        P2PR_APP_Context.P2PR_writeVal[0] = p_Notification->DataTransfered.p_Payload[0];
        P2PR_APP_Context.P2PR_writeVal[1] = p_Notification->DataTransfered.p_Payload[1];
        P2PR_APP_Context.P2PR_writeValLen = p_Notification->DataTransfered.Length;
        UTIL_SEQ_SetTask( 1u << CFG_TASK_FORWARD_WRITE_ID, CFG_SCH_PRIO_0);
            
        for ( loop = 0 ; loop < (P2P_DEVICE_COUNT_MAX * 2) ; loop++)
        {
          if (P2PR_APP_Context.P2PR_device_status[loop] == P2PR_DEV_CONNECTED)
          {
            P2PR_APP_Context.a_P2PR_device_char_write_level[loop] = p_Notification->DataTransfered.p_Payload[1];
            P2PR_notifDevInfo(loop);
          }
        }
      }
      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case P2PR_NOTIFFWD_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      P2PR_APP_Context.Notiffwd_Notification_Status = Notiffwd_NOTIFICATION_ON;
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case P2PR_NOTIFFWD_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      P2PR_APP_Context.Notiffwd_Notification_Status = Notiffwd_NOTIFICATION_OFF;
      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
      break;

    case P2PR_DEVINFO_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_ENABLED_EVT */
      P2PR_APP_Context.Devinfo_Notification_Status = Devinfo_NOTIFICATION_ON;

      notifDevInfoTable_index = 0;
      UTIL_SEQ_SetTask( 1u << CFG_TASK_DEV_TABLE_NOTIF_ID, CFG_SCH_PRIO_0);
      /* USER CODE END Service1Char3_NOTIFY_ENABLED_EVT */
      break;

    case P2PR_DEVINFO_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char3_NOTIFY_DISABLED_EVT */
      P2PR_APP_Context.Devinfo_Notification_Status = Devinfo_NOTIFICATION_OFF;
      /* USER CODE END Service1Char3_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void P2PR_APP_EvtRx(P2PR_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */
  uint8_t i;
  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */
    case P2PR_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      last_connHdl = p_Notification->ConnectionHandle;
      
      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case P2PR_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */
      for (i = 0 ; i < (P2P_DEVICE_COUNT_MAX * 2) ; i++)
      {
        if( P2PR_APP_Context.P2PR_device_connHdl[i] == p_Notification->ConnectionHandle)
        {
          P2PR_APP_Context.P2PR_device_status[i] = P2PR_DEV_LOST;
          P2PR_notifDevInfo(i);
        }
      }

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void P2PR_APP_Init(void)
{
  UNUSED(P2PR_APP_Context);
  P2PR_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  UTIL_SEQ_RegTask( 1u << CFG_TASK_FORWARD_NOTIF_ID, UTIL_SEQ_RFU, P2PR_ForwardNotification);
  UTIL_SEQ_RegTask( 1u << CFG_TASK_CONN_DEV_ID, UTIL_SEQ_RFU, P2PR_Connect_Request);
  UTIL_SEQ_RegTask( 1u << CFG_TASK_DEV_TABLE_NOTIF_ID, UTIL_SEQ_RFU, P2PR_notifDevInfoTable);

  P2PR_APP_Context.Notiffwd_Notification_Status = Notiffwd_NOTIFICATION_OFF;

  uint8_t i;
  for ( i = 0 ; i < (P2P_DEVICE_COUNT_MAX * 2) ; i++)
  {
    P2PR_APP_Context.P2PR_device_status[i] = P2PR_DEV_NONE;
    P2PR_APP_Context.P2PR_device_connHdl[i] = 0xFF;
    memset(&P2PR_APP_Context.a_P2PR_device_bd_addr[i][0], 
           0xFF, 
           sizeof(P2PR_APP_Context.a_P2PR_device_bd_addr[i]));
    P2PR_APP_Context.a_P2PR_device_char_notif_level[i] = 0;
    P2PR_APP_Context.a_P2PR_device_char_write_level[i] = 0;
    memset(&P2PR_APP_Context.a_P2PR_device_name[i][0], 
           0x00, 
           sizeof(P2PR_APP_Context.a_P2PR_device_name[i]));
    P2PR_APP_Context.a_P2PR_device_name_len[i] = 0;
  }
  
  notifDevInfoTable_index = 0;
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
uint8_t P2PR_setDeviceInfo(uint8_t id, uint8_t *p_bdAddr, uint8_t dev_status)
{
  if ((dev_status < P2PR_DEV_LAST) && (id <= P2P_DEVICE_COUNT_MAX))
  {
    P2PR_APP_Context.P2PR_device_status[id] = dev_status;
    
    memcpy(&P2PR_APP_Context.a_P2PR_device_bd_addr[id][0],
           p_bdAddr,
           BD_ADDR_SIZE);
  }

  return 0;
}

uint8_t P2PR_getDeviceInfo(uint8_t id, uint8_t *p_bdAddr, uint8_t *p_dev_status)
{
  if (id <= P2P_DEVICE_COUNT_MAX)
  {
    *p_dev_status = P2PR_APP_Context.P2PR_device_status[id];
    
    memcpy(&P2PR_APP_Context.a_P2PR_device_bd_addr[id][0],
           p_bdAddr,
           BD_ADDR_SIZE);
  }

  return 0;
}

uint8_t P2PR_getIndexFromConnHdl(uint16_t connHdl)
{
  uint8_t i, result = 0xFF;
  
  for ( i = 0 ; i < (P2P_DEVICE_COUNT_MAX * 2) ; i++)
  {
    if (P2PR_APP_Context.P2PR_device_connHdl[i] == connHdl)
    {
      result = i;
      break;
    }
  }
  
  return result;
}

uint16_t P2PR_getConnHdlFromIndex(uint8_t index)
{
  uint16_t result = 0xFFFF;

  if (index < (P2P_DEVICE_COUNT_MAX * 2))
  {
    result = P2PR_APP_Context.P2PR_device_connHdl[index];
  }
  
  return result;
}

uint8_t P2PR_getWriteData(uint32_t *p_P2PR_writeValAddr, uint8_t *p_P2PR_writeValLen)
{
  *p_P2PR_writeValAddr = (uint32_t)&P2PR_APP_Context.P2PR_writeVal[0];
  *p_P2PR_writeValLen = P2PR_APP_Context.P2PR_writeValLen;

  return 0;
}

uint8_t P2PR_setNotifLevel(uint8_t dev_idx, uint8_t level)
{
  P2PR_APP_Context.a_P2PR_device_char_notif_level[dev_idx] = level;

  return 0;
}

uint8_t P2PR_analyseAdvReport(hci_le_advertising_report_event_rp0 *p_adv_report)
{
  uint8_t adv_type, adv_data_size, found_status, manufBlueST;
  uint8_t *p_adv_data;
  uint16_t i, j, dev_idx;
  int32_t cmp_status;
  uint8_t adv_name[32];
  uint8_t adv_name_len;
  uint8_t bd_addr[BD_ADDR_SIZE];
  
  found_status = 0;
  memset(&adv_name[0], 0x00, sizeof(adv_name));
  adv_name_len = 0;
  memset(&bd_addr[0], 0x00, sizeof(bd_addr));
  adv_type = p_adv_report->Advertising_Report[0].Event_Type;
  adv_data_size = p_adv_report->Advertising_Report[0].Length_Data;

  if (adv_type == HCI_ADV_EVT_TYPE_ADV_IND)
  {
    uint8_t ad_length, ad_type;    
    p_adv_data = (uint8_t*)(&p_adv_report->Advertising_Report[0].Length_Data) + 1;
    
    i = 0;
    while(i < adv_data_size)
    {
      ad_length = p_adv_data[i];
      ad_type = p_adv_data[i + 1];
      
      switch (ad_type)
      {
        case AD_TYPE_FLAGS:
          break;
        case AD_TYPE_TX_POWER_LEVEL:
          break;
        case AD_TYPE_COMPLETE_LOCAL_NAME:
        {
          memcpy(&adv_name[0], &p_adv_data[i + 2], ad_length - 1);
          adv_name_len = ad_length - 1;
          break;
        }
        case AD_TYPE_MANUFACTURER_SPECIFIC_DATA:
        {
          manufBlueST = UINT8_MAX;
          if ((ad_length >= 7) && 
              (p_adv_data[i + 4] == 0x02) && 
              (p_adv_data[i + 6] == 0x83))
          {
            /* p2pServer BlueST v2 detected */
            manufBlueST = 0x02;
          }
          else if ((ad_length >= 7) && 
                   (p_adv_data[i + 2] == 0x01))
          {
            /* p2pServer BlueST v1 detected */
            manufBlueST = 0x01;
          }
          
          if(manufBlueST != UINT8_MAX)
          {
            dev_idx = UINT8_MAX;
            
            /* look for a free index entry */
            for(j = 0 ; j < (P2P_DEVICE_COUNT_MAX * 2) ; j++)
            {
              if (P2PR_APP_Context.P2PR_device_status[j] == P2PR_DEV_NONE)
              {
                dev_idx = j;
                break;
              }
            }
            
            /* check if this bd address is already registered */
            for(j = 0 ; j < (P2P_DEVICE_COUNT_MAX * 2) ; j++)
            {
              cmp_status = memcmp(&P2PR_APP_Context.a_P2PR_device_bd_addr[j][0],
                                  &p_adv_report->Advertising_Report[0].Address[0],
                                  BD_ADDR_SIZE);

              if (cmp_status == 0)
              {
                if (P2PR_APP_Context.P2PR_device_status[j] == P2PR_DEV_LOST)
                {
                  /* already registered, we connect again so we use same index */
                  dev_idx = j;                  
                }
                else
                {
                  /* already registered, not lost so we skip */
                  dev_idx = UINT8_MAX;
                }
                break;
              }
            }
       
#if 0 /* filter some devices based on BD address */
            uint8_t p2pServer1_DB[6] = {0x01,0x7B,0x2A,0xE1,0x80,0x00};
            uint8_t p2pServer2_DB[6] = {0x0C,0x7D,0x2A,0xE1,0x80,0x00};
            uint8_t p2pServer3_DB[6] = {0xF5,0x7B,0x2A,0xE1,0x80,0x00};
            int32_t p2pServer1_cmp;
            int32_t p2pServer2_cmp;
            int32_t p2pServer3_cmp;
            
            p2pServer1_cmp = memcmp(&p2pServer1_DB[0],
                                    &p_adv_report->Advertising_Report[0].Address[0],
                                    BD_ADDR_SIZE);
            p2pServer2_cmp = memcmp(&p2pServer2_DB[0],
                                    &p_adv_report->Advertising_Report[0].Address[0],
                                    BD_ADDR_SIZE);
            p2pServer3_cmp = memcmp(&p2pServer3_DB[0],
                                    &p_adv_report->Advertising_Report[0].Address[0],
                                    BD_ADDR_SIZE);

            if ( (p2pServer1_cmp == 0) ||  (p2pServer2_cmp == 0) || (p2pServer3_cmp == 0) )
            {
              
            }
            else
            {
              dev_idx = UINT8_MAX;
            }
#endif
            if(dev_idx < P2P_DEVICE_COUNT_MAX)
            {
              bd_addr[0] = p_adv_report->Advertising_Report[0].Address[0];
              bd_addr[1] = p_adv_report->Advertising_Report[0].Address[1];
              bd_addr[2] = p_adv_report->Advertising_Report[0].Address[2];
              bd_addr[3] = p_adv_report->Advertising_Report[0].Address[3];
              bd_addr[4] = p_adv_report->Advertising_Report[0].Address[4];
              bd_addr[5] = p_adv_report->Advertising_Report[0].Address[5];
              
              found_status = 1;
            }
          }
          break;
        }
        default:
          break;
      }/* end of switch*/
      
      i += ad_length + 1; /* increment the iterator to go on next element*/

    }/* end of while*/
    
    if (found_status != 0)
    {
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][0] = bd_addr[0];
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][1] = bd_addr[1];
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][2] = bd_addr[2];
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][3] = bd_addr[3];
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][4] = bd_addr[4];
      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][5] = bd_addr[5];

      P2PR_APP_Context.P2PR_device_status[dev_idx] = P2PR_DEV_FOUND;

      if(adv_name_len != 0)
      {
        memcpy(&P2PR_APP_Context.a_P2PR_device_name[dev_idx][0],  &adv_name[0], adv_name_len + 1);
        P2PR_APP_Context.a_P2PR_device_name_len[dev_idx] = adv_name_len + 1;
      }

      APP_DBG_MSG("  p2pServer name: %s, db addr: 0x%02X:%02X:%02X:%02X:%02X:%02X\n",
                      P2PR_APP_Context.a_P2PR_device_name[dev_idx],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][5],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][4],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][3],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][2],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][1],
                      P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][0]);
      APP_DBG_MSG("  -> stored into p2pRouter table at index %d\n", dev_idx);

      P2PR_notifDevInfo(dev_idx);
    }
  }
  
  return found_status;
}

void P2PR_ForwardNotification(void)
{
  P2PR_Notiffwd_SendNotification();

  return;
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void P2PR_Notiffwd_SendNotification(void) /* Property Notification */
{
  P2PR_APP_SendInformation_t notification_on_off = Notiffwd_NOTIFICATION_OFF;
  P2PR_Data_t p2pr_notification_data;

  p2pr_notification_data.p_Payload = (uint8_t*)a_P2PR_UpdateCharData;
  p2pr_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/
  p2pr_notification_data.Length = 2; 
  
  if(P2PR_APP_Context.Notiffwd_Notification_Status == Notiffwd_NOTIFICATION_ON)
  { 
    notification_on_off = Notiffwd_NOTIFICATION_ON;
  } 
  else
  {
    APP_DBG_MSG("notification are disabled\n"); 
  }
  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Notiffwd_NOTIFICATION_OFF)
  {
    P2PR_UpdateValue(P2PR_NOTIFFWD, &p2pr_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

__USED void P2PR_Devinfo_SendNotification(void) /* Property Notification */
{
  P2PR_APP_SendInformation_t notification_on_off = Devinfo_NOTIFICATION_OFF;
  P2PR_Data_t p2pr_notification_data;

  p2pr_notification_data.p_Payload = (uint8_t*)a_P2PR_UpdateCharData;
  p2pr_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char3_NS_1*/

  /* USER CODE END Service1Char3_NS_1*/

  if (notification_on_off != Devinfo_NOTIFICATION_OFF)
  {
    P2PR_UpdateValue(P2PR_DEVINFO, &p2pr_notification_data);
  }

  /* USER CODE BEGIN Service1Char3_NS_Last*/

  /* USER CODE END Service1Char3_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/
static void P2PR_Connect_Request(void)
{
  tBleStatus result;
  uint8_t device_index;
  
  for (device_index = 0 ; device_index < (P2P_DEVICE_COUNT_MAX * 2) ; device_index++)
  {
    if (P2PR_APP_Context.P2PR_device_status[device_index] == P2PR_DEV_FOUND)
    {
      BSP_LED_On(LED_BLUE);
      APP_DBG_MSG("Create connection to p2pServer stored in table at index %d\n",device_index);
      
      P2PR_APP_Context.P2PR_device_status[device_index] = P2PR_DEV_CONNECTING;
      
      result = aci_gap_create_connection(SCAN_INT_MS(500u), SCAN_WIN_MS(500u),
                                         GAP_PUBLIC_ADDR, 
                                         &P2PR_APP_Context.a_P2PR_device_bd_addr[device_index][0],
                                         GAP_PUBLIC_ADDR,
                                         CONN_INT_MS(50u), CONN_INT_MS(100u),
                                         0u,
                                         CONN_SUP_TIMEOUT_MS(5000u),
                                         CONN_CE_LENGTH_MS(10u), CONN_CE_LENGTH_MS(10u));      
      if (result == BLE_STATUS_SUCCESS)
      {
        APP_DBG_MSG("  wait for event HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE\n");
        UTIL_SEQ_WaitEvt(1u << CFG_IDLEEVT_NODE_CONNECTION_COMPLETE);

        P2PR_APP_Context.P2PR_device_status[device_index] = P2PR_DEV_CONNECTING;
        P2PR_APP_Context.P2PR_device_connHdl[device_index] = last_connHdl;
        GATT_CLIENT_APP_Set_Conn_Handle(device_index, P2PR_APP_Context.P2PR_device_connHdl[device_index]);

        result = aci_gatt_exchange_config(P2PR_APP_Context.P2PR_device_connHdl[device_index]);
        if (result != BLE_STATUS_SUCCESS)
        {
          APP_DBG_MSG("  Fail   : MTU exchange 0x%02X\n", result);
        }
        else
        {
          APP_DBG_MSG("  Success: MTU exchange\n");
          UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_NODE_MTU_EXCHANGED_COMPLETE);
          UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_PROC_GATT_COMPLETE);
        }

        APP_DBG_MSG("Discover services, characteristics and descriptors for table index %d\n",device_index);
        P2PR_APP_Context.P2PR_device_status[device_index] = P2PR_DEV_DISCOVERING;
        GATT_CLIENT_APP_Discover_services(device_index);

        P2PR_APP_Context.P2PR_device_status[device_index] = P2PR_DEV_CONNECTED;        
        BSP_LED_Off(LED_BLUE);
        
        P2PR_notifDevInfo(device_index);
      }
      else
      {
        P2PR_APP_Context.P2PR_device_status[device_index] = P2PR_DEV_NONE;
        APP_DBG_MSG("==>> GAP Create connection Failed , result: 0x%02x\n", result);
      }      
    }
  }

  return;
}

static uint8_t P2PR_notifDevInfo(uint8_t dev_idx)
{
  P2PR_Data_t data;
  uint8_t status, tab[32], n;
  
  if (dev_idx < (P2P_DEVICE_COUNT_MAX * 2))
  {
    status = 0;
    n = 0;
    tab[n++] = dev_idx;
    tab[n++] = P2PR_APP_Context.P2PR_device_status[dev_idx];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][5];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][4];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][3];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][2];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][1];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_bd_addr[dev_idx][0];  
    tab[n++] = P2PR_APP_Context.a_P2PR_device_char_write_level[dev_idx];
    tab[n++] = P2PR_APP_Context.a_P2PR_device_char_notif_level[dev_idx];
    memcpy(&tab[n],
           &P2PR_APP_Context.a_P2PR_device_name[dev_idx][0],
           P2PR_APP_Context.a_P2PR_device_name_len[dev_idx]);
    n += P2PR_APP_Context.a_P2PR_device_name_len[dev_idx];

    data.Length = n;
    data.p_Payload = &tab[0];
    P2PR_UpdateValue(P2PR_DEVINFO, &data);

    HostStack_Process();
  }
  else
  {
    status = 1;
  }
  
  return status;
}

static void P2PR_notifDevInfoTable(void)
{  
  for (; notifDevInfoTable_index < (P2P_DEVICE_COUNT_MAX * 2) ; notifDevInfoTable_index++)
  {
    if (P2PR_APP_Context.P2PR_device_status[notifDevInfoTable_index] != P2PR_DEV_NONE)
    {
      P2PR_notifDevInfo(notifDevInfoTable_index);
    }
    UTIL_SEQ_SetTask( 1u << CFG_TASK_DEV_TABLE_NOTIF_ID, CFG_SCH_PRIO_0);
  }

  return;
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
