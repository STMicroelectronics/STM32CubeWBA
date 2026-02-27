## __BLE_Thread_SED_FreeRTOS_Generic_FreeRTOS Application Description__

How to use the Concurrency mode BLE/Thread with COAP messages transmission over Thread for a Sleepy End Device.

This application demonstrates the BLE/Thread(SED) concurrency mode with a clock at 100MHz. The device is configured to attach to a Thread Leader 
and then sends COAP message every two seconds. This device also starts advertising and is available to accept incoming connection in BLE. This 
application demonstrates the Low Power, in Standby mode.

### __Keywords__

Connectivity, Concurrency, BLE, BLE protocol, Thread, COAP, STM32WBA, low power, Standby mode

### __Directory contents__

  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/app_common.h                                          App Common application configuration file for STM32WPAN Middleware
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/app_conf.h                                            Application configuration file for STM32WPAN Middleware
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/app_entry.h                                           Interface to the application 
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/main.h                                                Header for main.c file. This file contains the common defines of the application
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/stm32wbaxx_hal_conf.h                                 HAL configuration file
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/stm32wbaxx_it.h                                       This file contains the headers of the interrupt handlers
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/stm32wbaxx_dk_conf.h                                  STM32WBAXX DK board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_dk_conf.h
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Inc/utilities_conf.h                                      Header for configuration file for STM32 Utilities
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_ble.h                                       Header for ble application 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_coap.h                                      Header for COAP commands 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_thread.h                                    Header for thread application 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/ble_conf.h                                      Configuration file for BLE Middleware
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/ble_dbg_conf.h                                  Debug configuration file for BLE Middleware
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/dis.h                                           Header for service2.c 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/dis_app.h                                       Header for service2_app.c 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/hrs.h                                           Header for service1.c 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/hrs_app.h                                       Header for service1_app.c 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/bpka.h                                       This file contains the interface of the BLE PKA module
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/host_stack_if.h                              This file contains the interface for the stack tasks 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/ll_sys_if.h                                  Header file for ll_sys_if.c
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/Debug_GPIO/app_debug.h                           Real Time Debug module application APIs and signal table 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/Debug_GPIO/app_debug_signal_def.h                Real Time Debug module application signal definition 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/Debug_GPIO/debug_config.h                        Real Time Debug module general configuration file 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/Flash/simple_nvm_arbiter_conf.h                  Configuration header for simple_nvm_arbiter.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/LowPower/app_sys.h                               Header for app_sys.c 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/LowPower/user_low_power_config.h                 Header for user_low_power_config.c
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw.h                                         This file contains the interface of STM32 HW drivers
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw_if.h                                      Hardware Interface 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/stm32_lpm_if.h                               Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/timer_if.h                                   configuration of the timer_if.c instances 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/usart_if.h                                   Header file for stm32_adv_trace interface file 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/adc_ctrl.h                                      Header for ADC client manager module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/ble_timer.h                                     This header defines the timer functions used by the BLE stack 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/dbg_trace.h                                     Header for dbg_trace.c 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/otp.h                                           Header file for One Time Programmable (OTP) area 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/scm.h                                           Header for scm.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/stm_list.h                                      Header file for linked list library
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/utilities_common.h                              Common file to utilities 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/baes/baes.h                                     This file contains the interface of the basic AES software module
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/baes/baes_global.h                              This file contains the internal definitions of the AES software module
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/flash_driver.h                            Header for flash_driver.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/flash_manager.h                           Header for flash_manager.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/rf_timing_synchro.h                       Header for rf_timing_synchro.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/simple_nvm_arbiter.h                      Header for simple_nvm_arbiter.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/simple_nvm_arbiter_common.h               Common header of simple_nvm_arbiter.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/MemoryManager/advanced_memory_manager.h         Header for advance_memory_manager.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/MemoryManager/stm32_mm.h                        Header for stm32_mm.c module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Nvm/nvm.h                                       This file contains the interface of the NVM manager
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RFControl/rf_antenna_switch.h                   RF related module to handle dedictated GPIOs for antenna switch
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/debug_signals.h                         Real Time Debug module System and Link Layer signal definition 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/local_debug_tables.h                    Real Time Debug module System and Link Layer signal 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/RTDebug.h                               Real Time Debug module API declaration 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/RTDebug_dtb.h                           Real Time Debug module API declaration for DTB usage
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Src/app_entry.c                                           Entry point of the application 
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Src/main.c                                                Main program body 
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Src/stm32wbaxx_hal_msp.c                                  This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Src/stm32wbaxx_it.c                                       Interrupt Service Routines
  - BLE_Thread_SED_FreeRTOS_Generic/Core/Src/system_stm32wbaxx.c                                   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_ble.c                                       BLE Application
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_coap.c                                      COAP Commands Application
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/app_thread.c                                    Thread Application 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/dis.c                                           service2 definition 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/dis_app.c                                       service2_app application definition
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/hrs.c                                           service1 definition
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/App/hrs_app.c                                       service1_app application definition
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/bleplat.c                                    This file implements the platform functions for BLE stack library
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/bpka.c                                       This file implements the BLE PKA module
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/host_stack_if.c                              Source file for the stack tasks 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/linklayer_plat.c                             Source file for the linklayer plateform adaptation layer 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/ll_sys_if.c                                  Source file for initiating the system sequencer 
  - BLE_Thread_SED_FreeRTOS_Generic/STM32_WPAN/Target/power_table.c                                This file contains supported power tables 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/Debug_GPIO/app_debug.c                           Real Time Debug module application side APIs 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Config/LowPower/user_low_power_config.c                 Low power related user configuration
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw_aes.c                                     This file contains the AES driver for STM32WBA 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw_otp.c                                     This file contains the OTP driver
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw_pka.c                                     This file contains the PKA driver for STM32WBA 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/hw_rng.c                                     This file contains the RNG driver for STM32WBA 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/pka_p256.c                                   This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/stm32_lpm_if.c                               Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/timer_if.c                                   Configure RTC Alarm, Tick and Calendar manager 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Interfaces/usart_if.c                                   Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/adc_ctrl.c                                      Header for ADC client manager module 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/app_sys.c                                       Application system for STM32WPAN Middleware
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/ble_timer.c                                     This module implements the timer core functions 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/otp.c                                           Source file for One Time Programmable (OTP) area 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/scm.c                                           Functions for the System Clock Manager
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/stm_list.c                                      TCircular Linked List Implementation
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/baes/baes_cmac.c                                This file contains the AES CMAC implementation
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/baes/baes_ecb.c                                 This file contains the AES ECB functions implementation
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/flash_driver.c                            The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/flash_manager.c                           The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/rf_timing_synchro.c                       The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Flash/simple_nvm_arbiter.c                      The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/MemoryManager/advanced_memory_manager.c         Memory Manager 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/MemoryManager/stm32_mm.c                        Memory Manager 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/Nvm/nvm_emul.c                                  This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RFControl/rf_antenna_switch.c                   RF related module to handle dedictated GPIOs for antenna switch
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/RTDebug.c                               Real Time Debug module API definition 
  - BLE_Thread_SED_FreeRTOS_Generic/System/Modules/RTDebug/RTDebug_dtb.c                           Real Time Debug module API definition for DTB usage

