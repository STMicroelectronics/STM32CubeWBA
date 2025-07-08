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
#include "app_entry.h"
#include "app_menu_cfg.h"
#include "app_menu.h"
#include "app_conf.h"
#include "ble_gap_aci.h"
#include "log_module.h"

/* External variables ------------------------------------------------------- */
extern uint8_t volume_mute_byteicon[];
extern uint8_t volume_up_byteicon[];
extern uint8_t volume_down_byteicon[];
extern uint8_t microphone_mute_byteicon[];
extern uint8_t advertising_byteicon[];
extern uint8_t hourglass_byteicon[];
extern uint8_t input_byteicon[];
extern uint8_t input_output_byteicon[];
extern uint8_t output_byteicon[];
extern uint8_t bluetooth_byteicon[];
extern uint8_t stlogo_byteicon[];
extern uint8_t homelogo_byteicon[];
extern uint8_t arrow_up_byteicon[];
extern uint8_t arrow_down_byteicon[];
extern uint8_t arrow_return_byteicon[];

/* Private defines ---------------------------------------------------------- */
#define ADVERTISING_TIMEOUT 60000
#define STARTUP_TIMEOUT 1500
#define MAX_NUM_UNICAST_SERVERS 8

typedef struct
{
  uint8_t Address[6u];
  uint8_t AddressType;
} Unicast_Server_t;

/* Private variables -------------------------------------------------------- */
Menu_Icon_t access_menu_icon = {MENU_ICON_TYPE_CHAR, 0, 0, 0, '>'};
Menu_Icon_t return_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_return_byteicon, 8, 8, 0};
Menu_Icon_t homelogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &homelogo_byteicon, 128, 64, 0};
Menu_Icon_t bluetooth_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &bluetooth_byteicon, 16, 16, 0};
Menu_Icon_t stlogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &stlogo_byteicon, 16, 16, 0};
Menu_Icon_t volume_up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_up_byteicon, 16, 16, 0};
Menu_Icon_t volume_down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_down_byteicon, 16, 16, 0};
Menu_Icon_t volume_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_mute_byteicon, 16, 16, 0};
Menu_Icon_t microphone_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &microphone_mute_byteicon, 16, 16, 0};
Menu_Icon_t advertising_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &advertising_byteicon, 16, 16, 0};
Menu_Icon_t hourglass_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &hourglass_byteicon, 16, 16, 0};
Menu_Icon_t input_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_byteicon, 16, 16, 0};
Menu_Icon_t input_output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_output_byteicon, 16, 16, 0};
Menu_Icon_t output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &output_byteicon, 16, 16, 0};
Menu_Icon_t arrow_up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_up_byteicon, 16, 16, 0};
Menu_Icon_t arrow_down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_down_byteicon, 16, 16, 0};

Menu_Content_Text_t startup_text = {3, {"BLE Audio", "GMAP Central", "Press Right"}};
Menu_Content_Text_t scan_text = {2, {"Scanning", "nearby servers"}};
Menu_Content_Text_t connecting_text = {2, {"Connecting", "Device #0"}};
Menu_Content_Text_t linkup_text = {2, {"Linking Up", "Device #0"}};
Menu_Content_Text_t nostream_text = {3, {"Connected", "No Stream", ""}};
Menu_Content_Text_t streaming_text = {2, {"Sink 48KHz", "Sink 48KHz"}};
Menu_Content_Text_t rem_volume_text = {2, {"Remote Volume", ""}};
Menu_Content_Text_t loc_volume_text = {2, {"Local Volume", ""}};
Menu_Content_Text_t microphone_text = {2, {"Microphone", "IDLE"}};
Menu_Content_Text_t broadcast_config_text = {2, {"Broadcast Source", "48_4_g"}};
Menu_Content_Text_t broadcast_stream_text = {2, {"Broadcasting", "48KHz"}};

