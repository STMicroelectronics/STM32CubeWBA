/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "log_module.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "hids_app.h"
#include "hids.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "host_stack_if.h"
#if (CFG_LCD_SUPPORTED == 1)
#include "stm32wba55g_discovery_lcd.h"
#include "stm32_lcd.h"
#endif /* CFG_LCD_SUPPORTED */
#include "app_bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t buttons;
  int8_t x;
  int8_t y;
  int8_t wheel;
} mouse_report_t;

/* USER CODE END PTD */

typedef enum
{
  Inputrep_NOTIFICATION_OFF,
  Inputrep_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  HIDS_APP_SENDINFORMATION_LAST
} HIDS_APP_SendInformation_t;

typedef struct
{
  HIDS_APP_SendInformation_t     Inputrep_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  uint16_t SampleFrequency;
  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} HIDS_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MOUSE_REPORT_SIZE       52

#define MOUSE_LEFT_BUTTON       0x01
#define MOUSE_MIDDLE_BUTTON     0x04
#define MOUSE_RIGHT_BUTTON      0x02

#define MOUSE_SPEED             10
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static HIDS_APP_Context_t HIDS_APP_Context;

uint8_t a_HIDS_UpdateCharData[247];

/* USER CODE BEGIN PV */
#if (CFG_LCD_SUPPORTED == 1)
const uint8_t mouse[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
0x07, 0x03, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0E, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0C, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x18, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0xE0, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
0x30, 0xC1, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0xC3, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x31, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x8E, 0x00, 0x1F, 0x80, 0x00, 0x00, 0x00,
0x31, 0x8C, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x03, 0x8C, 0x00, 0xFF, 0xF8, 0x00, 0x00, 0x00,
0x03, 0x0C, 0x01, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x03, 0xFF, 0xFF, 0x00, 0x00, 0x00,
0x00, 0x08, 0x01, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xC0, 0x00, 0x00,
0x00, 0x00, 0x18, 0x1F, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x38, 0x0F, 0xFF, 0xF0, 0x00, 0x00,
0x00, 0x00, 0x78, 0x07, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x7C, 0x03, 0xFF, 0xE0, 0x00, 0x00,
0x00, 0x00, 0xFE, 0x01, 0xFF, 0x86, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x1F, 0x00, 0x00,
0x00, 0x00, 0xFF, 0x80, 0xFC, 0x7F, 0x80, 0x00, 0x00, 0x01, 0xFF, 0xC0, 0xF8, 0xFF, 0xC0, 0x00,
0x00, 0x01, 0xFF, 0xE0, 0x71, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x27, 0xFF, 0xF0, 0x00,
0x00, 0x00, 0xFF, 0xFE, 0x0F, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x1F, 0xFF, 0xF8, 0x00,
0x00, 0x00, 0x7F, 0xFF, 0x3F, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x7F, 0xFE, 0x7F, 0xFF, 0xFE, 0x00,
0x00, 0x00, 0x3F, 0xFC, 0xFF, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x3F, 0xF8, 0xFF, 0xFF, 0xFF, 0x00,
0x00, 0x00, 0x1F, 0xF9, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x0F, 0xF3, 0xFF, 0xFF, 0xFF, 0x80,
0x00, 0x00, 0x07, 0xE7, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x03, 0xE7, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x01, 0xCF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0xCF, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xC0,
0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0x80,
0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00,
0x00, 0x00, 0x00, 0x00, 0x3F, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFC, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xF0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif /* CFG_LCD_SUPPORTED */

static uint8_t report_mouse[MOUSE_REPORT_SIZE] =
{
  0x05, 0x01,         /* USAGE_PAGE (Generic Desktop) */
  0x09, 0x02,         /* USAGE (Mouse) */
  0xa1, 0x01,         /* COLLECTION (Application) */
  0x09, 0x01,         /*   USAGE (Pointer) */
  0xa1, 0x00,         /*   COLLECTION (Physical) */
  0x05, 0x09,         /*     USAGE_PAGE (Button) */
  0x19, 0x01,         /*     USAGE_MINIMUM (Button 1) */
  0x29, 0x03,         /*     USAGE_MAXIMUM (Button 3) */
  0x15, 0x00,         /*     LOGICAL_MINIMUM (0) */
  0x25, 0x01,         /*     LOGICAL_MAXIMUM (1) */
  0x95, 0x03,         /*     REPORT_COUNT (3) */
  0x75, 0x01,         /*     REPORT_SIZE (1) */
  0x81, 0x02,         /*     INPUT (Data,Var,Abs) */
  0x95, 0x01,         /*     REPORT_COUNT (1) */
  0x75, 0x05,         /*     REPORT_SIZE (5) */
  0x81, 0x03,         /*     INPUT (Cnst,Var,Abs) */
  0x05, 0x01,         /*     USAGE_PAGE (Generic Desktop) */
  0x09, 0x30,         /*     USAGE (X) */
  0x09, 0x31,         /*     USAGE (Y) */
  0x09, 0x38,         /*     USAGE (Wheel) */
  0x15, 0x81,         /*     LOGICAL_MINIMUM (-127) */
  0x25, 0x7f,         /*     LOGICAL_MAXIMUM (127) */
  0x75, 0x08,         /*     REPORT_SIZE (8) */
  0x95, 0x03,         /*     REPORT_COUNT (3) */
  0x81, 0x06,         /*     INPUT (Data,Var,Rel) */
  0xc0,               /*   END_COLLECTION (Physical) */
  0xc0,               /* END_COLLECTION (Application) */
};

static uint8_t usb_hid_version[4] = {0x01, 0x01, 0x00, 0x03}; // BASE USB HID SPEC VERSION
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void HIDS_Inputrep_SendNotification(void);

/* USER CODE BEGIN PFP */
#if (CFG_LCD_SUPPORTED == 1)
static void LCD_DrawIcon(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height, const uint8_t *pIcon, uint8_t inverted);
#endif /* CFG_LCD_SUPPORTED */
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void HIDS_Notification(HIDS_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case HIDS_INPUTREP_READ_EVT:
      /* USER CODE BEGIN Service1Char1_READ_EVT */

      /* USER CODE END Service1Char1_READ_EVT */
      break;

    case HIDS_INPUTREP_WRITE_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_EVT */

      /* USER CODE END Service1Char1_WRITE_EVT */
      break;

    case HIDS_INPUTREP_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      LOG_INFO_APP("HIDS_REP_NOTIFY_ENABLED_EVT\n");
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case HIDS_INPUTREP_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      LOG_INFO_APP("HIDS_REP_NOTIFY_DISABLED_EVT\n");
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case HIDS_REM_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */

      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case HIDS_HII_READ_EVT:
      /* USER CODE BEGIN Service1Char3_READ_EVT */

      /* USER CODE END Service1Char3_READ_EVT */
      break;

    case HIDS_HCP_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char4_WRITE_NO_RESP_EVT */
      if(((p_Notification->DataTransfered.p_Payload[0]) & 0x01) == 0x00)
      {
        LOG_INFO_APP("Suspend\r\n");
      }
      else
      {
        LOG_INFO_APP("Exit suspend\r\n");
      }
      /* USER CODE END Service1Char4_WRITE_NO_RESP_EVT */
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

void HIDS_APP_EvtRx(HIDS_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case HIDS_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */
#if (CFG_LCD_SUPPORTED == 1)
      /* ---- Display MOUSE ---- */
      BSP_LCD_Clear(LCD1, LCD_COLOR_BLACK);
      BSP_LCD_Refresh(LCD1);
      LCD_DrawIcon(0, 0 , 64, 64, (uint8_t *)mouse, TRUE);
      BSP_LCD_Refresh(LCD1);
      UTIL_LCD_DisplayStringAt(0, LINE(0), (uint8_t *)"ST BLE", RIGHT_MODE);
      UTIL_LCD_DisplayStringAt(0, LINE(1), (uint8_t *)"HID Mouse", RIGHT_MODE);
      UTIL_LCD_DisplayStringAt(0, LINE(4), (uint8_t *)"CONNECTED", RIGHT_MODE);
      BSP_LCD_Refresh(LCD1);
#endif
      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case HIDS_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

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

void HIDS_APP_Init(void)
{
  UNUSED(HIDS_APP_Context);
  HIDS_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  HIDS_Data_t msg_conf;
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;

  /* Set the Keyboard Report Map */
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&report_mouse, sizeof(report_mouse));
  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(report_mouse);

  result = HIDS_UpdateValue(HIDS_REM, &msg_conf);
  if( result != BLE_STATUS_SUCCESS )
  {
    LOG_INFO_APP("Sending of Report Map Failed error 0x%X\n", result);
  }
  
  /* Set the HID Information */
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&usb_hid_version, sizeof(usb_hid_version));
  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(usb_hid_version);

  result = HIDS_UpdateValue(HIDS_HII, &msg_conf);
  if( result != BLE_STATUS_SUCCESS )
  {
    LOG_INFO_APP("Sending of HID Information Failed error 0x%X\n", result);
  }
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */
#if (CFG_JOYSTICK_SUPPORTED == 1)
void APP_BSP_JoystickUpAction( void )
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  mouse_report_t mouse_report = {0};
  HIDS_Data_t msg_conf;
  
  mouse_report.y = -1 * MOUSE_SPEED;
  
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));

  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(mouse_report);
  
  result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HIDS_UpdateValue fails\n");
  }  
}

