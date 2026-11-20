## __BLE_HeartRate_Thread_SED_FreeRTOS Application Description__

How to use the Concurrency mode BLE/Thread with BLE Heart Rate profile and COAP messages transmission over Thread for a Sleepy End Device.

This application demonstrates the BLE/Thread(SED) concurrency mode. The device is configured to attach to a Thread Leader 
and then sends COAP message every two seconds. This device is also configured in BLE with Heart Rate profile, starts 
advertising and is available to accept incoming connection in BLE. This application demonstrates the Low Power, in Standby mode.

### __Keywords__

Connectivity, Concurrency, BLE, BLE protocol, Thread, COAP, STM32WBA, low power, Standby mode

### __Directory contents__

  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/app_common.h                            App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/app_conf.h                              Application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/app_entry.h                             Interface to the application
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/app_freertos.h                          Header for app_freertos.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/FreeRTOSConfig.h                        FreeRTOS configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/main.h                                  Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/stm32wba65i_discovery_conf.h            STM32WBA65I-DK1 board configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/stm32wbaxx_hal_conf.h                   HAL configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/stm32wbaxx_it.h                         This file contains the headers of the interrupt handlers
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/stm32_rtos.h                            Include file for all RTOS/Sequencer can be used on WBA
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Inc/utilities_conf.h                        Header for configuration file for STM32 Utilities
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/app_ble.h                         Header for ble application
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/app_thread.h                      Header for thread application
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/ble_conf.h                        Configuration file for BLE Middleware
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/ble_dbg_conf.h                    Debug configuration file for BLE Middleware
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/data_transfer.h                   Definition of the data to be transferred
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/dis.h                             Header for service2.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/dis_app.h                         Header for service2_app.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/hrs.h                             Header for service1.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/hrs_app.h                         Header for service1_app.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/host_stack_if.h                This file contains the interface for the stack tasks
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/ll_sys_if.h                    Header file for ll_sys_if.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/ADC_Ctrl/adc_ctrl_conf.h           Configuration header for adc_ctrl.c module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/common_wpan_conf.h                 Configuration file for STM32 WPAN Common Utilities
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/CRC_Ctrl/crc_ctrl_conf.h           Configuration header for crc_ctrl.c module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Debug_GPIO/app_debug.h             Real Time Debug module application APIs and signal table
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Debug_GPIO/app_debug_signal_def.h  Real Time Debug module application signal definition
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Debug_GPIO/debug_config.h          Real Time Debug module general configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.h    Configuration header for simple_nvm_arbiter.c module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Log/log_module_conf.h              Configuration header for the log module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/LowPower/peripheral_init.h         Header for peripheral init module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/LowPower/user_low_power_config.h   Header for user_low_power_config.c
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Interfaces/stm32_lpm_if.h                 Header for stm32_lpm_if.c module (device specific LP management)
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Modules/ble_timer.h                       This header defines the timer functions used by the BLE stack
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/app_entry.c                             Entry point of the application
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/app_freertos.c                          FreeRTOS tasks, queues and timers of the application
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/main.c                                  Main program body
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/stm32wba65i_discovery.c                 Board support functions of the STM32WBA65I-DK1 (LEDs, joystick, COM port)
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/stm32wbaxx_hal_msp.c                    This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/stm32wbaxx_it.c                         Interrupt Service Routines
  - BLE_HeartRate_Thread_SED_FreeRTOS/Core/Src/system_stm32wbaxx.c                     CMSIS Cortex-M33 Device Peripheral Access Layer System Source File
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/app_ble.c                         BLE Application
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/app_thread.c                      Thread Application
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/dis.c                             service2 definition
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/dis_app.c                         service2_app application definition
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/hrs.c                             service1 definition
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/App/hrs_app.c                         service1_app application definition
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/bleplat.c                      This file implements the platform functions for BLE stack library
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/host_stack_if.c                Source file for the stack tasks
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/linklayer_plat.c               Source file for the linklayer platform adaptation layer
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/ll_sys_if.c                    Source file for initiating the system sequencer
  - BLE_HeartRate_Thread_SED_FreeRTOS/STM32_WPAN/Target/power_table.c                  This file contains supported power tables
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/ADC_Ctrl/adc_ctrl_conf.c           Source for ADC client controller module configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/CRC_Ctrl/crc_ctrl_conf.c           Source for CRC client controller module configuration file
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Debug_GPIO/app_debug.c             Real Time Debug module application side APIs
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/Flash/simple_nvm_arbiter_conf.c    Configuration source for simple_nvm_arbiter.c module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/LowPower/peripheral_init.c         Peripheral init module
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Config/LowPower/user_low_power_config.c   Low power related user configuration
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Interfaces/stm32_lpm_if.c                 Low layer function to enter/exit low power modes (stop, sleep)
  - BLE_HeartRate_Thread_SED_FreeRTOS/System/Modules/ble_timer.c                       This module implements the timer core functions

