## __BLE_HeartRate_Thread_FreeRTOS Application Description__

How to use the Concurrency mode BLE/Thread with BLE Heart Rate profile and COAP messages transmission over Thread.

This application demonstrates the BLE/Thread concurrency mode. The device is configured to attach to a Thread Leader 
and then sends COAP message every one second. This device is also configured in BLE with Heart Rate profile, starts 
advertising and is available to accept incoming connection in BLE.

### __Keywords__

Connectivity, Concurrency, BLE protocol, Thread, COAP, STM32WBA, FreeRTOS

### __Directory contents__

  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_common.h                                          App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_conf.h                                            Application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_entry.h                                           Interface to the application 
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/main.h                                                Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wbaxx_hal_conf.h                                 HAL configuration file
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wbaxx_it.h                                       This file contains the headers of the interrupt handlers
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wbaxx_nucleo_conf.h                              STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/utilities_conf.h                                      Header for configuration file for STM32 Utilities
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_ble.h                                       Header for ble application 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_thread.h                                    Header for thread application 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/ble_conf.h                                      Configuration file for BLE Middleware
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/ble_dbg_conf.h                                  Debug configuration file for BLE Middleware
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis.h                                           Header for service2.c 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis_app.h                                       Header for service2_app.c 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs.h                                           Header for service1.c 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs_app.h                                       Header for service1_app.c 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bpka.h                                       This file contains the interface of the BLE PKA module
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/host_stack_if.h                              This file contains the interface for the stack tasks 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/ll_sys_if.h                                  Header file for ll_sys_if.c
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug.h                           Real Time Debug module application APIs and signal table 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug_signal_def.h                Real Time Debug module application signal definition 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/debug_config.h                        Real Time Debug module general configuration file 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.h                  Configuration header for simple_nvm_arbiter.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/app_sys.h                               Header for app_sys.c 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/user_low_power_config.h                 Header for user_low_power_config.c
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw.h                                         This file contains the interface of STM32 HW drivers
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw_if.h                                      Hardware Interface 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/stm32_lpm_if.h                               Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/timer_if.h                                   configuration of the timer_if.c instances 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/usart_if.h                                   Header file for stm32_adv_trace interface file 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/adc_ctrl.h                                      Header for ADC client manager module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/ble_timer.h                                     This header defines the timer functions used by the BLE stack 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/dbg_trace.h                                     Header for dbg_trace.c 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/otp.h                                           Header file for One Time Programmable (OTP) area 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/scm.h                                           Header for scm.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/stm_list.h                                      Header file for linked list library
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/utilities_common.h                              Common file to utilities 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/baes/baes.h                                     This file contains the interface of the basic AES software module
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/baes/baes_global.h                              This file contains the internal definitions of the AES software module
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/flash_driver.h                            Header for flash_driver.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/flash_manager.h                           Header for flash_manager.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/rf_timing_synchro.h                       Header for rf_timing_synchro.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/simple_nvm_arbiter.h                      Header for simple_nvm_arbiter.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/simple_nvm_arbiter_common.h               Common header of simple_nvm_arbiter.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/MemoryManager/advanced_memory_manager.h         Header for advance_memory_manager.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/MemoryManager/stm32_mm.h                        Header for stm32_mm.c module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Nvm/nvm.h                                       This file contains the interface of the NVM manager
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RFControl/rf_antenna_switch.h                   RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/debug_signals.h                         Real Time Debug module System and Link Layer signal definition 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/local_debug_tables.h                    Real Time Debug module System and Link Layer signal 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/RTDebug.h                               Real Time Debug module API declaration 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/RTDebug_dtb.h                           Real Time Debug module API declaration for DTB usage
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/app_entry.c                                           Entry point of the application 
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/main.c                                                Main program body 
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/stm32wbaxx_hal_msp.c                                  This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/stm32wbaxx_it.c                                       Interrupt Service Routines
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/system_stm32wbaxx.c                                   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_ble.c                                       BLE Application
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_thread.c                                    Thread Application 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis.c                                           service2 definition 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis_app.c                                       service2_app application definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs.c                                           service1 definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs_app.c                                       service1_app application definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bleplat.c                                    This file implements the platform functions for BLE stack library
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bpka.c                                       This file implements the BLE PKA module
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/host_stack_if.c                              Source file for the stack tasks 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/linklayer_plat.c                             Source file for the linklayer plateform adaptation layer 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/ll_sys_if.c                                  Source file for initiating the system sequencer 
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/power_table.c                                This file contains supported power tables 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug.c                           Real Time Debug module application side APIs 
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/user_low_power_config.c                 Low power related user configuration
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw_aes.c                                     This file contains the AES driver for STM32WBA 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw_otp.c                                     This file contains the OTP driver
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw_pka.c                                     This file contains the PKA driver for STM32WBA 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/hw_rng.c                                     This file contains the RNG driver for STM32WBA 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/pka_p256.c                                   This file is an optional part of the PKA driver for STM32WBA. It is dedicated to the P256 elliptic curve
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/stm32_lpm_if.c                               Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/timer_if.c                                   Configure RTC Alarm, Tick and Calendar manager 
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/usart_if.c                                   Source file for interfacing the stm32_adv_trace to hardware 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/adc_ctrl.c                                      Header for ADC client manager module 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/app_sys.c                                       Application system for STM32WPAN Middleware
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/ble_timer.c                                     This module implements the timer core functions 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/otp.c                                           Source file for One Time Programmable (OTP) area 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/scm.c                                           Functions for the System Clock Manager
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/stm_list.c                                      TCircular Linked List Implementation
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/baes/baes_cmac.c                                This file contains the AES CMAC implementation
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/baes/baes_ecb.c                                 This file contains the AES ECB functions implementation
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/flash_driver.c                            The Flash Driver module is the interface layer between Flash management modules and HAL Flash drivers
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/flash_manager.c                           The Flash Manager module provides an interface to write raw data from SRAM to FLASH
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/rf_timing_synchro.c                       The RF Timing Synchronization module provides an interface to synchronize the flash processing versus the RF activity to make sure the RF timing is not broken
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Flash/simple_nvm_arbiter.c                      The Simple NVM arbiter module provides an interface to write and/or restore data from SRAM to FLASH with use of NVMs
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/MemoryManager/advanced_memory_manager.c         Memory Manager 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/MemoryManager/stm32_mm.c                        Memory Manager 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/Nvm/nvm_emul.c                                  This file implements the RAM version of the NVM manager for STM32WBX. It is made for test purpose
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RFControl/rf_antenna_switch.c                   RF related module to handle dedictated GPIOs for antenna switch
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/RTDebug.c                               Real Time Debug module API definition 
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/RTDebug/RTDebug_dtb.c                           Real Time Debug module API definition for DTB usage

