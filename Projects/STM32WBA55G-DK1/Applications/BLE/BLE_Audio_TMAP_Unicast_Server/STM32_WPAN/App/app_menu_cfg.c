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

/* External variables ------------------------------------------------------- */
extern uint8_t volume_mute_byteicon[];
extern uint8_t volume_up_byteicon[];
extern uint8_t volume_down_byteicon[];
extern uint8_t initiate_call_byteicon[];
extern uint8_t answer_call_byteicon[];
extern uint8_t terminate_call_byteicon[];
extern uint8_t play_pause_byteicon[];
extern uint8_t next_track_byteicon[];
extern uint8_t previous_track_byteicon[];
extern uint8_t advertising_byteicon[];
extern uint8_t hourglass_byteicon[];
extern uint8_t input_byteicon[];
extern uint8_t input_output_byteicon[];
extern uint8_t output_byteicon[];
extern uint8_t bluetooth_byteicon[];
extern uint8_t stlogo_byteicon[];
extern uint8_t homelogo_byteicon[];

/* Private defines ---------------------------------------------------------- */
#define ADVERTISING_TIMEOUT 60000
#define STARTUP_TIMEOUT 1500

/* Private variables -------------------------------------------------------- */
Menu_Icon_t access_menu_icon = {MENU_ICON_TYPE_CHAR, 0, 0, 0, '>'};
Menu_Icon_t return_menu_icon = {MENU_ICON_TYPE_CHAR, 0, 0, 0, '<'};
Menu_Icon_t homelogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &homelogo_byteicon, 128, 64, 0};
Menu_Icon_t bluetooth_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &bluetooth_byteicon, 16, 16, 0};
Menu_Icon_t stlogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &stlogo_byteicon, 16, 16, 0};
Menu_Icon_t volume_up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_up_byteicon, 16, 16, 0};
Menu_Icon_t volume_down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_down_byteicon, 16, 16, 0};
Menu_Icon_t volume_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_mute_byteicon, 16, 16, 0};
Menu_Icon_t initiate_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &initiate_call_byteicon, 16, 16, 0};
Menu_Icon_t answer_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &answer_call_byteicon, 16, 16, 0};
Menu_Icon_t terminate_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &terminate_call_byteicon, 16, 16, 0};
Menu_Icon_t next_track_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &next_track_byteicon, 16, 16, 0};
Menu_Icon_t play_pause_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &play_pause_byteicon, 16, 16, 0};
Menu_Icon_t previous_track_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &previous_track_byteicon, 16, 16, 0};
Menu_Icon_t advertising_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &advertising_byteicon, 16, 16, 0};
Menu_Icon_t hourglass_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &hourglass_byteicon, 16, 16, 0};
Menu_Icon_t input_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_byteicon, 16, 16, 0};
Menu_Icon_t input_output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_output_byteicon, 16, 16, 0};
Menu_Icon_t output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &output_byteicon, 16, 16, 0};

Menu_Content_Text_t startup_text = {3, {"BLE Audio TMAP", "Unicast Server", "Press Right"}};
Menu_Content_Text_t waitcon_text = {2, {"Advertising as", "STM32WBA_0000"}};
Menu_Content_Text_t connecting_text = {1, {"Connecting"}};
Menu_Content_Text_t nostream_text = {3, {"Connected", "No Stream", ""}};
Menu_Content_Text_t streaming_text = {2, {"Streaming 48KHz", ""}};
Menu_Content_Text_t volume_text = {1, {"Volume"}};
Menu_Content_Text_t media_text = {2, {"Media PAUSED", "Track1"}};
Menu_Content_Text_t call_text = {2, {"Call State", "IDLE"}};

Menu_Page_t *p_stlogo_menu;
Menu_Page_t *p_startup_menu;
Menu_Page_t *p_waitcon_menu;
Menu_Page_t *p_connecting_menu;
Menu_Page_t *p_nostream_menu;
Menu_Page_t *p_streaming_menu;
Menu_Page_t *p_control_menu;
Menu_Page_t *p_volume_control_menu;
Menu_Page_t *p_call_control_menu;
Menu_Page_t *p_media_control_menu;
Menu_Page_t *p_config_menu;

static UTIL_TIMER_Object_t Advertising_Timer;
static UTIL_TIMER_Object_t Startup_Timer;

