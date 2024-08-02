/**
******************************************************************************
* @file    app_menu_cfg.c
* @author  MCD Application Team
* @brief   Configuration interface of menu for application
******************************************************************************
* @attention
*
* Copyright (c) 2020-2021 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "app_menu_cfg.h"
#include "app_menu.h"
#include "app_conf.h"
#include "ble_gap_aci.h"
#include "log_module.h"
#include "stm32_timer.h"

/* External variables ------------------------------------------------------- */
extern uint8_t volume_mute_byteicon[];
extern uint8_t volume_up_byteicon[];
extern uint8_t volume_down_byteicon[];
extern uint8_t initiate_call_byteicon[];
extern uint8_t answer_call_byteicon[];
extern uint8_t terminate_call_byteicon[];
extern uint8_t microphone_mute_byteicon[];
extern uint8_t next_preset_byteicon[];
extern uint8_t previous_preset_byteicon[];
extern uint8_t advertising_byteicon[];
extern uint8_t hourglass_byteicon[];
extern uint8_t input_byteicon[];
extern uint8_t input_output_byteicon[];
extern uint8_t output_byteicon[];
extern uint8_t bluetooth_byteicon[];
extern uint8_t stlogo_byteicon[];
extern uint8_t homelogo_byteicon[];
extern uint8_t arrow_right_byteicon[];
extern uint8_t arrow_up_byteicon[];
extern uint8_t arrow_down_byteicon[];
extern uint8_t arrow_left_byteicon[];

/* Private defines ---------------------------------------------------------- */
#define ADVERTISING_TIMEOUT 60000
#define STARTUP_TIMEOUT 1500

/* Private variables -------------------------------------------------------- */
Menu_Icon_t access_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_right_byteicon, 16, 16, 0};
Menu_Icon_t return_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_left_byteicon, 16, 16, 0};
Menu_Icon_t up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_up_byteicon, 16, 16, 0};
Menu_Icon_t down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_down_byteicon, 16, 16, 0};
Menu_Icon_t homelogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &homelogo_byteicon, 128, 64, 0};
Menu_Icon_t bluetooth_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &bluetooth_byteicon, 16, 16, 0};
Menu_Icon_t stlogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &stlogo_byteicon, 16, 16, 0};
Menu_Icon_t volume_up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_up_byteicon, 16, 16, 0};
Menu_Icon_t volume_down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_down_byteicon, 16, 16, 0};
Menu_Icon_t volume_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_mute_byteicon, 16, 16, 0};
Menu_Icon_t initiate_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &initiate_call_byteicon, 16, 16, 0};
Menu_Icon_t answer_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &answer_call_byteicon, 16, 16, 0};
Menu_Icon_t terminate_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &terminate_call_byteicon, 16, 16, 0};
Menu_Icon_t microphone_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &microphone_mute_byteicon, 16, 16, 0};
Menu_Icon_t next_preset_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &next_preset_byteicon, 16, 16, 0};
Menu_Icon_t previous_preset_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &previous_preset_byteicon, 16, 16, 0};
Menu_Icon_t advertising_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &advertising_byteicon, 16, 16, 0};
Menu_Icon_t hourglass_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &hourglass_byteicon, 16, 16, 0};
Menu_Icon_t input_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_byteicon, 16, 16, 0};
Menu_Icon_t input_output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_output_byteicon, 16, 16, 0};
Menu_Icon_t output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &output_byteicon, 16, 16, 0};

Menu_Content_Text_t startup_text = {3, {"BLE Audio", "HAP Peripheral", "Press Right"}};
Menu_Content_Text_t csip_config_text = {2, {"Audio Config:", "Banded HA"}};
Menu_Content_Text_t waitcon_text = {2, {"Advertising as", "STM32WBA_0000"}};
Menu_Content_Text_t connecting_text = {1, {"Connecting"}};
Menu_Content_Text_t nostream_text = {3, {"Connected", "No Stream", ""}};
Menu_Content_Text_t streaming_text = {2, {"Streaming 48KHz", ""}};
Menu_Content_Text_t volume_text = {1, {"Volume"}};
Menu_Content_Text_t preset_text = {2, {"Preset 1", "Preset Name"}};
Menu_Content_Text_t call_text = {2, {"Call State", "IDLE"}};
Menu_Content_Text_t microphone_text = {2, {"Microphone", "Mute OFF"}};

