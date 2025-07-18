/**
 ******************************************************************************
 * @file    ias.c
 * @author  MCD Application Team
 * @brief   Immediate Alert Service
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2021 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "hap_config.h"
#if (HAP_HA_IAS_SUPPORT == 1u)
#include "ias.h"
#include "hap_log.h"
#include "uuid.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint16_t SvcHdle;                  /**< Service handle */
  uint16_t AlertLevelCharHdle;       /**< Characteristic handle */
}IAS_Context_t;


/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
#define PLACE_IN_SECTION( __x__ )  __attribute__((section (__x__)))
/* Private variables ---------------------------------------------------------*/

/**
 * START of Section BLE_DRIVER_CONTEXT
 */

PLACE_IN_SECTION("BLE_DRIVER_CONTEXT") static IAS_Context_t IAS_Context;

/**
 * END of Section BLE_DRIVER_CONTEXT
 */


/* Private function prototypes -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/
/* Private functions ----------------------------------------------------------*/

/**
 * @brief  Event handler
 * @param  Event: Address of the buffer holding the Event
 * @retval Ack: Return whether the Event has been managed or not
 */
SVCCTL_EvtAckStatus_t IAS_Event_Handler(void *Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *event_pckt;
  evt_blecore_aci *blecore_evt;
  aci_gatt_attribute_modified_event_rp0* attribute_modified;
  IAS_Notification_Evt_t                Notification;

  return_value = SVCCTL_EvtNotAck;
  event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)Event)->data);

  switch(event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
    {
      blecore_evt = (evt_blecore_aci*)event_pckt->data;
      switch(blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          attribute_modified = (aci_gatt_attribute_modified_event_rp0*)blecore_evt->data;
          Notification.ConnHandle = attribute_modified->Connection_Handle;
          if(attribute_modified->Attr_Handle == (IAS_Context.AlertLevelCharHdle + 1))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            if(attribute_modified->Attr_Data[0] == 0)
            {
              Notification.EvtOpcode = IAS_NO_ALERT_EVT;
              IAS_Notification(&Notification);
            }
            else if(attribute_modified->Attr_Data[0] == 1)
            {
              Notification.EvtOpcode = IAS_MID_ALERT_EVT;
              IAS_Notification(&Notification);
            }
            else if(attribute_modified->Attr_Data[0] == 2)
            {
              Notification.EvtOpcode = IAS_HIGH_ALERT_EVT;
              IAS_Notification(&Notification);
            }
          }
        }
        break;

        default:
          break;
      }
    }
    break; /* HCI_HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE_SPECIFIC */

    default:
      break;
  }

  return(return_value);
}/* end IAS_Event_Handler */


/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval Status of the operation
 */
tBleStatus IAS_Init(void)
{
  uint16_t uuid;
  tBleStatus hciCmdResult;

  /**
   *  Add Immediate Alert Service
   *
   * Max_Attribute_Records = 2*no_of_char + 1
   * service_max_attribute_record = 1 for Immediate Alert service +
   *                                2 for alert level characteristic +
   */
  uuid = IMMEDIATE_ALERT_SERVICE_UUID;
  hciCmdResult = aci_gatt_add_service(UUID_TYPE_16,
                                      (Service_UUID_t *) &uuid,
                                      PRIMARY_SERVICE,
                                      3,
                                      &(IAS_Context.SvcHdle));
  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_IAS_MSG ("Immediate Alert Service (IAS) is added Successfully %04X\n",
                 IAS_Context.SvcHdle);
  }
  else
  {
    BLE_DBG_IAS_MSG ("FAILED to add Immediate Alert Service (IAS), Error: %02X !!\n",
                 hciCmdResult);
  }

  /**
   *  Add Alert Level Characteristic
   */
  uuid = ALERT_LEVEL_CHARACTERISTIC_UUID;
  hciCmdResult = aci_gatt_add_char(IAS_Context.SvcHdle,
                                   UUID_TYPE_16,
                                   (Char_UUID_t *) &uuid ,
                                   1,
                                   CHAR_PROP_WRITE_WITHOUT_RESP,
                                   ATTR_PERMISSION_NONE,
                                   GATT_NOTIFY_ATTRIBUTE_WRITE,
                                   MIN_ENCRY_KEY_SIZE,
                                   CHAR_VALUE_LEN_CONSTANT,
                                   &(IAS_Context.AlertLevelCharHdle));

  if (hciCmdResult == BLE_STATUS_SUCCESS)
  {
    BLE_DBG_IAS_MSG ("Alert Level Characteristic Added Successfully  %04X \n",
                 IAS_Context.AlertLevelCharHdle);
  }
  else
  {
    BLE_DBG_IAS_MSG ("FAILED to add Alert Level Characteristic, Error: %02X !!\n",
                 hciCmdResult);
  }

  return hciCmdResult;
}


#endif /* (HAP_HA_IAS_SUPPORT == 1u) */
