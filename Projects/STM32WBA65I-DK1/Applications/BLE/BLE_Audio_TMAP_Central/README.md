## __BLE_Audio_TMAP_Central Application Description__

How to use the Telephony and Media Audio Profile (TMAP) in Unicast Client role (Call Gateway and/or Unicast Media Sender) and Broadcast Source role (Broadcast Media Sender) as specified by the Bluetooth SIG.


The TMAP Central application, in __Unicast Client role__, performs the following actions:<br>
- Scans and connects to remote Unicast Servers (Call Terminal and/or Unicast Media Receiver).<br>
- Initiate Isochronous Connection.<br>
- Supports Volume Control, Call Control, and Media Control features.<br>

The TMAP Central application, in __Broadcast Source role__, advertises and broadcasts audio streams to remote Broadcast Sinks.<br>
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Directory contents__

  - BLE_Audio_TMAP_Central/Core/Inc/app_common.h                                    App Common application configuration file for STM32WPAN Middleware.
  - BLE_Audio_TMAP_Central/Core/Inc/app_conf.h                                      Application configuration file for STM32WPAN Middleware.
  - BLE_Audio_TMAP_Central/Core/Inc/app_entry.h                                     Interface to the application
  - BLE_Audio_TMAP_Central/Core/Inc/main.h                                          Header for main.c file. This file contains the common defines of the application.
  - BLE_Audio_TMAP_Central/Core/Inc/stm32wba65i_discovery_conf.h                   	STM32WBA65I_DK1 board configuration file.
  - BLE_Audio_TMAP_Central/Core/Inc/stm32wbaxx_hal_conf.h                           HAL configuration file. 
  - BLE_Audio_TMAP_Central/Core/Inc/stm32wbaxx_it.h                                 This file contains the headers of the interrupt handlers. 
  - BLE_Audio_TMAP_Central/Core/Inc/stm32_assert.h                                  STM32 assert file. 
  - BLE_Audio_TMAP_Central/Core/Inc/stm32_rtos.h                                    Include file for all RTOS/Sequencer can be used on WBA 
  - BLE_Audio_TMAP_Central/Core/Inc/utilities_conf.h                                Header for configuration file for STM32 Utilities.
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_ble.h                                 Header for ble application 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_menu.h                                Header for Menu file. 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_menu_cfg.h                            Header for Menu Configuration file. 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/ble_conf.h                                Configuration file for BLE Middleware. 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/ble_dbg_conf.h                            Debug configuration file for BLE Middleware. 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/tmap_app.h                                Header for tmap_app.c module 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/tmap_app_conf.h                           Application configuration file for tmap_app.c module 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/usecase_dev_mgmt.h          Device Management Interface for Use Case Profiles 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap.h                 This file contains definitions used for Telephony and Media Audio Profile
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_alloc.h           This file contains definitions used for Telephony and Media Audio Profile Allocation context
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_config.h          This file contains Telephony and Media Audio Profile Configuration 
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_db.h              This file contains definitions used for TMAP Profile Database.
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_log.h             This file contains definitions and configuration used for Telephony and Media Audio Profile Logging
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmas.h                 This file contains definitions used for Telephony and Media Audio Service
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/bpka.h                                 This file contains the interface of the BLE PKA module. 
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/host_stack_if.h                        This file contains the interface for the stack tasks 
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/ll_sys_if.h                            Header file for initiating system 
  - BLE_Audio_TMAP_Central/System/Config/CRC_Ctrl/crc_ctrl_conf.h                   Configuration Header for crc_ctrl.c module 
  - BLE_Audio_TMAP_Central/System/Config/Debug_GPIO/app_debug.h                     Real Time Debug module application APIs and signal table 
  - BLE_Audio_TMAP_Central/System/Config/Debug_GPIO/app_debug_signal_def.h          Real Time Debug module application signal definition 
  - BLE_Audio_TMAP_Central/System/Config/Debug_GPIO/debug_config.h                  Real Time Debug module general configuration file
  - BLE_Audio_TMAP_Central/System/Config/Flash/simple_nvm_arbiter_conf.h            Configuration header for simple_nvm_arbiter.c module
  - BLE_Audio_TMAP_Central/System/Config/Log/log_module_conf.h                      Header file of the log module.
  - BLE_Audio_TMAP_Central/System/Config/LowPower/app_sys.h                         Header for app_sys.c
  - BLE_Audio_TMAP_Central/System/Config/LowPower/peripheral_init.h                 Header for peripheral init module
  - BLE_Audio_TMAP_Central/System/Config/LowPower/user_low_power_config.h           Header for user_low_power_config.c
  - BLE_Audio_TMAP_Central/System/Interfaces/stm32_lpm_if.h                         Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_Audio_TMAP_Central/System/Modules/ble_timer.h                               This header defines the timer functions used by the BLE stack
  - BLE_Audio_TMAP_Central/System/Modules/general_config.h                          This file contains definitions that can be changed to configure some modules of the STM32 firmware application.
  - BLE_Audio_TMAP_Central/Core/Src/app_entry.c                                     Entry point of the application
  - BLE_Audio_TMAP_Central/Core/Src/image.c                                         image used for the LCD
  - BLE_Audio_TMAP_Central/Core/Src/main.c                                          : Main program body
  - BLE_Audio_TMAP_Central/Core/Src/stm32wbaxx_hal_msp.c                            This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_Audio_TMAP_Central/Core/Src/stm32wbaxx_it.c                                 Interrupt Service Routines.
  - BLE_Audio_TMAP_Central/Core/Src/system_stm32wbaxx.c                             CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_bsp.c                                 Application to manage BSP
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_ble.c                                 BLE Application
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_menu.c                                Initialize the Menu module
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/app_menu_cfg.c                            Initialize and setup the menu
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/tmap_app.c                                TMAP Application
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/usecase_dev_mgmt.c          This file contains Device Management for Use Case Profiles
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap.c                 This file contains Telephony and Media Audio Profile feature
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_alloc.c           This file contains Telephony and Media Audio Profile Allocation context
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmap_db.c              This file contains interfaces of TMAP Database
  - BLE_Audio_TMAP_Central/STM32_WPAN/App/AudioUseCases/tmap/tmas.c                 This file contains Telephony and Media Audio Service
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/bleplat.c                              This file implements the platform functions for BLE stack library.
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/ble_audio_if.c                         This file implements the interface functions for BLE Audio Profiles of the Generic Audio Framework
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/ble_audio_plat.c                       This file implements the platform functions for BLE Audio Profiles
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/ble_codec.c                            This file implements the Codec functions for BLE stack library.
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/bpka.c                                 This file implements the BLE PKA module.
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/codec_if.c                             File for codec manager integration : process and high speed timer This timer needs to run on the same clock domain as the audio interface
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/host_stack_if.c                        Source file for the stack tasks
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/linklayer_plat.c                       Source file for the linklayer plateform adaptation layer
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/ll_sys_if.c                            Source file for initiating system
  - BLE_Audio_TMAP_Central/STM32_WPAN/Target/power_table.c                          This file contains supported power tables
  - BLE_Audio_TMAP_Central/System/Config/CRC_Ctrl/crc_ctrl_conf.c                   Source for CRC client controller module configuration file
  - BLE_Audio_TMAP_Central/System/Config/Debug_GPIO/app_debug.c                     Real Time Debug module application side APIs
  - BLE_Audio_TMAP_Central/System/Config/Flash/simple_nvm_arbiter_conf.c            The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_Audio_TMAP_Central/System/Config/LowPower/peripheral_init.c                 Header for peripheral init module
  - BLE_Audio_TMAP_Central/System/Config/LowPower/user_low_power_config.c           Low power related user configuration.
  - BLE_Audio_TMAP_Central/System/Interfaces/stm32_lpm_if.c                         Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_Audio_TMAP_Central/System/Modules/ble_timer.c                               This module implements the timer core functions


### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices with SMPS.
  <br>Connect the Discovery Board to your PC with a USB cable type C
  <br>Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.
  <br>Connect Audio Input on Jack Connector CN4 for Media streaming

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
<br>
<br>
 - __At Startup__
 <br>After pressing the Right direction on the joystick, the user accesses a menu offering following actions:
	- __Start Unicast__ : the application starts the features associated to the TMAP Unicast Client role (Unicast Client, Media Control, Call Control, Volume Control).<br>
	- __Start Broadcast__ : the application starts the features associated to the TMAP Broadcast Source role.<br>
	- __Clear Sec. DB__ : clear all the information of the bonded devices
<br>
 - __Start Unicast__
<br>Once "Start Unicast" in TMAP Central is executed, the TMAP Central starts scanning of nearby Unicast Servers
	- Select the Unicast Server you want to connect with the Right direction on the joystick. Pairing and Link-up are initiated automatically.<br>
  If the remote TMAP Peripheral is a Set member of a Coordinated Set, the application starts discovery and linkup of the other Set members of a Coordinated Set (if CFG_BLE_NUM_LINK > 1 and APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY = 1)
	- Once connected to the Unicast Server, use the Right direction on the joystick to access the menu:
		- __Select Audio Stream__: Access a submenu where you can :
          - Start Media (48KHz unidirectional stereo stream).
          - Start Telephony (32KHz bidirectional mono stream if connected to one audio device ; 16KHz when connected to 2 Set members of a Coordinated Set).
          - Stop the current stream.
	  - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Controller role (Volume Up / Volume Down / Mute)<br>
	  - __Select Local Volume Control__: Access a panel where the local volume can be adjusted (Volume Up / Volume Down / Mute)<br>
	  - __Select Media Control__: Access a panel where the local media tracks can be controlled and displayed using the MCP as Media Control Server (Next Track / Previous Track / Play|Pause)<br>
	  - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Server (Incoming Call / Answer Call / Terminate Call)<br>
	  - __Select Disconnect__: Disconnect the connected devices
<br>
 - __Start Broadcast__
<br>Once "Start Broadcast" in TMAP Central is executed, the application first offers to select a Streaming frequency configuration in a menu.
    - Once Streaming frequency selected by using Up/Down direction, push the joystick in the Right direction to start Audio Stream Broadcast.
    - Once Broadcasting is started, to stop broadcasting, push the joystick in the Left direction.
<br>
 - __Clear Security Database__
<br>Once "Clear Sec. DB" in TMAP Central is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developped by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Telephony_%26_Media_Audio_Profile"> Bluetooth® Low Energy audio - Telephony & Media Audio Profile</a>