### __Hardware and Software environment__

- This example runs on STM32WBA65xx devices.
- Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK).
- This example has been tested with an STMicroelectronics STM32WBA65RI-Nucleo board and can be easily 
	tailored to any other supported device and development board.

### __How to use it?__

Minimum requirements for the demo:

- One STM32WBA65xx-Nucleo board flashed with BLE_HeartRate_Thread_FreeRTOS application using current generated binary.
- Use a second board flashed with application Thread_Coap_Generic. Run this board first so that it becomes the Thread Leader.

In order to make the program work, you must do the following:

- Connect a STM32WBA65xx-Nucleo board to your PC. 
- Open your preferred toolchain.
- Rebuild all files and flash the board with the executable file.

In a Thread network, nodes are split into two forwarding roles: **Router** or **End Device**.    
The Thread **Leader** is a Router that is responsible for managing the set of Routers in a Thread network.    
An End Device (or child) communicates primarily with a **single Router**.    

Our Application uses two devices :

- the Thread_Coap_Generic device will act as a Leader (Router).
- the BLE_HeartRate_Thread_FreeRTOS device will act as an End Device (mode child). 

After the reset of the Thread_Coap_Generic device, it will be in Leader mode (**Green LED2 ON**).    
Then power on the BLE_HeartRate_Thread_FreeRTOS device, it will be in Child mode (**Red LED3 ON**).
After a while, the roles being not fixed with thread, the roles can be switching. The Thread_Coap_Generic device might switch to Router mode (**Red LED3 ON**).
And at the same time, the BLE_HeartRate_Thread_FreeRTOS device, might switch to the Leader mode (**Green LED2 ON**). 
In case of switching role, the periodic toggling every one second will be stopped until the SW1 is resuming it.

