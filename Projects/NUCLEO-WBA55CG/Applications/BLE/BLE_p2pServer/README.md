## __BLE_p2pServer Application Description__

How to demonstrate Point-to-Point communication using BLE (as GATT server).

p2pServer application advertises and waits for a connection from either:

 - p2pClient application running on STM32WBAxx devices
 - ST BLE Toolbox smartphone application

Once connected, p2pServer can receive message from the Client and send notification to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_p2pServer/Core/Inc/app_common.h                                             App Common application configuration file for STM32WPAN Middleware
  - BLE_p2pServer/Core/Inc/app_conf.h                                               Application configuration file for STM32WPAN Middleware
  - BLE_p2pServer/Core/Inc/app_entry.h                                              Interface to the application 
  - BLE_p2pServer/Core/Inc/main.h                                                   Header for main.c file. This file contains the common defines of the application
  - BLE_p2pServer/Core/Inc/stm32wbaxx_hal_conf.h                                    HAL configuration file
  - BLE_p2pServer/Core/Inc/stm32wbaxx_it.h                                          This file contains the headers of the interrupt handlers
  - BLE_p2pServer/Core/Inc/stm32wbaxx_nucleo_conf.h                                 STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_p2pServer/Core/Inc/utilities_conf.h                                         Header for configuration file for STM32 Utilities
  - BLE_p2pServer/STM32_WPAN/App/app_ble.h                                          Header for ble application 
  - BLE_p2pServer/STM32_WPAN/App/ble_conf.h                                         Configuration file for BLE Middleware
  - BLE_p2pServer/STM32_WPAN/App/ble_dbg_conf.h                                     Debug configuration file for BLE Middleware
  - BLE_p2pServer/STM32_WPAN/App/p2p_server.h                                       Header for p2p_server.c 
  - BLE_p2pServer/STM32_WPAN/App/p2p_server_app.h                                   Header for p2p_server_app.c 
  - BLE_p2pServer/STM32_WPAN/Target/bpka.h                                          This file contains the interface of the BLE PKA module 
  - BLE_p2pServer/STM32_WPAN/Target/host_stack_if.h                                 This file contains the interface for the stack tasks 
  - BLE_p2pServer/STM32_WPAN/Target/ll_sys_if.h                                     Header file for ll_sys_if.c
  - BLE_p2pServer/System/Config/Debug_GPIO/app_debug.h                              Real Time Debug module application APIs and signal table 
  - BLE_p2pServer/System/Config/Debug_GPIO/app_debug_signal_def.h                   Real Time Debug module application signal definition 
  - BLE_p2pServer/System/Config/Debug_GPIO/debug_config.h                           Real Time Debug module general configuration file 
  - BLE_p2pServer/System/Config/Flash/simple_nvm_arbiter_conf.h                     Configuration header for simple_nvm_arbiter.c module 
  - BLE_p2pServer/System/Config/LowPower/app_sys.h                                  Header for app_sys.c 
  - BLE_p2pServer/System/Config/LowPower/user_low_power_config.h                    Header for user_low_power_config.c
  - BLE_p2pServer/System/Interfaces/hw.h                                            This file contains the interface of STM32 HW drivers 
  - BLE_p2pServer/System/Interfaces/hw_if.h                                         Hardware Interface 
  - BLE_p2pServer/System/Interfaces/stm32_lpm_if.h                                  Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_p2pServer/System/Interfaces/timer_if.h                                      configuration of the timer_if.c instances 
  - BLE_p2pServer/System/Interfaces/usart_if.h                                      Header file for stm32_adv_trace interface file 
  - BLE_p2pServer/System/Modules/adc_ctrl.h                                         Header for ADC client manager module 
  - BLE_p2pServer/System/Modules/ble_timer.h                                        This header defines the timer functions used by the BLE stack 
  - BLE_p2pServer/System/Modules/dbg_trace.h                                        Header for dbg_trace.c 
  - BLE_p2pServer/System/Modules/otp.h                                              Header file for One Time Programmable (OTP) area 
  - BLE_p2pServer/System/Modules/scm.h                                              Header for scm.c module 
  - BLE_p2pServer/System/Modules/stm_list.h                                         Header file for linked list library 
  - BLE_p2pServer/System/Modules/utilities_common.h                                 Common file to utilities 
  - BLE_p2pServer/System/Modules/baes/baes.h                                        This file contains the interface of the basic AES software module 
  - BLE_p2pServer/System/Modules/baes/baes_global.h                                 This file contains the internal definitions of the AES software module
  - BLE_p2pServer/System/Modules/Flash/flash_driver.h                               Header for flash_driver.c module 
  - BLE_p2pServer/System/Modules/Flash/flash_manager.h                              Header for flash_manager.c module 
  - BLE_p2pServer/System/Modules/Flash/rf_timing_synchro.h                          Header for rf_timing_synchro.c module 
  - BLE_p2pServer/System/Modules/Flash/simple_nvm_arbiter.h                         Header for simple_nvm_arbiter.c module 
  - BLE_p2pServer/System/Modules/Flash/simple_nvm_arbiter_common.h                  Common header of simple_nvm_arbiter.c module 
  - BLE_p2pServer/System/Modules/MemoryManager/advanced_memory_manager.h            Header for advance_memory_manager.c module 
  - BLE_p2pServer/System/Modules/MemoryManager/stm32_mm.h                           Header for stm32_mm.c module 
  - BLE_p2pServer/System/Modules/Nvm/nvm.h                                          This file contains the interface of the NVM manager 
  - BLE_p2pServer/System/Modules/RFControl/rf_antenna_switch.h                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer/System/Modules/RTDebug/debug_signals.h                            Real Time Debug module System and Link Layer signal definition 
  - BLE_p2pServer/System/Modules/RTDebug/local_debug_tables.h                       Real Time Debug module System and Link Layer signal 
  - BLE_p2pServer/System/Modules/RTDebug/RTDebug.h                                  Real Time Debug module API declaration 
  - BLE_p2pServer/System/Modules/RTDebug/RTDebug_dtb.h                              Real Time Debug module API declaration for DTB usage
  - BLE_p2pServer/Core/Src/app_entry.c                                              Entry point of the application 
  - BLE_p2pServer/Core/Src/main.c                                                   Main program body 
  - BLE_p2pServer/Core/Src/stm32wbaxx_hal_msp.c                                     This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_p2pServer/Core/Src/stm32wbaxx_it.c                                          Interrupt Service Routines 
  - BLE_p2pServer/Core/Src/system_stm32wbaxx.c                                      CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_p2pServer/STM32_WPAN/App/app_ble.c                                          BLE Application 
  - BLE_p2pServer/STM32_WPAN/App/p2p_server.c                                       p2p_server definition 
  - BLE_p2pServer/STM32_WPAN/App/p2p_server_app.c                                   p2p_server_app application definition 
  - BLE_p2pServer/STM32_WPAN/Target/bleplat.c                                       This file implements the platform functions for BLE stack library 
  - BLE_p2pServer/STM32_WPAN/Target/bpka.c                                          This file implements the BLE PKA module 
  - BLE_p2pServer/STM32_WPAN/Target/host_stack_if.c                                 Source file for the stack tasks 
  - BLE_p2pServer/STM32_WPAN/Target/linklayer_plat.c                                Source file for the linklayer plateform adaptation layer 
  - BLE_p2pServer/STM32_WPAN/Target/ll_sys_if.c                                     Source file for initiating the system sequencer 
  - BLE_p2pServer/STM32_WPAN/Target/power_table.c                                   This file contains supported power tables 
  - BLE_p2pServer/System/Config/Debug_GPIO/app_debug.c                              Real Time Debug module application side APIs 
  - BLE_p2pServer/System/Config/LowPower/user_low_power_config.c                    Low power related user configuration
  - BLE_p2pServer/System/Interfaces/hw_aes.c                                        This file contains the AES driver for STM32WBA 
  - BLE_p2pServer/System/Interfaces/hw_otp.c                                        This file contains the OTP driver 
  - BLE_p2pServer/System/Interfaces/hw_pka.c                                        This file contains the PKA driver for STM32WBA 
  - BLE_p2pServer/System/Interfaces/hw_rng.c                                        This file contains the RNG driver for STM32WBA 
  - BLE_p2pServer/System/Interfaces/pka_p256.c                                      This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_p2pServer/System/Interfaces/stm32_lpm_if.c                                  Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_p2pServer/System/Interfaces/timer_if.c                                      Configure RTC Alarm, Tick and Calendar manager 
  - BLE_p2pServer/System/Interfaces/usart_if.c                                      Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_p2pServer/System/Modules/adc_ctrl.c                                         Header for ADC client manager module 
  - BLE_p2pServer/System/Modules/app_sys.c                                          Application system for STM32WPAN Middleware 
  - BLE_p2pServer/System/Modules/ble_timer.c                                        This module implements the timer core functions 
  - BLE_p2pServer/System/Modules/otp.c                                              Source file for One Time Programmable (OTP) area 
  - BLE_p2pServer/System/Modules/scm.c                                              Functions for the System Clock Manager 
  - BLE_p2pServer/System/Modules/stm_list.c                                         TCircular Linked List Implementation 
  - BLE_p2pServer/System/Modules/baes/baes_cmac.c                                   This file contains the AES CMAC implementation 
  - BLE_p2pServer/System/Modules/baes/baes_ecb.c                                    This file contains the AES ECB functions implementation 
  - BLE_p2pServer/System/Modules/Flash/flash_driver.c                               The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_p2pServer/System/Modules/Flash/flash_manager.c                              The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_p2pServer/System/Modules/Flash/rf_timing_synchro.c                          The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_p2pServer/System/Modules/Flash/simple_nvm_arbiter.c                         The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_p2pServer/System/Modules/MemoryManager/advanced_memory_manager.c            Memory Manager 
  - BLE_p2pServer/System/Modules/MemoryManager/stm32_mm.c                           Memory Manager 
  - BLE_p2pServer/System/Modules/Nvm/nvm_emul.c                                     This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_p2pServer/System/Modules/RFControl/rf_antenna_switch.c                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer/System/Modules/RTDebug/RTDebug.c                                  Real Time Debug module API definition 
  - BLE_p2pServer/System/Modules/RTDebug/RTDebug_dtb.c                              Real Time Debug module API definition for DTB usage

