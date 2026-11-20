## __BLE_HeartRate_Thread_FreeRTOS Application Description__

How to use the Concurrency mode BLE/Thread with BLE Heart Rate profile and COAP messages transmission over Thread.

This application demonstrates the BLE/Thread concurrency mode. The device is configured to attach to a Thread Leader 
and then sends COAP message every one second. This device is also configured in BLE with Heart Rate profile, starts 
advertising and is available to accept incoming connection in BLE.

### __Keywords__

Connectivity, Concurrency, BLE protocol, Thread, COAP, STM32WBA, FreeRTOS

### __Directory contents__

  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_common.h                            App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_conf.h                              Application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_entry.h                             Interface to the application
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/app_freertos.h                          Header for app_freertos.c
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/FreeRTOSConfig.h                        FreeRTOS configuration file
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/main.h                                  Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wba65i_discovery_conf.h            STM32WBA65I-DK1 board configuration file
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wbaxx_hal_conf.h                   HAL configuration file
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32wbaxx_it.h                         This file contains the headers of the interrupt handlers
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/stm32_rtos.h                            Include file for all RTOS/Sequencer can be used on WBA
  - BLE_HeartRate_Thread_FreeRTOS/Core/Inc/utilities_conf.h                        Header for configuration file for STM32 Utilities
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_ble.h                         Header for ble application
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_thread.h                      Header for thread application
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/ble_conf.h                        Configuration file for BLE Middleware
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/ble_dbg_conf.h                    Debug configuration file for BLE Middleware
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/data_transfer.h                   Definition of the data to be transferred
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis.h                             Header for service2.c
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis_app.h                         Header for service2_app.c
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs.h                             Header for service1.c
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs_app.h                         Header for service1_app.c
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bpka.h                         This file contains the interface of the BLE PKA module
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/host_stack_if.h                This file contains the interface for the stack tasks
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/ll_sys_if.h                    Header file for ll_sys_if.c
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/ADC_Ctrl/adc_ctrl_conf.h           Configuration header for adc_ctrl.c module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/common_wpan_conf.h                 Configuration file for STM32 WPAN Common Utilities
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/CRC_Ctrl/crc_ctrl_conf.h           Configuration header for crc_ctrl.c module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug.h             Real Time Debug module application APIs and signal table
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug_signal_def.h  Real Time Debug module application signal definition
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/debug_config.h          Real Time Debug module general configuration file
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.h    Configuration header for simple_nvm_arbiter.c module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Log/log_module_conf.h              Configuration header for the log module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/peripheral_init.h         Header for peripheral init module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/user_low_power_config.h   Header for user_low_power_config.c
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/stm32_lpm_if.h                 Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/ble_timer.h                       This header defines the timer functions used by the BLE stack
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/app_entry.c                             Entry point of the application
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/app_freertos.c                          FreeRTOS tasks, queues and timers of the application
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/main.c                                  Main program body
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/stm32wbaxx_hal_msp.c                    This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/stm32wbaxx_it.c                         Interrupt Service Routines
  - BLE_HeartRate_Thread_FreeRTOS/Core/Src/system_stm32wbaxx.c                     CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_ble.c                         BLE Application
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/app_thread.c                      Thread Application
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis.c                             service2 definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/dis_app.c                         service2_app application definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs.c                             service1 definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/App/hrs_app.c                         service1_app application definition
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bleplat.c                      This file implements the platform functions for BLE stack library
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/bpka.c                         This file implements the BLE PKA module
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/host_stack_if.c                Source file for the stack tasks
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/linklayer_plat.c               Source file for the linklayer platform adaptation layer
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/ll_sys_if.c                    Source file for initiating the system sequencer
  - BLE_HeartRate_Thread_FreeRTOS/STM32_WPAN/Target/power_table.c                  This file contains supported power tables
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/ADC_Ctrl/adc_ctrl_conf.c           Source for ADC client controller module configuration file
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/CRC_Ctrl/crc_ctrl_conf.c           Source for CRC client controller module configuration file
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Debug_GPIO/app_debug.c             Real Time Debug module application side APIs
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.c    Configuration source for simple_nvm_arbiter.c module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/peripheral_init.c         Peripheral init module
  - BLE_HeartRate_Thread_FreeRTOS/System/Config/LowPower/user_low_power_config.c   Low power related user configuration
  - BLE_HeartRate_Thread_FreeRTOS/System/Interfaces/stm32_lpm_if.c                 Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_HeartRate_Thread_FreeRTOS/System/Modules/ble_timer.c                       This module implements the timer core functions

The other modules used by the application (Link Layer, System Clock Manager, Flash manager, Memory manager, RTDebug, BasicAES, ...) are shared between the projects and are located in `Projects/Common/WPAN`.

### __Hardware and Software environment__

