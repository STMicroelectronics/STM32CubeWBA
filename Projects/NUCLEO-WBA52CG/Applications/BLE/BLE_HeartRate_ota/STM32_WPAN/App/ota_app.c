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
#include "ota_app.h"
#include "ota.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "flash_driver.h"
#include "flash_manager.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */
typedef enum
{
  User_Conf,
  Fw_App,
} OTA_APP_FileType_t;

/* USER CODE END PTD */

typedef enum
{
  Conf_INDICATION_OFF,
  Conf_INDICATION_ON,
  /* USER CODE BEGIN Service3_APP_SendInformation_t */
  OTA_APP_No_Pending,
  OTA_APP_Pending,
  OTA_APP_Ready_Pending,
  /* USER CODE END Service3_APP_SendInformation_t */
  OTA_APP_SENDINFORMATION_LAST
} OTA_APP_SendInformation_t;

typedef struct
{
  OTA_APP_SendInformation_t     Conf_Indication_Status;
  /* USER CODE BEGIN Service3_APP_Context_t */
  uint32_t base_address;
  uint8_t sectors;
  uint32_t write_value[60];
  uint8_t  write_value_index;
  uint8_t  file_type;
  /* USER CODE END Service3_APP_Context_t */
  uint16_t              ConnectionHandle;
} OTA_APP_Context_t;

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
static OTA_APP_Context_t OTA_APP_Context;

uint8_t a_OTA_UpdateCharData[247];

/* USER CODE BEGIN PV */
static uint32_t size_left;
static uint32_t address_offset;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void OTA_Conf_SendIndication(void);

