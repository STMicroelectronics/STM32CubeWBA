/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    lhci.c
 * @author  MCD Application Team
 * @brief   HCI command for the system channel
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
#include "ble_common.h"
#include "ble_std.h"

#include "app_ble.h"
#include "rf_antenna_switch.h"
#include "lhci.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
extern uint32_t FW_Version;

/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/
void LHCI_C1_Write_Register( BleCmdSerial_t *pcmd )
{
  LHCI_C1_Write_Register_cmd_t * p_param;
  uint32_t primask_bit;

  primask_bit = __get_PRIMASK(); /**< backup PRIMASK bit */

  p_param = (LHCI_C1_Write_Register_cmd_t *) pcmd->cmd.payload;

  switch (p_param->busw)
  {
  case LHCI_8bits:
    __disable_irq();

    *(uint8_t *) (p_param->add) = ((*(uint8_t *) (p_param->add)) & (~(p_param->mask)))
        		      | (p_param->val & p_param->mask);

    __set_PRIMASK(primask_bit); /**< Restore PRIMASK bit*/
    break;

  case LHCI_16bits:
    __disable_irq();

    *(uint16_t *) (p_param->add) = ((*(uint16_t *) (p_param->add)) & (~(p_param->mask)))
        		      | (p_param->val & p_param->mask);

    __set_PRIMASK(primask_bit); /**< Restore PRIMASK bit*/
    break;

  default: /**< case SHCI_32BITS */
    __disable_irq();

    *(uint32_t *) (p_param->add) = ((*(uint32_t *) (p_param->add)) & (~(p_param->mask)))
        		      | (p_param->val & p_param->mask);

    __set_PRIMASK(primask_bit); /**< Restore PRIMASK bit*/
    break;
  }

  ((BleEvtSerial_t*) pcmd)->type = TL_LOCRSP_PKT_TYPE;
  ((BleEvtSerial_t*) pcmd)->evt.evtcode = HCI_COMMAND_COMPLETE_EVT_CODE;
  ((BleEvtSerial_t*) pcmd)->evt.plen = TL_EVT_CS_PAYLOAD_SIZE;
  ;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->cmdcode = LHCI_OPCODE_C1_WRITE_REG;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload[0]= 0x00;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->numcmd = 1;

  return;
}

void LHCI_C1_Read_Register( BleCmdSerial_t *pcmd )
{
  LHCI_C1_Read_Register_cmd_t * p_param;
  uint32_t rsp_val;
  uint8_t busw;

  p_param = (LHCI_C1_Read_Register_cmd_t *) ((BleCmdSerial_t*) pcmd)->cmd.payload;
  busw = p_param->busw;

  switch (busw)
  {
  case LHCI_8bits:
    rsp_val = *(uint8_t *) (p_param->add);
    break;

  case LHCI_16bits:
    rsp_val = *(uint16_t *) (p_param->add);
    break;

  default: /**< case SHCI_32BITS */
    rsp_val = *(uint32_t *) (p_param->add);
    break;
  }

  ((BleEvtSerial_t*) pcmd)->type = TL_LOCRSP_PKT_TYPE;
  ((BleEvtSerial_t*) pcmd)->evt.evtcode = HCI_COMMAND_COMPLETE_EVT_CODE;
  ((BleEvtSerial_t*) pcmd)->evt.plen = HCI_COMMAND_HDR_SIZE + sizeof(LHCI_C1_Read_Register_ccrp_t);
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->cmdcode = LHCI_OPCODE_C1_READ_REG;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->numcmd = 1;
  ((LHCI_C1_Read_Register_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->status =
      0x00;
  ((LHCI_C1_Read_Register_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->val = rsp_val;

  return;
}

void LHCI_C1_Read_Device_Information( BleCmdSerial_t *pcmd )
{
  ((BleEvtSerial_t*) pcmd)->type = TL_LOCRSP_PKT_TYPE;
  ((BleEvtSerial_t*) pcmd)->evt.evtcode = HCI_COMMAND_COMPLETE_EVT_CODE;
  ((BleEvtSerial_t*) pcmd)->evt.plen = HCI_COMMAND_HDR_SIZE + sizeof(LHCI_C1_Device_Information_ccrp_t);
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->cmdcode = LHCI_OPCODE_C1_DEVICE_INF;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->numcmd = 1;

  /**
   * status
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->status =
      0x00;

  /**
   * revision id
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->rev_id =
      (uint16_t)LL_DBGMCU_GetRevisionID();

  /**
   * device code id
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->dev_code_id =
      (uint16_t)LL_DBGMCU_GetDeviceID();

  /**
   * package type
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->package_type =
      (uint8_t)LL_GetPackageType();

  /**
   * device type id
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->device_type_id =
      (uint8_t)LL_FLASH_GetDeviceID();

  /**
   * st company id
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->st_company_id =
      LL_FLASH_GetSTCompanyID();

  /**
   * UID64
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->uid64 =
      LL_FLASH_GetUDN();

  /**
   * UID96_0
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->uid96_0 =
      LL_GetUID_Word0();

  /**
   * UID96_1
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->uid96_1 =
      LL_GetUID_Word1();

  /**
   * UID96_2
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->uid96_2 =
      LL_GetUID_Word2();

  /**
   * Reserved Fields
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField1 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField2 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField3 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField4 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField5 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField6 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField7 = 0xFFFFFFFF;
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->ReservedField.ReservedField8 = 0xFFFFFFFF;

  /**
   * AppFwInf
   */
  ((LHCI_C1_Device_Information_ccrp_t *) (((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload))->AppFwInf = FW_Version;

  return;
}

void LHCI_C1_RF_CONTROL_AntennaSwitch( BleCmdSerial_t *pcmd )
{
  LHCI_C1_RF_CONTROL_AntennaSwitch_cmd_t * p_param = (LHCI_C1_RF_CONTROL_AntennaSwitch_cmd_t *) pcmd->cmd.payload;

  /* Call RF CONTROL Antenna Switch system interface */
  RF_CONTROL_AntennaSwitch((rf_antenna_switch_state_t) p_param->enable);

  /* LHCI_OPCODE_C1_RF_CONTROL_ANTENNA_SWITCH command response */
  ((BleEvtSerial_t*) pcmd)->type = TL_LOCRSP_PKT_TYPE;
  ((BleEvtSerial_t*) pcmd)->evt.evtcode = HCI_COMMAND_COMPLETE_EVT_CODE;
  ((BleEvtSerial_t*) pcmd)->evt.plen = TL_EVT_CS_PAYLOAD_SIZE;

  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->cmdcode = LHCI_OPCODE_C1_RF_CONTROL_ANTENNA_SWITCH;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->payload[0]= 0x00;
  ((TL_CcEvt_t *) (((BleEvtSerial_t*) pcmd)->evt.payload))->numcmd = 1;

  return;
}