/* Private functions prototypes-----------------------------------------------*/
static void Menu_Start_Advertising(void);
static void Menu_Stop_Advertising(void);
static void Menu_Volume_Up(void);
static void Menu_Volume_Down(void);
static void Menu_Volume_Mute(void);
static void Menu_Call_Answer(void);
static void Menu_Call_Terminate(void);
static void Menu_Media_NextTrack(void);
static void Menu_Media_PlayPause(void);
static void Menu_Media_PreviousTrack(void);
static void Menu_Disconnect(void);
static void Menu_ClearSecDB(void);
static void Menu_Advertising_TimerCallback(void *arg);
static void Menu_Startup_TimerCallback(void *arg);

/* Exported Functions Definition -------------------------------------------- */
/**
 * @brief Initialize and setup the menu
 */
void Menu_Config(void)
{
  Menu_Action_t access_config_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t clear_db_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_ClearSecDB, 0};
  Menu_Action_t access_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t start_advertising_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Advertising, 0};
  Menu_Action_t stop_advertising_action = {MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_Stop_Advertising, 0};

  Menu_Action_t entry_menu_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_call_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_media_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t entry_menu_disconnect_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Disconnect, 0};

  Menu_Action_t volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Volume_Up, 0};
  Menu_Action_t volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Volume_Down, 0};
  Menu_Action_t volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Volume_Mute, 0};

  Menu_Action_t call_control_up_action = {MENU_ACTION_CALLBACK, &answer_call_icon, &Menu_Call_Answer, 0};
  Menu_Action_t call_control_down_action = {MENU_ACTION_CALLBACK, &terminate_call_icon, &Menu_Call_Terminate, 0};

  Menu_Action_t media_control_up_action = {MENU_ACTION_CALLBACK, &next_track_icon, &Menu_Media_NextTrack, 0};
  Menu_Action_t media_control_down_action = {MENU_ACTION_CALLBACK, &previous_track_icon, &Menu_Media_PreviousTrack, 0};
  Menu_Action_t media_control_right_action = {MENU_ACTION_CALLBACK, &play_pause_icon, &Menu_Media_PlayPause, 0};

  Menu_Init();

  p_stlogo_menu = Menu_CreatePage(MENU_TYPE_LOGO);
  p_startup_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_waitcon_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_connecting_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_nostream_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_streaming_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_control_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_volume_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_call_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_media_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_config_menu = Menu_CreatePage(MENU_TYPE_LIST);

  start_advertising_action.pPage = (struct Menu_Page_t*) p_waitcon_menu;
  access_config_menu_action.pPage = (struct Menu_Page_t*) p_config_menu;
  access_menu_action.pPage = (struct Menu_Page_t*) p_control_menu;
  entry_menu_volume_action.pPage = (struct Menu_Page_t*) p_volume_control_menu;
  entry_menu_call_action.pPage = (struct Menu_Page_t*) p_call_control_menu;
  entry_menu_media_action.pPage = (struct Menu_Page_t*) p_media_control_menu;

  Menu_SetControlContent(p_startup_menu, &startup_text, &stlogo_icon);
  Menu_SetControlAction(p_startup_menu, MENU_DIRECTION_RIGHT, access_config_menu_action);
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
  Menu_AddListEntry(p_control_menu, "Media...", entry_menu_media_action);
  Menu_AddListEntry(p_control_menu, "Disconnect", entry_menu_disconnect_action);

  Menu_AddListEntry(p_config_menu, "Start Adv.", start_advertising_action);
  Menu_AddListEntry(p_config_menu, "Clear Sec. DB", clear_db_action);

  Menu_SetControlContent(p_volume_control_menu, &volume_text, 0);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_UP, volume_control_up_action);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_DOWN, volume_control_down_action);
  Menu_SetControlAction(p_volume_control_menu, MENU_DIRECTION_RIGHT, volume_control_right_action);

  Menu_SetControlContent(p_call_control_menu,&call_text, 0);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_UP, call_control_up_action);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_DOWN, call_control_down_action);

  Menu_SetControlContent(p_media_control_menu, &media_text, 0);
  Menu_SetControlAction(p_media_control_menu, MENU_DIRECTION_UP, media_control_up_action);
  Menu_SetControlAction(p_media_control_menu, MENU_DIRECTION_DOWN, media_control_down_action);
  Menu_SetControlAction(p_media_control_menu, MENU_DIRECTION_RIGHT, media_control_right_action);

  Menu_SetActivePage(p_stlogo_menu);
  UTIL_TIMER_Create(&Startup_Timer, STARTUP_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Startup_TimerCallback, 0);
  UTIL_TIMER_Start(&Startup_Timer);
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
#if (CFG_JOYSTICK_SUPPORTED == 1)
  Menu_SetActivePage(p_startup_menu);
