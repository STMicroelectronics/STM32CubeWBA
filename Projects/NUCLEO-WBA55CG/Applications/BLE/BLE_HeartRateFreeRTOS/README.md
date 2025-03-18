## __BLE_HeartRateFreeRTOS Application Description__

How to use the Heart Rate profile as specified by the BLE SIG with X-CUBEFREERTOS pack.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, FreeRTOS

### __Directory contents__

  - BLE_HeartRateFreeRTOS/Core/Inc/app_common.h                                             App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRateFreeRTOS/Core/Inc/app_conf.h                                               Application configuration file for STM32WPAN Middleware
  - BLE_HeartRateFreeRTOS/Core/Inc/app_entry.h                                              Interface to the application 
  - BLE_HeartRateFreeRTOS/Core/Inc/main.h                                                   Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRateFreeRTOS/Core/Inc/stm32wbaxx_hal_conf.h                                    HAL configuration file
  - BLE_HeartRateFreeRTOS/Core/Inc/stm32wbaxx_it.h                                          This file contains the headers of the interrupt handlers
  - BLE_HeartRateFreeRTOS/Core/Inc/stm32wbaxx_nucleo_conf.h                                 STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_HeartRateFreeRTOS/Core/Inc/utilities_conf.h                                         Header for configuration file for STM32 Utilities
  - BLE_HeartRateFreeRTOS/Core/Inc/app_freertos.h                                           Header for FreeRTOS objects managed by X-CUBEFREERTOS pack
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/app_ble.h                                          Header for ble application 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/ble_conf.h                                         Configuration file for BLE Middleware
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/ble_dbg_conf.h                                     Debug configuration file for BLE Middleware
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/dis.h                                              Header for service2.c 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/dis_app.h                                          Header for service2_app.c 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/hrs.h                                              Header for service1.c 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/hrs_app.h                                          Header for service1_app.c 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/bpka.h                                          This file contains the interface of the BLE PKA module
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/host_stack_if.h                                 This file contains the interface for the stack tasks 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/ll_sys_if.h                                     Header file for ll_sys_if.c
  - BLE_HeartRateFreeRTOS/System/Config/Debug_GPIO/app_debug.h                              Real Time Debug module application APIs and signal table 
  - BLE_HeartRateFreeRTOS/System/Config/Debug_GPIO/app_debug_signal_def.h                   Real Time Debug module application signal definition 
  - BLE_HeartRateFreeRTOS/System/Config/Debug_GPIO/debug_config.h                           Real Time Debug module general configuration file 
  - BLE_HeartRateFreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.h                     Configuration header for simple_nvm_arbiter.c module 
  - BLE_HeartRateFreeRTOS/System/Config/LowPower/app_sys.h                                  Header for app_sys.c 
  - BLE_HeartRateFreeRTOS/System/Config/LowPower/user_low_power_config.h                    Header for user_low_power_config.c
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw.h                                            This file contains the interface of STM32 HW drivers
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw_if.h                                         Hardware Interface 
  - BLE_HeartRateFreeRTOS/System/Interfaces/stm32_lpm_if.h                                  Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_HeartRateFreeRTOS/System/Interfaces/timer_if.h                                      configuration of the timer_if.c instances 
  - BLE_HeartRateFreeRTOS/System/Interfaces/usart_if.h                                      Header file for stm32_adv_trace interface file 
  - BLE_HeartRateFreeRTOS/System/Modules/adc_ctrl.h                                         Header for ADC client manager module 
  - BLE_HeartRateFreeRTOS/System/Modules/ble_timer.h                                        This header defines the timer functions used by the BLE stack 
  - BLE_HeartRateFreeRTOS/System/Modules/dbg_trace.h                                        Header for dbg_trace.c 
  - BLE_HeartRateFreeRTOS/System/Modules/otp.h                                              Header file for One Time Programmable (OTP) area 
  - BLE_HeartRateFreeRTOS/System/Modules/scm.h                                              Header for scm.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/stm_list.h                                         Header file for linked list library
  - BLE_HeartRateFreeRTOS/System/Modules/utilities_common.h                                 Common file to utilities 
  - BLE_HeartRateFreeRTOS/System/Modules/baes/baes.h                                        This file contains the interface of the basic AES software module
  - BLE_HeartRateFreeRTOS/System/Modules/baes/baes_global.h                                 This file contains the internal definitions of the AES software module
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/flash_driver.h                               Header for flash_driver.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/flash_manager.h                              Header for flash_manager.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/rf_timing_synchro.h                          Header for rf_timing_synchro.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/simple_nvm_arbiter.h                         Header for simple_nvm_arbiter.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/simple_nvm_arbiter_common.h                  Common header of simple_nvm_arbiter.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/MemoryManager/advanced_memory_manager.h            Header for advance_memory_manager.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/MemoryManager/stm32_mm.h                           Header for stm32_mm.c module 
  - BLE_HeartRateFreeRTOS/System/Modules/Nvm/nvm.h                                          This file contains the interface of the NVM manager
  - BLE_HeartRateFreeRTOS/System/Modules/RFControl/rf_antenna_switch.h                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/debug_signals.h                            Real Time Debug module System and Link Layer signal definition 
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/local_debug_tables.h                       Real Time Debug module System and Link Layer signal 
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/RTDebug.h                                  Real Time Debug module API declaration 
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/RTDebug_dtb.h                              Real Time Debug module API declaration for DTB usage
  - BLE_HeartRateFreeRTOS/Core/Src/app_entry.c                                              Entry point of the application 
  - BLE_HeartRateFreeRTOS/Core/Src/main.c                                                   Main program body 
  - BLE_HeartRateFreeRTOS/Core/Src/stm32wbaxx_hal_msp.c                                     This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRateFreeRTOS/Core/Src/stm32wbaxx_it.c                                          Interrupt Service Routines
  - BLE_HeartRateFreeRTOS/Core/Src/system_stm32wbaxx.c                                      CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_HeartRateFreeRTOS/Core/Src/app_freertos.c                                           FreeRTOS objects managed by X-CUBEFREERTOS pack
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/app_ble.c                                          BLE Application 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/dis.c                                              service2 definition 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/dis_app.c                                          service2_app application definition
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/hrs.c                                              service1 definition
  - BLE_HeartRateFreeRTOS/STM32_WPAN/App/hrs_app.c                                          service1_app application definition
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/bleplat.c                                       This file implements the platform functions for BLE stack library
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/bpka.c                                          This file implements the BLE PKA module
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/host_stack_if.c                                 Source file for the stack tasks 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/linklayer_plat.c                                Source file for the linklayer plateform adaptation layer 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/ll_sys_if.c                                     Source file for initiating the system sequencer 
  - BLE_HeartRateFreeRTOS/STM32_WPAN/Target/power_table.c                                   This file contains supported power tables 
  - BLE_HeartRateFreeRTOS/System/Config/Debug_GPIO/app_debug.c                              Real Time Debug module application side APIs 
  - BLE_HeartRateFreeRTOS/System/Config/LowPower/user_low_power_config.c                    Low power related user configuration
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw_aes.c                                        This file contains the AES driver for STM32WBA 
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw_otp.c                                        This file contains the OTP driver
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw_pka.c                                        This file contains the PKA driver for STM32WBA 
  - BLE_HeartRateFreeRTOS/System/Interfaces/hw_rng.c                                        This file contains the RNG driver for STM32WBA 
  - BLE_HeartRateFreeRTOS/System/Interfaces/pka_p256.c                                      This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_HeartRateFreeRTOS/System/Interfaces/stm32_lpm_if.c                                  Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_HeartRateFreeRTOS/System/Interfaces/timer_if.c                                      Configure RTC Alarm, Tick and Calendar manager 
  - BLE_HeartRateFreeRTOS/System/Interfaces/usart_if.c                                      Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_HeartRateFreeRTOS/System/Modules/adc_ctrl.c                                         Header for ADC client manager module 
  - BLE_HeartRateFreeRTOS/System/Modules/app_sys.c                                          Application system for STM32WPAN Middleware
  - BLE_HeartRateFreeRTOS/System/Modules/ble_timer.c                                        This module implements the timer core functions 
  - BLE_HeartRateFreeRTOS/System/Modules/otp.c                                              Source file for One Time Programmable (OTP) area 
  - BLE_HeartRateFreeRTOS/System/Modules/scm.c                                              Functions for the System Clock Manager
  - BLE_HeartRateFreeRTOS/System/Modules/stm_list.c                                         TCircular Linked List Implementation
  - BLE_HeartRateFreeRTOS/System/Modules/baes/baes_cmac.c                                   This file contains the AES CMAC implementation
  - BLE_HeartRateFreeRTOS/System/Modules/baes/baes_ecb.c                                    This file contains the AES ECB functions implementation
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/flash_driver.c                               The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/flash_manager.c                              The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/rf_timing_synchro.c                          The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_HeartRateFreeRTOS/System/Modules/Flash/simple_nvm_arbiter.c                         The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_HeartRateFreeRTOS/System/Modules/MemoryManager/advanced_memory_manager.c            Memory Manager 
  - BLE_HeartRateFreeRTOS/System/Modules/MemoryManager/stm32_mm.c                           Memory Manager 
  - BLE_HeartRateFreeRTOS/System/Modules/Nvm/nvm_emul.c                                     This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_HeartRateFreeRTOS/System/Modules/RFControl/rf_antenna_switch.c                      RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/RTDebug.c                                  Real Time Debug module API definition 
  - BLE_HeartRateFreeRTOS/System/Modules/RTDebug/RTDebug_dtb.c                              Real Time Debug module API definition for DTB usage

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.
  - Connect the Nucleo Board to your PC with a USB cable to ST-LINK connector (USB_STLINK). 
  - This application is by default configured to support low power mode ( No traces - No debugger ) 
  - Low Power configuration can be modified in app_conf.h (CFG_LPM_LEVEL)

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 On the android/ios device, enable the Bluetooth communications, and if not done before:

   - Install the ST BLE Toolbox application on the android device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

   - You can also install the ST BLE Sensor application on the android/ios device:
	   - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	   - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>
	
 Power on the Nucleo board with the BLE_HeartRateFreeRTOS application.

- Then, click on the App icon, ST BLE Toolbox (android/ios device),
   You can either open ST BLE Sensor application (android/ios device).

- In the Heart Rate interface, HearRate and energy measurement are launched and displayed in graphs,
  you can reset the energy measurement.
   - Pairing is supported: button B2 clears the security database when the device is not connected. 
   When connected with a client, button B2 send the slave pairing request, here a popup asks you to associate your device.
   You can either bond from the smartphone by clicking on Bond button in the ST BLE Toolbox application interface.
   - This example supports switch to 2Mbits PHY, pressing button B1 while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
   - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing button B1 while advertising allows to restart fast advertising.
   - Pressing button B3 while connected allows to update the connection interval. 

