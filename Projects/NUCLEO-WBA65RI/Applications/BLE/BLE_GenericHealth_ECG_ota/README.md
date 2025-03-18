## __BLE_GenericHealth_ECG_ota Application Description__

How to use the Generic Health Sensor profile as specified by the BLE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Directory contents__

  - BLE_GenericHealth_ECG_ota/Core/Inc/app_common.h                                                    App Common application configuration file for STM32WPAN Middleware.
  - BLE_GenericHealth_ECG_ota/Core/Inc/app_conf.h                                                      Application configuration file for STM32WPAN Middleware.
  - BLE_GenericHealth_ECG_ota/Core/Inc/app_entry.h                                                     Interface to the application
  - BLE_GenericHealth_ECG_ota/Core/Inc/main.h                                                          Header for main.c file. This file contains the common defines of the application.  
  - BLE_GenericHealth_ECG_ota/Core/Inc/stm32wbaxx_hal_conf.h                                           HAL configuration file.
  - BLE_GenericHealth_ECG_ota/Core/Inc/stm32wbaxx_it.h                                                 This file contains the headers of the interrupt handlers.
  - BLE_GenericHealth_ECG_ota/Core/Inc/stm32wbaxx_nucleo_conf.h                                        STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h                                                                                 .
  - BLE_GenericHealth_ECG_ota/Core/Inc/utilities_conf.h                                                Header for configuration file for STM32 Utilities.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/app_ble.h                                                 Header for ble application
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/bas.h                                                     Header for service3.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/bas_app.h                                                 Header for service3_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ble_conf.h                                                Configuration file for BLE Middleware.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ble_dbg_conf.h                                            Debug configuration file for BLE Middleware.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/dis.h                                                     Header for service2.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/dis_app.h                                                 Header for service2_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ets.h                                                     Header for service5.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ets_app.h                                                 Header for service5_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs.h                                                     Header for service1.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_app.h                                                 Header for service1_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_ccp.h                                                 Header for idss_ccp.c module
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_cp.h                                                  Header for ghs_cp.c module
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_db.h                                                  Header for ghs_db.c module
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_racp.h                                                Header for ghs_racp.c module
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/rcs.h                                                     Header for service6.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/rcs_app.h                                                 Header for service6_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/uds.h                                                     Header for service4.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/uds_app.h                                                 Header for service4_app.c
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/bpka.h                                                 This file contains the interface of the BLE PKA module.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/host_stack_if.h                                        This file contains the interface for the stack tasks
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/ll_sys_if.h                                            Header file for initiating system
  - BLE_GenericHealth_ECG_ota/System/Config/ADC_Ctrl/adc_ctrl_conf.h                                   Configuration Header for adc_ctrl.c module
  - BLE_GenericHealth_ECG_ota/System/Config/CRC_Ctrl/crc_ctrl_conf.h                                   Configuration Header for crc_ctrl.c module
  - BLE_GenericHealth_ECG_ota/System/Config/Debug_GPIO/app_debug.h                                     Real Time Debug module application APIs and signal table
  - BLE_GenericHealth_ECG_ota/System/Config/Debug_GPIO/app_debug_signal_def.h                          Real Time Debug module application signal definition
  - BLE_GenericHealth_ECG_ota/System/Config/Debug_GPIO/debug_config.h                                  Real Time Debug module general configuration file
  - BLE_GenericHealth_ECG_ota/System/Config/Flash/simple_nvm_arbiter_conf.h                            Configuration header for simple_nvm_arbiter.c module
  - BLE_GenericHealth_ECG_ota/System/Config/Log/log_module.h                                           Header file of the log module.
  - BLE_GenericHealth_ECG_ota/System/Config/LowPower/app_sys.h                                         Header for app_sys.c
  - BLE_GenericHealth_ECG_ota/System/Config/LowPower/peripheral_init.h                                 Header for peripheral init module
  - BLE_GenericHealth_ECG_ota/System/Config/LowPower/user_low_power_config.h                           Header for user_low_power_config.c
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw.h                                                   This file contains the interface of STM32 HW drivers.
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw_if.h                                                Hardware Interface
  - BLE_GenericHealth_ECG_ota/System/Interfaces/stm32_lpm_if.h                                         Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_GenericHealth_ECG_ota/System/Interfaces/timer_if.h                                             configuration of the timer_if.c instances
  - BLE_GenericHealth_ECG_ota/System/Interfaces/usart_if.h                                             Header file for stm32_adv_trace interface file
  - BLE_GenericHealth_ECG_ota/System/Modules/adc_ctrl.h                                                Header for ADC client manager module
  - BLE_GenericHealth_ECG_ota/System/Modules/ble_timer.h                                               This header defines the timer functions used by the BLE stack
  - BLE_GenericHealth_ECG_ota/System/Modules/crc_ctrl.h                                                Header for CRC client manager module
  - BLE_GenericHealth_ECG_ota/System/Modules/dbg_trace.h                                               Header for dbg_trace.c
  - BLE_GenericHealth_ECG_ota/System/Modules/otp.h                                                     Header file for One Time Programmable (OTP) area
  - BLE_GenericHealth_ECG_ota/System/Modules/scm.h                                                     Header for scm.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/stm_list.h                                                Header file for linked list library.
  - BLE_GenericHealth_ECG_ota/System/Modules/temp_measurement.h                                        Header for temp_measurement.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/utilities_common.h                                        Common file to utilities
  - BLE_GenericHealth_ECG_ota/System/Modules/baes/baes.h                                               This file contains the interface of the basic AES software module.
  - BLE_GenericHealth_ECG_ota/System/Modules/baes/baes_global.h                                        This file contains the internal definitions of the AES software module.
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/flash_driver.h                                      Header for flash_driver.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/flash_manager.h                                     Header for flash_manager.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/rf_timing_synchro.h                                 Header for rf_timing_synchro.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/simple_nvm_arbiter.h                                Header for simple_nvm_arbiter.c module      
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/simple_nvm_arbiter_common.h                         Common header of simple_nvm_arbiter.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/MemoryManager/advanced_memory_manager.h                   Header for advance_memory_manager.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/MemoryManager/stm32_mm.h                                  Header for stm32_mm.c module
  - BLE_GenericHealth_ECG_ota/System/Modules/Nvm/nvm.h                                                 This file contains the interface of the NVM manager.
  - BLE_GenericHealth_ECG_ota/System/Modules/RFControl/rf_antenna_switch.h                             RF related module to handle dedictated GPIOs for antenna switch
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/debug_signals.h                                   Real Time Debug module System and Link Layer signal definition
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/local_debug_tables.h                              Real Time Debug module System and Link Layer signal
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/RTDebug.h                                         Real Time Debug module API declaration
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/RTDebug_dtb.h                                     Real Time Debug module API declaration for DTB usage
  - BLE_GenericHealth_ECG_ota/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.h		     Header file for the serial commands interpreter module.
  - BLE_GenericHealth_ECG_ota/Core/Src/app_entry.c                                                     Entry point of the application      
  - BLE_GenericHealth_ECG_ota/Core/Src/main.c                                                          Main program body      
  - BLE_GenericHealth_ECG_ota/Core/Src/stm32wbaxx_hal_msp.c                                            This file provides code for the MSP Initialization and de-Initialization codes.      
  - BLE_GenericHealth_ECG_ota/Core/Src/stm32wbaxx_it.c                                                 Interrupt Service Routines.
  - BLE_GenericHealth_ECG_ota/Core/Src/system_stm32wbaxx.c                                             CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/Core/syscalls.c                            STM32CubeIDE Minimal System calls file
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/Core/sysmem.c                              STM32CubeIDE System Memory calls file
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/bas.c                       service3 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/bas_app.c                   service3_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/dis.c                       service2 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/dis_app.c                   service2_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ets.c                       service5 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ets_app.c                   service5_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ghs.c                       service1 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ghs_app.c                   service1_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ghs_cp.c                    GHS CP
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ghs_db.c                    GHS Records Database
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/ghs_racp.c                  GHS RACP
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/rcs.c                       service6 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/rcs_app.c                   service6_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/uds.c                       service4 definition.
  - BLE_GenericHealth_ECG_ota/STM32CubeIDE/Application/User/STM32_WPAN/App/uds_app.c                   service4_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/app_ble.c                                                 BLE Application
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/bas.c                                                     service3 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/bas_app.c                                                 service3_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/dis.c                                                     service2 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/dis_app.c                                                 service2_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ets.c                                                     service5 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ets_app.c                                                 service5_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs.c                                                     service1 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_app.c                                                 service1_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_ccp.c                                                 CCP response code to the previous request
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_cp.c                                                  GHS CP
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_db.c                                                  GHS Records Database
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/ghs_racp.c                                                GHS RACP
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/rcs.c                                                     service6 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/rcs_app.c                                                 service6_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/uds.c                                                     service4 definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/App/uds_app.c                                                 service4_app application definition.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/bleplat.c                                              This file implements the platform functions for BLE stack library.
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/bpka.c                                                 This file implements the BLE PKA module.      
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/host_stack_if.c                                        Source file for the stack tasks      
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/linklayer_plat.c                                       Source file for the linklayer plateform adaptation layer      
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/ll_sys_if.c                                            Source file for initiating system      
  - BLE_GenericHealth_ECG_ota/STM32_WPAN/Target/power_table.c                                          This file contains supported power tables
  - BLE_GenericHealth_ECG_ota/System/Config/ADC_Ctrl/adc_ctrl_conf.c                                   Source for ADC client controller module configuration file
  - BLE_GenericHealth_ECG_ota/System/Config/CRC_Ctrl/crc_ctrl_conf.c                                   Source for CRC client controller module configuration file
  - BLE_GenericHealth_ECG_ota/System/Config/Debug_GPIO/app_debug.c                                     Real Time Debug module application side APIs
  - BLE_GenericHealth_ECG_ota/System/Config/Flash/simple_nvm_arbiter_conf.c                            The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_GenericHealth_ECG_ota/System/Config/Log/log_module.c                                           Source file of the log module.
  - BLE_GenericHealth_ECG_ota/System/Config/LowPower/peripheral_init.c                                 tbd module
  - BLE_GenericHealth_ECG_ota/System/Config/LowPower/user_low_power_config.c                           Low power related user configuration.
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw_aes.c                                               This file contains the AES driver for STM32WBA
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw_otp.c                                               This file contains the OTP driver.
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw_pka.c                                               This file contains the PKA driver for STM32WBA
  - BLE_GenericHealth_ECG_ota/System/Interfaces/hw_rng.c                                               This file contains the RNG driver for STM32WBA
  - BLE_GenericHealth_ECG_ota/System/Interfaces/pka_p256.c                                             This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve.
  - BLE_GenericHealth_ECG_ota/System/Interfaces/stm32_lpm_if.c                                         Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_GenericHealth_ECG_ota/System/Interfaces/timer_if.c                                             Configure RTC Alarm, Tick and Calendar manager
  - BLE_GenericHealth_ECG_ota/System/Interfaces/usart_if.c                                             Source file for interfacing the stm32_adv_trace to hardware
  - BLE_GenericHealth_ECG_ota/System/Modules/adc_ctrl.c                                                Header for ADC client manager module
  - BLE_GenericHealth_ECG_ota/System/Modules/app_sys.c                                                 Application system for STM32WPAN Middleware.
  - BLE_GenericHealth_ECG_ota/System/Modules/ble_timer.c                                               This module implements the timer core functions
  - BLE_GenericHealth_ECG_ota/System/Modules/crc_ctrl.c                                                Source for CRC client controller module
  - BLE_GenericHealth_ECG_ota/System/Modules/otp.c                                                     Source file for One Time Programmable (OTP) area
  - BLE_GenericHealth_ECG_ota/System/Modules/scm.c                                                     Functions for the System Clock Manager.
  - BLE_GenericHealth_ECG_ota/System/Modules/stm_list.c                                                TCircular Linked List Implementation.
  - BLE_GenericHealth_ECG_ota/System/Modules/temp_measurement.c                                        Temp measurement module
  - BLE_GenericHealth_ECG_ota/System/Modules/baes/baes_cmac.c                                          This file contains the AES CMAC implementation.
  - BLE_GenericHealth_ECG_ota/System/Modules/baes/baes_ecb.c                                           This file contains the AES ECB functions implementation.
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/flash_driver.c                                      The Flas.h Driver module is the interface layer between Flash.h management modules and HAL Flash.h drivers
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/flash_manager.c                                     The Flas.h Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/rf_timing_synchro.c                                 The RF Timing Synchronization module provides an interface to synchronize the flash.h processing versus the RF activity to make sure the RF timing is not broken
  - BLE_GenericHealth_ECG_ota/System/Modules/Flash/simple_nvm_arbiter.c                                The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_GenericHealth_ECG_ota/System/Modules/MemoryManager/advanced_memory_manager.c                   Memory Manager
  - BLE_GenericHealth_ECG_ota/System/Modules/MemoryManager/stm32_mm.c                                         
  - BLE_GenericHealth_ECG_ota/System/Modules/Nvm/nvm_emul.c                                            This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose.  
  - BLE_GenericHealth_ECG_ota/System/Modules/RFControl/rf_antenna_switch.c                             RF related module to handle dedictated GPIOs for antenna swit.h                                          
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/RTDebug.c                                         Real Time Debug module API definition  
  - BLE_GenericHealth_ECG_ota/System/Modules/RTDebug/RTDebug_dtb.c                                     Real Time Debug module API definition for DTB usage
  - BLE_GenericHealth_ECG_ota/System/Modules/SerialCmdInterpreter/serial_cmd_interpreter.c             Source file for the serial commands interpreter module.
  

### __Hardware and Software environment__

  - This example runs on STM32WBAxx devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
  - This application is by default configured to support low power mode ( No traces - No debugger ) 
  - Low Power configuration can be modified in app_conf.h (CFG_LPM_LEVEL)

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash.h the board with the executable file

 On the android/ios device, enable the Bluetoo.h communications, and if not done before:

   - Install the ST BLE Toolbox application on the android device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
   
 Power on the Nucleo board with the BLE_GenericHealth_ECG_ota application.

- Then, click on the App icon, ST BLE Toolbox (android/ios device),

   - In the Generic Health Sensor interface, you read and write all the Generic Health Sensor characteristics,
   - Pairing is supported: button B2 clears the security database when the device is not connected. 
   When connected with a client, button B2 send the slave pairing request, here a popup asks you to associate your device.
   You can either bond from the smartphone by clicking on Bond button in the ST BLE Toolbox application interface.
   - This example supports switch to 2Mbits PHY, pressing button B1 while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
   - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing button B1 while advertising allows to restart fast advertising.
   - Pressing button B3 while connected allows to update the connection interval. 

