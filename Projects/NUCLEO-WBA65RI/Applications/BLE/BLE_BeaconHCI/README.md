## __BLE_BeaconHCI Application Description__

How to advertise 5 types of beacon ( Eddystone uid, Eddystone url, Eddystone uid+tlm, Eddystone url+tlm, iBeacon ) using HCI.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_BeaconHCI/Core/Inc/app_common.h                                                App Common application configuration file for STM32WPAN Middleware
  - BLE_BeaconHCI/Core/Inc/app_conf.h                                                  Application configuration file for STM32WPAN Middleware
  - BLE_BeaconHCI/Core/Inc/app_entry.h                                                 Interface to the application 
  - BLE_BeaconHCI/Core/Inc/main.h                                                      Header for main.c file. This file contains the common defines of the application.
  - BLE_BeaconHCI/Core/Inc/stm32wbaxx_hal_conf.h                                       HAL configuration file
  - BLE_BeaconHCI/Core/Inc/stm32wbaxx_it.h                                             This file contains the headers of the interrupt handlers
  - BLE_BeaconHCI/Core/Inc/stm32wbaxx_nucleo_conf.h                                    STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_BeaconHCI/Core/Inc/utilities_conf.h                                            Header for configuration file for STM32 Utilities
  - BLE_BeaconHCI/STM32_WPAN/App/app_ble.h                                             Header for ble application 
  - BLE_BeaconHCI/STM32_WPAN/App/ble_conf.h                                            Configuration file for BLE Middleware
  - BLE_BeaconHCI/STM32_WPAN/App/ble_dbg_conf.h                                        Debug configuration file for BLE Middleware
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_beacon.h                                    Eddystone beacon header file
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_tlm_service.h                               Eddystone TLM beacon header file  
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_uid_service.h                               Eddystone UID beacon header file    
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_url_service.h                               Eddystone URL beacon header file    
  - BLE_BeaconHCI/STM32_WPAN/App/ibeacon.h                                             Header for ibeacon_application.c module 
  - BLE_BeaconHCI/STM32_WPAN/App/ibeacon_service.h                                     iBeacon header file  
  - BLE_BeaconHCI/STM32_WPAN/Target/bpka.h                                             This file contains the interface of the BLE PKA module
  - BLE_BeaconHCI/STM32_WPAN/Target/host_stack_if.h                                    This file contains the interface for the stack tasks 
  - BLE_BeaconHCI/STM32_WPAN/Target/ll_sys_if.h                                        Header file for ll_sys_if.c
  - BLE_BeaconHCI/System/Config/ADC_Ctrl/adc_ctrl_conf.h                               Configuration Header for ADC controller module 
  - BLE_BeaconHCI/System/Config/CRC_Ctrl/crc_ctrl_conf.h                               Configuration Header for CRC controller module 
  - BLE_BeaconHCI/System/Config/Debug_GPIO/app_debug.h                                 Real Time Debug module application APIs and signal table 
  - BLE_BeaconHCI/System/Config/Debug_GPIO/app_debug_signal_def.h                      Real Time Debug module application signal definition 
  - BLE_BeaconHCI/System/Config/Debug_GPIO/debug_config.h                              Real Time Debug module general configuration file 
  - BLE_BeaconHCI/System/Config/Flash/simple_nvm_arbiter_conf.h                        Configuration header for simple_nvm_arbiter.c module 
  - BLE_BeaconHCI/System/Config/Log/log_module.h                                       Configuration Header for log module 
  - BLE_BeaconHCI/System/Config/LowPower/app_sys.h                                     Header for app_sys.c 
  - BLE_BeaconHCI/System/Config/LowPower/peripheral_init.h                             Header for peripheral init module 
  - BLE_BeaconHCI/System/Config/LowPower/user_low_power_config.h                       Header for user_low_power_config.c
  - BLE_BeaconHCI/System/Interfaces/hw.h                                               This file contains the interface of STM32 HW drivers
  - BLE_BeaconHCI/System/Interfaces/hw_if.h                                            Hardware Interface 
  - BLE_BeaconHCI/System/Interfaces/stm32_lpm_if.h                                     Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_BeaconHCI/System/Interfaces/timer_if.h                                         configuration of the timer_if.c instances 
  - BLE_BeaconHCI/System/Interfaces/usart_if.h                                         Header file for stm32_adv_trace interface file 
  - BLE_BeaconHCI/System/Modules/adc_ctrl.h                                            Header for ADC client manager module 
  - BLE_BeaconHCI/System/Modules/ble_timer.h                                           This header defines the timer functions used by the BLE stack 
  - BLE_BeaconHCI/System/Modules/crc_ctrl.h                                            Header for CRC client manager module 
  - BLE_BeaconHCI/System/Modules/dbg_trace.h                                           Header for dbg_trace.c 
  - BLE_BeaconHCI/System/Modules/otp.h                                                 Header file for One Time Programmable (OTP) area 
  - BLE_BeaconHCI/System/Modules/scm.h                                                 Header for scm.c module 
  - BLE_BeaconHCI/System/Modules/stm_list.h                                            Header file for linked list library
  - BLE_BeaconHCI/System/Modules/temp_measurement.h                                    Header file for temperature measurement module
  - BLE_BeaconHCI/System/Modules/utilities_common.h                                    Common file to utilities 
  - BLE_BeaconHCI/System/Modules/baes/baes.h                                           This file contains the interface of the basic AES software module
  - BLE_BeaconHCI/System/Modules/baes/baes_global.h                                    This file contains the internal definitions of the AES software module
  - BLE_BeaconHCI/System/Modules/Flash/flash_driver.h                                  Header for flash_driver.c module 
  - BLE_BeaconHCI/System/Modules/Flash/flash_manager.h                                 Header for flash_manager.c module 
  - BLE_BeaconHCI/System/Modules/Flash/rf_timing_synchro.h                             Header for rf_timing_synchro.c module 
  - BLE_BeaconHCI/System/Modules/Flash/simple_nvm_arbiter.h                            Header for simple_nvm_arbiter.c module 
  - BLE_BeaconHCI/System/Modules/Flash/simple_nvm_arbiter_common.h                     Common header of simple_nvm_arbiter.c module 
  - BLE_BeaconHCI/System/Modules/MemoryManager/advanced_memory_manager.h               Header for advance_memory_manager.c module 
  - BLE_BeaconHCI/System/Modules/MemoryManager/stm32_mm.h                              Header for stm32_mm.c module 
  - BLE_BeaconHCI/System/Modules/Nvm/nvm.h                                             This file contains the interface of the NVM manager
  - BLE_BeaconHCI/System/Modules/RFControl/rf_antenna_switch.h                         RF related module to handle dedictated GPIOs for antenna switch
  - BLE_BeaconHCI/System/Modules/RTDebug/debug_signals.h                               Real Time Debug module System and Link Layer signal definition 
  - BLE_BeaconHCI/System/Modules/RTDebug/local_debug_tables.h                          Real Time Debug module System and Link Layer signal 
  - BLE_BeaconHCI/System/Modules/RTDebug/RTDebug.h                                     Real Time Debug module API declaration 
  - BLE_BeaconHCI/System/Modules/RTDebug/RTDebug_dtb.h                                 Real Time Debug module API declaration for DTB usage
  - BLE_BeaconHCI/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h         Header file for the serial commands interpreter module
  - BLE_BeaconHCI/Core/Src/app_entry.c                                                 Entry point of the application 
  - BLE_BeaconHCI/Core/Src/main.c                                                      Main program body 
  - BLE_BeaconHCI/Core/Src/stm32wbaxx_hal_msp.c                                        This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_BeaconHCI/Core/Src/stm32wbaxx_it.c                                             Interrupt Service Routines
  - BLE_BeaconHCI/Core/Src/system_stm32wbaxx.c                                         CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_BeaconHCI/STM32_WPAN/App/app_ble.c                                             BLE Application 
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_tlm_service.c                               Eddystone TLM
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_uid_service.c                               Eddystone UID
  - BLE_BeaconHCI/STM32_WPAN/App/eddystone_url_service.c                               Eddystone URL
  - BLE_BeaconHCI/STM32_WPAN/App/ibeacon_service.c                                     iBeacon 
  - BLE_BeaconHCI/STM32_WPAN/Target/bleplat.c                                          This file implements the platform functions for BLE stack library
  - BLE_BeaconHCI/STM32_WPAN/Target/bpka.c                                             This file implements the BLE PKA module
  - BLE_BeaconHCI/STM32_WPAN/Target/host_stack_if.c                                    Source file for the stack tasks 
  - BLE_BeaconHCI/STM32_WPAN/Target/linklayer_plat.c                                   Source file for the linklayer plateform adaptation layer 
  - BLE_BeaconHCI/STM32_WPAN/Target/ll_sys_if.c                                        Source file for initiating the system sequencer 
  - BLE_BeaconHCI/STM32_WPAN/Target/power_table.c                                      This file contains supported power tables 
  - BLE_BeaconHCI/System/Config/ADC_Ctrl/adc_ctrl_conf.c                               Source for ADC client controller module configuration file 
  - BLE_BeaconHCI/System/Config/CRC_Ctrl/crc_ctrl_conf.c                               Source for CRC client controller module configuration file 
  - BLE_BeaconHCI/System/Config/Debug_GPIO/app_debug.c                                 Real Time Debug module application side APIs 
  - BLE_BeaconHCI/System/Config/Log/log_module.c                                       Source file of the log module 
  - BLE_BeaconHCI/System/Config/LowPower/user_low_power_config.c                       Low power related user configuration
  - BLE_BeaconHCI/System/Config/LowPower/peripheral_init.c                             Source for peripheral init module 
  - BLE_BeaconHCI/System/Interfaces/hw_aes.c                                           This file contains the AES driver for STM32WBA 
  - BLE_BeaconHCI/System/Interfaces/hw_otp.c                                           This file contains the OTP driver
  - BLE_BeaconHCI/System/Interfaces/hw_pka.c                                           This file contains the PKA driver for STM32WBA 
  - BLE_BeaconHCI/System/Interfaces/hw_rng.c                                           This file contains the RNG driver for STM32WBA 
  - BLE_BeaconHCI/System/Interfaces/pka_p256.c                                         This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve.
  - BLE_BeaconHCI/System/Interfaces/stm32_lpm_if.c                                     Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_BeaconHCI/System/Interfaces/timer_if.c                                         Configure RTC Alarm, Tick and Calendar manager 
  - BLE_BeaconHCI/System/Interfaces/usart_if.c                                         Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_BeaconHCI/System/Modules/adc_ctrl.c                                            Header for ADC client manager module 
  - BLE_BeaconHCI/System/Modules/app_sys.c                                             Application system for STM32WPAN Middleware
  - BLE_BeaconHCI/System/Modules/ble_timer.c                                           This module implements the timer core functions 
  - BLE_BeaconHCI/System/Modules/crc_ctrl.c                                            Source for CRC client controller module 
  - BLE_BeaconHCI/System/Modules/otp.c                                                 Source file for One Time Programmable (OTP) area 
  - BLE_BeaconHCI/System/Modules/scm.c                                                 Functions for the System Clock Manager
  - BLE_BeaconHCI/System/Modules/stm_list.c                                            TCircular Linked List Implementation
  - BLE_BeaconHCI/System/Modules/temp_measurement.c                                    Temperature measurement module
  - BLE_BeaconHCI/System/Modules/baes/baes_cmac.c                                      This file contains the AES CMAC implementation
  - BLE_BeaconHCI/System/Modules/baes/baes_ecb.c                                       This file contains the AES ECB functions implementation
  - BLE_BeaconHCI/System/Modules/Flash/flash_driver.c                                  The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_BeaconHCI/System/Modules/Flash/flash_manager.c                                 The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_BeaconHCI/System/Modules/Flash/rf_timing_synchro.c                             The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_BeaconHCI/System/Modules/Flash/simple_nvm_arbiter.c                            The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_BeaconHCI/System/Modules/MemoryManager/advanced_memory_manager.c               Memory Manager 
  - BLE_BeaconHCI/System/Modules/MemoryManager/stm32_mm.c                              Memory Manager 
  - BLE_BeaconHCI/System/Modules/Nvm/nvm_emul.c                                        This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_BeaconHCI/System/Modules/RFControl/rf_antenna_switch.c                         RF related module to handle dedictated GPIOs for antenna switch
  - BLE_BeaconHCI/System/Modules/RTDebug/RTDebug.c                                     Real Time Debug module API definition 
  - BLE_BeaconHCI/System/Modules/RTDebug/RTDebug_dtb.c                                 Real Time Debug module API definition for DTB usage
  - BLE_BeaconHCI/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c         Source file for the serial commands interpreter module 

### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to C to ST-LINK connector (USB_STLINK). 

### __How to use it?__

In order to make the program work, you must do the following:

 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 - 4 Beacon types are available and have to be defined in app_con.h file : 
   - #define CFG_BEACON_TYPE must be set at one of the following value :
     - CFG_EDDYSTONE_UID_BEACON_TYPE
     - CFG_EDDYSTONE_URL_BEACON_TYPE
     - CFG_EDDYSTONE_UID_BEACON_TYPE | CFG_EDDYSTONE_TLM_BEACON_TYPE
     - CFG_EDDYSTONE_URL_BEACON_TYPE | CFG_EDDYSTONE_TLM_BEACON_TYPE
     - CFG_IBEACON
 - Run the example
 - On the smartphone, download a Beacon Scanner application
 - Enable Bluetooth communications
 - Run Beacon Scanner application and start scanning
 - The Beacon information are displayed on the smartphone.
