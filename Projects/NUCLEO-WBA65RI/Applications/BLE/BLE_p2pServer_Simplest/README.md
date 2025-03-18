## __BLE_p2pServer_Simplest Application Description__

How to demonstrate Point-to-Point communication using BLE as simple as possible.

p2pServer application advertises and waits for a connection from either:

 - p2pClient application running on STM32WBAxx devices
 - ST BLE Toolbox smartphone application

Once connected, p2pServer can receive message from the Client and send notification to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile

### __Directory contents__

  - BLE_p2pServer/Core/Inc/app_common.h                                             App Common application configuration file for STM32WPAN Middleware
  - BLE_p2pServer/Core/Inc/app_conf.h                                               Application configuration file for STM32WPAN Middleware
  - BLE_p2pServer/Core/Inc/app_debug.h                                              Not use in this project
  - BLE_p2pServer/Core/Inc/app_entry.h                                              Interface to the application 
  - BLE_p2pServer/Core/Inc/main.h                                                   Header for main.c file. This file contains the common defines of the application
  - BLE_p2pServer/Core/Inc/stm32wbaxx_hal_conf.h                                    HAL configuration file
  - BLE_p2pServer/Core/Inc/stm32wbaxx_it.h                                          This file contains the headers of the interrupt handlers
  - BLE_p2pServer/Core/Inc/utilities_conf.h                                         Header for configuration file for STM32 Utilities
  - BLE_p2pServer/STM32_WPAN/App/skel_ble.h                                         Header for ble application 
  - BLE_p2pServer/STM32_WPAN/App/ble_conf.h                                         Configuration file for BLE Middleware
  - BLE_p2pServer/STM32_WPAN/App/ble_dbg_conf.h                                     Debug configuration file for BLE Middleware
  - BLE_p2pServer/STM32_WPAN/Target/host_stack_if.h                                 This file contains the interface for the stack tasks 
  - BLE_p2pServer/STM32_WPAN/Target/ll_sys_if.h                                     Header file for ll_sys_if.c
  - BLE_p2pServer/System/Modules/dbg_trace.h                                        Header for dbg_trace.c 
  - BLE_p2pServer/System/Modules/stm_list.h                                         Header file for linked list library 
  - BLE_p2pServer/System/Modules/utilities_common.h                                 Common file to utilities 
  - BLE_p2pServer/Core/Src/app_entry.c                                              Entry point of the application 
  - BLE_p2pServer/Core/Src/main.c                                                   Main program body 
  - BLE_p2pServer/Core/Src/stm32wbaxx_hal_msp.c                                     This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_p2pServer/Core/Src/stm32wbaxx_it.c                                          Interrupt Service Routines 
  - BLE_p2pServer/Core/Src/system_stm32wbaxx.c                                      CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_p2pServer/STM32_WPAN/App/skel_ble.c                                          BLE Application 
  - BLE_p2pServer/STM32_WPAN/Target/bleplat.c                                       This file implements the platform functions for BLE stack library 
  - BLE_p2pServer/STM32_WPAN/Target/host_stack_if.c                                 Source file for the stack tasks 
  - BLE_p2pServer/STM32_WPAN/Target/linklayer_plat.c                                Source file for the linklayer plateform adaptation layer 
  - BLE_p2pServer/STM32_WPAN/Target/ll_sys_if.c                                     Source file for initiating the system sequencer 
  - BLE_p2pServer/STM32_WPAN/Target/power_table.c                                   This file contains supported power tables 
  - BLE_p2pServer/System/Modules/stm_list.c                                         TCircular Linked List Implementation 

### __Hardware and Software environment__

  - This application runs on STM32WBA65 Nucleo board.
    
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
   select the P2PS_SIMPLEST in the device list.
 - On p2pServer device B1 click, a notification message is sent toward connected smartphone.
 - On smartphone interface, send 1/0 in HEX to switch On/Off LED1 of the Nucleo board.