Menu_Page_t *p_stlogo_menu;
Menu_Page_t *p_startup_menu;
Menu_Page_t *p_csip_config_menu;
Menu_Page_t *p_waitcon_menu;
Menu_Page_t *p_connecting_menu;
Menu_Page_t *p_nostream_menu;
Menu_Page_t *p_streaming_menu;
Menu_Page_t *p_control_menu;
Menu_Page_t *p_volume_control_menu;
Menu_Page_t *p_call_control_menu;
Menu_Page_t *p_microphone_control_menu;
Menu_Page_t *p_preset_control_menu;
Menu_Page_t *p_config_menu;

static UTIL_TIMER_Object_t Advertising_Timer;
#if (CFG_JOYSTICK_SUPPORTED == 1)
static UTIL_TIMER_Object_t Startup_Timer;
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

static uint8_t csip_conf_id = 0;
static uint8_t app_initialization_done = 0;

/* Private functions prototypes-----------------------------------------------*/
static void Menu_Start_Advertising(void);
static void Menu_CSIP_Conf_Up(void);
static void Menu_CSIP_Conf_Down(void);
static void Menu_Stop_Advertising(void);
static void Menu_Volume_Up(void);
static void Menu_Volume_Down(void);
static void Menu_Volume_Mute(void);
static void Menu_Call_Answer(void);
static void Menu_Call_Terminate(void);
static void Menu_Microphone_Mute(void);
static void Menu_Preset_NextPreset(void);
static void Menu_Preset_PreviousPreset(void);
static void Menu_Disconnect(void);
static void Menu_ClearSecDB(void);
static void Menu_Advertising_TimerCallback(void *arg);
#if (CFG_JOYSTICK_SUPPORTED == 1)
static void Menu_Startup_TimerCallback(void *arg);
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
/* Exported Functions Definition -------------------------------------------- */
/**
 * @brief Initialize and setup the menu
 */
