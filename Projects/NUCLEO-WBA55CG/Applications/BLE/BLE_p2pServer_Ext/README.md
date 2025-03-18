## __BLE_p2pServer_Ext Application Description__

Demonstrate STM32WBA using BLE basic plus stack version to use several extended advertising sets.

p2pServer_Ext application advertise several extended advertising sets:

- A legacy advertising
- A connectable extended advertising
- A scannable extended advertising
- A not connectable and not scannable extended advertising

These sets are configurable in file adv_ext_app.c.

### __Keywords__

Connectivity, BLE, extended, advertising

### __Directory contents__

  - BLE_p2pServer_Ext/Core/Inc/app_common.h                                         App Common application configuration file for STM32WPAN Middleware
  - BLE_p2pServer_Ext/Core/Inc/app_conf.h                                           Application configuration file for STM32WPAN Middleware
  - BLE_p2pServer_Ext/Core/Inc/app_entry.h                                          Interface to the application 
  - BLE_p2pServer_Ext/Core/Inc/main.h                                               Header for main.c file. This file contains the common defines of the application
  - BLE_p2pServer_Ext/Core/Inc/stm32wbaxx_hal_conf.h                                HAL configuration file
  - BLE_p2pServer_Ext/Core/Inc/stm32wbaxx_it.h                                      This file contains the headers of the interrupt handlers
  - BLE_p2pServer_Ext/Core/Inc/stm32wbaxx_nucleo_conf.h                             STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_p2pServer_Ext/Core/Inc/utilities_conf.h                                     Header for configuration file for STM32 Utilities
  - BLE_p2pServer_Ext/STM32_WPAN/App/adv_ext_app.h                                  Advertising extended application header file
  - BLE_p2pServer_Ext/STM32_WPAN/App/app_ble.h                                      Header for ble application 
  - BLE_p2pServer_Ext/STM32_WPAN/App/ble_conf.h                                     Configuration file for BLE Middleware
  - BLE_p2pServer_Ext/STM32_WPAN/App/ble_dbg_conf.h                                 Debug configuration file for BLE Middleware
  - BLE_p2pServer_Ext/STM32_WPAN/App/p2p_server.h                                   Header for p2p_server.c 
  - BLE_p2pServer_Ext/STM32_WPAN/App/p2p_server_app.h                               Header for p2p_server_app.c 
  - BLE_p2pServer_Ext/STM32_WPAN/Target/bpka.h                                      This file contains the interface of the BLE PKA module
  - BLE_p2pServer_Ext/STM32_WPAN/Target/host_stack_if.h                             This file contains the interface for the stack tasks 
  - BLE_p2pServer_Ext/STM32_WPAN/Target/ll_sys_if.h                                 Header file for ll_sys_if.c
  - BLE_p2pServer_Ext/System/Config/ADC_Ctrl/adc_ctrl_conf.h                               Configuration Header for ADC controller module 
  - BLE_p2pServer_Ext/System/Config/CRC_Ctrl/crc_ctrl_conf.h                               Configuration Header for CRC controller module 
  - BLE_p2pServer_Ext/System/Config/Debug_GPIO/app_debug.h                          Real Time Debug module application APIs and signal table 
  - BLE_p2pServer_Ext/System/Config/Debug_GPIO/app_debug_signal_def.h               Real Time Debug module application signal definition 
  - BLE_p2pServer_Ext/System/Config/Debug_GPIO/debug_config.h                       Real Time Debug module general configuration file 
  - BLE_p2pServer_Ext/System/Config/Flash/simple_nvm_arbiter_conf.h                 Configuration header for simple_nvm_arbiter.c module 
  - BLE_p2pServer_Ext/System/Config/Log/log_module.h                                       Configuration Header for log module 
  - BLE_p2pServer_Ext/System/Config/LowPower/app_sys.h                              Header for app_sys.c 
  - BLE_p2pServer_Ext/System/Config/LowPower/peripheral_init.h                             Header for peripheral init module 
  - BLE_p2pServer_Ext/System/Config/LowPower/user_low_power_config.h                Header for user_low_power_config.c
  - BLE_p2pServer_Ext/System/Interfaces/hw.h                                        This file contains the interface of STM32 HW drivers
  - BLE_p2pServer_Ext/System/Interfaces/hw_if.h                                     Hardware Interface 
  - BLE_p2pServer_Ext/System/Interfaces/stm32_lpm_if.h                              Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_p2pServer_Ext/System/Interfaces/timer_if.h                                  configuration of the timer_if.c instances 
  - BLE_p2pServer_Ext/System/Interfaces/usart_if.h                                  Header file for stm32_adv_trace interface file 
  - BLE_p2pServer_Ext/System/Modules/adc_ctrl.h                                     Header for ADC client manager module 
  - BLE_p2pServer_Ext/System/Modules/ble_timer.h                                    This header defines the timer functions used by the BLE stack 
  - BLE_p2pServer_Ext/System/Modules/crc_ctrl.h                                            Header for CRC client manager module 
  - BLE_p2pServer_Ext/System/Modules/dbg_trace.h                                    Header for dbg_trace.c 
  - BLE_p2pServer_Ext/System/Modules/otp.h                                          Header file for One Time Programmable (OTP) area 
  - BLE_p2pServer_Ext/System/Modules/scm.h                                          Header for scm.c module 
  - BLE_p2pServer_Ext/System/Modules/stm_list.h                                     Header file for linked list library
  - BLE_p2pServer_Ext/System/Modules/temp_measurement.h                                    Header file for temperature measurement module
  - BLE_p2pServer_Ext/System/Modules/utilities_common.h                             Common file to utilities 
  - BLE_p2pServer_Ext/System/Modules/baes/baes.h                                    This file contains the interface of the basic AES software module
  - BLE_p2pServer_Ext/System/Modules/baes/baes_global.h                             This file contains the internal definitions of the AES software module
  - BLE_p2pServer_Ext/System/Modules/Flash/flash_driver.h                           Header for flash_driver.c module 
  - BLE_p2pServer_Ext/System/Modules/Flash/flash_manager.h                          Header for flash_manager.c module 
  - BLE_p2pServer_Ext/System/Modules/Flash/rf_timing_synchro.h                      Header for rf_timing_synchro.c module 
  - BLE_p2pServer_Ext/System/Modules/Flash/simple_nvm_arbiter.h                     Header for simple_nvm_arbiter.c module 
  - BLE_p2pServer_Ext/System/Modules/Flash/simple_nvm_arbiter_common.h              Common header of simple_nvm_arbiter.c module 
  - BLE_p2pServer_Ext/System/Modules/MemoryManager/advanced_memory_manager.h        Header for advance_memory_manager.c module 
  - BLE_p2pServer_Ext/System/Modules/MemoryManager/stm32_mm.h                       Header for stm32_mm.c module 
  - BLE_p2pServer_Ext/System/Modules/Nvm/nvm.h                                      This file contains the interface of the NVM manager
  - BLE_p2pServer_Ext/System/Modules/RFControl/rf_antenna_switch.h                  RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer_Ext/System/Modules/RTDebug/debug_signals.h                        Real Time Debug module System and Link Layer signal definition 
  - BLE_p2pServer_Ext/System/Modules/RTDebug/local_debug_tables.h                   Real Time Debug module System and Link Layer signal 
  - BLE_p2pServer_Ext/System/Modules/RTDebug/RTDebug.h                              Real Time Debug module API declaration 
  - BLE_p2pServer_Ext/System/Modules/RTDebug/RTDebug_dtb.h                          Real Time Debug module API declaration for DTB usage
  - BLE_p2pServer_Ext/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h         Header file for the serial commands interpreter module
  - BLE_p2pServer_Ext/Core/Src/app_entry.c                                          Entry point of the application 
  - BLE_p2pServer_Ext/Core/Src/main.c                                               Main program body 
  - BLE_p2pServer_Ext/Core/Src/stm32wbaxx_hal_msp.c                                 This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_p2pServer_Ext/Core/Src/stm32wbaxx_it.c                                      Interrupt Service Routines. 
  - BLE_p2pServer_Ext/Core/Src/system_stm32wbaxx.c                                  CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_p2pServer_Ext/STM32_WPAN/App/adv_ext_app.c                                  Advertising extended application file
  - BLE_p2pServer_Ext/STM32_WPAN/App/app_ble.c                                      BLE Application 
  - BLE_p2pServer_Ext/STM32_WPAN/App/p2p_server.c                                   p2p_server definition
  - BLE_p2pServer_Ext/STM32_WPAN/App/p2p_server_app.c                               p2p_server_app application definition
  - BLE_p2pServer_Ext/STM32_WPAN/Target/bleplat.c                                   This file implements the platform functions for BLE stack library
  - BLE_p2pServer_Ext/STM32_WPAN/Target/bpka.c                                      This file implements the BLE PKA module
  - BLE_p2pServer_Ext/STM32_WPAN/Target/host_stack_if.c                             Source file for the stack tasks 
  - BLE_p2pServer_Ext/STM32_WPAN/Target/linklayer_plat.c                            Source file for the linklayer plateform adaptation layer 
  - BLE_p2pServer_Ext/STM32_WPAN/Target/ll_sys_if.c                                 Source file for initiating the system sequencer 
  - BLE_p2pServer_Ext/STM32_WPAN/Target/power_table.c                               This file contains supported power tables 
  - BLE_p2pServer_Ext/System/Config/ADC_Ctrl/adc_ctrl_conf.c                               Source for ADC client controller module configuration file 
  - BLE_p2pServer_Ext/System/Config/CRC_Ctrl/crc_ctrl_conf.c                               Source for CRC client controller module configuration file 
  - BLE_p2pServer_Ext/System/Config/Debug_GPIO/app_debug.c                          Real Time Debug module application side APIs 
  - BLE_p2pServer_Ext/System/Config/Log/log_module.c                                       Source file of the log module 
  - BLE_p2pServer_Ext/System/Config/LowPower/user_low_power_config.c                Low power related user configuration
  - BLE_p2pServer_Ext/System/Config/LowPower/peripheral_init.c                             Source for peripheral init module 
  - BLE_p2pServer_Ext/System/Interfaces/hw_aes.c                                    This file contains the AES driver for STM32WBA 
  - BLE_p2pServer_Ext/System/Interfaces/hw_otp.c                                    This file contains the OTP driver
  - BLE_p2pServer_Ext/System/Interfaces/hw_pka.c                                    This file contains the PKA driver for STM32WBA 
  - BLE_p2pServer_Ext/System/Interfaces/hw_rng.c                                    This file contains the RNG driver for STM32WBA 
  - BLE_p2pServer_Ext/System/Interfaces/pka_p256.c                                  This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_p2pServer_Ext/System/Interfaces/stm32_lpm_if.c                              Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_p2pServer_Ext/System/Interfaces/timer_if.c                                  Configure RTC Alarm, Tick and Calendar manager 
  - BLE_p2pServer_Ext/System/Interfaces/usart_if.c                                  Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_p2pServer_Ext/System/Modules/adc_ctrl.c                                     Header for ADC client manager module 
  - BLE_p2pServer_Ext/System/Modules/app_sys.c                                      Application system for STM32WPAN Middleware
  - BLE_p2pServer_Ext/System/Modules/ble_timer.c                                    This module implements the timer core functions 
  - BLE_p2pServer_Ext/System/Modules/crc_ctrl.c                                            Source for CRC client controller module 
  - BLE_p2pServer_Ext/System/Modules/otp.c                                          Source file for One Time Programmable (OTP) area 
  - BLE_p2pServer_Ext/System/Modules/scm.c                                          Functions for the System Clock Manager
  - BLE_p2pServer_Ext/System/Modules/stm_list.c                                     TCircular Linked List Implementation
  - BLE_p2pServer_Ext/System/Modules/temp_measurement.c                                    Temperature measurement module
  - BLE_p2pServer_Ext/System/Modules/baes/baes_cmac.c                               This file contains the AES CMAC implementation
  - BLE_p2pServer_Ext/System/Modules/baes/baes_ecb.c                                This file contains the AES ECB functions implementation
  - BLE_p2pServer_Ext/System/Modules/Flash/flash_driver.c                           The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_p2pServer_Ext/System/Modules/Flash/flash_manager.c                          The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_p2pServer_Ext/System/Modules/Flash/rf_timing_synchro.c                      The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_p2pServer_Ext/System/Modules/Flash/simple_nvm_arbiter.c                     The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_p2pServer_Ext/System/Modules/MemoryManager/advanced_memory_manager.c        Memory Manager 
  - BLE_p2pServer_Ext/System/Modules/MemoryManager/stm32_mm.c                       Memory Manager 
  - BLE_p2pServer_Ext/System/Modules/Nvm/nvm_emul.c                                 This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_p2pServer_Ext/System/Modules/RFControl/rf_antenna_switch.c                  RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer_Ext/System/Modules/RTDebug/RTDebug.c                              Real Time Debug module API definition 
  - BLE_p2pServer_Ext/System/Modules/RTDebug/RTDebug_dtb.c                          Real Time Debug module API definition for DTB usage
  - BLE_p2pServer_Ext/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c         Source file for the serial commands interpreter module 

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.
  - Another STM32WBAxx Nucleo board  may be necessary to run p2pClient_Ext application.
    
### __How to use it?__

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 __You can interact with p2pServer_Ext application with a smartphone:__

 - Install and launch ST BLE Toolbox application on android or iOS smartphone
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

 - Enable extended advertising in scan panel
 - You can see data advertised by p2pServer_Ext application.
 - Moreover, you can connect to the legacy adverting set, your device act like in p2pServer application.

 __You can interact with p2pServer_Ext application with another Nucleo board running p2pClient_Ext:__

 - Power up p2pClient_Ext devices next to p2pServer_Ext device.
 - On p2pClient_Ext device button B1 click, to start scanning
   - p2pServer_Ext connectable extended advertising in discovered, BD address is registered.
   - Then, on p2pClient_Ext device button B3 click, connection is performed.

__You can interact with p2pServer_Ext application with another Nucleo board running p2pClient:__

 - Power up p2pClient devices next to p2pServer_Ext device.
 - On p2pClient device button B1 click, to start scanning
   - p2pServer_Ext connectable legacy advertising in discovered, BD address is registered.
   - Then, connection is performed.