### __Hardware and Software environment__

  - This application runs on STM32WBA55 Nucleo board.
  - Another Nucleo board may be necessary to run p2pClient or p2pRouter application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with p2pServer application with a smartphone:__

 - Install and launch ST BLE Toolbox application on android or iOS smartphone
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

 - Open ST BLE Toolbox application:
   select the P2PS_WBAxx in the device list, where xx is the 2 last digits of the BD ADDRESS.
 - On p2pServer device B1 click, a notification message is sent toward connected smartphone.
 - On smartphone interface, click on the LED icon to switch On/Off LED1 of the Nucleo board.

 __Antother possibility is to interact with p2pServer application with another STM32WBA52 Nucleo board:__

 - Power up p2pClient devices next to p2pServer device.
 - On p2pClient device, click on button B1 to launch a scan. A connection is initiated if a p2pServer device is detected.
    - On p2pServer device, click on button B1 to send a notification message toward connected p2pClient device.
    - On p2pClient device, click on button B1, to write a message toward connected p2pServer device.
 
 - Adertising is stopped after 60s, button B1 click allows to restart it.
 - When not connected, button B2 click allows to clear security database.
 - Once connected, button B3 click allows to update connection interval parameters.

Multi connection support:

- Pressing button B2 while the device is already connected starts a new advertising to allow multi-connection.