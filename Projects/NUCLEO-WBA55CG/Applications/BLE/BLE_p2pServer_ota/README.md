## __BLE_p2pServer_ota Application Description__

Demonstrate STM32WBA acting as BLE peripheral and GATT server.

BLE_p2pServer_ota application advertise and wait for a connection from either:

- BLE_p2pClient application running on STM32WBxx or STM32WBAxx devices
- BLE_p2pServer application running on STM32WBxx or STM32WBAxx devices
- ST BLE Toolbox smartphone application
- ST BLE Sensor smartphone application

Once connected, p2pServer can receive write and send notification messages.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile

### __Directory contents__

  - BLE_p2pServer_ota/Core/Inc/app_common.h                                         App Common application configuration file for STM32WPAN Middleware. 
  - BLE_p2pServer_ota/Core/Inc/app_conf.h                                           Application configuration file for STM32WPAN Middleware. 
  - BLE_p2pServer_ota/Core/Inc/app_entry.h                                          Interface to the application 
  - BLE_p2pServer_ota/Core/Inc/main.h                                               Header for main.c file. This file contains the common defines of the application.
  - BLE_p2pServer_ota/Core/Inc/stm32wbaxx_hal_conf.h                                HAL configuration file. 
  - BLE_p2pServer_ota/Core/Inc/stm32wbaxx_it.h                                      This file contains the headers of the interrupt handlers. 
  - BLE_p2pServer_ota/Core/Inc/stm32wbaxx_nucleo_conf.h                             STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h .
  - BLE_p2pServer_ota/Core/Inc/utilities_conf.h                                     Header for configuration file for STM32 Utilities. 
  - BLE_p2pServer_ota/STM32_WPAN/App/app_ble.h                                      Header for ble application 
  - BLE_p2pServer_ota/STM32_WPAN/App/ble_conf.h                                     Configuration file for BLE Middleware. 
  - BLE_p2pServer_ota/STM32_WPAN/App/ble_dbg_conf.h                                 Debug configuration file for BLE Middleware. 
  - BLE_p2pServer_ota/STM32_WPAN/App/ota.h                                          Header for STM32_WPAN 
  - BLE_p2pServer_ota/STM32_WPAN/App/ota_app.h                                      Header for STM32_WPAN 
  - BLE_p2pServer_ota/STM32_WPAN/App/p2p_server.h                                   Header for p2p_server.c 
  - BLE_p2pServer_ota/STM32_WPAN/App/p2p_server_app.h                               Header for p2p_server_app.c 
  - BLE_p2pServer_ota/STM32_WPAN/Target/bpka.h                                      This file contains the interface of the BLE PKA module. 
  - BLE_p2pServer_ota/STM32_WPAN/Target/host_stack_if.h                             This file contains the interface for the stack tasks 
  - BLE_p2pServer_ota/STM32_WPAN/Target/ll_sys_if.h                                 Header file for ll_sys_if.c
  - BLE_p2pServer_ota/System/Config/Debug_GPIO/app_debug.h                          Real Time Debug module application APIs and signal table 
  - BLE_p2pServer_ota/System/Config/Debug_GPIO/app_debug_signal_def.h               Real Time Debug module application signal definition 
  - BLE_p2pServer_ota/System/Config/Debug_GPIO/debug_config.h                       Real Time Debug module general configuration file 
  - BLE_p2pServer_ota/System/Config/Flash/simple_nvm_arbiter_conf.h                 Configuration header for simple_nvm_arbiter.c module 
  - BLE_p2pServer_ota/System/Config/LowPower/app_sys.h                              Header for app_sys.c 
  - BLE_p2pServer_ota/System/Config/LowPower/user_low_power_config.h                Header for user_low_power_config.c
  - BLE_p2pServer_ota/System/Interfaces/hw.h                                        This file contains the interface of STM32 HW drivers. 
  - BLE_p2pServer_ota/System/Interfaces/hw_if.h                                     Hardware Interface 
  - BLE_p2pServer_ota/System/Interfaces/stm32_lpm_if.h                              Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_p2pServer_ota/System/Interfaces/timer_if.h                                  configuration of the timer_if.c instances 
  - BLE_p2pServer_ota/System/Interfaces/usart_if.h                                  Header file for stm32_adv_trace interface file 
  - BLE_p2pServer_ota/System/Modules/adc_ctrl.h                                     Header for ADC client manager module 
  - BLE_p2pServer_ota/System/Modules/ble_timer.h                                    This header defines the timer functions used by the BLE stack 
  - BLE_p2pServer_ota/System/Modules/dbg_trace.h                                    Header for dbg_trace.c 
  - BLE_p2pServer_ota/System/Modules/otp.h                                          Header file for One Time Programmable (OTP) area 
  - BLE_p2pServer_ota/System/Modules/scm.h                                          Header for scm.c module 
  - BLE_p2pServer_ota/System/Modules/stm_list.h                                     Header file for linked list library. 
  - BLE_p2pServer_ota/System/Modules/utilities_common.h                             Common file to utilities 
  - BLE_p2pServer_ota/System/Modules/baes/baes.h                                    This file contains the interface of the basic AES software module. 
  - BLE_p2pServer_ota/System/Modules/baes/baes_global.h                             This file contains the internal definitions of the AES software module.
  - BLE_p2pServer_ota/System/Modules/Flash/flash_driver.h                           Header for flash_driver.c module 
  - BLE_p2pServer_ota/System/Modules/Flash/flash_manager.h                          Header for flash_manager.c module 
  - BLE_p2pServer_ota/System/Modules/Flash/rf_timing_synchro.h                      Header for rf_timing_synchro.c module 
  - BLE_p2pServer_ota/System/Modules/Flash/simple_nvm_arbiter.h                     Header for simple_nvm_arbiter.c module 
  - BLE_p2pServer_ota/System/Modules/Flash/simple_nvm_arbiter_common.h              Common header of simple_nvm_arbiter.c module 
  - BLE_p2pServer_ota/System/Modules/MemoryManager/advanced_memory_manager.h        Header for advance_memory_manager.c module 
  - BLE_p2pServer_ota/System/Modules/MemoryManager/stm32_mm.h                       Header for stm32_mm.c module 
  - BLE_p2pServer_ota/System/Modules/Nvm/nvm.h                                      This file contains the interface of the NVM manager. 
  - BLE_p2pServer_ota/System/Modules/RFControl/rf_antenna_switch.h                  RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer_ota/System/Modules/RTDebug/debug_signals.h                        Real Time Debug module System and Link Layer signal definition 
  - BLE_p2pServer_ota/System/Modules/RTDebug/local_debug_tables.h                   Real Time Debug module System and Link Layer signal 
  - BLE_p2pServer_ota/System/Modules/RTDebug/RTDebug.h                              Real Time Debug module API declaration 
  - BLE_p2pServer_ota/System/Modules/RTDebug/RTDebug_dtb.h                          Real Time Debug module API declaration for DTB usage
  - BLE_p2pServer_ota/Core/Src/app_entry.c                                          Entry point of the application 
  - BLE_p2pServer_ota/Core/Src/main.c                                               Main program body 
  - BLE_p2pServer_ota/Core/Src/stm32wbaxx_hal_msp.c                                 This file provides code for the MSP Initialization and de-Initialization codes.
  - BLE_p2pServer_ota/Core/Src/stm32wbaxx_it.c                                      Interrupt Service Routines. 
  - BLE_p2pServer_ota/Core/Src/system_stm32wbaxx.c                                  CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_p2pServer_ota/STM32_WPAN/App/app_ble.c                                      BLE Application 
  - BLE_p2pServer_ota/STM32_WPAN/App/ota.c                                          STM32_WPAN definition. 
  - BLE_p2pServer_ota/STM32_WPAN/App/ota_app.c                                      STM32_WPAN application definition. 
  - BLE_p2pServer_ota/STM32_WPAN/App/p2p_server.c                                   p2p_server definition. 
  - BLE_p2pServer_ota/STM32_WPAN/App/p2p_server_app.c                               p2p_server_app application definition. 
  - BLE_p2pServer_ota/STM32_WPAN/Target/bleplat.c                                   This file implements the platform functions for BLE stack library. 
  - BLE_p2pServer_ota/STM32_WPAN/Target/bpka.c                                      This file implements the BLE PKA module. 
  - BLE_p2pServer_ota/STM32_WPAN/Target/host_stack_if.c                             Source file for the stack tasks 
  - BLE_p2pServer_ota/STM32_WPAN/Target/linklayer_plat.c                            Source file for the linklayer plateform adaptation layer 
  - BLE_p2pServer_ota/STM32_WPAN/Target/ll_sys_if.c                                 Source file for initiating the system sequencer 
  - BLE_p2pServer_ota/STM32_WPAN/Target/power_table.c                               This file contains supported power tables 
  - BLE_p2pServer_ota/System/Config/Debug_GPIO/app_debug.c                          Real Time Debug module application side APIs 
  - BLE_p2pServer_ota/System/Config/LowPower/user_low_power_config.c                Low power related user configuration
  - BLE_p2pServer_ota/System/Interfaces/hw_aes.c                                    This file contains the AES driver for STM32WBA 
  - BLE_p2pServer_ota/System/Interfaces/hw_otp.c                                    This file contains the OTP driver. 
  - BLE_p2pServer_ota/System/Interfaces/hw_pka.c                                    This file contains the PKA driver for STM32WBA 
  - BLE_p2pServer_ota/System/Interfaces/hw_rng.c                                    This file contains the RNG driver for STM32WBA 
  - BLE_p2pServer_ota/System/Interfaces/pka_p256.c                                  This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve.
  - BLE_p2pServer_ota/System/Interfaces/stm32_lpm_if.c                              Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_p2pServer_ota/System/Interfaces/timer_if.c                                  Configure RTC Alarm, Tick and Calendar manager 
  - BLE_p2pServer_ota/System/Interfaces/usart_if.c                                  Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_p2pServer_ota/System/Modules/adc_ctrl.c                                     Header for ADC client manager module 
  - BLE_p2pServer_ota/System/Modules/app_sys.c                                      Application system for STM32WPAN Middleware. 
  - BLE_p2pServer_ota/System/Modules/ble_timer.c                                    This module implements the timer core functions 
  - BLE_p2pServer_ota/System/Modules/otp.c                                          Source file for One Time Programmable (OTP) area 
  - BLE_p2pServer_ota/System/Modules/scm.c                                          Functions for the System Clock Manager. 
  - BLE_p2pServer_ota/System/Modules/stm_list.c                                     TCircular Linked List Implementation. 
  - BLE_p2pServer_ota/System/Modules/baes/baes_cmac.c                               This file contains the AES CMAC implementation. 
  - BLE_p2pServer_ota/System/Modules/baes/baes_ecb.c                                This file contains the AES ECB functions implementation. 
  - BLE_p2pServer_ota/System/Modules/Flash/flash_driver.c                           The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_p2pServer_ota/System/Modules/Flash/flash_manager.c                          The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_p2pServer_ota/System/Modules/Flash/rf_timing_synchro.c                      The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_p2pServer_ota/System/Modules/Flash/simple_nvm_arbiter.c                     The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs.
  - BLE_p2pServer_ota/System/Modules/MemoryManager/advanced_memory_manager.c        Memory Manager 
  - BLE_p2pServer_ota/System/Modules/MemoryManager/stm32_mm.c                       Memory Manager
  - BLE_p2pServer_ota/System/Modules/Nvm/nvm_emul.c                                 This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose.
  - BLE_p2pServer_ota/System/Modules/RFControl/rf_antenna_switch.c                  RF related module to handle dedictated GPIOs for antenna switch
  - BLE_p2pServer_ota/System/Modules/RTDebug/RTDebug.c                              Real Time Debug module API definition 
  - BLE_p2pServer_ota/System/Modules/RTDebug/RTDebug_dtb.c                          Real Time Debug module API definition for DTB usage

