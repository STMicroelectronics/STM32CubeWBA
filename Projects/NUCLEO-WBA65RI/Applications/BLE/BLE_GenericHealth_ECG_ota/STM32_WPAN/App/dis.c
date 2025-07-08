/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    dis.c
  * @author  MCD Application Team
  * @brief   dis definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "log_module.h"
#include "dis.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  DisSvcHdle;                  /**< Dis Service Handle */
  uint16_t  SrsCharHdle;                  /**< SRS Characteristic Handle */
  uint16_t  MansCharHdle;                  /**< MANS Characteristic Handle */
  uint16_t  MonsCharHdle;                  /**< MONS Characteristic Handle */
  uint16_t  SnsCharHdle;                  /**< SNS Characteristic Handle */
  uint16_t  SyidCharHdle;                  /**< SYID Characteristic Handle */
  uint16_t  UdifmdCharHdle;                  /**< UDIFMD Characteristic Handle */
  uint16_t  FrsCharHdle;                  /**< FRS Characteristic Handle */
  uint16_t  HrsCharHdle;                  /**< HRS Characteristic Handle */
  uint16_t  IrcdlCharHdle;                  /**< IRCDL Characteristic Handle */
  uint16_t  PniCharHdle;                  /**< PNI Characteristic Handle */
/* USER CODE BEGIN Context */

/* USER CODE END Context */
}DIS_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeSrs = 32;
static const uint16_t SizeMans = 32;
static const uint16_t SizeMons = 32;
static const uint16_t SizeSns = 32;
static const uint16_t SizeSyid = 8;
static const uint16_t SizeUdifmd = 40;
static const uint16_t SizeFrs = 32;
static const uint16_t SizeHrs = 32;
static const uint16_t SizeIrcdl = 32;
static const uint16_t SizePni = 7;