void APP_BSP_JoystickRightAction( void )
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  mouse_report_t mouse_report = {0};
  HIDS_Data_t msg_conf;
  
  mouse_report.x = 1 * MOUSE_SPEED;
  
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));

  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(mouse_report);
  
  result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HIDS_UpdateValue fails\n");
  }  
}

void APP_BSP_JoystickDownAction( void )
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  mouse_report_t mouse_report = {0};
  HIDS_Data_t msg_conf;
  
  mouse_report.y = 1 * MOUSE_SPEED;
  
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));

  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(mouse_report);
  
  result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HIDS_UpdateValue fails\n");
  }  
}

void APP_BSP_JoystickLeftAction( void )
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  mouse_report_t mouse_report = {0};
  HIDS_Data_t msg_conf;
  
  mouse_report.x = -1 * MOUSE_SPEED;
  
  memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
  memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));

  msg_conf.p_Payload = a_HIDS_UpdateCharData;
  msg_conf.Length = sizeof(mouse_report);
  
  result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
  if(result != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("HIDS_UpdateValue fails\n");
  }
}

void APP_BSP_JoystickSelectAction( void )
{
  tBleStatus result = BLE_STATUS_INVALID_PARAMS;
  mouse_report_t mouse_report = {0};
  HIDS_Data_t msg_conf;
  static UTIL_TIMER_Time_t time_snapshot = 0;
  
  /* perform a click only once per second */
  if(UTIL_TIMER_GetCurrentTime() - time_snapshot > 1000)
  {
    mouse_report.buttons |= MOUSE_LEFT_BUTTON;
    
    memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
    memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));

    msg_conf.p_Payload = a_HIDS_UpdateCharData;
    msg_conf.Length = sizeof(mouse_report);
    
    result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
    if(result != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("HIDS_UpdateValue fails\n");
    }
   
    /* Release Button for the selection */
    mouse_report.buttons &= ~MOUSE_LEFT_BUTTON;
    mouse_report.x = 0;
    mouse_report.y = 0;

    memset((void*)a_HIDS_UpdateCharData, 0, sizeof(a_HIDS_UpdateCharData));
    memcpy((void*)a_HIDS_UpdateCharData, (void *)&mouse_report, sizeof(mouse_report));
    
    msg_conf.p_Payload = a_HIDS_UpdateCharData;
    msg_conf.Length = sizeof(mouse_report);
    
    result = HIDS_UpdateValue(HIDS_INPUTREP, &msg_conf);
    if(result != BLE_STATUS_SUCCESS)
    {
      LOG_INFO_APP("HIDS_UpdateValue fails\n");
    }
  }
  
  time_snapshot = UTIL_TIMER_GetCurrentTime();
}
#endif /* CFG_JOYSTICK_SUPPORTED == 1 */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void HIDS_Inputrep_SendNotification(void) /* Property Notification */
{
  HIDS_APP_SendInformation_t notification_on_off = Inputrep_NOTIFICATION_OFF;
  HIDS_Data_t hids_notification_data;

  hids_notification_data.p_Payload = (uint8_t*)a_HIDS_UpdateCharData;
  hids_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1 */

  /* USER CODE END Service1Char1_NS_1 */

  if (notification_on_off != Inputrep_NOTIFICATION_OFF)
  {
    HIDS_UpdateValue(HIDS_INPUTREP, &hids_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last */

  /* USER CODE END Service1Char1_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

#if (CFG_LCD_SUPPORTED == 1)
static void LCD_DrawIcon(uint16_t Xpos, uint16_t Ypos, uint16_t width, uint16_t height, const uint8_t *pIcon, uint8_t inverted)
{
  uint32_t i = 0, j = 0;
  uint8_t bit = 0;
  uint16_t index = 0;

  for(i = 0; i < height; i++)
  {
    for (j = 0; j < width; j++)
    {
      bit = (j % 8);
      if ((pIcon[index] << bit) & (0x80))
      {
        if(inverted == TRUE)
          BSP_LCD_WritePixel(0, (Xpos + j), Ypos + i, LCD_COLOR_WHITE);
        else
          BSP_LCD_WritePixel(0, (Xpos + j), Ypos + i, LCD_COLOR_BLACK);
      }
      else
      {
        if(inverted == TRUE)
          BSP_LCD_WritePixel(0, (Xpos + j), Ypos + i, LCD_COLOR_BLACK);
        else
          BSP_LCD_WritePixel(0, (Xpos + j), Ypos + i, LCD_COLOR_WHITE);
      }
      if (bit == 7)
      {
        index++;
      }
    }
  }
}
#endif /* CFG_LCD_SUPPORTED */
/* USER CODE END FD_LOCAL_FUNCTIONS */