### __Hardware and Software environment__

  - This application runs on STM32WBA55 Nucleo board.
  - Another STM32WBAxx Nucleo board may be necessary to run p2pClient or p2pRouter application.
    
### __How to use it?__

In order to make the program work, you must do the following:
 - Verify that the Maximum supported ATT_MTU size configured in app_conf.h file is 251 bytes
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory:
   BLE_ApplicationInstallManager binary is downloaded at the memory address 0x08000000
   BLE_p2pServer_ota binary is downloaded at the memory address 0x08006000 
 - Run the example

 __You can interact with p2pServer_ota application with a smartphone:__

 - Install and launch ST BLE Toolbox or ST BLE Sensor applications on android or iOS smartphone
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>
   - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	 - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>
	
 - Power on the Nucleo board with the BLE_ApplicationInstallManager and BLE_p2pServer_ota applications.
 - Open ST BLE Toolbox application:
   select the P2PS_WBAxx in the device list, where xx is the 2 last digits of the BD ADDRESS.
 - Scroll right and select either the P2P Server interface or the Over The Air Update Server interface

 - In the P2P Server interface:
   - Click on the LED icon of the application interface to switch On/Off LED1 of the Nucleo board.
   - Press the Button B1 of the board to notify the smartphone application.
   - You can change connection update interval by pressing Button B3.

 - In the Over The Air Update Server interface, select the STM32WBA device type.
   - The STM32WBA interface offers the choice to download an Application Binary or a User Conf Binary
   - Memory mapping is defined as following: the sectors 0 and 1 is reserved for the BLE_ApplicationInstallManager application, 
    sector 2 to sector 61 are dedicated to BLE_p2pServer_ota application, sector 62 to sector 122 are dedicated for the new application, 
    sector 123 is dedicated to User Configuration Data, sector 124 to sector 127 reserved for NVM and Static  
   - When selecting Application Binary the default Address offset 0x080000 is displayed, 
    the download address of the new application is then: 0x08080000.
    An address offset between 0x080000 to (0x0F4000 - size in sectors of the new application) can be chosen.
   - When selecting the User Conf Binary, the Address offset to choose is the address offset of the configured User Configuration Data area defined in the app_conf.h header file,
    0x0F6000 in the current BLE_HearRate_ota application example.
   - With SELECT FILE you can choose the binary of the new application with OTA or the binary of the User Configuration Data to download
   - After this binary choice, a calculated Number of sectors to erase is displayed resulting of the size in sectors of the application or the user configuration data to download
   - With the Force it choice, the Number of sectors to erase can be forced or not
   - After selecting download icon, the download is in progress
   - In the case of a new application download, a reboot on this new application is done at the end of the download 

 __You can interact with p2pServer_ota application with another Nucleo board:__
 
 - Power up p2pClient devices next to p2pServer device.
 - On p2pClient device button B1 click, scan and then connect is initiated to a p2pServer device surrounding.
 - Once connected:
    - On p2pServer device button B1 click, a notification message is sent toward connected p2pClient device.
    - On p2pClient device button B1 click, a write message is sent toward connected p2pServer device.
 
- Adertising is stopped after 60s, button B1 click allows to restart it.
- When not connected, button B2 click allows to clear security database.
- Once connected, button B3 click allows to update connection interval parameters.

Multi connection support:

- Pressing button B2 while the device is already connected starts a new advertising to allow multi-connection.
  