The other modules used by the application (Link Layer, System Clock Manager, Flash manager, Memory manager, RTDebug, BasicAES, ...) are shared between the projects and are located in `Projects/Common/WPAN`.

### __Hardware and Software environment__

- This example runs on STM32WBA65xx devices.
- Connect the DK Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).
- This example has been tested with an STMicroelectronics STM32WBA65RI-DK board and can be easily 
	tailored to any other supported device and development board.

### __How to use it?__

Minimum requirements for the demo:

- One STM32WBA65xx-DK board flashed with BLE_HeartRate_Thread_SED application using current generated binary.
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
- the BLE_HeartRate_Thread_SED device will act as an End Device (mode child). 

After the reset of the Thread_Coap_Generic device, it will be in Leader mode (**Green LED2 ON**).    
Then power on the BLE_HeartRate_Thread_SED device, it will be in Child mode (LEDs are not driven because of Low Power).  

Once attached to the Thread Leader (Thread_Coap_Generic device), the Thread Child (BLE_HeartRate_Thread_SED device) starts sending **COAP command (Non-Confirmable)**
to the Thread Leader every two seconds. The Thread Leader will receive COAP commands to toggle its **blue LED1**. 
The periodic sending starts when the device becomes Child and is stopped when it is detached from the Thread network.
It restarts automatically once the device is a Child again.

On top of this periodic toggling, Joystick-down sends one **COAP command (Non-Confirmable)** to the Thread Leader.
Joystick-left sends one **COAP command (Confirmable)** to the Thread Leader.

These additional commands are easier to observe with the log terminal of the Thread_Coap_Generic device.

<pre>
	
  ___________________________                       ___________________________
  |BLE_HeartRate_Thread_SED |                       | Thread_Coap_Generic     |
  |_________________________|                       |_________________________|  
  |                         |                       |                         |
  |                   		|                       |                         |
  |          every 2sec --> |======> COAP =========>| BLUE LED TOGGLE (ON/OFF)|
  |                         | Resource "light"      |                         |
  |                         | Mode: Multicast       |                         |
  |                         | Type: Non-Confirmable |                         |
  |                         | Code: Put             |                         |
  |                         |                       |                         |
  |   Joystick down -->     |======> COAP =========>|                         |
  |   (one shot)            | Type: Non-Confirmable |                         |
  |                         |                       |                         |
  |   Joystick left -->     |======> COAP =========>|                         |
  |   (one shot)            | Type: Confirmable     |                         |
  |                         |                       |                         |  
  ---------------------------                       ---------------------------
  | Role : Child            |                       | Role : Leader           |
  |                         |                       |                         |
  |                			|                       | LED : Green             |
  |                         |                       |                         |
  |_________________________|                       |_________________________|

  
</pre> 

During Thread activity, the BLE_HeartRate_Thread_SED device is also configured in BLE with Heart Rate profile, performs advertising and is available
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

Traces are disabled because of Low Power.

### __Power Consumption__

Power consumption can be measured using the X-DK-LPM01A board solution.

On STM32WBA65xx, the consumption should be:

- Standby mode: 5 uA