### __Hardware and Software environment__

- This example runs on STM32WBA65xx devices.
- Connect the DK Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).
- This example has been tested with an STMicroelectronics STM32WBA65RI-DK board and can be easily 
	tailored to any other supported device and development board.

### __How to use it?__

Minimum requirements for the demo:

- One STM32WBA65xx-DK board flashed with BLE_Thread_SED_FreeRTOS_Generic application using current generated binary.
- Use a second board flashed with application Thread_Coap_Generic. Run this board first so that it becomes the Thread Leader.

In order to make the program work, you must do the following:

- Connect a STM32WBA65xx-DK board to your PC. 
- Open your preferred toolchain.
- Rebuild all files and flash the board with the executable file.

In a Thread network, nodes are split into two forwarding roles: **Router** or **End Device**.    
The Thread **Leader** is a Router that is responsible for managing the set of Routers in a Thread network.    
An End Device (or child) communicates primarily with a **single Router**.    

Our Application uses two devices :

- the Thread_Coap_Generic device will act as a Leader (Router).
- the BLE_Thread_SED_FreeRTOS_Generic device will act as an End Device (mode child). 

After the reset of the Thread_Coap_Generic device, it will be in Leader mode (**Green LED2 ON**).    
Then power on the BLE_Thread_SED_FreeRTOS_Generic device, it will be in Child mode (LEDs are not driven because of Low Power).  

Once attached to the Thread Leader (Thread_Coap_Generic device), the Thread Child (BLE_Thread_SED_FreeRTOS_Generic device) starts sending **COAP command (Non-Confirmable)**
to the Thread Leader every two seconds after an initial joystick down. The Thread Leader will receive COAP commands to toggle its **blue LED1**. 

<pre>
	
  ___________________________                       ___________________________
  |BLE_Thread_SED_FreeRTOS_G|                       |   Thread_Coap_Generic   |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |  after a joystick down  |                       |                         |
  |          every 2sec --> |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         | Resource "light"      |                         |
  |                         | Mode: Multicast       |                         |
  |                         | Type: Non-Confirmable |                         |
  |                         | Code: Put             |                         |
  |                         |                       |                         |
  |                         |                       |                         |
  |                         |                       |                         |  
  ---------------------------                       ---------------------------
  | Role : Child            |                       | Role : Leader           |
  |                         |                       |                         |
  |                			    |                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|

  
</pre> 

During Thread activity, the BLE_Thread_SED_FreeRTOS_Generic also performs advertising and is available to accept incoming connection in BLE. The joystick left triggers the BLE advertising and joystick select stops BLE advertising.    

On the android/ios device, enable the Bluetooth communications, and if not done before:

- Install the ST BLE Toolbox application on the android device:
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>



- Then, click on the App icon, ST BLE Toolbox (android/ios device).

  
- After 60s of advertising, the application switch from fast advertising to low power advertising.

### __Traces__

Traces are disabled because of Low Power.

### __Power Consumption__

Power consumption can be measured using the X-DK-LPM01A board solution.

On STM32WBA65xx, the consumption should be:

- Standby mode: 5 uA