#else /*(CFG_JOYSTICK_SUPPORTED == 1)*/
  uint8_t status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT,0);
  LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
  Menu_SetActivePage(p_waitcon_menu);
#endif /*(CFG_JOYSTICK_SUPPORTED == 1)*/
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
 * @brief Set Call State
 */
void Menu_SetCallState(char *pCallState)
{
  snprintf(call_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s", pCallState);
  Menu_Print();
}

/**
 * @brief Set Media State
 */
void Menu_SetMediaState(char *pMediaState)
{
  snprintf(media_text.Lines[0], MENU_CONTROL_MAX_LINE_LEN, "Media %s", pMediaState);
  Menu_Print();
}

/**
 * @brief Set Track Title
 */
void Menu_SetTrackTitle(uint8_t *pTrackTitle, uint8_t TrackNameLen)
{
  if (TrackNameLen <= 14)
  {
    snprintf(media_text.Lines[1], TrackNameLen +1, "%s", pTrackTitle);
  }
  else
  {
    char display_string[15];
    snprintf(display_string, 12, "%s", pTrackTitle);
    display_string[11] = '.';
    display_string[12] = '.';
    display_string[13] = '.';
    display_string[14] = '\0';
    snprintf(media_text.Lines[1], 15, "%s", display_string);
  }
  Menu_Print();
}

/* Private Functions Definition --------------------------------------------- */
/**
 * @brief Start Advertising Callback
 */
static void Menu_Start_Advertising(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Start Advertising\n");
  /* Start Advertising */
  status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT,0);
  LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
  UTIL_TIMER_Create(&Advertising_Timer, ADVERTISING_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Advertising_TimerCallback, 0);
  UTIL_TIMER_Start(&Advertising_Timer);
}
/**
 * @brief Start Advertising Callback
 */
static void Menu_Stop_Advertising(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Stop Advertising\n");
  status = TMAPAPP_StopAdvertising();
  LOG_INFO_APP("TMAPAPP_StopAdvertising() returns status 0x%02X\n",status);
  UTIL_TIMER_Stop(&Advertising_Timer);
}

/**
 * @brief Volume Up Callback
 */
static void Menu_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  TMAPAPP_VolumeUp();
}

/**
 * @brief Volume Down Callback
 */
static void Menu_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  TMAPAPP_VolumeDown();
}

/**
 * @brief Volume Mute Callback
 */
static void Menu_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  TMAPAPP_ToggleMute();
}

/**
 * @brief Call Answer Callback
 */
static void Menu_Call_Answer(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Answer Call\n");
  TMAPAPP_AnswerCall();
}

/**
 * @brief Call terminate Callback
 */
static void Menu_Call_Terminate(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Terminate Call\n");
  TMAPAPP_TerminateCall();
}

/**
 * @brief Next Track Callback
 */
static void Menu_Media_NextTrack(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Next Track\n");
  TMAPAPP_NextTrack();
}

/**
 * @brief Play Pause Callback
 */
static void Menu_Media_PlayPause(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Play/Pause\n");
  TMAPAPP_PlayPause();
}

/**
 * @brief Previous track Callback
 */
static void Menu_Media_PreviousTrack(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Previous Track\n");
  TMAPAPP_PreviousTrack();
}

/**
 * @brief Disconnect Callback
 */
static void Menu_Disconnect(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Disconnect\n");
  TMAPAPP_Disconnect();
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

/**
 * @brief Startup Timer Callback
 */
static void Menu_Startup_TimerCallback(void *arg)
{
  LOG_INFO_APP("[APP_MENU_CONF] Startup timer expired, Display home\n");
  Menu_SetStartupPage();
}
