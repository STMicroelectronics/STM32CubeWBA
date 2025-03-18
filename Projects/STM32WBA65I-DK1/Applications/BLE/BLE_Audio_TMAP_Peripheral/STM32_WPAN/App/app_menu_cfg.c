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
#include "stm32_seq.h"

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
extern uint8_t arrow_right_byteicon[];
extern uint8_t arrow_up_byteicon[];
extern uint8_t arrow_down_byteicon[];
extern uint8_t arrow_return_byteicon[];

/* Private defines ---------------------------------------------------------- */
#define ADVERTISING_TIMEOUT             (60000u)
#define STARTUP_TIMEOUT                 (1500u)
#define MAX_NUM_BROADCAST_SOURCE        (6u)

typedef struct
{
  uint8_t AdvSID;
  uint8_t Address[6u];
  uint8_t AddressType;
} Broadcast_Source_t;

/* Private variables -------------------------------------------------------- */
Menu_Icon_t access_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_right_byteicon, 16, 16, 0};
Menu_Icon_t return_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_return_byteicon, 8, 8, 0};
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
Menu_Icon_t next_track_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &next_track_byteicon, 16, 16, 0};
Menu_Icon_t play_pause_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &play_pause_byteicon, 16, 16, 0};
Menu_Icon_t previous_track_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &previous_track_byteicon, 16, 16, 0};
Menu_Icon_t advertising_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &advertising_byteicon, 16, 16, 0};
Menu_Icon_t hourglass_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &hourglass_byteicon, 16, 16, 0};
Menu_Icon_t input_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_byteicon, 16, 16, 0};
Menu_Icon_t input_output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_output_byteicon, 16, 16, 0};
Menu_Icon_t output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &output_byteicon, 16, 16, 0};

Menu_Content_Text_t startup_text = {3, {"BLE Audio", "TMAP Peripheral", "Press Right"}};
Menu_Content_Text_t csip_config_text = {2, {"Audio Config:", "Headphones"}};
Menu_Content_Text_t broadcast_scan_text = {2, {"Scanning nearby", "sources"}};
Menu_Content_Text_t pa_sync_text = {1, {"Synchronizing PA"}};
Menu_Content_Text_t bis_sync_text = {1, {"Synchronizing BIS"}};
Menu_Content_Text_t broadcast_synced_text = {2, {"Synchronized", "48KHz"}};
Menu_Content_Text_t scan_deleg_synced_text = {2, {"Synced 48KHz", ""}};
Menu_Content_Text_t waitcon_text = {2, {"Advertising as", "STM32WBA_0000"}};
Menu_Content_Text_t connecting_text = {1, {"Connecting"}};
Menu_Content_Text_t nostream_text = {3, {"Connected", "No Stream", ""}};
Menu_Content_Text_t streaming_text = {2, {"Streaming 48KHz", ""}};
Menu_Content_Text_t volume_text = {1, {"Volume"}};
Menu_Content_Text_t media_text = {2, {"Media PAUSED", "Track1"}};
Menu_Content_Text_t call_text = {2, {"Call State", "IDLE"}};

Menu_Page_t *p_stlogo_menu;
Menu_Page_t *p_startup_menu;
Menu_Page_t *p_csip_config_menu;
Menu_Page_t *p_broadcast_scan_menu;
Menu_Page_t *p_broadcast_list_menu;
Menu_Page_t *p_pa_sync_menu;
Menu_Page_t *p_bis_sync_menu;
Menu_Page_t *p_broadcast_synced_menu;
Menu_Page_t *p_waitcon_menu;
Menu_Page_t *p_connecting_menu;
Menu_Page_t *p_nostream_menu;
Menu_Page_t *p_streaming_menu;
Menu_Page_t *p_control_menu;
Menu_Page_t *p_volume_control_menu;
Menu_Page_t *p_call_control_menu;
Menu_Page_t *p_media_control_menu;
Menu_Page_t *p_config_menu;
Menu_Page_t *p_broadcast_menu;
Menu_Page_t *p_scan_deleg_synced_menu;
Menu_Page_t *p_scan_deleg_control_menu;

