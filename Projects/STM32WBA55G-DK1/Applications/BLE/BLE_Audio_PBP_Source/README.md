## __BLE_Audio_PBP_Source Application Description__

How to use the Public Broadcast profile in Source role as specified by the Bluetooth SIG.

PBP Source application advertises and broadcasts audio streams to remote PBP Sinks.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Directory contents__

  - BLE_Audio_PBP_Source/Core/Inc/app_common.h                                      App Common application configuration file for STM32WPAN Middleware.
  - BLE_Audio_PBP_Source/Core/Inc/app_conf.h                                        Application configuration file for STM32WPAN Middleware.
  - BLE_Audio_PBP_Source/Core/Inc/app_entry.h                                       Interface to the application 
  - BLE_Audio_PBP_Source/Core/Inc/main.h                                            Header for main.c file. This file contains the common defines of the application.
  - BLE_Audio_PBP_Source/Core/Inc/stm32wba55g_discovery_conf.h                      STM32WBA55G_DK1 board configuration file.
  - BLE_Audio_PBP_Source/Core/Inc/stm32wbaxx_hal_conf.h                             HAL configuration file. 
  - BLE_Audio_PBP_Source/Core/Inc/stm32wbaxx_it.h                                   This file contains the headers of the interrupt handlers. 
  - BLE_Audio_PBP_Source/Core/Inc/stm32_assert.h                                    STM32 assert file. 
  - BLE_Audio_PBP_Source/Core/Inc/stm32_rtos.h                                      Include file for all RTOS/Sequencer can be used on WBA 
  - BLE_Audio_PBP_Source/Core/Inc/utilities_conf.h                                  Header for configuration file for STM32 Utilities.
  - BLE_Audio_PBP_Source/STM32_WPAN/App/app_ble.h                                   Header for ble application 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/ble_conf.h                                  Configuration file for BLE Middleware. 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/ble_dbg_conf.h                              Debug configuration file for BLE Middleware. 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/pbp_app.h                                   Header for pbp_app.c module 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/pbp_app_conf.h                              Application configuration file for pbp_app.c module 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp.h                     This file contains definitions used for the Public Broadcast Profile 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp_alloc.h               This file contains definitions used for Public Broadcast Profile Allocation context
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp_config.h              This file contains Public Broadcast Profile Configuration 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp_log.h                 This file contains definitions and configuration used for Public Broadcast Profile Logging
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/bpka.h                                   This file contains the interface of the BLE PKA module. 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/host_stack_if.h                          This file contains the interface for the stack tasks 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/ll_sys_if.h                              Header file for initiating system 
  - BLE_Audio_PBP_Source/System/Config/CRC_Ctrl/crc_ctrl_conf.h                     Configuration Header for crc_ctrl.c module 
  - BLE_Audio_PBP_Source/System/Config/Debug_GPIO/app_debug.h                       Real Time Debug module application APIs and signal table 
  - BLE_Audio_PBP_Source/System/Config/Debug_GPIO/app_debug_signal_def.h            Real Time Debug module application signal definition 
  - BLE_Audio_PBP_Source/System/Config/Debug_GPIO/debug_config.h                    Real Time Debug module general configuration file 
  - BLE_Audio_PBP_Source/System/Config/Flash/simple_nvm_arbiter_conf.h              Configuration header for simple_nvm_arbiter.c module 
  - BLE_Audio_PBP_Source/System/Config/Log/log_module_conf.h                        Header file of the log module. 
  - BLE_Audio_PBP_Source/System/Config/LowPower/app_sys.h                           Header for app_sys.c 
  - BLE_Audio_PBP_Source/System/Config/LowPower/peripheral_init.h                   Header for peripheral init module 
  - BLE_Audio_PBP_Source/System/Config/LowPower/user_low_power_config.h             Header for user_low_power_config.c 
  - BLE_Audio_PBP_Source/System/Interfaces/stm32_lpm_if.h                           Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_Audio_PBP_Source/System/Modules/ble_timer.h                                 This header defines the timer functions used by the BLE stack 
  - BLE_Audio_PBP_Source/System/Modules/general_config.h                            This file contains definitions that can be changed to configure some modules of the STM32 firmware application.
  - BLE_Audio_PBP_Source/Core/Src/app_entry.c                                       Entry point of the application 
  - BLE_Audio_PBP_Source/Core/Src/image.c                                           image used for the LCD 
  - BLE_Audio_PBP_Source/Core/Src/main.c                                            Main program body 
  - BLE_Audio_PBP_Source/Core/Src/stm32wbaxx_hal_msp.c                              This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_Audio_PBP_Source/Core/Src/stm32wbaxx_it.c                                   Interrupt Service Routines. 
  - BLE_Audio_PBP_Source/Core/Src/system_stm32wbaxx.c                               CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/app_ble.c                                   BLE Application 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/pbp_app.c                                   Public Broadcast Profile Application 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp.c                     This file contains Public Broadcast Profile feature 
  - BLE_Audio_PBP_Source/STM32_WPAN/App/AudioUseCases/pbp/pbp_alloc.c               This file contains Public Broadcast Profile Allocation context 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/bleplat.c                                This file implements the platform functions for BLE stack library. 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/ble_audio_if.c                           This file implements the interface functions for BLE Audio Profiles of the Generic Audio Framework
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/ble_audio_plat.c                         This file implements the platform functions for BLE Audio Profiles 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/ble_codec.c                              This file implements the Codec functions for BLE stack library. 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/bpka.c                                   This file implements the BLE PKA module. 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/codec_if.c                               File for codec manager integration : process and high speed timer This timer needs to run on the same clock domain as the audio interface
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/host_stack_if.c                          Source file for the stack tasks 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/linklayer_plat.c                         Source file for the linklayer plateform adaptation layer 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/ll_sys_if.c                              Source file for initiating system 
  - BLE_Audio_PBP_Source/STM32_WPAN/Target/power_table.c                            This file contains supported power tables 
  - BLE_Audio_PBP_Source/System/Config/CRC_Ctrl/crc_ctrl_conf.c                     Source for CRC client controller module configuration file 
  - BLE_Audio_PBP_Source/System/Config/Debug_GPIO/app_debug.c                       Real Time Debug module application side APIs 
  - BLE_Audio_PBP_Source/System/Config/Flash/simple_nvm_arbiter_conf.c              The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_Audio_PBP_Source/System/Config/LowPower/peripheral_init.c                   Header for peripheral init module 
  - BLE_Audio_PBP_Source/System/Config/LowPower/user_low_power_config.c             Low power related user configuration. 
  - BLE_Audio_PBP_Source/System/Interfaces/stm32_lpm_if.c                           Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_Audio_PBP_Source/System/Modules/ble_timer.c                                 This module implements the timer core functions

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.
    Connect the Discovery Board to your PC with a USB cable type C.
	Connect Audio Input on Jack Connector CN4.
  - Another Discovery board may be necessary to run PBP Sink application.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open IAR toolchain
 - Source ID is configurable thanks to BAP_BROADCAST_SOURCE_ID in pbp_app_conf.h file
 - Rebuild all files and flash the board with the executable file
 - Run the example

 - At startup, PBP Source starts Audio Stream Broadcast
 - Stream Audio in the Jack Connector CN4.
 - Source ID is displayed on the LCD interface


 - Power up a PBP Sink device with Source ID matching with the BAP_BROADCAST_SOURCE_ID value
 - Once PBP Source application is started and Audio Stream Broadcast is launched :
    - On the PBP Sink device, select with the Joystick the Source ID of the remote PBP Source to synchronize to.
 - Once PBP Sink device is synchronized to PBP Source and Audio Stream is received, Audio is available on Jack Connector.