Menu_Page_t *p_stlogo_menu;
Menu_Page_t *p_startup_menu;
Menu_Page_t *p_scan_menu;
Menu_Page_t *p_devices_list_menu;
Menu_Page_t *p_connecting_menu;
Menu_Page_t *p_linkup_menu;
Menu_Page_t *p_nostream_menu;
Menu_Page_t *p_streaming_menu;
Menu_Page_t *p_control_menu;
Menu_Page_t *p_audio_stream_menu;
Menu_Page_t *p_rem_volume_control_menu;
Menu_Page_t *p_loc_volume_control_menu;
Menu_Page_t *p_microphone_control_menu;
Menu_Page_t *p_config_menu;
Menu_Page_t *p_broadcast_config_menu;
Menu_Page_t *p_broadcast_stream_menu;

Menu_Action_t select_device_action;

static UTIL_TIMER_Object_t Startup_Timer;

Unicast_Server_t a_unicast_servers[MAX_NUM_UNICAST_SERVERS];
uint8_t num_unicast_servers;
uint8_t stream_active = 0;
uint8_t stream_selected_freq_sink = 5;
uint8_t stream_selected_freq_source = 3;
uint8_t broadcast_freq_id = 7;

uint8_t a_broadcast_conf_id[8] = {
  LC3_QOS_8_2_1,
  LC3_QOS_16_2_1,
  LC3_QOS_24_2_1,
  LC3_QOS_32_1_1,
  LC3_QOS_48_1_g,
  LC3_QOS_48_2_g,
  LC3_QOS_48_3_g,
  LC3_QOS_48_4_g,
};
char *a_broadcast_conf_name[8] = {
  "8_2",
  "16_2",
  "24_2",
  "32_2",
  "48_1_g",
  "48_2_g",
  "48_3_g",
  "48_4_g",
};

/* Private functions prototypes-----------------------------------------------*/
static void Menu_Start_Scanning(void);
static void Menu_Stop_Scanning(void);
static void Menu_Remote_Volume_Up(void);
static void Menu_Remote_Volume_Down(void);
static void Menu_Remote_Volume_Mute(void);
static void Menu_Local_Volume_Up(void);
static void Menu_Local_Volume_Down(void);
static void Menu_Local_Volume_Mute(void);
static void Menu_Disconnect(void);
static void Menu_SelectDevice(uint8_t id);
static void Menu_StartStreamGameVoice(void);
static void Menu_StartStreamGame(void);
static void Menu_StartStreamVoice(void);
static void Menu_StartStreamVoiceRec(void);
static void Menu_StopStream(void);
static void Menu_Microphone_Mute(void);
static void Menu_ClearSecDB(void);
static void Menu_Startup_TimerCallback(void *arg);
static void Menu_Start_Broadcast(void);
static void Menu_Stop_Broadcast(void);
static void Menu_Broadcast_Conf_Up(void);
static void Menu_Broadcast_Conf_Down(void);


/* Exported Functions Definition -------------------------------------------- */
/**
 * @brief Initialize and setup the menu
 */