Menu_Action_t select_broadcast_action;

static UTIL_TIMER_Object_t Advertising_Timer;
#if (CFG_JOYSTICK_SUPPORTED == 1)
static UTIL_TIMER_Object_t Startup_Timer;
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

static uint8_t csip_conf_id = 0;
static uint8_t app_initialization_done = 0;

uint8_t num_broadcast_sources = 0;
Broadcast_Source_t a_broadcast_sources[MAX_NUM_BROADCAST_SOURCE];

/* Private functions prototypes-----------------------------------------------*/
static void Menu_Start_Advertising(void);
static void Menu_Start_Advertising_ScanDeleg(void);
static void Menu_CSIP_Conf_Up(void);
static void Menu_CSIP_Conf_Down(void);
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
static void Menu_SelectBroadcast(uint8_t id);
static void Menu_StartBroadcastScan(void);
static void Menu_StopBroadcastScan(void);
static void Menu_StopBroadcastSync(void);
static void Menu_StopScanDelegSync(void);
static void Menu_AdvTimerTaskHandler(void);
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
  Menu_Action_t broadcast_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t start_broadcast_scan_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartBroadcastScan, 0};
  Menu_Action_t stop_broadcast_scan_action = {MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_StopBroadcastScan, 0};
  Menu_Action_t stop_broadcast_sync_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StopBroadcastSync, 0};
  Menu_Action_t csip_config_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t csip_config_up_action = {MENU_ACTION_CALLBACK, &up_icon, &Menu_CSIP_Conf_Up, 0};
  Menu_Action_t csip_config_down_action = {MENU_ACTION_CALLBACK, &down_icon, Menu_CSIP_Conf_Down, 0};
  Menu_Action_t start_advertising_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Advertising, 0};
  Menu_Action_t start_advertising_sde_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Advertising_ScanDeleg, 0};
  Menu_Action_t stop_advertising_action = {MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_Stop_Advertising, 0};
  Menu_Action_t stop_scan_deleg_sync_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StopScanDelegSync, 0};
  Menu_Action_t scan_deleg_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};

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

  select_broadcast_action.ActionType = MENU_ACTION_LIST_CALLBACK;
  select_broadcast_action.pIcon = &access_menu_icon;
  select_broadcast_action.ListCallback = &Menu_SelectBroadcast;

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
  p_media_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_config_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_broadcast_scan_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_broadcast_list_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_pa_sync_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_bis_sync_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_broadcast_synced_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_broadcast_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_scan_deleg_synced_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_scan_deleg_control_menu = Menu_CreatePage(MENU_TYPE_LIST);

  csip_config_action.pPage = (struct Menu_Page_t*) p_csip_config_menu;
  start_advertising_action.pPage = (struct Menu_Page_t*) p_waitcon_menu;
  start_advertising_sde_action.pPage = (struct Menu_Page_t*) p_waitcon_menu;
  start_broadcast_scan_action.pPage = (struct Menu_Page_t*) p_broadcast_scan_menu;
  access_config_menu_action.pPage = (struct Menu_Page_t*) p_config_menu;
  access_menu_action.pPage = (struct Menu_Page_t*) p_control_menu;
  broadcast_menu_action.pPage = (struct Menu_Page_t*) p_broadcast_menu;
  entry_menu_volume_action.pPage = (struct Menu_Page_t*) p_volume_control_menu;
  entry_menu_call_action.pPage = (struct Menu_Page_t*) p_call_control_menu;
  entry_menu_media_action.pPage = (struct Menu_Page_t*) p_media_control_menu;
  scan_deleg_menu_action.pPage =  (struct Menu_Page_t*) p_scan_deleg_control_menu;

  Menu_SetControlContent(p_startup_menu, &startup_text, &stlogo_icon);
  Menu_SetControlAction(p_startup_menu, MENU_DIRECTION_RIGHT, access_config_menu_action);

  Menu_SetControlContent(p_csip_config_menu, &csip_config_text, 0);
  Menu_SetControlAction(p_csip_config_menu, MENU_DIRECTION_UP, csip_config_up_action);
  Menu_SetControlAction(p_csip_config_menu, MENU_DIRECTION_DOWN, csip_config_down_action);

  Menu_SetControlContent(p_waitcon_menu, &waitcon_text, &advertising_icon);
  Menu_SetControlAction(p_waitcon_menu, MENU_DIRECTION_LEFT, stop_advertising_action);

  Menu_SetControlContent(p_broadcast_scan_menu, &broadcast_scan_text, &advertising_icon);
  Menu_SetControlAction(p_broadcast_scan_menu, MENU_DIRECTION_LEFT, stop_advertising_action);

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

  Menu_AddListEntry(p_config_menu, "Start Unicast", start_advertising_action);
  Menu_AddListEntry(p_config_menu, "Start Broadcast", start_broadcast_scan_action);
  Menu_AddListEntry(p_config_menu, "Start Scan Deleg", start_advertising_sde_action);
  Menu_AddListEntry(p_config_menu, "Audio Config", csip_config_action);
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

  Menu_AddListEntry(p_broadcast_menu, "Volume...", entry_menu_volume_action);
  Menu_AddListEntry(p_broadcast_menu, "Stop Sync", stop_broadcast_sync_action);

  Menu_SetControlContent(p_broadcast_scan_menu, &broadcast_scan_text, &advertising_icon);
  Menu_SetControlAction(p_broadcast_scan_menu, MENU_DIRECTION_LEFT, stop_broadcast_scan_action);
  Menu_SetControlContent(p_pa_sync_menu, &pa_sync_text, &hourglass_icon);
  Menu_SetControlContent(p_bis_sync_menu, &bis_sync_text, &hourglass_icon);
  Menu_SetControlContent(p_broadcast_synced_menu, &broadcast_synced_text, 0);
  Menu_SetControlAction(p_broadcast_synced_menu, MENU_DIRECTION_RIGHT, broadcast_menu_action);
  Menu_SetControlAction(p_broadcast_list_menu, MENU_DIRECTION_LEFT, stop_broadcast_scan_action);

  Menu_SetControlContent(p_scan_deleg_synced_menu, &scan_deleg_synced_text, &input_icon);
  Menu_SetControlAction(p_scan_deleg_synced_menu, MENU_DIRECTION_RIGHT, scan_deleg_menu_action);

  Menu_AddListEntry(p_scan_deleg_control_menu, "Volume...", entry_menu_volume_action);
  Menu_AddListEntry(p_scan_deleg_control_menu, "Stop Sync", stop_scan_deleg_sync_action);
  Menu_AddListEntry(p_scan_deleg_control_menu, "Disconnect", entry_menu_disconnect_action);

  UTIL_SEQ_RegTask(1U << CFG_TASK_APP_ADV_TIMER_ID, UTIL_SEQ_RFU, Menu_AdvTimerTaskHandler);