void Menu_Config(void)
{
  Menu_Action_t access_config_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t clear_db_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_ClearSecDB, 0};
  Menu_Action_t access_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t csip_config_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t csip_config_up_action = {MENU_ACTION_CALLBACK, &up_icon, &Menu_CSIP_Conf_Up, 0};
  Menu_Action_t csip_config_down_action = {MENU_ACTION_CALLBACK, &down_icon, Menu_CSIP_Conf_Down, 0};
  Menu_Action_t start_advertising_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Advertising, 0};
  Menu_Action_t stop_advertising_action = {MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_Stop_Advertising, 0};

  Menu_Action_t entry_menu_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_call_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_microphone_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_preset_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_disconnect_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Disconnect, 0};

  Menu_Action_t volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Volume_Up, 0};
  Menu_Action_t volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Volume_Down, 0};
  Menu_Action_t volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Volume_Mute, 0};

  Menu_Action_t call_control_up_action = {MENU_ACTION_CALLBACK, &answer_call_icon, &Menu_Call_Answer, 0};
  Menu_Action_t call_control_down_action = {MENU_ACTION_CALLBACK, &terminate_call_icon, &Menu_Call_Terminate, 0};

  Menu_Action_t microphone_control_right_action = {MENU_ACTION_CALLBACK, &microphone_mute_icon, &Menu_Microphone_Mute, 0, 0};

  Menu_Action_t preset_control_up_action = {MENU_ACTION_CALLBACK, &next_preset_icon, &Menu_Preset_NextPreset, 0};
  Menu_Action_t preset_control_down_action = {MENU_ACTION_CALLBACK, &previous_preset_icon, &Menu_Preset_PreviousPreset, 0};

  Menu_Init();

  p_stlogo_menu = Menu_CreatePage(MENU_TYPE_LOGO);
  p_startup_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_csip_config_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_waitcon_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_connecting_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_nostream_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_streaming_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_control_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_volume_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_call_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_microphone_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_preset_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_config_menu = Menu_CreatePage(MENU_TYPE_LIST);

  csip_config_action.pPage = (struct Menu_Page_t*) p_csip_config_menu;
  start_advertising_action.pPage = (struct Menu_Page_t*) p_waitcon_menu;
  access_config_menu_action.pPage = (struct Menu_Page_t*) p_config_menu;
  access_menu_action.pPage = (struct Menu_Page_t*) p_control_menu;
  entry_menu_volume_action.pPage = (struct Menu_Page_t*) p_volume_control_menu;
  entry_menu_call_action.pPage = (struct Menu_Page_t*) p_call_control_menu;
  entry_menu_microphone_action.pPage = (struct Menu_Page_t*) p_microphone_control_menu;
  entry_menu_preset_action.pPage = (struct Menu_Page_t*) p_preset_control_menu;

  Menu_SetControlContent(p_startup_menu, &startup_text, &stlogo_icon);
  Menu_SetControlAction(p_startup_menu, MENU_DIRECTION_RIGHT, access_config_menu_action);

  Menu_SetControlContent(p_csip_config_menu, &csip_config_text, 0);
  Menu_SetControlAction(p_csip_config_menu, MENU_DIRECTION_UP, csip_config_up_action);
  Menu_SetControlAction(p_csip_config_menu, MENU_DIRECTION_DOWN, csip_config_down_action);

  Menu_SetControlContent(p_waitcon_menu, &waitcon_text, &advertising_icon);
  Menu_SetControlAction(p_waitcon_menu, MENU_DIRECTION_LEFT, stop_advertising_action);

  Menu_SetControlContent(p_connecting_menu, &connecting_text, &hourglass_icon);

  Menu_SetControlContent(p_nostream_menu, &nostream_text, 0);
  Menu_SetControlAction(p_nostream_menu, MENU_DIRECTION_RIGHT, access_menu_action);

  Menu_SetControlContent(p_streaming_menu, &streaming_text, 0);
  Menu_SetControlAction(p_streaming_menu, MENU_DIRECTION_RIGHT, access_menu_action);

  Menu_SetLogo(p_stlogo_menu, &homelogo_icon);

  Menu_AddListEntry(p_control_menu, "Volume...", entry_menu_volume_action);
  Menu_AddListEntry(p_control_menu, "Call...", entry_menu_call_action);
  Menu_AddListEntry(p_control_menu, "Microphone...", entry_menu_microphone_action);
  Menu_AddListEntry(p_control_menu, "Preset...", entry_menu_preset_action);
  Menu_AddListEntry(p_control_menu, "Disconnect", entry_menu_disconnect_action);

  Menu_AddListEntry(p_config_menu, "Start Adv.", start_advertising_action);
  Menu_AddListEntry(p_config_menu, "Audio Config", csip_config_action);
  Menu_AddListEntry(p_config_menu, "Clear Sec. DB", clear_db_action);

  Menu_SetControlContent(p_volume_control_menu, &volume_text, 0);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_UP, volume_control_up_action);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_DOWN, volume_control_down_action);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_RIGHT, volume_control_right_action);

  Menu_SetControlContent(p_call_control_menu,&call_text, 0);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_UP, call_control_up_action);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_DOWN, call_control_down_action);

  Menu_SetControlContent(p_microphone_control_menu,&microphone_text, 0);
  Menu_SetControlAction(p_microphone_control_menu, MENU_DIRECTION_RIGHT, microphone_control_right_action);

  Menu_SetControlContent(p_preset_control_menu, &preset_text, 0);
  Menu_SetControlAction(p_preset_control_menu, MENU_DIRECTION_UP, preset_control_up_action);
  Menu_SetControlAction(p_preset_control_menu, MENU_DIRECTION_DOWN, preset_control_down_action);

#if (CFG_JOYSTICK_SUPPORTED == 1)
  Menu_SetActivePage(p_stlogo_menu);
  UTIL_TIMER_Create(&Startup_Timer, STARTUP_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Startup_TimerCallback, 0);
  UTIL_TIMER_Start(&Startup_Timer);
#else /*(CFG_JOYSTICK_SUPPORTED == 1)*/
  HAPAPP_Init(0);
  uint8_t status = HAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
  LOG_INFO_APP("HAPAPP_StartAdvertising() returns status 0x%02X\n",status);
  Menu_SetActivePage(p_waitcon_menu);
#endif /*(CFG_JOYSTICK_SUPPORTED == 1)*/
}

