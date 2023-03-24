## __BLE_HeartRate Application Description__

How to use the Heart Rate profile as specified by the BLE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_HeartRate/Core/Inc/app_common.h                                             App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRate/Core/Inc/app_conf.h                                               Application configuration file for STM32WPAN Middleware
  - BLE_HeartRate/Core/Inc/app_entry.h                                              Interface to the application 
  - BLE_HeartRate/Core/Inc/main.h                                                   Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRate/Core/Inc/stm32wbaxx_hal_conf.h                                    HAL configuration file
  - BLE_HeartRate/Core/Inc/stm32wbaxx_it.h                                          This file contains the headers of the interrupt handlers
  - BLE_HeartRate/Core/Inc/stm32wbaxx_nucleo_conf.h                                 STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_HeartRate/Core/Inc/utilities_conf.h                                         Header for configuration file for STM32 Utilities
  - BLE_HeartRate/STM32_WPAN/App/app_ble.h                                          Header for ble application 
  - BLE_HeartRate/STM32_WPAN/App/ble_conf.h                                         Configuration file for BLE Middleware
  - BLE_HeartRate/STM32_WPAN/App/ble_dbg_conf.h                                     Debug configuration file for BLE Middleware
  - BLE_HeartRate/STM32_WPAN/App/dis.h                                              Header for service2.c 
  - BLE_HeartRate/STM32_WPAN/App/dis_app.h                                          Header for service2_app.c 
  - BLE_HeartRate/STM32_WPAN/App/hrs.h                                              Header for service1.c 
  - BLE_HeartRate/STM32_WPAN/App/hrs_app.h                                          Header for service1_app.c 
  - BLE_HeartRate/STM32_WPAN/Target/bpka.h                                          This file contains the interface of the BLE PKA module
  - BLE_HeartRate/STM32_WPAN/Target/host_stack_if.h                                 This file contains the interface for the stack tasks 
  - BLE_HeartRate/STM32_WPAN/Target/ll_sys_if.h                                     Header file for ll_sys_if.c
  - BLE_HeartRate/System/Config/Debug_GPIO/app_debug.h                              Real Time Debug module application APIs and signal table 
  - BLE_HeartRate/System/Config/Debug_GPIO/app_debug_signal_def.h                   Real Time Debug module application signal definition 
  - BLE_HeartRate/System/Config/Debug_GPIO/debug_config.h                           Real Time Debug module general configuration file 
  - BLE_HeartRate/System/Config/Flash/simple_nvm_arbiter_conf.h                     Configuration header for simple_nvm_arbiter.c module 
  - BLE_HeartRate/System/Config/LowPower/app_sys.h                                  Header for app_sys.c 
  - BLE_HeartRate/System/Interfaces/hw.h                                            This file contains the interface of STM32 HW drivers
  - BLE_HeartRate/System/Interfaces/hw_if.h                                         Hardware Interface 
  - BLE_HeartRate/System/Interfaces/stm32_lpm_if.h                                  Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_HeartRate/System/Interfaces/timer_if.h                                      configuration of the timer_if.c instances 
  - BLE_HeartRate/System/Interfaces/usart_if.h                                      Header file for stm32_adv_trace interface file 
  - BLE_HeartRate/System/Modules/adc_ctrl.h                                         Header for ADC client manager module 
  - BLE_HeartRate/System/Modules/ble_timer.h                                        This header defines the timer functions used by the BLE stack 
  - BLE_HeartRate/System/Modules/dbg_trace.h                                        Header for dbg_trace.c 
  - BLE_HeartRate/System/Modules/general_config.h                                   This file contains definitions that can be changed to configure some modules of the STM32 firmware application
  - BLE_HeartRate/System/Modules/otp.h                                              Header file for One Time Programmable (OTP) area 
  - BLE_HeartRate/System/Modules/scm.h                                              Header for scm.c module 
  - BLE_HeartRate/System/Modules/stm_list.h                                         Header file for linked list library
  - BLE_HeartRate/System/Modules/utilities_common.h                                 Common file to utilities 
  - BLE_HeartRate/System/Modules/baes/baes.h                                        This file contains the interface of the basic AES software module
  - BLE_HeartRate/System/Modules/baes/baes_global.h                                 This file contains the internal definitions of the AES software module
  - BLE_HeartRate/System/Modules/Flash/flash_driver.h                               Header for flash_driver.c module 
  - BLE_HeartRate/System/Modules/Flash/flash_manager.h                              Header for flash_manager.c module 
  - BLE_HeartRate/System/Modules/Flash/rf_timing_synchro.h                          Header for rf_timing_synchro.c module 
  - BLE_HeartRate/System/Modules/Flash/simple_nvm_arbiter.h                         Header for simple_nvm_arbiter.c module 
  - BLE_HeartRate/System/Modules/Flash/simple_nvm_arbiter_common.h                  Common header of simple_nvm_arbiter.c module 
  - BLE_HeartRate/System/Modules/MemoryManager/advanced_memory_manager.h            Header for advance_memory_manager.c module 
  - BLE_HeartRate/System/Modules/MemoryManager/stm32_mm.h                           Header for stm32_mm.c module 
  - BLE_HeartRate/System/Modules/Nvm/nvm.h                                          This file contains the interface of the NVM manager
  - BLE_HeartRate/System/Modules/RTDebug/debug_signals.h                            Real Time Debug module System and Link Layer signal definition 
  - BLE_HeartRate/System/Modules/RTDebug/local_debug_tables.h                       Real Time Debug module System and Link Layer signal 
  - BLE_HeartRate/System/Modules/RTDebug/RTDebug.h                                  Real Time Debug module API declaration 
  - BLE_HeartRate/Core/Src/app_entry.c                                              Entry point of the application 
  - BLE_HeartRate/Core/Src/main.c                                                   Main program body 
  - BLE_HeartRate/Core/Src/stm32wbaxx_hal_msp.c                                     This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRate/Core/Src/stm32wbaxx_it.c                                          Interrupt Service Routines
  - BLE_HeartRate/Core/Src/system_stm32wbaxx.c                                      CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_HeartRate/STM32_WPAN/App/app_ble.c                                          BLE Application 
  - BLE_HeartRate/STM32_WPAN/App/dis.c                                              service2 definition 
  - BLE_HeartRate/STM32_WPAN/App/dis_app.c                                          service2_app application definition
  - BLE_HeartRate/STM32_WPAN/App/hrs.c                                              service1 definition
  - BLE_HeartRate/STM32_WPAN/App/hrs_app.c                                          service1_app application definition
  - BLE_HeartRate/STM32_WPAN/Target/bleplat.c                                       This file implements the platform functions for BLE stack library
  - BLE_HeartRate/STM32_WPAN/Target/bpka.c                                          This file implements the BLE PKA module
  - BLE_HeartRate/STM32_WPAN/Target/host_stack_if.c                                 Source file for the stack tasks 
  - BLE_HeartRate/STM32_WPAN/Target/linklayer_plat.c                                Source file for the linklayer plateform adaptation layer 
  - BLE_HeartRate/STM32_WPAN/Target/ll_sys_if.c                                     Source file for initiating the system sequencer 
  - BLE_HeartRate/STM32_WPAN/Target/power_table.c                                   This file contains supported power tables 
  - BLE_HeartRate/System/Config/Debug_GPIO/app_debug.c                              Real Time Debug module application side APIs 
  - BLE_HeartRate/System/Interfaces/hw_aes.c                                        This file contains the AES driver for STM32WBA 
  - BLE_HeartRate/System/Interfaces/hw_otp.c                                        This file contains the OTP driver
  - BLE_HeartRate/System/Interfaces/hw_pka.c                                        This file contains the PKA driver for STM32WBA 
  - BLE_HeartRate/System/Interfaces/hw_rng.c                                        This file contains the RNG driver for STM32WBA 
  - BLE_HeartRate/System/Interfaces/pka_p256.c                                      This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_HeartRate/System/Interfaces/stm32_lpm_if.c                                  Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_HeartRate/System/Interfaces/timer_if.c                                      Configure RTC Alarm, Tick and Calendar manager 
  - BLE_HeartRate/System/Interfaces/usart_if.c                                      Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_HeartRate/System/Modules/adc_ctrl.c                                         Header for ADC client manager module 
  - BLE_HeartRate/System/Modules/app_sys.c                                          Application system for STM32WPAN Middleware
  - BLE_HeartRate/System/Modules/ble_timer.c                                        This module implements the timer core functions 
  - BLE_HeartRate/System/Modules/otp.c                                              Source file for One Time Programmable (OTP) area 
  - BLE_HeartRate/System/Modules/scm.c                                              Functions for the System Clock Manager
  - BLE_HeartRate/System/Modules/stm_list.c                                         TCircular Linked List Implementation
  - BLE_HeartRate/System/Modules/baes/baes_cmac.c                                   This file contains the AES CMAC implementation
  - BLE_HeartRate/System/Modules/baes/baes_ecb.c                                    This file contains the AES ECB functions implementation
  - BLE_HeartRate/System/Modules/Flash/flash_driver.c                               The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_HeartRate/System/Modules/Flash/flash_manager.c                              The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_HeartRate/System/Modules/Flash/rf_timing_synchro.c                          The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_HeartRate/System/Modules/Flash/simple_nvm_arbiter.c                         The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_HeartRate/System/Modules/MemoryManager/advanced_memory_manager.c            Memory Manager 
  - BLE_HeartRate/System/Modules/MemoryManager/stm32_mm.c                           Memory Manager 
  - BLE_HeartRate/System/Modules/Nvm/nvm_emul.c                                     This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_HeartRate/System/Modules/RTDebug/RTDebug.c                                  Real Time Debug module API definition 