#if (CFG_JOYSTICK_SUPPORTED == 1)
  Menu_SetActivePage(p_stlogo_menu);
  UTIL_TIMER_Create(&Startup_Timer, STARTUP_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Startup_TimerCallback, 0);
  UTIL_TIMER_Start(&Startup_Timer);
#else /*(CFG_JOYSTICK_SUPPORTED == 1)*/
  TMAPAPP_Init(0);
  uint8_t status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, GAP_APPEARANCE_HEADPHONES);
  LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
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
void Menu_SetIdentifier(char *pId, uint8_t len)
{
  UTIL_MEM_cpy_8(&waitcon_text.Lines[1][0], pId, len);
}

/**
 * @brief Set Current volume value
 */
void Menu_SetVolume(uint8_t Volume)
{
  snprintf(nostream_text.Lines[2], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(streaming_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(volume_text.Lines[0], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(scan_deleg_synced_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
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

/**
 * @brief Add a scanned broadcast source to the device list
 */
void Menu_AddBroadcastSource(uint8_t AdvSID, uint8_t *pAddress, uint8_t AddressType, char *pDeviceName)
{
  uint8_t i;
  uint8_t duplicate = 0;
  for (i = 0; i < num_broadcast_sources; i++)
  {
    if (memcmp(a_broadcast_sources[i].Address, pAddress, 6u) == 0)
    {
      duplicate = 1;
    }
  }

  if (duplicate == 0 && num_broadcast_sources < MAX_NUM_BROADCAST_SOURCE)
  {
    UTIL_MEM_cpy_8(&a_broadcast_sources[num_broadcast_sources].Address[0], pAddress, 6u);
    a_broadcast_sources[num_broadcast_sources].AddressType = AddressType;
    a_broadcast_sources[num_broadcast_sources].AdvSID = AdvSID;

    Menu_AddListEntry(p_broadcast_list_menu, pDeviceName, select_broadcast_action);

    if (num_broadcast_sources == 0)
    {
      Menu_SetActivePage(p_broadcast_list_menu);
      p_broadcast_list_menu->pReturnPage = (struct Menu_Page_t *) p_config_menu;
    }
    else
    {
      Menu_Print();
    }

    num_broadcast_sources++;
  }
}

/**
 * @brief Set the PA Sync page as active
 */
void Menu_SetPASyncPage(void)
{
  Menu_SetActivePage(p_pa_sync_menu);
}

/**
 * @brief Set the BIS Sync Page as active
 */
void Menu_SetBISSyncPage(void)
{
  Menu_SetActivePage(p_bis_sync_menu);
}

/**
 * @brief Set the Synchronized Page as active
 */
void Menu_SetBroadcastSyncedPage(char* pSamplerateText)
{
  snprintf(&broadcast_synced_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", pSamplerateText);
  p_broadcast_synced_menu->pIcon = &input_icon;
  Menu_SetActivePage(p_broadcast_synced_menu);
}

/**
 * @brief Set the Scanning Page as active
 */
void Menu_SetBroadcastScanPage(void)
{
  num_broadcast_sources = 0;
  Menu_ClearList(p_broadcast_list_menu);
  Menu_SetActivePage(p_broadcast_scan_menu);
}

/**
 * @brief Set the Synchronized Page as active for the Scan Delegator Role
 */
void Menu_SetScanDelegSyncedPage(char* pSamplerateText)
{
  snprintf(&scan_deleg_synced_text.Lines[0][0], MENU_CONTROL_MAX_LINE_LEN, "Synced %s", pSamplerateText);
  p_broadcast_synced_menu->pIcon = &input_icon;
  Menu_SetActivePage(p_scan_deleg_synced_menu);
}

/**
 * @brief Set Profiles Linked Up
 */
void Menu_SetProfilesLinked(audio_profile_t Profiles)
{
  char ccp_string[5] = "";
  char mcp_string[5] = "";
  
  if (Profiles & AUDIO_PROFILE_CCP)
  {
	snprintf(ccp_string, 5, " CCP");
  }
  
  if (Profiles & AUDIO_PROFILE_MCP)
  {
	snprintf(mcp_string, 5, " MCP");
  }
  
  snprintf(nostream_text.Lines[0], MENU_CONTROL_MAX_LINE_LEN, "Connected%s%s", ccp_string, mcp_string);

  Menu_Print();
}

/* Private Functions Definition --------------------------------------------- */

/**
 * @brief CSIP Conf Up Callback
 */
static void Menu_CSIP_Conf_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] CSIP Conf Up\n");

  csip_conf_id = (csip_conf_id + 1) % (APP_CSIP_SET_MEMBER_SIZE + 1);

  switch (csip_conf_id)
  {
    case 0:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Headphones");
      break;
    }
    case 1:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Earbud Left");
      break;
    }
    case 2:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Earbud Right");
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
    csip_conf_id = APP_CSIP_SET_MEMBER_SIZE;
  }

  switch (csip_conf_id)
  {
    case 0:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Headphones");
      break;
    }
    case 1:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Earbud Left");
      break;
    }
    case 2:
    {
      snprintf(csip_config_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Earbud Right");
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
  uint8_t status = BLE_STATUS_SUCCESS;
  uint16_t appearance;
  LOG_INFO_APP("[APP_MENU_CONF] Start Unicast Advertising\n");
  TMAPAPP_SetBroadcastMode(APP_BROADCAST_MODE_NONE);
  if (app_initialization_done == 0u)
  {

    TMAPAPP_Init(csip_conf_id);
    if (csip_conf_id > 0)
    {
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
      /* Register CSIS */
      status = CSIPAPP_RegisterCSIS(0,
                                    CSIP_SIRK_IS_NOT_OOB,
                                    0x00, /*0x00 : Encrypted, 0x01 : PlainText*/
                                    (uint8_t *)APP_SIRK,
                                    APP_CSIP_SET_MEMBER_SIZE,
                                    csip_conf_id);
      LOG_INFO_APP("Register Set Member rank %d/%d returns status 0x%02X\n",
                   csip_conf_id,
                   APP_CSIP_SET_MEMBER_SIZE,
                   status);
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
      appearance = GAP_APPEARANCE_EARBUD;
    }
    else
    {
      appearance = GAP_APPEARANCE_HEADPHONES;
    }
    if (status == BLE_STATUS_SUCCESS)
    {
      app_initialization_done = 1u;
      /* Remove Action buttons */
      p_csip_config_menu->ActionDown.ActionType = 0;
      p_csip_config_menu->ActionUp.ActionType = 0;
    }
  }
  else
  {
    if (csip_conf_id > 0)
    {
      appearance = GAP_APPEARANCE_EARBUD;
    }
    else
    {
      appearance = GAP_APPEARANCE_HEADPHONES;
    }
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    /* Start Advertising */
    status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 0, appearance);
    LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
    UTIL_TIMER_Create(&Advertising_Timer, ADVERTISING_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Advertising_TimerCallback, 0);
    UTIL_TIMER_Start(&Advertising_Timer);
    UNUSED(status);
  }
}

/**
 * @brief Start Advertising with solicitation request Callback
 */
static void Menu_Start_Advertising_ScanDeleg(void)
{
  uint8_t status = BLE_STATUS_SUCCESS;
  uint16_t appearance;
  LOG_INFO_APP("[APP_MENU_CONF] Start Scan Delegator Advertising\n");
  TMAPAPP_SetBroadcastMode(APP_BROADCAST_MODE_SCAN_DELEGATOR);
  if (app_initialization_done == 0u)
  {
    TMAPAPP_Init(csip_conf_id);
    if (csip_conf_id > 0)
    {
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
      /* Register CSIS */
      status = CSIPAPP_RegisterCSIS(0,
                                    CSIP_SIRK_IS_NOT_OOB,
                                    0x00, /*0x00 : Encrypted, 0x01 : PlainText*/
                                    (uint8_t *)APP_SIRK,
                                    APP_CSIP_SET_MEMBER_SIZE,
                                    csip_conf_id);
      LOG_INFO_APP("Register Set Member rank %d/%d returns status 0x%02X\n",
                   csip_conf_id,
                   APP_CSIP_SET_MEMBER_SIZE,
                   status);
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
      appearance = GAP_APPEARANCE_EARBUD;
    }
    else
    {
      appearance = GAP_APPEARANCE_HEADPHONES;
    }
    if (status == BLE_STATUS_SUCCESS)
    {
      app_initialization_done = 1u;
      /* Remove Action buttons */
      p_csip_config_menu->ActionDown.ActionType = 0;
      p_csip_config_menu->ActionUp.ActionType = 0;
    }
  }
  else
  {
    if (csip_conf_id > 0)
    {
      appearance = GAP_APPEARANCE_EARBUD;
    }
    else
    {
      appearance = GAP_APPEARANCE_HEADPHONES;
    }
  }

  if (status == BLE_STATUS_SUCCESS)
  {
    /* Start Advertising */
    status = TMAPAPP_StartAdvertising(CAP_GENERAL_ANNOUNCEMENT, 1, appearance);
    LOG_INFO_APP("TMAPAPP_StartAdvertising() returns status 0x%02X\n",status);
    UTIL_TIMER_Create(&Advertising_Timer, ADVERTISING_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Advertising_TimerCallback, 0);
    UTIL_TIMER_Start(&Advertising_Timer);
    UNUSED(status);
  }
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
  UNUSED(status);
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

static void Menu_SelectBroadcast(uint8_t id)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Selected device %d\n", id);

  status = TMAPAPP_SyncToPA(a_broadcast_sources[id].AdvSID,
                           &a_broadcast_sources[id].Address[0],
                           a_broadcast_sources[id].AddressType);
  LOG_INFO_APP("PBPAPP_SyncToPA() returns status 0x%02X\n",status);

  if (status == BLE_STATUS_SUCCESS)
  {
    Menu_SetPASyncPage();
  }
}

/**
 * @brief Start Broadcast Scan Callback
 */
static void Menu_StartBroadcastScan(void)
{
  uint8_t status = BLE_STATUS_SUCCESS;
  LOG_INFO_APP("[APP_MENU_CONF] Start Scanning\n");
  TMAPAPP_SetBroadcastMode(APP_BROADCAST_MODE_SINK_ONLY);

  if (app_initialization_done == 0u)
  {

    TMAPAPP_Init(csip_conf_id);
    if (csip_conf_id > 0)
    {
#if (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u)
      /* Register CSIS */
      status = CSIPAPP_RegisterCSIS(0,
                                    CSIP_SIRK_IS_NOT_OOB,
                                    0x00, /*0x00 : Encrypted, 0x01 : PlainText*/
                                    (uint8_t *)APP_SIRK,
                                    APP_CSIP_SET_MEMBER_SIZE,
                                    csip_conf_id);
      LOG_INFO_APP("Register Set Member rank %d/%d returns status 0x%02X\n",
                   csip_conf_id,
                   APP_CSIP_SET_MEMBER_SIZE,
                   status);
#endif /* (APP_CSIP_ROLE_SET_MEMBER_SUPPORT == 1u) */
    }
    if (status == BLE_STATUS_SUCCESS)
    {
      app_initialization_done = 1u;
      /* Remove Action buttons */
      p_csip_config_menu->ActionDown.ActionType = 0;
      p_csip_config_menu->ActionUp.ActionType = 0;
    }
  }
  if (status == BLE_STATUS_SUCCESS)
  {
    num_broadcast_sources = 0;
    Menu_ClearList(p_broadcast_list_menu);
    /* Start Scanning */
    status = TMAPAPP_StartSink();
    LOG_INFO_APP("TMAPAPP_StartSink() returns status 0x%02X\n",status);
    UNUSED(status);
  }
}

/**
 * @brief Stop Broadcast Scan Callback
 */
static void Menu_StopBroadcastScan(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Stop Scanning\n");
  status = TMAPAPP_StopSink();
  LOG_INFO_APP("TMAPAPP_StopSink() returns status 0x%02X\n",status);
  UNUSED(status);
}

/**
 * @brief Stop Broadcast Sync Callback
 */
static void Menu_StopBroadcastSync(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Stop Sync\n");
  num_broadcast_sources = 0;
  Menu_ClearList(p_broadcast_list_menu);
  TMAPAPP_StopSink();
  Menu_SetActivePage(p_broadcast_scan_menu);
}

/**
 * @brief Stop Scan Deleg Sync Callback
 */
static void Menu_StopScanDelegSync(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Stop Scan Deleg Sync\n");
  TMAPAPP_StopSink();
  Menu_SetActivePage(p_nostream_menu);
}

/**
 * @brief Advertising Timer Callback
 */
static void Menu_Advertising_TimerCallback(void *arg)
{
  UTIL_SEQ_SetTask(1U << CFG_TASK_APP_ADV_TIMER_ID, CFG_SEQ_PRIO_0);
}

/**
 * @brief Timer Task Handler
 */
static void Menu_AdvTimerTaskHandler(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] ADV Timer Expired, stop advertising\n");
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