/**
 * @brief Set the connecting page as active
 */
void Menu_SetConnectingPage(void)
{
  Menu_SetActivePage(p_connecting_menu);
  UTIL_TIMER_Stop(&Advertising_Timer);
}

/**
 * @brief Set the NoStream Page as active
 */
void Menu_SetNoStreamPage(void)
{
  Menu_SetActivePage(p_nostream_menu);
}

/**
 * @brief Set the Streaming page as active
 * @param pSamplerateText: Samplerate string of the stream
 * @param AudioRole: Audio Role of the stream
 */
void Menu_SetStreamingPage(char* pSamplerateText, Audio_Role_t AudioRole)
{
  snprintf(&streaming_text.Lines[0][0], MENU_CONTROL_MAX_LINE_LEN, "Streaming %s", pSamplerateText);
  if (AudioRole == AUDIO_ROLE_SINK)
  {
    p_streaming_menu->pIcon = &input_icon;
  }
  else if (AudioRole == AUDIO_ROLE_SOURCE)
  {
    p_streaming_menu->pIcon = &output_icon;
  }
  else
  {
    p_streaming_menu->pIcon = &input_output_icon;
  }
  Menu_SetActivePage(p_streaming_menu);
}

/**
 * @brief Set the WaitConnection Page as active
 */
void Menu_SetWaitConnPage(void)
{
  Menu_SetActivePage(p_waitcon_menu);
}

/**
 * @brief Set the Startup Page as active
 */
void Menu_SetStartupPage(void)
{
  Menu_SetActivePage(p_startup_menu);
}

/**
 * @brief Set the 4 characters Device Identifier
 */
void Menu_SetIdentifier(char *pId)
{
  UTIL_MEM_cpy_8(&waitcon_text.Lines[1][9], pId, 4);
}

/**
 * @brief Set Current volume value
 */
void Menu_SetVolume(uint8_t Volume)
{
  snprintf(nostream_text.Lines[2], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(streaming_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(volume_text.Lines[0], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  Menu_Print();
}

/**
 * @brief Set Current Microphone Mute Value
 */
void Menu_SetMicMute(uint8_t Mute)
{
  if (Mute == 1u)
  {
    snprintf(microphone_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Mute ON");
  }
  else
  {
    snprintf(microphone_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Mute OFF");
  }
  Menu_Print();
}

/**
 * @brief Set Call State
 */
void Menu_SetCallState(char *pCallState)
{
  snprintf(call_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s", pCallState);
  Menu_Print();
}

/**
 * @brief Set Preset ID
 */
void Menu_SetPresetID(uint8_t ID)
{
  snprintf(preset_text.Lines[0], MENU_CONTROL_MAX_LINE_LEN, "Preset %d", ID);
  Menu_Print();
}

/**
 * @brief Set Preset Name
 */
void Menu_SetPresetName(char *pMediaState)
{
  snprintf(preset_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s", pMediaState);
  Menu_Print();
}

/* Private Functions Definition --------------------------------------------- */

/**
 * @brief CSIP Conf Up Callback
 */
static void Menu_CSIP_Conf_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] CSIP Conf Up\n");

  csip_conf_id = (csip_conf_id + 1) % (APP_CSIP_SET_MEMBER_SIZE + 2);

  switch (csip_conf_id)
  {
    case 0:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Banded HA");
      break;
    }
    case 1:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Monaural HA");
      break;
    }
    case 2:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Left Binaural HA");
      break;
    }
    case 3:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Right Binaural HA");
      break;
    }
    default:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Unknown Device");
    }
  }
  Menu_Print();
}

/**
 * @brief CSIP Conf Down Callback
 */
static void Menu_CSIP_Conf_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] CSIP Conf Down\n");
  if (csip_conf_id > 0)
  {
    csip_conf_id--;
  }
  else
  {
    csip_conf_id = APP_CSIP_SET_MEMBER_SIZE + 1;
  }

  switch (csip_conf_id)
  {
    case 0:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Banded HA");
      break;
    }
    case 1:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Monaural HA");
      break;
    }
    case 2:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Left Binaural HA");
      break;
    }
    case 3:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Right Binaural HA");
      break;
    }
    default:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Unknown Device");
    }
  }
  Menu_Print();
}

