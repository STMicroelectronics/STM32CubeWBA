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

/* External variables ------------------------------------------------------- */
extern uint8_t volume_mute_byteicon[];
extern uint8_t volume_up_byteicon[];
extern uint8_t volume_down_byteicon[];
extern uint8_t answer_call_byteicon[];
extern uint8_t terminate_call_byteicon[];
extern uint8_t incoming_call_byteicon[];
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
extern uint8_t arrow_left_byteicon[];

/* Private defines ---------------------------------------------------------- */
#define ADVERTISING_TIMEOUT 60000
#define STARTUP_TIMEOUT 1500
#define MAX_NUM_UNICAST_SERVERS 10

typedef struct
{
  uint8_t Address[6u];
  uint8_t AddressType;
} Unicast_Server_t;

/* Private variables -------------------------------------------------------- */
Menu_Icon_t access_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_right_byteicon, 16, 16, 0};
Menu_Icon_t return_menu_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &arrow_left_byteicon, 16, 16, 0};
Menu_Icon_t homelogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &homelogo_byteicon, 128, 64, 0};
Menu_Icon_t bluetooth_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &bluetooth_byteicon, 16, 16, 0};
Menu_Icon_t stlogo_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &stlogo_byteicon, 16, 16, 0};
Menu_Icon_t volume_up_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_up_byteicon, 16, 16, 0};
Menu_Icon_t volume_down_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_down_byteicon, 16, 16, 0};
Menu_Icon_t volume_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &volume_mute_byteicon, 16, 16, 0};
Menu_Icon_t answer_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &answer_call_byteicon, 16, 16, 0};
Menu_Icon_t terminate_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &terminate_call_byteicon, 16, 16, 0};
Menu_Icon_t incoming_call_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &incoming_call_byteicon, 16, 16, 0};
Menu_Icon_t microphone_mute_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &microphone_mute_byteicon, 16, 16, 0};
Menu_Icon_t next_preset_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &next_preset_byteicon, 16, 16, 0};
Menu_Icon_t previous_preset_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &previous_preset_byteicon, 16, 16, 0};
Menu_Icon_t advertising_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &advertising_byteicon, 16, 16, 0};
Menu_Icon_t hourglass_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &hourglass_byteicon, 16, 16, 0};
Menu_Icon_t input_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_byteicon, 16, 16, 0};
Menu_Icon_t input_output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &input_output_byteicon, 16, 16, 0};
Menu_Icon_t output_icon = {MENU_ICON_TYPE_IMAGE, (uint8_t *) &output_byteicon, 16, 16, 0};

Menu_Content_Text_t startup_text = {3, {"BLE Audio", "HAP Central", "Press Right"}};
Menu_Content_Text_t scan_text = {2, {"Scanning", "nearby servers"}};
Menu_Content_Text_t connecting_text = {2, {"Connecting", "Device #0"}};
Menu_Content_Text_t linkup_text = {2, {"Linking Up", "Device #0"}};
Menu_Content_Text_t nostream_text = {3, {"Connected", "No Stream", ""}};
Menu_Content_Text_t streaming_text = {2, {"Streaming 48KHz", ""}};
Menu_Content_Text_t rem_volume_text = {2, {"Remote Volume", ""}};
Menu_Content_Text_t loc_volume_text = {2, {"Local Volume", ""}};
Menu_Content_Text_t preset_text = {2, {"Preset 1", "Preset Name"}};
Menu_Content_Text_t call_text = {2, {"Call State", "IDLE"}};
Menu_Content_Text_t microphone_text = {2, {"Microphone", "IDLE"}};

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
Menu_Page_t *p_call_control_menu;
Menu_Page_t *p_microphone_control_menu;
Menu_Page_t *p_preset_control_menu;
Menu_Page_t *p_config_menu;

Menu_Action_t select_device_action;

static UTIL_TIMER_Object_t Startup_Timer;

Unicast_Server_t a_unicast_servers[MAX_NUM_UNICAST_SERVERS];
uint8_t num_unicast_servers;
uint8_t stream_active = 0;