void Menu_Config(void)
{
  Menu_Action_t access_config_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0};
  Menu_Action_t clear_db_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_ClearSecDB, 0};
  Menu_Action_t access_menu_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t start_scanning_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Scanning, 0, 0};
  Menu_Action_t stop_scanning_action = {MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_Stop_Scanning, 0, 0};
  Menu_Action_t config_broadcast_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t stop_broadcast_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &return_menu_icon, &Menu_Stop_Broadcast, 0, 0};

  Menu_Action_t entry_menu_stream_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_remote_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_local_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_microphone_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_disconnect_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Disconnect, 0, 0};

  Menu_Action_t stream_start_game_voice_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartStreamGameVoice, 0, 0};
  Menu_Action_t stream_start_game_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartStreamGame, 0, 0};
  Menu_Action_t stream_start_voice_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartStreamVoice, 0, 0};
  Menu_Action_t stream_start_voice_rec_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartStreamVoiceRec, 0, 0};
  Menu_Action_t stream_stop_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StopStream, 0, 0};

  Menu_Action_t rem_volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Remote_Volume_Up, 0, 0};
  Menu_Action_t rem_volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Remote_Volume_Down, 0, 0};
  Menu_Action_t rem_volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Remote_Volume_Mute, 0, 0};

  Menu_Action_t loc_volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Local_Volume_Up, 0, 0};
  Menu_Action_t loc_volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Local_Volume_Down, 0, 0};
  Menu_Action_t loc_volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Local_Volume_Mute, 0, 0};

  Menu_Action_t microphone_control_right_action = {MENU_ACTION_CALLBACK, &microphone_mute_icon, &Menu_Microphone_Mute, 0, 0};

  Menu_Action_t broadcast_conf_down_action = {MENU_ACTION_CALLBACK, &arrow_down_icon, &Menu_Broadcast_Conf_Down, 0, 0};
  Menu_Action_t start_broadcast_action = {MENU_ACTION_MENU_PAGE|MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Start_Broadcast, 0, 0};

  select_device_action.ActionType = MENU_ACTION_LIST_CALLBACK;
  select_device_action.pIcon = &access_menu_icon;
  select_device_action.ListCallback = &Menu_SelectDevice;

  Menu_Init();

  p_stlogo_menu = Menu_CreatePage(MENU_TYPE_LOGO);
  p_startup_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_scan_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_connecting_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_linkup_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_nostream_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_streaming_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_control_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_rem_volume_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_loc_volume_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_microphone_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_devices_list_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_audio_stream_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_config_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_broadcast_config_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_broadcast_stream_menu = Menu_CreatePage(MENU_TYPE_CONTROL);

  start_scanning_action.pPage = (struct Menu_Page_t*) p_scan_menu;
  access_config_menu_action.pPage = (struct Menu_Page_t*) p_config_menu;
  access_menu_action.pPage = (struct Menu_Page_t*) p_control_menu;
  entry_menu_stream_action.pPage = (struct Menu_Page_t*) p_audio_stream_menu;
  entry_menu_remote_volume_action.pPage = (struct Menu_Page_t*) p_rem_volume_control_menu;
  entry_menu_local_volume_action.pPage = (struct Menu_Page_t*) p_loc_volume_control_menu;
  entry_menu_microphone_action.pPage = (struct Menu_Page_t*) p_microphone_control_menu;
  config_broadcast_action.pPage = (struct Menu_Page_t*) p_broadcast_config_menu;
  start_broadcast_action.pPage = (struct Menu_Page_t*) p_broadcast_stream_menu;

  Menu_SetControlContent(p_startup_menu, &startup_text, &stlogo_icon);
  Menu_SetControlAction(p_startup_menu, MENU_DIRECTION_RIGHT, access_config_menu_action);
  Menu_SetControlContent(p_scan_menu, &scan_text, &advertising_icon);
  Menu_SetControlAction(p_scan_menu, MENU_DIRECTION_LEFT, stop_scanning_action);
  Menu_SetControlContent(p_connecting_menu, &connecting_text, &hourglass_icon);
  Menu_SetControlContent(p_linkup_menu, &linkup_text, &hourglass_icon);
  Menu_SetControlContent(p_nostream_menu, &nostream_text, 0);
  Menu_SetControlAction(p_nostream_menu, MENU_DIRECTION_RIGHT, access_menu_action);
  Menu_SetControlContent(p_streaming_menu, &streaming_text, 0);
  Menu_SetControlAction(p_streaming_menu, MENU_DIRECTION_RIGHT, access_menu_action);

  Menu_SetLogo(p_stlogo_menu, &homelogo_icon);

  Menu_AddListEntry(p_control_menu, "Audio Stream...", entry_menu_stream_action);
  Menu_AddListEntry(p_control_menu, "Remote Volume...", entry_menu_remote_volume_action);
  Menu_AddListEntry(p_control_menu, "Local Volume...", entry_menu_local_volume_action);
  Menu_AddListEntry(p_control_menu, "Microphone...", entry_menu_microphone_action);
  Menu_AddListEntry(p_control_menu, "Disconnect", entry_menu_disconnect_action);

  Menu_AddListEntry(p_audio_stream_menu, "Game + Voice", stream_start_game_voice_action);
  Menu_AddListEntry(p_audio_stream_menu, "Game", stream_start_game_action);
  Menu_AddListEntry(p_audio_stream_menu, "Voice", stream_start_voice_action);
  Menu_AddListEntry(p_audio_stream_menu, "Voice Rec", stream_start_voice_rec_action);
  Menu_AddListEntry(p_audio_stream_menu, "Audio Stop", stream_stop_action);

  Menu_AddListEntry(p_config_menu, "Start Unicast", start_scanning_action);
  Menu_AddListEntry(p_config_menu, "Start Broadcast", config_broadcast_action);
  Menu_AddListEntry(p_config_menu, "Clear Sec. DB", clear_db_action);
  Menu_SetControlAction(p_devices_list_menu, MENU_DIRECTION_LEFT, stop_scanning_action);

  Menu_SetControlContent(p_rem_volume_control_menu, &rem_volume_text, 0);
  Menu_SetControlAction(p_rem_volume_control_menu, MENU_DIRECTION_UP, rem_volume_control_up_action);
  Menu_SetControlAction(p_rem_volume_control_menu, MENU_DIRECTION_DOWN, rem_volume_control_down_action);
  Menu_SetControlAction(p_rem_volume_control_menu, MENU_DIRECTION_RIGHT, rem_volume_control_right_action);

  Menu_SetControlContent(p_loc_volume_control_menu, &loc_volume_text, 0);
  Menu_SetControlAction(p_loc_volume_control_menu, MENU_DIRECTION_UP, loc_volume_control_up_action);
  Menu_SetControlAction(p_loc_volume_control_menu, MENU_DIRECTION_DOWN, loc_volume_control_down_action);
  Menu_SetControlAction(p_loc_volume_control_menu, MENU_DIRECTION_RIGHT, loc_volume_control_right_action);

  Menu_SetControlContent(p_microphone_control_menu,&microphone_text, 0);
  Menu_SetControlAction(p_microphone_control_menu, MENU_DIRECTION_RIGHT, microphone_control_right_action);

  Menu_SetControlContent(p_broadcast_config_menu, &broadcast_config_text, 0);
  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_DOWN, broadcast_conf_down_action);
  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_RIGHT, start_broadcast_action);

  Menu_SetControlContent(p_broadcast_stream_menu, &broadcast_stream_text, &output_icon);
  Menu_SetControlAction(p_broadcast_stream_menu, MENU_DIRECTION_LEFT, stop_broadcast_action);

  num_unicast_servers = 0;

  Menu_SetActivePage(p_stlogo_menu);
  UTIL_TIMER_Create(&Startup_Timer, STARTUP_TIMEOUT, UTIL_TIMER_ONESHOT, &Menu_Startup_TimerCallback, 0);
  UTIL_TIMER_Start(&Startup_Timer);
}