/* USER CODE BEGIN PFP */
static void DeleteSlot( uint8_t page_idx );
static void FM_WriteCallback (FM_FlashOp_Status_t Status);
/* Flag for write status  */
static FM_FlashOp_Status_t FM_WriteStatus;
/* Write test callback */
static FM_CallbackNode_t FM_WriteStatusCallback = 
{
  /* Header for chained list */
  .NodeList = 
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = FM_WriteCallback
};
static void FM_EraseCallback (FM_FlashOp_Status_t Status);
/* Flag for write status  */
static FM_FlashOp_Status_t FM_EraseStatus;
/* Write test callback */
static FM_CallbackNode_t FM_EraseStatusCallback = 
{
  /* Header for chained list */
  .NodeList = 
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = FM_EraseCallback
};
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void OTA_Notification(OTA_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_Notification_1 */
  OTA_Data_t msg_conf;
  /* USER CODE END Service3_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_Notification_Service3_EvtOpcode */
    case OTA_CONF_EVT:
      {
        /**
         * The Remote notifies it has send all the data to be written in Flash
         */
        /**
         * Decide now what to do after all the data has been written in Flash
         */
        switch(OTA_APP_Context.file_type)
        {
          case Fw_App:
            {
              APP_DBG("OTA_CONF_EVT: Reboot on new application\n");
              /**
               * Reboot on FW Application
               */
              CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_FW_APP;
              
              /**
               * Give the download sector
               */
              CFG_OTA_START_SECTOR_IDX_VAL_MSG = (OTA_APP_Context.base_address - FLASH_BASE) >> 13;
     
              NVIC_SystemReset(); /* it waits until reset */
            }
            break;
            
          default:
            break;
        }
      }
      break;

    case OTA_READY_EVT:
      break;
    /* USER CODE END Service3_Notification_Service3_EvtOpcode */

    case OTA_BASE_ADR_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service3Char1_WRITE_NO_RESP_EVT */
      {
        switch( ((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Command )
        {
          case OTA_STOP_ALL_UPLOAD:
            break;

          case OTA_USER_CONF_UPLOAD:
            {
              uint32_t first_valid_address;
              FM_Cmd_Status_t error = FM_ERROR;

              OTA_APP_Context.file_type = User_Conf;
              ((uint8_t*)&OTA_APP_Context.base_address)[0] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[2];
              ((uint8_t*)&OTA_APP_Context.base_address)[1] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[1];
              ((uint8_t*)&OTA_APP_Context.base_address)[2] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[0];
              OTA_APP_Context.base_address |= FLASH_BASE;
              if(p_Notification->DataTransfered.Length > 4)
              {
                OTA_APP_Context.sectors = ((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Sectors;
              }
              else
              {
                OTA_APP_Context.sectors = 0;
              }
              
              OTA_APP_Context.write_value_index = 0;
              OTA_APP_Context.Conf_Indication_Status = OTA_APP_Ready_Pending;
              first_valid_address = CFG_USER_CFG_SLOT_START_SECTOR_INDEX * FLASH_PAGE_SIZE + FLASH_BASE;
              if(((OTA_APP_Context.base_address & 0xF) == 0) &&
                 (OTA_APP_Context.sectors <= CFG_USER_CFG_NB_SECTORS) &&
                 (OTA_APP_Context.base_address >= first_valid_address) &&
                 ((OTA_APP_Context.base_address + ((OTA_APP_Context.sectors) * FLASH_PAGE_SIZE)) <= (first_valid_address + (CFG_USER_CFG_NB_SECTORS * FLASH_PAGE_SIZE))))
              { /* Download address is 128 bits aligned */
                /* Size of file to download fit in user configuration slot */
                /* Download address is in the user configuration area */
                /* End download address fit in the download area */
                /* Erase the sector */
                /* Clear events before start testing */
                UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                while(error != FM_OK)
                {  
                  /* Flash manager erase */
                  if(OTA_APP_Context.sectors == 0)
                    OTA_APP_Context.sectors = CFG_USER_CFG_NB_SECTORS;
                  error = FM_Erase((uint32_t)CFG_USER_CFG_SLOT_START_SECTOR_INDEX, 
                                   (uint32_t)(OTA_APP_Context.sectors),
                                   &FM_EraseStatusCallback);

                  /* Check write op. */
                  if (error == FM_OK)
                  {
                    /* Wait for write callback to be invoked */
                    UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                    /* Clear events before start testing */
                    UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                    /* Check status of write op. */
                    if (FM_EraseStatus != FM_OPERATION_COMPLETE)
                    {
                      error = FM_ERROR;
                      APP_DBG("OTA_USER_CONF_UPLOAD: FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
                    }
                  }
                  else if(error == FM_BUSY)
                  {
                    /* Wait for write callback to be invoked */
                    UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                    
                    /* Clear events before start testing */
                    UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  }
                  else
                  {
                    APP_DBG("OTA_USER_CONF_UPLOAD: FM_ERROR\n");
                  }
                } /* while(error != FM_OK) */

                a_OTA_UpdateCharData[0] = OTA_READY_TO_RECEIVE_FILE;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                msg_conf.Length = 1;
                OTA_UpdateValue(OTA_CONF, &msg_conf);
              }
              else
              {
                APP_DBG("OTA_USER_CONF_UPLOAD: Not ready to receive file, oversized\n", OTA_APP_Context.base_address, OTA_APP_Context.sectors);
                APP_DBG("OTA_USER_CONF_UPLOAD: First 128 bits aligned address to download should be: 0x%x\n", first_valid_address);
                a_OTA_UpdateCharData[0] = OTA_NOT_READY_TO_RECEIVE_FILE;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                msg_conf.Length = 1;
                OTA_UpdateValue(OTA_CONF, &msg_conf);
              }
            }
            break;

          case OTA_APPLICATION_UPLOAD:
            {
              uint32_t first_valid_address;
              FM_Cmd_Status_t error = FM_ERROR;
              
              OTA_APP_Context.file_type = Fw_App;
              ((uint8_t*)&OTA_APP_Context.base_address)[0] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[2];
              ((uint8_t*)&OTA_APP_Context.base_address)[1] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[1];
              ((uint8_t*)&OTA_APP_Context.base_address)[2] = (((uint8_t*)((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Base_Addr))[0];
              OTA_APP_Context.base_address |= FLASH_BASE;
              OTA_APP_Context.sectors = 0;
              if(p_Notification->DataTransfered.Length > 4)
              {
                OTA_APP_Context.sectors = ((OTA_Base_Addr_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Sectors;
              }
              else
              {
                OTA_APP_Context.sectors = CFG_DOWNLOAD_ACTIVE_NB_SECTORS;
              }
              
              OTA_APP_Context.write_value_index = 0;
              OTA_APP_Context.Conf_Indication_Status = OTA_APP_Ready_Pending;
              first_valid_address = ((CFG_ACTIVE_SLOT_START_SECTOR_INDEX + CFG_APP_SLOT_PAGE_SIZE) * FLASH_PAGE_SIZE) + FLASH_BASE;
              if(((OTA_APP_Context.base_address & 0xF) == 0) &&
                 (OTA_APP_Context.sectors <= CFG_DOWNLOAD_ACTIVE_NB_SECTORS) &&
                 (OTA_APP_Context.base_address >= first_valid_address) &&
                 ((OTA_APP_Context.base_address + ((OTA_APP_Context.sectors) * FLASH_PAGE_SIZE)) <= (first_valid_address + (CFG_APP_SLOT_PAGE_SIZE * FLASH_PAGE_SIZE))))
              { /* Download address is 128 bits aligned */
                /* Size of file to download fit in download slot */
                /* Download address is in the download area */
                /* End download address fit in the download area */
                /* Erase the sectors */
                /* Clear events before start testing */
                UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                while(error != FM_OK)
                {  
                  /* Flash manager write */
                  if(OTA_APP_Context.sectors == 0)
                    OTA_APP_Context.sectors = CFG_DOWNLOAD_ACTIVE_NB_SECTORS;
                  error = FM_Erase((uint32_t)((OTA_APP_Context.base_address - FLASH_BASE) >> 13), 
                                   (uint32_t)(OTA_APP_Context.sectors),
                                   &FM_EraseStatusCallback);

                  /* Check write op. */
                  if (error == FM_OK)
                  {
                    /* Wait for write callback to be invoked */
                    UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                    /* Clear events before start testing */
                    UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);

                    /* Check status of write op. */
                    if (FM_EraseStatus != FM_OPERATION_COMPLETE)
                    {
                      error = FM_ERROR;
                      APP_DBG("OTA_APPLICATION_UPLOAD: FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
                    }
                  }
                  else if(error == FM_BUSY)
                  {
                    /* Wait for write callback to be invoked */
                    UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                    
                    /* Clear events before start testing */
                    UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
                  }
                  else
                  {
                    APP_DBG("OTA_APPLICATION_UPLOAD: FM_ERROR\n");
                  }
                } /* while(error != FM_OK) */
                
                msg_conf.Length = 1;
                a_OTA_UpdateCharData[0] = OTA_READY_TO_RECEIVE_FILE;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                OTA_UpdateValue(OTA_CONF, &msg_conf);
              }
              else
              {
                msg_conf.Length = 1;
                APP_DBG("OTA_APPLICATION_UPLOAD: Not ready to receive file, oversized\n", OTA_APP_Context.base_address, OTA_APP_Context.sectors);
                APP_DBG("OTA_APPLICATION_UPLOAD: First 128 bits aligned address to download should be: 0x%x\n", first_valid_address);
                a_OTA_UpdateCharData[0] = OTA_NOT_READY_TO_RECEIVE_FILE;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                OTA_UpdateValue(OTA_CONF, &msg_conf);
              }
            }
            break;

          case OTA_UPLOAD_FINISHED:
            {
              if(OTA_APP_Context.file_type == Fw_App)
              { /* Reboot only after new application download */
                OTA_APP_Context.Conf_Indication_Status = OTA_APP_Pending;
                msg_conf.Length = 1;
                a_OTA_UpdateCharData[0] = OTA_REBOOT_CONFIRMED;
                msg_conf.p_Payload = a_OTA_UpdateCharData;
                OTA_UpdateValue(OTA_CONF, &msg_conf);
              }
            }
            break;

          case OTA_CANCEL_UPLOAD:
            {
              APP_DBG("OTA_CANCEL_UPLOAD\n");
            }
            break;

          default:
            break;
        }
      }
      /* USER CODE END Service3Char1_WRITE_NO_RESP_EVT */
      break;

    case OTA_CONF_INDICATE_ENABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_ENABLED_EVT */
      APP_DBG("OTA_CONF_INDICATE_ENABLED_EVT\n");
      /* USER CODE END Service3Char2_INDICATE_ENABLED_EVT */
      break;

    case OTA_CONF_INDICATE_DISABLED_EVT:
      /* USER CODE BEGIN Service3Char2_INDICATE_DISABLED_EVT */
      APP_DBG("OTA_CONF_INDICATE_DISABLED_EVT\n");
      /* USER CODE END Service3Char2_INDICATE_DISABLED_EVT */
      break;

    case OTA_RAW_DATA_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service3Char3_WRITE_NO_RESP_EVT */
      {
        FM_Cmd_Status_t error = FM_ERROR;
        
        /**
         * Write in Flash the data received in the BLE packet
         */
        size_left = p_Notification->DataTransfered.Length;
        
        /**
         * For the flash manager the address of the data to be stored in FLASH shall be 32bits aligned
         * and the address where the data shall be written shall be 128bits aligned
         */
        memcpy( (uint8_t*)&OTA_APP_Context.write_value,
                ((OTA_Raw_Data_Event_Format_t*)(p_Notification->DataTransfered.p_Payload))->Raw_Data,
                size_left );

        /* Clear events before start testing */
        UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);

        while(error != FM_OK)
        {  
          /* Flash manager write */
          error = FM_Write ((uint32_t *)(&OTA_APP_Context.write_value[0]),
                            (uint32_t *)((OTA_APP_Context.base_address) + address_offset),
                            size_left >> 2,
                            &FM_WriteStatusCallback);

          /* Check write op. */
          if (error == FM_OK)
          {
            /* Wait for write callback to be invoked */
            UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);

            /* Clear events before start testing */
            UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);

            /* Check status of write op. */
            if (FM_WriteStatus != FM_OPERATION_COMPLETE)
            {
              error = FM_ERROR;
              APP_DBG("OTA_RAW_DATA_ID: FM_WriteStatus != FM_OPERATION_COMPLETE => FM_ERROR\n");
            }
          }
          else if(error == FM_BUSY)
          {
            /* Wait for write callback to be invoked */
            UTIL_SEQ_WaitEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
            
            /* Clear events before start testing */
            UTIL_SEQ_ClrEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
          }
          else
          {
            APP_DBG("OTA_RAW_DATA_ID: FM_ERROR\n");
          }
        } /* while(error != FM_OK) */

        /* Update write offset address for the next FLASH write */
        address_offset += size_left;
      }
      /* USER CODE END Service3Char3_WRITE_NO_RESP_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_Notification_default */

      /* USER CODE END Service3_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service3_Notification_2 */

  /* USER CODE END Service3_Notification_2 */
  return;
}

void OTA_APP_EvtRx(OTA_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service3_APP_EvtRx_1 */

  /* USER CODE END Service3_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service3_APP_EvtRx_Service3_EvtOpcode */

    /* USER CODE END Service3_Notification_Service3_EvtOpcode */
    case OTA_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service3_APP_CONN_HANDLE_EVT */
      break;

    case OTA_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service3_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service3_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service3_APP_EvtRx_default */

      /* USER CODE END Service3_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service3_APP_EvtRx_2 */

  /* USER CODE END Service3_APP_EvtRx_2 */

  return;
}

void OTA_APP_Init(void)
{
  UNUSED(OTA_APP_Context);
  OTA_Init();

  /* USER CODE BEGIN Service3_APP_Init */
  DeleteSlot( CFG_DOWNLOAD_SLOT_START_SECTOR_INDEX ); /* Erase download slot */
  FM_WriteStatus = FM_OPERATION_AVAILABLE;
  size_left = 0;
  address_offset = 0;
  /* USER CODE END Service3_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
/**
 * Get Confiramation status
 */
uint8_t OTA_APP_GetConfStatus(void)
{
  return(OTA_APP_Context.Conf_Indication_Status);
}

/**
 * Erase active or download slot
 */
static void DeleteSlot( uint8_t page_idx )
{

  /**
   * The number of sectors to erase is read from SRAM1.
   * It shall be checked whether the number of sectors to erase does not overlap on the secured Flash
   * The limit can be read from the SFSA option byte which provides the first secured sector address.
   */

  uint32_t last_page_idx = page_idx + CFG_APP_SLOT_PAGE_SIZE - 1;
  FLASH_EraseInitTypeDef p_erase_init;
  uint32_t page_error;
  uint32_t NbrOfPageToBeErased = (uint32_t)CFG_APP_SLOT_PAGE_SIZE;

  if(page_idx < CFG_ACTIVE_SLOT_START_SECTOR_INDEX)
  {
    /**
     * Something has been wrong as there is no case we should delete the BLE_BootMngr application
     * Reboot on the active firmware application
     */
    CFG_OTA_REBOOT_VAL_MSG = CFG_REBOOT_ON_FW_APP;
    NVIC_SystemReset(); /* it waits until reset */
  }

  if ((page_idx + NbrOfPageToBeErased - 1) > last_page_idx)
  {
    NbrOfPageToBeErased = last_page_idx - page_idx + 1;
  }

  p_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
  p_erase_init.NbPages = NbrOfPageToBeErased;
  p_erase_init.Page = (uint32_t)page_idx;

  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS); /* Clear all Flash flags before write operation*/
  
  HAL_FLASH_Unlock();

  HAL_FLASHEx_Erase(&p_erase_init, &page_error);

  HAL_FLASH_Lock();
  
  return;
}

static void FM_WriteCallback (FM_FlashOp_Status_t Status)
{
  /* Update status */ 
  FM_WriteStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT_RSP_ID);
}

static void FM_EraseCallback (FM_FlashOp_Status_t Status)
{
  /* Update status */ 
  FM_EraseStatus = Status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt ( 1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT_RSP_ID);
}
/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void OTA_Conf_SendIndication(void) /* Property Indication */
{
  OTA_APP_SendInformation_t indication_on_off = Conf_INDICATION_OFF;
  OTA_Data_t ota_indication_data;

  ota_indication_data.p_Payload = (uint8_t*)a_OTA_UpdateCharData;
  ota_indication_data.Length = 0;

  /* USER CODE BEGIN Service3Char2_IS_1*/

  /* USER CODE END Service3Char2_IS_1*/

  if (indication_on_off != Conf_INDICATION_OFF)
  {
    OTA_UpdateValue(OTA_CONF, &ota_indication_data);
  }

  /* USER CODE BEGIN Service3Char2_IS_Last*/

  /* USER CODE END Service3Char2_IS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/* USER CODE END FD_LOCAL_FUNCTIONS*/