/* Private functions prototypes-----------------------------------------------*/
static void Menu_Start_Scanning(void);
static void Menu_Stop_Scanning(void);
static void Menu_Remote_Volume_Up(void);
static void Menu_Remote_Volume_Down(void);
static void Menu_Remote_Volume_Mute(void);
static void Menu_Local_Volume_Up(void);
static void Menu_Local_Volume_Down(void);
static void Menu_Local_Volume_Mute(void);
static void Menu_Call_Answer(void);
static void Menu_Call_Terminate(void);
static void Menu_Call_Incoming(void);
static void Menu_Microphone_Mute(void);
static void Menu_Preset_NextPreset(void);
static void Menu_Preset_PreviousPreset(void);
static void Menu_Disconnect(void);
static void Menu_SelectDevice(uint8_t id);
static void Menu_StartMediaStream(void);
static void Menu_StartTelephonyStream(void);
static void Menu_StopStream(void);
static void Menu_ClearSecDB(void);
static void Menu_Startup_TimerCallback(void *arg);

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

  Menu_Action_t entry_menu_stream_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_remote_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_local_volume_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_call_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_microphone_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_preset_action = {MENU_ACTION_MENU_PAGE, &access_menu_icon, 0, 0, 0};
  Menu_Action_t entry_menu_disconnect_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_Disconnect, 0, 0};

  Menu_Action_t rem_volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Remote_Volume_Up, 0, 0};
  Menu_Action_t rem_volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Remote_Volume_Down, 0, 0};
  Menu_Action_t rem_volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Remote_Volume_Mute, 0, 0};

  Menu_Action_t loc_volume_control_up_action = {MENU_ACTION_CALLBACK, &volume_up_icon, &Menu_Local_Volume_Up, 0, 0};
  Menu_Action_t loc_volume_control_down_action = {MENU_ACTION_CALLBACK, &volume_down_icon, &Menu_Local_Volume_Down, 0, 0};
  Menu_Action_t loc_volume_control_right_action = {MENU_ACTION_CALLBACK, &volume_mute_icon, &Menu_Local_Volume_Mute, 0, 0};

  Menu_Action_t call_control_up_action = {MENU_ACTION_CALLBACK, &incoming_call_icon, &Menu_Call_Incoming, 0, 0};
  Menu_Action_t call_control_down_action = {MENU_ACTION_CALLBACK, &terminate_call_icon, &Menu_Call_Terminate, 0, 0};
  Menu_Action_t call_control_right_action = {MENU_ACTION_CALLBACK, &answer_call_icon, &Menu_Call_Answer, 0, 0};

  Menu_Action_t microphone_control_right_action = {MENU_ACTION_CALLBACK, &microphone_mute_icon, &Menu_Microphone_Mute, 0, 0};

  Menu_Action_t preset_control_up_action = {MENU_ACTION_CALLBACK, &next_preset_icon, &Menu_Preset_NextPreset, 0};
  Menu_Action_t preset_control_down_action = {MENU_ACTION_CALLBACK, &previous_preset_icon, &Menu_Preset_PreviousPreset, 0};

  select_device_action.ActionType = MENU_ACTION_LIST_CALLBACK;
  select_device_action.pIcon = &access_menu_icon;
  select_device_action.ListCallback = &Menu_SelectDevice;

  Menu_Action_t stream_start_media_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartMediaStream, 0, 0};
  Menu_Action_t stream_start_telephony_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StartTelephonyStream, 0, 0};
  Menu_Action_t stream_stop_action = {MENU_ACTION_CALLBACK, &access_menu_icon, &Menu_StopStream, 0, 0};

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
  p_call_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_microphone_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_preset_control_menu = Menu_CreatePage(MENU_TYPE_CONTROL);
  p_devices_list_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_audio_stream_menu = Menu_CreatePage(MENU_TYPE_LIST);
  p_config_menu = Menu_CreatePage(MENU_TYPE_LIST);

  start_scanning_action.pPage = (struct Menu_Page_t*) p_scan_menu;
  access_config_menu_action.pPage = (struct Menu_Page_t*) p_config_menu;
  access_menu_action.pPage = (struct Menu_Page_t*) p_control_menu;
  entry_menu_stream_action.pPage = (struct Menu_Page_t*) p_audio_stream_menu;
  entry_menu_remote_volume_action.pPage = (struct Menu_Page_t*) p_rem_volume_control_menu;
  entry_menu_local_volume_action.pPage = (struct Menu_Page_t*) p_loc_volume_control_menu;
  entry_menu_call_action.pPage = (struct Menu_Page_t*) p_call_control_menu;
  entry_menu_microphone_action.pPage = (struct Menu_Page_t*) p_microphone_control_menu;
  entry_menu_preset_action.pPage = (struct Menu_Page_t*) p_preset_control_menu;

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
  Menu_AddListEntry(p_control_menu, "Call...", entry_menu_call_action);
  Menu_AddListEntry(p_control_menu, "Microphone...", entry_menu_microphone_action);
  Menu_AddListEntry(p_control_menu, "Preset...", entry_menu_preset_action);
  Menu_AddListEntry(p_control_menu, "Disconnect", entry_menu_disconnect_action);

  Menu_AddListEntry(p_audio_stream_menu, "Start Media", stream_start_media_action);
  Menu_AddListEntry(p_audio_stream_menu, "Start Telephony", stream_start_telephony_action);
  Menu_AddListEntry(p_audio_stream_menu, "Stop", stream_stop_action);

  Menu_AddListEntry(p_config_menu, "Start Scan", start_scanning_action);
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

  Menu_SetControlContent(p_call_control_menu,&call_text, 0);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_UP, call_control_up_action);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_DOWN, call_control_down_action);
  Menu_SetControlAction(p_call_control_menu, MENU_DIRECTION_RIGHT, call_control_right_action);

  Menu_SetControlContent(p_microphone_control_menu,&microphone_text, 0);
  Menu_SetControlAction(p_microphone_control_menu, MENU_DIRECTION_RIGHT, microphone_control_right_action);

  Menu_SetControlContent(p_preset_control_menu, &preset_text, 0);
  Menu_SetControlAction(p_preset_control_menu, MENU_DIRECTION_UP, preset_control_up_action);
  Menu_SetControlAction(p_preset_control_menu, MENU_DIRECTION_DOWN, preset_control_down_action);

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
void Menu_SetStreamingPage(char* pSamplerateText, Audio_Role_t AudioRole)
{
  stream_active = 1;
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
  snprintf(nostream_text.Lines[2], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
  snprintf(streaming_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "Volume: %d%%", (uint8_t) (((uint16_t) Volume)*100/255));
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
void Menu_SetPresetName(char *pPresetName)
{
  snprintf(preset_text.Lines[1], MENU_CONTROL_MAX_LINE_LEN, "%s", pPresetName);
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
  status = HAPAPP_StartScanning();
  LOG_INFO_APP("HAPAPP_StartScanning() returns status 0x%02X\n",status);
  UNUSED(status);
}
/**
 * @brief Stop Connection Callback
 */
static void Menu_Stop_Scanning(void)
{
  uint8_t status;
  LOG_INFO_APP("[APP_MENU_CONF] Stop Scanning\n");
  status = HAPAPP_StopScanning();
  LOG_INFO_APP("HAPAPP_StopScanning() returns status 0x%02X\n",status);
  UNUSED(status);
}

/**
 * @brief Remote Volume Up Callback
 */
static void Menu_Remote_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  HAPAPP_RemoteVolumeUp();
}

/**
 * @brief Remote Volume Down Callback
 */
static void Menu_Remote_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  HAPAPP_RemoteVolumeDown();
}

