## __BLE_HID_Mouse Application Description__

How to use the Human Interface Device over GATT profile for a mouse as specified by the BLE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_HID_Mouse/Core/Inc/app_common.h                                             App Common application configuration file for STM32WPAN Middleware
  - BLE_HID_Mouse/Core/Inc/app_conf.h                                               Application configuration file for STM32WPAN Middleware
  - BLE_HID_Mouse/Core/Inc/app_entry.h                                              Interface to the application 
  - BLE_HID_Mouse/Core/Inc/main.h                                                   Header for main.c file. This file contains the common defines of the application
  - BLE_HID_Mouse/Core/Inc/stm32wbaxx_hal_conf.h                                    HAL configuration file
  - BLE_HID_Mouse/Core/Inc/stm32wbaxx_it.h                                          This file contains the headers of the interrupt handlers
  - BLE_HID_Mouse/Core/Inc/stm32wba55g_discovery_conf.h                             STM32WBA55G_DK1 board configuration file. This file should be copied to the application folder and renamed to tm32wba55g_discovery_conf.h
  - BLE_HID_Mouse/Core/Inc/utilities_conf.h                                         Header for configuration file for STM32 Utilities
  - BLE_HID_Mouse/STM32_WPAN/App/app_ble.h                                          Header for ble application 
  - BLE_HID_Mouse/STM32_WPAN/App/ble_conf.h                                         Configuration file for BLE Middleware
  - BLE_HID_Mouse/STM32_WPAN/App/ble_dbg_conf.h                                     Debug configuration file for BLE Middleware
  - BLE_HID_Mouse/STM32_WPAN/App/hids.h                                             Header for service1.c 
  - BLE_HID_Mouse/STM32_WPAN/App/hids_app.h                                         Header for service1_app.c 
  - BLE_HID_Mouse/STM32_WPAN/App/dis.h                                              Header for service2.c 
  - BLE_HID_Mouse/STM32_WPAN/App/dis_app.h                                          Header for service2_app.c 
  - BLE_HID_Mouse/STM32_WPAN/App/bas.h                                              Header for service3.c 
  - BLE_HID_Mouse/STM32_WPAN/App/bas_app.h                                          Header for service3_app.c 
  - BLE_HID_Mouse/STM32_WPAN/Target/bpka.h                                          This file contains the interface of the BLE PKA module
  - BLE_HID_Mouse/STM32_WPAN/Target/host_stack_if.h                                 This file contains the interface for the stack tasks 
  - BLE_HID_Mouse/STM32_WPAN/Target/ll_sys_if.h                                     Header file for ll_sys_if.c
  - BLE_HID_Mouse/System/Config/Debug_GPIO/app_debug.h                              Real Time Debug module application APIs and signal table 
  - BLE_HID_Mouse/System/Config/Debug_GPIO/app_debug_signal_def.h                   Real Time Debug module application signal definition 
  - BLE_HID_Mouse/System/Config/Debug_GPIO/debug_config.h                           Real Time Debug module general configuration file 
  - BLE_HID_Mouse/System/Config/Flash/simple_nvm_arbiter_conf.h                     Configuration header for simple_nvm_arbiter.c module 
  - BLE_HID_Mouse/System/Config/Log/log_module.h                                    Configuration Header for log module 
  - BLE_HID_Mouse/System/Config/LowPower/app_sys.h                                  Header for app_sys.c 
  - BLE_HID_Mouse/System/Config/LowPower/peripheral_init.h                          Header for peripheral init module 
  - BLE_HID_Mouse/System/Config/LowPower/user_low_power_config.h                    Header for user_low_power_config.c
  - BLE_HID_Mouse/System/Interfaces/hw.h                                            This file contains the interface of STM32 HW drivers
  - BLE_HID_Mouse/System/Interfaces/hw_if.h                                         Hardware Interface 
  - BLE_HID_Mouse/System/Interfaces/stm32_lpm_if.h                                  Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_HID_Mouse/System/Interfaces/timer_if.h                                      configuration of the timer_if.c instances 
  - BLE_HID_Mouse/System/Interfaces/usart_if.h                                      Header file for stm32_adv_trace interface file 
  - BLE_HID_Mouse/System/Modules/ble_timer.h                                        This header defines the timer functions used by the BLE stack 
  - BLE_HID_Mouse/System/Modules/crc_ctrl.h                                         Header for CRC client manager module 
  - BLE_HID_Mouse/System/Modules/dbg_trace.h                                        Header for dbg_trace.c 
  - BLE_HID_Mouse/System/Modules/otp.h                                              Header file for One Time Programmable (OTP) area 
  - BLE_HID_Mouse/System/Modules/scm.h                                              Header for scm.c module 
  - BLE_HID_Mouse/System/Modules/stm_list.h                                         Header file for linked list library
  - BLE_HID_Mouse/System/Modules/utilities_common.h                                 Common file to utilities 
  - BLE_HID_Mouse/System/Modules/baes/baes.h                                        This file contains the interface of the basic AES software module
  - BLE_HID_Mouse/System/Modules/baes/baes_global.h                                 This file contains the internal definitions of the AES software module
  - BLE_HID_Mouse/System/Modules/Flash/flash_driver.h                               Header for flash_driver.c module 
  - BLE_HID_Mouse/System/Modules/Flash/flash_manager.h                              Header for flash_manager.c module 
  - BLE_HID_Mouse/System/Modules/Flash/rf_timing_synchro.h                          Header for rf_timing_synchro.c module 
  - BLE_HID_Mouse/System/Modules/Flash/simple_nvm_arbiter.h                         Header for simple_nvm_arbiter.c module 
  - BLE_HID_Mouse/System/Modules/Flash/simple_nvm_arbiter_common.h                  Common header of simple_nvm_arbiter.c module 
  - BLE_HID_Mouse/System/Modules/MemoryManager/advanced_memory_manager.h            Header for advance_memory_manager.c module 
  - BLE_HID_Mouse/System/Modules/MemoryManager/stm32_mm.h                           Header for stm32_mm.c module 
  - BLE_HID_Mouse/System/Modules/Nvm/nvm.h                                          This file contains the interface of the NVM manager
  - BLE_HID_Mouse/System/Modules/RFControl/rf_antenna_switch.h                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HID_Mouse/System/Modules/RTDebug/debug_signals.h                            Real Time Debug module System and Link Layer signal definition 
  - BLE_HID_Mouse/System/Modules/RTDebug/local_debug_tables.h                       Real Time Debug module System and Link Layer signal 
  - BLE_HID_Mouse/System/Modules/RTDebug/RTDebug.h                                  Real Time Debug module API declaration 
  - BLE_HID_Mouse/System/Modules/RTDebug/RTDebug_dtb.h                              Real Time Debug module API declaration for DTB usage
  - BLE_HID_Mouse/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h      Header file for the serial commands interpreter module
  - BLE_HID_Mouse/Core/Src/app_entry.c                                              Entry point of the application 
  - BLE_HID_Mouse/Core/Src/main.c                                                   Main program body 
  - BLE_HID_Mouse/Core/Src/stm32wbaxx_hal_msp.c                                     This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HID_Mouse/Core/Src/stm32wbaxx_it.c                                          Interrupt Service Routines
  - BLE_HID_Mouse/Core/Src/system_stm32wbaxx.c                                      CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_HID_Mouse/STM32_WPAN/App/app_ble.c                                          BLE Application 
  - BLE_HID_Mouse/STM32_WPAN/App/hids.c                                             service1 definition
  - BLE_HID_Mouse/STM32_WPAN/App/hids_app.c                                         service1_app application definition
  - BLE_HID_Mouse/STM32_WPAN/App/dis.c                                              service2 definition
  - BLE_HID_Mouse/STM32_WPAN/App/dis_app.c                                          service2_app application definition
  - BLE_HID_Mouse/STM32_WPAN/App/bas.c                                              service3 definition
  - BLE_HID_Mouse/STM32_WPAN/App/bas_app.c                                          service3_app application definition
  - BLE_HID_Mouse/STM32_WPAN/Target/bleplat.c                                       This file implements the platform functions for BLE stack library
  - BLE_HID_Mouse/STM32_WPAN/Target/bpka.c                                          This file implements the BLE PKA module
  - BLE_HID_Mouse/STM32_WPAN/Target/host_stack_if.c                                 Source file for the stack tasks 
  - BLE_HID_Mouse/STM32_WPAN/Target/linklayer_plat.c                                Source file for the linklayer plateform adaptation layer 
  - BLE_HID_Mouse/STM32_WPAN/Target/ll_sys_if.c                                     Source file for initiating the system sequencer 
  - BLE_HID_Mouse/STM32_WPAN/Target/power_table.c                                   This file contains supported power tables 
  - BLE_HID_Mouse/System/Config/CRC_Ctrl/crc_ctrl_conf.c                            Source for CRC client controller module configuration file 
  - BLE_HID_Mouse/System/Config/Debug_GPIO/app_debug.c                              Real Time Debug module application side APIs 
  - BLE_HID_Mouse/System/Config/Log/log_module.c                                    Source file of the log module 
  - BLE_HID_Mouse/System/Config/LowPower/user_low_power_config.c                    Low power related user configuration
  - BLE_HID_Mouse/System/Config/LowPower/peripheral_init.c                          Source for peripheral init module 
  - BLE_HID_Mouse/System/Interfaces/hw_aes.c                                        This file contains the AES driver for STM32WBA 
  - BLE_HID_Mouse/System/Interfaces/hw_otp.c                                        This file contains the OTP driver
  - BLE_HID_Mouse/System/Interfaces/hw_pka.c                                        This file contains the PKA driver for STM32WBA 
  - BLE_HID_Mouse/System/Interfaces/hw_rng.c                                        This file contains the RNG driver for STM32WBA 
  - BLE_HID_Mouse/System/Interfaces/pka_p256.c                                      This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_HID_Mouse/System/Interfaces/stm32_lpm_if.c                                  Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_HID_Mouse/System/Interfaces/timer_if.c                                      Configure RTC Alarm, Tick and Calendar manager 
  - BLE_HID_Mouse/System/Interfaces/usart_if.c                                      Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_HID_Mouse/System/Modules/app_sys.c                                          Application system for STM32WPAN Middleware
  - BLE_HID_Mouse/System/Modules/ble_timer.c                                        This module implements the timer core functions 
  - BLE_HID_Mouse/System/Modules/crc_ctrl.c                                         Source for CRC client controller module 
  - BLE_HID_Mouse/System/Modules/otp.c                                              Source file for One Time Programmable (OTP) area 
  - BLE_HID_Mouse/System/Modules/scm.c                                              Functions for the System Clock Manager
  - BLE_HID_Mouse/System/Modules/stm_list.c                                         TCircular Linked List Implementation
  - BLE_HID_Mouse/System/Modules/baes/baes_cmac.c                                   This file contains the AES CMAC implementation
  - BLE_HID_Mouse/System/Modules/baes/baes_ecb.c                                    This file contains the AES ECB functions implementation
  - BLE_HID_Mouse/System/Modules/Flash/flash_driver.c                               The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_HID_Mouse/System/Modules/Flash/flash_manager.c                              The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_HID_Mouse/System/Modules/Flash/rf_timing_synchro.c                          The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_HID_Mouse/System/Modules/Flash/simple_nvm_arbiter.c                         The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_HID_Mouse/System/Modules/MemoryManager/advanced_memory_manager.c            Memory Manager 
  - BLE_HID_Mouse/System/Modules/MemoryManager/stm32_mm.c                           Memory Manager 
  - BLE_HID_Mouse/System/Modules/Nvm/nvm_emul.c                                     This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_HID_Mouse/System/Modules/RFControl/rf_antenna_switch.c                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HID_Mouse/System/Modules/RTDebug/RTDebug.c                                  Real Time Debug module API definition 
  - BLE_HID_Mouse/System/Modules/RTDebug/RTDebug_dtb.c                              Real Time Debug module API definition for DTB usage
  - BLE_HID_Mouse/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c      Source file for the serial commands interpreter module 

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx Discovery kit.
  - Connect the Discovery Kit Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 You can pair the board with the BLE of a computer or a android phone. 
 Now you can control the mouse pointer by using the joystick direction of the STM32WBA55xx Discovery kit, 
 selection can be done with a press on the joystick.