Once attached to the Thread Leader (Thread_Coap_Generic device), the Thread Child (BLE_HeartRate_Thread device) starts sending COAP command (Non-Confirmable) to the Thread Leader every one second. The Thread Leader will receive COAP commands to toggle its blue LED1 unless stoppedwith button SW2.

press the SW1 Push-Button on board A to restart (if stopped) the periodic 1 sec sending of a COAP command (Non-Confirmable) from BLE_HeartRate_Thread device to Thread_Coap_Generic device.
The Thread_Coap_Generic device will receive COAP commands to toggle its blue LED1.
press the SW2 Push-Button on boad A to stop the periodic (1 second) sending of Coap messages and send once a COAP command (Confirmable) from BLE_HeartRate_Thread device to Thread_Coap_Generic device.
The Thread_Coap_Generic device will receive COAP commands and send to BLE_HeartRate_Thread_FreeRTOS device a Coap Data response and toggle its blue LED1 once.

<pre>
	
                   __________________________________                       __________________________________
                   |  BLE_HeartRate_Thread_FreeRTOS |                       | Thread_Coap_Generic            |
                   |________________________________|                       |________________________________|
                   | USART1                         |                       | USART1                         |
                   |  every 1 sec (after reset) --> |======>  COAP ======>  | BLUE LED TOGGLE (ON/OFF)       |
                   |  or restarting (if stopped)    | Resource :light       |  every 1 sec                   |
                   |     with SW1                   | Mode: Multicast       |                                |
                   |                                | Type: Non-Confirmable |                                |
                   |                                | Code: Put             |                                |
                   |                                |                       |                                |
                   |                                |                       |                                |
       PushB SW2=> | SW2 stop the periodic toggling |=====>  COAP =========>|----------                      |
                   | every 1 sec                    | Resource light        |         |                      |
                   | SW1 re-start the periodic      | Mode: Multicast       |  CoapRequestHandler()          |
                   | toggling every 1 sec           | Type: Confirmable     |         |                      |
                   |                                | Code: Put             |         v                      |
                   |                                |                       |  CoapSendDataResponse()        |
                   |                                |                       |         |                      |
                   |                                |                       |         v                      |
                   |    CoapDataRespHandler() <--   |<===== COAP ========== | -------                        |
                   |                                |                       | BLUE LED TOGGLE ONCE (ON/OFF)  | 
                   |                                |                       |                                |
       PushB SW1=> |     restarting every 1 sec --> |======>  COAP ======>  | BLUE LED TOGGLE (ON/OFF)       |
                   |       with SW1 (if stopped)    | Resource :light       |  every 1 sec                   |
                   |                                | Mode: Multicast       |                                |
                   |                                | Type: Non-Confirmable |                                |
                   |                                | Code: Put             |                                |
                   |                                |                       |                                |
                   |--------------------------------|                       |--------------------------------|
                   | Role : Child                   |                       | Role : Leader                  |
                   |                                |                       |                                |
                   | LED : Red                      |                       | LED : Green                    |
                   |--------------------------------|                       |--------------------------------|
                   | ROLE re-negociation phase      | Details not described | ROLE negociation phase         |
                   |            every 1sec -->      |==>  NOTHING!          |                                |  
                   |--------------------------------|                       |--------------------------------|
                   | Role : Leader                  |                       | Role : Router                  |
                   |                                |                       |                                |
                   | LED : Green                    |                       | LED : Red                      |
                   |--------------------------------|                       |--------------------------------|
                   |            every 1sec -->      |==>  NOTHING!          |                                | 
                   |                                |                       |                                |
                   |--------------------------------|                       |--------------------------------|
       PushB SW1=> |     restarting every 1 sec --> |======>  COAP ======>  | BLUE LED TOGGLE (ON/OFF)       |
                   |       with SW1 (if stopped)    | Resource :light       |  every 1 sec                   |
                   |                                | Mode: Multicast       |                                |
                   |                                | Type: Non-Confirmable |                                |
                   |                                | Code: Put             |                                |
                   |                                |                       |                                |
       PushB SW2=> | SW2 stop the periodic toggling |=====>  COAP =========>|----------                      |
                   | every 1 sec                    | Resource light        |         |                      |
                   | SW1 re-start the periodic      | Mode: Multicast       |  CoapRequestHandler()          |
                   | toggling every 1 sec           | Type: Confirmable     |         |                      |
                   |                                | Code: Put             |         v                      |
                   |                                |                       |  CoapSendDataResponse()        |
                   |                                |                       |         |                      |
                   |                                |                       |         v                      |
                   |    CoapDataRespHandler() <--   |<===== COAP ========== | -------                        |
                   |                                |                       | BLUE LED TOGGLE ONCE (ON/OFF)  | 
                   |                                |                       |                                |  
                   |--------------------------------|                       |--------------------------------|
                   | Role : Leader                  |                       | Role : Router                  |
                   |                                |                       |                                |
                   | LED : Green                    |                       | LED : Red                      |
                   |________________________________|                       |________________________________|
                   |                                |                       |                                |
                   |       _________________________|                       |       _________________________|
                   |      |COM2                     |                       |      |LPUART1                    |
                   |      |                         |                       |      |                         |
                   |      |     Thread stack control|                       |      |     Thread stack control|
                   |      |       via Cli commands  |                       |      |       via Cli commands  |
                   |      |                         |                       |      |                         |
                   |      |                         |                       |      |                Tx CN3-32|
                   |      |                         |                       |      |                Rx CN3-34|
                   |      |_________________________|                       |      |_________________________|
                   |                                |                       |                                |
                   |________________________________|                       |________________________________|

  