/**
 * @brief Set the connecting page as active
 */
void Menu_SetConnectingPage(uint16_t ConnHandle)
{
  snprintf(connecting_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Device #%d", ConnHandle);
  Menu_SetActivePage(p_connecting_menu);
}

/**
 * @brief Set the Linkup Page as active
 */
void Menu_SetLinkupPage(uint16_t ConnHandle)
{
  snprintf(linkup_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Device #%d", ConnHandle);
  Menu_SetActivePage(p_linkup_menu);
}

/**
 * @brief Set the NoStream Page as active
 */
void Menu_SetNoStreamPage(void)
{
  stream_active = 0;
  Menu_SetActivePage(p_nostream_menu);
}

/**
 * @brief Set the Streaming page as active
 * @param pSamplerateText: Samplerate string of the stream
 * @param AudioRole: Audio Role of the stream
 */
void Menu_SetStreamingPage(char* pSamplerateSinkText, char* pSamplerateSourceText, Audio_Role_t AudioRole)
{
  stream_active = 1;

  if (AudioRole & AUDIO_ROLE_SINK)
  {
    snprintf(&streaming_text.Lines[0][0], MENU_CONTROL_MAX_LINE_LEN, "Sink %s", pSamplerateSinkText);
  }
  else
  {
    snprintf(&streaming_text.Lines[0][0], MENU_CONTROL_MAX_LINE_LEN, "%s", "");
  }

  if (AudioRole & AUDIO_ROLE_SOURCE)
  {
    snprintf(&streaming_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "Source %s", pSamplerateSourceText);
  }
  else
  {
    snprintf(&streaming_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", "");
  }

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
void Menu_SetScanningPage(void)
{
  num_unicast_servers = 0;
  Menu_ClearList(p_devices_list_menu);
  Menu_SetActivePage(p_scan_menu);
}

/**
 * @brief Set the Startup Page as active
 */
void Menu_SetStartupPage(void)
{
  Menu_SetActivePage(p_startup_menu);
}

/**
 * @brief Set the Config Page as active
 */
void Menu_SetConfigPage(void)
{
  Menu_SetActivePage(p_config_menu);
}

/**
 * @brief Add a scanned unicast server to the device list
 */
void Menu_AddUnicastServer(uint8_t *pAddress, uint8_t AddressType, char *pDeviceName)
{
  uint8_t i;
  uint8_t duplicate = 0;
  for (i = 0; i < num_unicast_servers; i++)
  {
    if (memcmp(a_unicast_servers[i].Address, pAddress, 6u) == 0)
    {
      duplicate = 1;
    }
  }

  if (duplicate == 0 && num_unicast_servers < MAX_NUM_UNICAST_SERVERS)
  {
    UTIL_MEM_cpy_8(&a_unicast_servers[num_unicast_servers].Address[0], pAddress, 6u);
    a_unicast_servers[num_unicast_servers].AddressType = AddressType;

    Menu_AddListEntry(p_devices_list_menu, pDeviceName, select_device_action);

    if (num_unicast_servers == 0)
    {
      Menu_SetActivePage(p_devices_list_menu);
      p_devices_list_menu->pReturnPage = (struct Menu_Page_t *) p_config_menu;
    }
    else
    {
      Menu_Print();
    }

    num_unicast_servers++;
  }
}

/**
 * @brief Set Remote volume value
 */
void Menu_SetRemoteVolume(uint8_t Volume)
{
  snprintf(nostream_text.Lines[nostream_text.NumLines - 1], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(rem_volume_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  Menu_Print();
}

/**
 * @brief Set Local volume value
 */
void Menu_SetLocalVolume(uint8_t Volume)
{
  snprintf(loc_volume_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  Menu_Print();
}

/**
 * @brief Set Current Remote Microphone Mute Value
 */
void Menu_SetRemoteMicMute(uint8_t Mute)
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
 * @brief Set Profiles Linked Up
 */
void Menu_SetProfilesLinked(audio_profile_t Profiles)
{  
	char bap_string[5] = "";
	char vcp_string[5] = "";
	char micp_string[6] = "";
  if (Profiles != 0 && linkup_text.NumLines == 2)
  {
    snprintf(linkup_text.Lines[2], MENU_CONTROL_MAX_LINE_LEN, "%s", linkup_text.Lines[1]);
    linkup_text.NumLines = 3;
  }
  if (Profiles != 0 && nostream_text.NumLines == 3)
  {
    snprintf(nostream_text.Lines[3], MENU_CONTROL_MAX_LINE_LEN, "%s", nostream_text.Lines[2]);
    snprintf(nostream_text.Lines[2], MENU_CONTROL_MAX_LINE_LEN, "%s", nostream_text.Lines[1]);
    nostream_text.NumLines = 4;
  }
  if (Profiles != 0)
  {
    if (Profiles & AUDIO_PROFILE_UNICAST)
    {
      snprintf(bap_string, 5, " BAP");
    }
    if (Profiles & AUDIO_PROFILE_VCP)
    {
      snprintf(vcp_string, 5, " VCP");
    }
    if (Profiles & AUDIO_PROFILE_MICP)
    {
      snprintf(micp_string, 6, " MICP");
    }
    snprintf(linkup_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s%s%s", bap_string, vcp_string, micp_string);
    snprintf(nostream_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s%s%s", bap_string, vcp_string, micp_string);
  }
  Menu_Print();
}

/* Private Functions Definition --------------------------------------------- */
/**
 * @brief Start Scanning Callback
 */
static void Menu_Start_Scanning(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Start Scanning\n");
  num_unicast_servers = 0;
  Menu_ClearList(p_devices_list_menu);
  /* Start Scanning */
  status = GMAPAPP_StartScanning();
  UNUSED(status);
}
/**
 * @brief Stop Connection Callback
 */
static void Menu_Stop_Scanning(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Stop Scanning\n");
  status = GMAPAPP_StopScanning();
  LOG_INFO_APP("GMAPAPP_StopScanning() returns status 0x%02X\n",status);
  UNUSED(status);
}

/**
 * @brief Remote Volume Up Callback
 */
static void Menu_Remote_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  GMAPAPP_RemoteVolumeUp();
}

/**
 * @brief Remote Volume Down Callback
 */
static void Menu_Remote_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  GMAPAPP_RemoteVolumeDown();
}

/**
 * @brief Remote Volume Mute Callback
 */
static void Menu_Remote_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  GMAPAPP_RemoteToggleMute();
}

/**
 * @brief Local Volume Up Callback
 */
static void Menu_Local_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  GMAPAPP_LocalVolumeUp();
}

/**
 * @brief Local Volume Down Callback
 */
static void Menu_Local_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  GMAPAPP_LocalVolumeDown();
}

/**
 * @brief Local Volume Mute Callback
 */
static void Menu_Local_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  GMAPAPP_LocalToggleMute();
}

/**
 * @brief Mute Microphone Callback
 */
static void Menu_Microphone_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Microphone Mute\n");
  GMAPAPP_RemoteToggleMicMute();
}

/**
 * @brief Disconnect Callback
 */
static void Menu_Disconnect(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Disconnect\n");
  GMAPAPP_Disconnect();
}

/**
 * @brief Clear Security Database Callback
 */
static void Menu_ClearSecDB(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Clear Security DB\n");
  aci_gap_clear_security_db();
  GMAPAPP_ClearDatabase();
  Menu_SetActivePage(p_startup_menu);
}

/**
 * @brief Select Device Callback
 */
static void Menu_SelectDevice(uint8_t id)
{
  LOG_INFO_APP("[APP_MENU_CONF] Selected device %d\n", id);

  GMAPAPP_CreateConnection(&a_unicast_servers[id].Address[0], a_unicast_servers[id].AddressType);
}

/**
 * @brief Start Game + Voice Stream Callback
 */
static void Menu_StartStreamGameVoice(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Game + Voice Stream\n");

  GMAPAPP_StartStream(LC3_QOS_32_2_gs,
                      2,
                      AUDIO_CONTEXT_GAME,
                      LC3_QOS_16_2_gs, 1,
                      AUDIO_CONTEXT_CONVERSATIONAL,
                      AUDIO_DRIVER_CONFIG_LINEIN);
}

/**
 * @brief Start Game Stream Callback
 */
static void Menu_StartStreamGame(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Game Stream\n");

  GMAPAPP_StartStream(LC3_QOS_48_4_gr,
                      2,
                      AUDIO_CONTEXT_GAME,
                      0,
                      0,
                      0,
                      AUDIO_DRIVER_CONFIG_LINEIN);
}

/**
 * @brief Start Voice Stream Callback
 */
static void Menu_StartStreamVoice(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Voice Stream\n");
  
  if (APP_GetNumActiveACLConnections() == 1)
  {
    GMAPAPP_StartStream(LC3_QOS_32_2_gs,
                        1,
                        AUDIO_CONTEXT_CONVERSATIONAL,
                        LC3_QOS_32_2_gs,
                        1,
                        AUDIO_CONTEXT_CONVERSATIONAL,
                        AUDIO_DRIVER_CONFIG_HEADSET);
  }
  else
  {
    GMAPAPP_StartStream(LC3_QOS_32_2_gs,
                        1,
                        AUDIO_CONTEXT_CONVERSATIONAL,
                        LC3_QOS_16_2_gs,
                        1,
                        AUDIO_CONTEXT_CONVERSATIONAL,
                        AUDIO_DRIVER_CONFIG_HEADSET);
  }
}

/**
 * @brief Start Voice Record Stream Callback
 */
static void Menu_StartStreamVoiceRec(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Voice Record Stream\n");

  GMAPAPP_StartStream(0,
                      0,
                      0,
                      LC3_QOS_32_2_gs,
                      1,
                      AUDIO_CONTEXT_CONVERSATIONAL,
                      AUDIO_DRIVER_CONFIG_HEADSET);
}

/**
 * @brief Stop Stream Callback
 */
static void Menu_StopStream(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Stop Stream\n");

  GMAPAPP_StopStream();
}

/**
 * @brief Startup Timer Callback
 */
static void Menu_Startup_TimerCallback(void *arg)
{
  LOG_INFO_APP("[APP_MENU_CONF] Startup timer expired, Display home\n");
  Menu_SetStartupPage();
}

/**
 * @brief Start Broadcast Callback
 */
static void Menu_Start_Broadcast(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Broadcast\n");
  GMAPAPP_StartBroadcastSource(a_broadcast_conf_id[broadcast_freq_id]);
}

/**
 * @brief Stop Broadcast Callback
 */
static void Menu_Stop_Broadcast(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Stop Broadcast\n");
  GMAPAPP_StopBroadcastSource();
}

/**
 * @brief Broadcast Config Up callback
 */
static void Menu_Broadcast_Conf_Up(void)
{
  Menu_Action_t broadcast_conf_up_action = {MENU_ACTION_CALLBACK, &arrow_up_icon, &Menu_Broadcast_Conf_Up, 0, 0};
  Menu_Action_t broadcast_conf_down_action = {MENU_ACTION_CALLBACK, &arrow_down_icon, &Menu_Broadcast_Conf_Down, 0, 0};
  LOG_INFO_APP("[APP_MENU_CONF] Broadcast Conf Up\n");

  broadcast_freq_id = (broadcast_freq_id + 1) % 8;

  if (broadcast_freq_id == 7)
  {
    broadcast_conf_up_action.ActionType = 0;
  }

  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_UP, broadcast_conf_up_action);
  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_DOWN, broadcast_conf_down_action);

  snprintf(&broadcast_config_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", a_broadcast_conf_name[broadcast_freq_id]);
  snprintf(&broadcast_stream_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", a_broadcast_conf_name[broadcast_freq_id]);

  Menu_Print();
}

/**
 * @brief Broadcast Config Down callback
 */
static void Menu_Broadcast_Conf_Down(void)
{
  Menu_Action_t broadcast_conf_up_action = {MENU_ACTION_CALLBACK, &arrow_up_icon, &Menu_Broadcast_Conf_Up, 0, 0};
  Menu_Action_t broadcast_conf_down_action = {MENU_ACTION_CALLBACK, &arrow_down_icon, &Menu_Broadcast_Conf_Down, 0, 0};
  LOG_INFO_APP("[APP_MENU_CONF] Broadcast Conf Down\n");

  if (broadcast_freq_id > 0)
  {
    broadcast_freq_id--;
  }

  if (broadcast_freq_id == 0)
  {
    broadcast_conf_down_action.ActionType = 0;
  }

  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_UP, broadcast_conf_up_action);
  Menu_SetControlAction(p_broadcast_config_menu, MENU_DIRECTION_DOWN, broadcast_conf_down_action);

  snprintf(&broadcast_config_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", a_broadcast_conf_name[broadcast_freq_id]);
  snprintf(&broadcast_stream_text.Lines[1][0], MENU_CONTROL_MAX_LINE_LEN, "%s", a_broadcast_conf_name[broadcast_freq_id]);

  Menu_Print();
}