- This example runs on STM32WBA65xx devices.
- Connect the Discovery Kit to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).
- This example has been tested with an STMicroelectronics STM32WBA65i-DK board and can be easily 
	tailored to any other supported device and development board.

### __How to use it?__

Minimum requirements for the demo:

- One STM32WBA65i-DK board flashed with BLE_HeartRate_Thread_FreeRTOS application using current generated binary.
- Use a NUCLEO-WBA65RI board flashed with application Thread_Coap_Generic (NUCLEO-WBA65RI\Applications\Thread\Thread_Coap_Generic). Run this board first so that it becomes the Thread Leader.

In order to make the program work, you must do the following:

- Connect a STM32WBA65i-DK to your PC. 
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
While the device is detached from the Thread network (e.g. during a role change), the periodic sending is stopped.
It restarts automatically once the device is again Child, Router or Leader.

Once attached to the Thread Leader (Thread_Coap_Generic device), the Thread Child (BLE_HeartRate_Thread_FreeRTOS device) starts sending **COAP command (Non-Confirmable)**
to the Thread Leader every one second. The Thread Leader will receive COAP commands to toggle its **blue LED1**.  
On top of this periodic toggling, Joystick-right sends a **COAP command (Non-Confirmable)** immediately and restarts the periodic sending (every one second) if it was stopped.
Joystick-left sends a **COAP command (Confirmable)** and stops the periodic sending. Press joystick right to resume it.

- Board A  sends a **COAP command (Non-Confirmable)** from BLE_HeartRate_Thread_FreeRTOS device to Thread_Coap_Generic device every 1 sec.  
The Thread_Coap_Generic device will receive COAP commands to toggle its **blue LED1**.

<pre>
	
  _______________________________                       ___________________________
  |BLE_HeartRate_Thread_FreeRTOS|                       | Thread_Coap_Generic     |
  |_____________________________|                       |_________________________|  
  |                             |                       |                         |
  |                             |                       |                         |
  |   every 1sec (after PowerUp)|                       |                         |
  |       or Joystick right-->	|======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         	| Resource "light"      |                         |
  |                             | Mode: Multicast       |                         |
  |          	                | Type: Non-Confirmable |                         |
  |                             | Code: Put             |                         |
  |                             |                       |                         |
  |                             |                       |                         |
  |           Joystick left-->  |=====> COAP ==========>|-------->                |
  |                             | Resource "light"      |         |               |
  |                             | Mode: Multicast       |  CoapRequestHandler()   |
  |                             | Type: Confirmable     |         |               |
  |                             | Code: Put             |         |               |
  |                             |                       |         v               |
  |                             |                       |  CoapSendDataResponse() |
  |                             |                       |         |               |
  |                             |                       |         v               |
  | CoapDataRespHandler()<--    |<===== COAP <==========| <-------                |
  |                             |                       | BLUE LED TOGGLE (ON/OFF)| 
  |-----------------------------|                       |-------------------------|
  | Role : Child                |                       | Role : Leader           |
  |                             |                       |                         |
  | LED : Red                   |                       | LED : Green             |
  |-----------------------------|                       |-------------------------|
  | ROLE re-negotiation phase   | Details not described | ROLE negotiation phase  |
  |  periodic sending stopped   |==>  NOTHING!          |                         | 
  |-----------------------------|                       |-------------------------|
  |                             |                       |                         |
  | Role : Leader               |                       | Role : Router           |
  |                             |                       |                         |
  | LED : Green                 |                       | LED : Red               |
  |-----------------------------|                       |-------------------------|
  |   every 1sec (restarted     |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |   automatically) -->        | Type: Non-Confirmable |                         |
  |                             |                       |                         |
  |-----------------------------|                       |-------------------------|
  |         Joystick right-->	|======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |  (restarts every 1sec if    | Resource "light"      |                         |
  |   stopped by Joystick left) | Mode: Multicast       |                         |
  |          	                | Type: Non-Confirmable |                         |
  |                             | Code: Put             |                         |
  |                             |                       |                         |
  |                             |                       |                         |
  |           Joystick left-->  |=====> COAP ==========>|-------->                |
  |                             | Resource "light"      |         |               |
  |                             | Mode: Multicast       |  CoapRequestHandler()   |
  |                             | Type: Confirmable     |         |               |
  |                             | Code: Put             |         |               |
  |                             |                       |         v               |
  |                             |                       |  CoapSendDataResponse() |
  |                             |                       |         |               |
  |                             |                       |         v               |
  | CoapDataRespHandler()<--    |<===== COAP <==========| <-------                |
  |                             |                       | BLUE LED TOGGLE (ON/OFF)| 
  |-----------------------------|                       |-------------------------|
  | Role : Leader               |                       | Role : Router           |
  |                             |                       |                         |
  | LED : Green                 |                       | LED : Red               |
  |_____________________________|                       |_________________________|

  
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

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : &lt;LF&gt;  
