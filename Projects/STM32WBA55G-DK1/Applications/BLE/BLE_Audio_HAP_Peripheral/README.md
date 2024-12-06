
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## __BLE_Audio_HAP_Peripheral Application Description__

How to use the Hearing Access Profile in Unicast Server role (Hearing Aid role) as specified by the Bluetooth SIG.

The HAP Peripheral application advertises and accepts Isochronous Connection from a remote HAP Central (Hearing Aid Unicast Client).
The HAP Periphaer supports Volume Control, Microphone Control and Call Control features.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Directory contents__


  - BLE_Audio_HAP_Peripheral/Core/Inc/app_common.h                                     	App Common application configuration file for STM32WPAN Middleware.
  - BLE_Audio_HAP_Peripheral/Core/Inc/app_conf.h                                       	Application configuration file for STM32WPAN Middleware.
  - BLE_Audio_HAP_Peripheral/Core/Inc/app_entry.h                                      	Interface to the application
  - BLE_Audio_HAP_Peripheral/Core/Inc/main.h                                        	This file contains the common defines of the application.
  - BLE_Audio_HAP_Peripheral/Core/Inc/stm32wba55g_discovery_conf.h                   	STM32WBA55G_DK1 board configuration file.
  - BLE_Audio_HAP_Peripheral/Core/Inc/stm32wbaxx_hal_conf.h                            	HAL configuration file.
  - BLE_Audio_HAP_Peripheral/Core/Inc/stm32wbaxx_it.h                                  	This file contains the headers of the interrupt handlers.
  - BLE_Audio_HAP_Peripheral/Core/Inc/stm32_assert.h                                   	STM32 assert file.
  - BLE_Audio_HAP_Peripheral/Core/Inc/stm32_rtos.h                                     	Include file for all RTOS/Sequencer can be used on WBA
  - BLE_Audio_HAP_Peripheral/Core/Inc/utilities_conf.h                                 	Header for configuration file for STM32 Utilities.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_ble.h                                  	Header for ble application
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_menu.h                                 	Header for Menu file.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_menu_cfg.h                             	Header for Menu Configuration file.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/ble_conf.h                                 	Configuration file for BLE Middleware.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/ble_dbg_conf.h                             	Debug configuration file for BLE Middleware.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/hap_app.h                                  	Header for hap_app.c module
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/hap_app_conf.h                             	Application configuration file for hap_app.c module
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/usecase_dev_mgmt.h           	Device Management Interface for Use Case Profiles
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap.h                    	This file contains definitions used for Hearing Access Profile
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_alloc.h              	This file contains definitions used for Hearing Access Profile Allocation context
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_config.h             	This file contains Hearing Access Profile Configuration
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_db.h               	This file contains definitions Hearing Access Profile Dababase management
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_ha.h                 	This file contains definitions used for Hearing Access Profile's Hearing Aid role
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_harc.h               	This file contains definitions used for Hearing Access Profile's Hearing Aid Remote Controller role
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_iac.h               	This file contains definitions used for Hearing Access Profile's Immediate Alert Client role
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_log.h           		This file contains definitions and configuration used for Hearing Access Profile Logging
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/has.h                    	This file contains definitions used for Hearing Access Service
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/ias.h                    	This file contains definitions used for Immediate Alert Service
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/bpka.h                                  	This file contains the interface of the BLE PKA module.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/host_stack_if.h                        	This file contains the interface for the stack tasks
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/ll_sys_if.h                             	Header file for initiating system
  - BLE_Audio_HAP_Peripheral/System/Config/CRC_Ctrl/crc_ctrl_conf.h                    	Configuration Header for crc_ctrl.c module
  - BLE_Audio_HAP_Peripheral/System/Config/Debug_GPIO/app_debug.h                      	Real Time Debug module application APIs and signal table
  - BLE_Audio_HAP_Peripheral/System/Config/Debug_GPIO/app_debug_signal_def.h           	Real Time Debug module application signal definition
  - BLE_Audio_HAP_Peripheral/System/Config/Debug_GPIO/debug_config.h                   	Real Time Debug module general configuration file
  - BLE_Audio_HAP_Peripheral/System/Config/Flash/simple_nvm_arbiter_conf.h             	Configuration header for simple_nvm_arbiter.c module
  - BLE_Audio_HAP_Peripheral/System/Config/Log/log_module_conf.h                       	Header file of the log module.
  - BLE_Audio_HAP_Peripheral/System/Config/LowPower/app_sys.h                          	Header for app_sys.c
  - BLE_Audio_HAP_Peripheral/System/Config/LowPower/peripheral_init.h                  	Header for peripheral init module
  - BLE_Audio_HAP_Peripheral/System/Config/LowPower/user_low_power_config.h            	Header for user_low_power_config.c
  - BLE_Audio_HAP_Peripheral/System/Interfaces/stm32_lpm_if.h                          	Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_Audio_HAP_Peripheral/System/Modules/ble_timer.h                                	This header defines the timer functions used by the BLE stack
  - BLE_Audio_HAP_Peripheral/System/Modules/general_config.h                         	This file contains definitions that can be changed to configure some modules of the STM32 firmware application
  - BLE_Audio_HAP_Peripheral/Core/Src/app_entry.c                                      	Entry point of the application
  - BLE_Audio_HAP_Peripheral/Core/Src/image.c                                          	Image used for the LCD
  - BLE_Audio_HAP_Peripheral/Core/Src/main.c                                           	Main program body
  - BLE_Audio_HAP_Peripheral/Core/Src/stm32wbaxx_hal_msp.c                             	This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_Audio_HAP_Peripheral/Core/Src/stm32wbaxx_it.c                                  	Interrupt Service Routines.
  - BLE_Audio_HAP_Peripheral/Core/Src/system_stm32wbaxx.c                              	CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_ble.c                                  	BLE Application
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_menu.c                                 	Initialize the Menu module
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/app_menu_cfg.c                             	Initialize and setup the menu
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/hap_app.c                                  	HAP Application
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/usecase_dev_mgmt.c           	This file contains Device Management for Use Case Profiles
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap.c                    	This file contains Hearing Access Profile feature
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_alloc.c              	This file contains Hearing Access Profile Allocation context
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_db.c                 	This file contains interfaces of HAP Database
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_ha.c                 	This file contains Hearing Access Profile feature for Hearing Aid
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_harc.c               	This file contains Hearing Access Profile feature for Hearing Aid
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/hap_iac.c                	This file contains Hearing Access Profile feature for Immediate
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/has.c                    	This file contains Hearing Access Service
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/App/AudioUseCases/hap/ias.c                    	Immediate Alert Service
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/bleplat.c                               	This file implements the platform functions for BLE stack library.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/ble_audio_if.c                          	This file implements the interface functions for BLE Audio Profiles of the Generic Audio Framework
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/ble_audio_plat.c                        	This file implements the platform functions for BLE Audio Profiles
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/ble_codec.c                             	This file implements the Codec functions for BLE stack library.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/bpka.c                                  	This file implements the BLE PKA module.
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/codec_if.c   							File for codec manager integration : process and high speed timer. This timer needs to run on the same clock domain as the audio interface
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/host_stack_if.c                         	Source file for the stack tasks
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/linklayer_plat.c                        	Source file for the linklayer plateform adaptation layer
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/ll_sys_if.c                             	Source file for initiating system
  - BLE_Audio_HAP_Peripheral/STM32_WPAN/Target/power_table.c                           	This file contains supported power tables
  - BLE_Audio_HAP_Peripheral/System/Config/CRC_Ctrl/crc_ctrl_conf.c                    	Source for CRC client controller module configuration file
  - BLE_Audio_HAP_Peripheral/System/Config/Debug_GPIO/app_debug.c                      	Real Time Debug module application side APIs
  - BLE_Audio_HAP_Peripheral/System/Config/Flash/simple_nvm_arbiter_conf.c 				The Simple NVM arbiter module provides an interface to write and/or  restore data from SRAM to FLASH with use of NVMs.
  - BLE_Audio_HAP_Peripheral/System/Config/LowPower/peripheral_init.c                  	Header for peripheral init module
  - BLE_Audio_HAP_Peripheral/System/Config/LowPower/user_low_power_config.c            	Low power related user configuration.
  - BLE_Audio_HAP_Peripheral/System/Interfaces/stm32_lpm_if.c                          	Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_Audio_HAP_Peripheral/System/Modules/ble_timer.c                                	This module implements the timer core functions

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices with SMPS.
    Connect the Discovery Board to your PC with a USB cable type C. 
	Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 - At startup, after pressing the Right direction on the joystick and selecting "Start Adv.", the HAP Unicast Server starts advertising with a name displayed on the screen
 - Using a device compatible with BLE Audio Unicast Client role (Smartphone supporting LE Audio Feature for example), connect to the HAP Peripheral acting as Unicast Server and perform Link-Up of available services
 - Once the Unicast Client is connected, it can start a streaming procedure (Media Playing or Call) to send audio to the Unicast Server. High Quality Media unidirectional streams can be established, as well as conversational bidirectional streams.
 - Using the Right direction on the joystick during connection, the menu can be accessed:
	- Select Volume Control to access a panel where the volume can be adjusted and displayed using the VCP as VCP Renderer role (Volume Up / Volume Down / Mute)
	- Select Microphone Control to access a panel where the microphone mute state can be adjusted and displayed using the MICP as MICP Device role (Microphone Mute)
	- Select Call Control to access a panel where the current calls can be controlled and displayed using the CCP as Call Control Client (Answer Call / Terminate Call)
	- Select Preset Control to access a panel where the active Hearing Aid preset can be selected using the HAP as Hearing Aid role (Next Preset / Previous Preset)
	- Select Disconnect to disconnect from the device
:::
:::


:::
:::