</pre> 

During Thread activity, the BLE_HeartRate_Thread_FreeRTOS device is also configured in BLE with Heart Rate profile, performs advertising and is available
to accept incoming connection in BLE.    

On the android/ios device, enable the Bluetooth communications, and if not done before:

- Install the ST BLE Toolbox application on the android device:
    - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>


- Then, click on the App icon, ST BLE Toolbox (android/ios device).

- In the Heart Rate interface, HearRate and energy measurement are launched and displayed in graphs,
  you can reset the energy measurement.
  
- After 60s of advertising, the application switch from fast advertising to low power advertising.

### __Traces__

* To get the traces you need to connect your board to the Hyperterminal (through the STLink Virtual COM Port).

* The UART must be configured as follows:<br>
  - BaudRate       = 115200 baud</br>
  - Word Length    = 8 Bits</br>
  - Stop Bit       = 1 bit</br>
  - Parity         = none</br>
  - Flow control   = none</br>
  - Terminal   "Go to the Line" :    <LF>   

* It is also possible to control and configure the Thread stack through Cli commands. For that, connect the COM2 of your board to an Hyperterminal through STLink cable.<br>
The Serial interface must be configured as follows:<br>
  - BaudRate       = 115200 baud</br>
  - Word Length    = 8 Bits</br>
  - Stop Bit       = 1 bit</br>
  - Parity         = none</br>
  - Flow control   = none</br>
  - Terminal   "Go to the Line" :    <LF>   <br>

  The command 'help' can be used to display the list of all available cli commands.<br>