### __Hardware and Software environment__

  - This example runs on STM32WBA52xx devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
  - This application is by default configured to support low power mode ( No traces - No debugger ) 
  - Low Power configuration can be modified in app_conf.h (CFG_FULL_LOW_POWER, CFG_DBG_SUPPORTED, CFG_DEBUG_APP_TRACE)

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 On the android/ios device, enable the Bluetooth communications, and if not done before:

   - Install the ST BLE Sensor application on the android/ios device:
	   - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	   - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>

   - You can also install the ST BLE Toolbox application on the android device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
	
 Power on the Nucleo board with the BLE_HeartRate application.

 - Then, click on the App icon, ST BLE Sensor (android/ios device),
   connect to a device,
   select the HR_xx in the device list.
 - The Heart Rate is displayed each second on the smartphone.
 - Pairing is supported: 
   - Button B1 clears the security database, 
   - Button B2 requests the slave req pairing.
 - This example supports switch to 2Mbits PHY, Button B3 is used to enable the feature.

 You can either open ST BLE Toolbox application (android/ios device).

 - Select the HR_xx in the device list and connect to the device.
 - Scroll right and select Heart Rate interface,
 - HearRate and energy measurement are launched and displayed in graphs,
   you can reset the energy measurement.
 - Pairing is supported: 
   - Button B1 clears the security database, 
   - Button B2 requests the slave req pairing, here a popup asks you to associate your device.
 - You can either bond from the smartphone by clicking on Bond Button.
 - This example supports switch to 2Mbits PHY, Button B3 is used to enable the feature.