/**
 * @brief Remote Volume Mute Callback
 */
static void Menu_Remote_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  HAPAPP_RemoteToggleMute();
}

/**
 * @brief Local Volume Up Callback
 */
static void Menu_Local_Volume_Up(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume+\n");
  HAPAPP_LocalVolumeUp();
}

/**
 * @brief Local Volume Down Callback
 */
static void Menu_Local_Volume_Down(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume-\n");
  HAPAPP_LocalVolumeDown();
}

/**
 * @brief Local Volume Mute Callback
 */
static void Menu_Local_Volume_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Volume Mute\n");
  HAPAPP_LocalToggleMute();
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
 * @brief Incoming Answer Callback
 */
static void Menu_Call_Incoming(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Incoming Call\n");
  HAPAPP_IncomingCall();
}

/**
 * @brief Mute Microphone Callback
 */
static void Menu_Microphone_Mute(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Microphone Mute\n");
  HAPAPP_RemoteToggleMicMute();
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
  Menu_SetActivePage(p_scan_menu);
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
 * @brief Select Device Callback
 */
static void Menu_SelectDevice(uint8_t id)
{
  LOG_INFO_APP("[APP_MENU_CONF] Selected device %d\n", id);

  HAPAPP_CreateConnection(&a_unicast_servers[id].Address[0], a_unicast_servers[id].AddressType);
}

/**
 * @brief Start Media Stream Callback
 */
static void Menu_StartMediaStream(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Media Stream\n");

  HAPAPP_StartMediaStream();

  if (stream_active == 1)
  {
    Menu_SetActivePage(p_streaming_menu);
  }
  else
  {
    Menu_SetActivePage(p_nostream_menu);
  }
}

/**
 * @brief Start Telephony Stream Callback
 */
static void Menu_StartTelephonyStream(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Start Telephony Stream\n");

  HAPAPP_StartTelephonyStream();

  if (stream_active == 1)
  {
    Menu_SetActivePage(p_streaming_menu);
  }
  else
  {
    Menu_SetActivePage(p_nostream_menu);
  }
}

/**
 * @brief Stop Stream Callback
 */
static void Menu_StopStream(void)
{
  LOG_INFO_APP("[APP_MENU_CONF] Stop Stream\n");

  HAPAPP_StopStream();
}

/**
 * @brief Startup Timer Callback
 */
static void Menu_Startup_TimerCallback(void *arg)
{
  LOG_INFO_APP("[APP_MENU_CONF] Startup timer expired, Display home\n");
  Menu_SetStartupPage();
}