/**
 * @brief Start Advertising Callback
 */
static void Menu_Start_Advertising(void)
{
  uint8_t status;
  uint16_t appearance;
  LOG_INFO_APP("[APP_MENU_CONF] Start Advertising\n");
  if (app_initialization_done == 0u)
  {

    HAPAPP_Init(csip_conf_id);
    if (csip_conf_id > 1)
    {
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
      /* Register CSIS */
      status = CSIPAPP_RegisterCSIS(0,
                                    CSIP_SIRK_IS_NOT_OOB,
                                    0x01, /*0x00 : Encrypted, 0x01 : PlainText*/
                                    (uint8_t *)APP_SIRK,
                                    APP_CSIP_SET_MEMBER_SIZE,
                                    csip_conf_id - 1);
      LOG_INFO_APP("Register Set Member rank %d/%d returns status 0x%02X\n",
                   csip_conf_id - 1,
                   APP_CSIP_SET_MEMBER_SIZE,
                   status);
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
    }
    appearance = GAP_APPEARANCE_GENERIC_HEARING_AID;
    app_initialization_done = 1u;
    /* Remove Action buttons */
    p_csip_config_menu->ActionDown.ActionType = 0;
    p_csip_config_menu->ActionUp.ActionType = 0;
  }

  /* Start Advertising */
  status = HAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, appearance);
  LOG_INFO_APP("HAPAPP_StartAdvertising() returns status 0x%02X\n",status);
  UTIL_TIMER_Create(&Advertising_Timer, ADVERTISING_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Advertising_TimerCallback, 0);
  UTIL_TIMER_Start(&Advertising_Timer);
  UNUSED(status);
}
/**
 * @brief Start Advertising Callback
 */
static void Menu_Stop_Advertising(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Stop Advertising\n");
  status = HAPAPP_StopAdvertising();
  LOG_INFO_APP("HAPAPP_StopAdvertising() returns status 0x%02X\n",status);
  UTIL_TIMER_Stop(&Advertising_Timer);
  UNUSED(status);
}

/**
 * @brief Volume Up Callback
 */
static void Menu_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  HAPAPP_VolumeUp();
}

/**
 * @brief Volume Down Callback
 */
static void Menu_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  HAPAPP_VolumeDown();
}

/**
 * @brief Volume Mute Callback
 */
static void Menu_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  HAPAPP_ToggleMute();
}

/**
 * @brief Call Answer Callback
 */
static void Menu_Call_Answer(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Answer Call\n");
  HAPAPP_AnswerCall();
}

/**
 * @brief Call terminate Callback
 */
static void Menu_Call_Terminate(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Terminate Call\n");
  HAPAPP_TerminateCall();
}

/**
 * @brief Mute Microphone Callback
 */
static void Menu_Microphone_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Microphone Mute\n");
  HAPAPP_ToggleMicrophoneMute();
}

/**
 * @brief Next Preset Callback
 */
static void Menu_Preset_NextPreset(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Next Preset\n");
  HAPAPP_NextPreset();
}

/**
 * @brief Previous preset Callback
 */
static void Menu_Preset_PreviousPreset(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Previous Preset\n");
  HAPAPP_PreviousPreset();
}

/**
 * @brief Disconnect Callback
 */
static void Menu_Disconnect(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Disconnect\n");
  HAPAPP_Disconnect();
  Menu_SetActivePage(p_waitcon_menu);
}

/**
 * @brief Clear Security Database Callback
 */
static void Menu_ClearSecDB(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Clear Security DB\n");
  aci_gap_clear_security_db();
  Menu_SetActivePage(p_startup_menu);
}

/**
 * @brief Advertising Timer Callback
 */
static void Menu_Advertising_TimerCallback(void *arg)
{
  LOG_INFO_APP("[APP_MENU_CONF] Timer Expired, stop advertising\n");
  Menu_SetStartupPage();
  Menu_Stop_Advertising();
}

#if (CFG_JOYSTICK_SUPPORTED == 1)
/**
 * @brief Startup Timer Callback
 */
static void Menu_Startup_TimerCallback(void *arg)
{
  LOG_INFO_APP("[APP_MENU_CONF] Startup timer expired, Display home\n");
  Menu_SetStartupPage();
}
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