static DIS_Context_t DIS_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t DIS_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t DIS_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  /* USER CODE BEGIN Service2_EventHandler_1 */

  /* USER CODE END Service2_EventHandler_1 */

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)p_Event)->data);

  switch(p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch(p_blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        }
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR */

      /* USER CODE END EVT_VENDOR */
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES */

      /* USER CODE END EVENT_PCKT_CASES */

    default:
      /* USER CODE BEGIN EVENT_PCKT */

      /* USER CODE END EVENT_PCKT */
      break;
  }

  /* USER CODE BEGIN Service2_EventHandler_2 */

  /* USER CODE END Service2_EventHandler_2 */

  return(return_value);
}/* end DIS_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void DIS_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService2Svc_1 */

  /* USER CODE END SVCCTL_InitService2Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(DIS_EventHandler);

  /**
   * DIS
   *
   * Max_Attribute_Records = 1 + 2*10 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for DIS +
   *                                2 for SRS +
   *                                2 for MANS +
   *                                2 for MONS +
   *                                2 for SNS +
   *                                2 for SYID +
   *                                2 for UDIFMD +
   *                                2 for FRS +
   *                                2 for HRS +
   *                                2 for IRCDL +
   *                                2 for PNI +
   *                              = 21
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 21;

  /* USER CODE BEGIN SVCCTL_InitService */

  /* USER CODE END SVCCTL_InitService */

  uuid.Char_UUID_16 = 0x180a;
  ret = aci_gatt_add_service(UUID_TYPE_16,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(DIS_Context.DisSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: DIS, error code: 0x%x \n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: DisSvcHdle = 0x%04X\n",DIS_Context.DisSvcHdle);
  }

  /**
   * SRS
   */
  uuid.Char_UUID_16 = 0x2a28;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeSrs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.SrsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : SRS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : SrsCharHdle = 0x%04X\n",DIS_Context.SrsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char1 */

  /**
   * MANS
   */
  uuid.Char_UUID_16 = 0x2a29;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeMans,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.MansCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : MANS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : MansCharHdle = 0x%04X\n",DIS_Context.MansCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char2 */

  /**
   * MONS
   */
  uuid.Char_UUID_16 = 0x2a24;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeMons,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.MonsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : MONS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : MonsCharHdle = 0x%04X\n",DIS_Context.MonsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char3 */

  /**
   * SNS
   */
  uuid.Char_UUID_16 = 0x2a25;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeSns,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.SnsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : SNS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : SnsCharHdle = 0x%04X\n",DIS_Context.SnsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char4 */

  /**
   * SYID
   */
  uuid.Char_UUID_16 = 0x2a23;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeSyid,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(DIS_Context.SyidCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : SYID, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : SyidCharHdle = 0x%04X\n",DIS_Context.SyidCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char5 */

  /**
   * UDIFMD
   */
  uuid.Char_UUID_16 = 0x2bff;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeUdifmd,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.UdifmdCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : UDIFMD, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : UdifmdCharHdle = 0x%04X\n",DIS_Context.UdifmdCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char6 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char6 */

  /**
   * FRS
   */
  uuid.Char_UUID_16 = 0x2a26;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeFrs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.FrsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : FRS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : FrsCharHdle = 0x%04X\n",DIS_Context.FrsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char7 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char7 */

  /**
   * HRS
   */
  uuid.Char_UUID_16 = 0x2a27;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeHrs,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.HrsCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : HRS, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : HrsCharHdle = 0x%04X\n",DIS_Context.HrsCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char8 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char8 */

  /**
   * IRCDL
   */
  uuid.Char_UUID_16 = 0x2a2a;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizeIrcdl,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(DIS_Context.IrcdlCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : IRCDL, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : IrcdlCharHdle = 0x%04X\n",DIS_Context.IrcdlCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char9 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char9 */

  /**
   * PNI
   */
  uuid.Char_UUID_16 = 0x2a50;
  ret = aci_gatt_add_char(DIS_Context.DisSvcHdle,
                          UUID_TYPE_16,
                          (Char_UUID_t *) &uuid,
                          SizePni,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_NONE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(DIS_Context.PniCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : PNI, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : PniCharHdle = 0x%04X\n",DIS_Context.PniCharHdle);
  }

  /* USER CODE BEGIN SVCCTL_InitService2Char10 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService2Char10 */

  /* USER CODE BEGIN SVCCTL_InitService2Svc_2 */

  /* USER CODE END SVCCTL_InitService2Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  pData: Structure holding data to update
 *
 */
tBleStatus DIS_UpdateValue(DIS_CharOpcode_t CharOpcode, DIS_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service2_App_Update_Char_1 */

  /* USER CODE END Service2_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case DIS_SRS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.SrsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value SRS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value SRS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_1 */

      /* USER CODE END Service2_Char_Value_1 */
      break;

    case DIS_MANS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.MansCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value MANS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value MANS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_2 */

      /* USER CODE END Service2_Char_Value_2 */
      break;

    case DIS_MONS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.MonsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value MONS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value MONS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_3 */

      /* USER CODE END Service2_Char_Value_3 */
      break;

    case DIS_SNS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.SnsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value SNS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value SNS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_4 */

      /* USER CODE END Service2_Char_Value_4 */
      break;

    case DIS_SYID:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.SyidCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value SYID command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value SYID command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_5 */

      /* USER CODE END Service2_Char_Value_5 */
      break;

    case DIS_UDIFMD:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.UdifmdCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value UDIFMD command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value UDIFMD command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_6 */

      /* USER CODE END Service2_Char_Value_6 */
      break;

    case DIS_FRS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.FrsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value FRS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value FRS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_7 */

      /* USER CODE END Service2_Char_Value_7 */
      break;

    case DIS_HRS:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.HrsCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value HRS command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value HRS command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_8 */

      /* USER CODE END Service2_Char_Value_8 */
      break;

    case DIS_IRCDL:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.IrcdlCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value IRCDL command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value IRCDL command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_9 */

      /* USER CODE END Service2_Char_Value_9 */
      break;

    case DIS_PNI:
      ret = aci_gatt_update_char_value(DIS_Context.DisSvcHdle,
                                       DIS_Context.PniCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_DEBUG_APP("  Fail   : aci_gatt_update_char_value PNI command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_DEBUG_APP("  Success: aci_gatt_update_char_value PNI command\n");
      }
      /* USER CODE BEGIN Service2_Char_Value_10 */

      /* USER CODE END Service2_Char_Value_10 */
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service2_App_Update_Char_2 */

  /* USER CODE END Service2_App_Update_Char_2 */

  return ret;
}
