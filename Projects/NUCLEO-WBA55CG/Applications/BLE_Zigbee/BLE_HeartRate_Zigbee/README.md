## __BLE_HeartRate_Zigbee Application Description__

How to use the Concurrency mode BLE/Zigbee with BLE Heart Rate profile and Zigbee OnOff Toggle transmission.

This application demonstrates the BLE/Zigbee concurrency mode. The device is configured to attach to a Zigbee_OnOff_Server_Coord 
and then sends OnOff Toggle every one second. This device is also configured in BLE with Heart Rate profile, starts advertising
and is available to accept incoming connection in BLE.

### __Keywords__

Connectivity, Concurrency, BLE, BLE protocol, Zigbee, OnOff cluster, STM32WBA

### __Directory contents__

  - BLE_HeartRate_Zigbee/Core/Inc/app_common.h                               			App Common application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Zigbee/Core/Inc/app_conf.h                                   			Application configuration file for STM32WPAN Middleware
  - BLE_HeartRate_Zigbee/Core/Inc/app_entry.h                         					Interface to the application 
  - BLE_HeartRate_Zigbee/Core/Inc/main.h                                             	Header for main.c file. This file contains the common defines of the application
  - BLE_HeartRate_Zigbee/Core/Inc/stm32wbaxx_hal_conf.h                              	HAL configuration file
  - BLE_HeartRate_Zigbee/Core/Inc/stm32wbaxx_it.h                                    	This file contains the headers of the interrupt handlers
  - BLE_HeartRate_Zigbee/Core/Inc/stm32wbaxx_nucleo_conf.h                           	STM32WBAXX nucleo board configuration file. This file should be copied to the application folder and renamed to stm32wbaxx_nucleo_conf.h
  - BLE_HeartRate_Zigbee/Core/Inc/utilities_conf.h                                   	Header for configuration file for STM32 Utilities
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_bsp.h                                    	Header for bsp application
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_ble.h                                    	Header for ble application 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_zigbee.h                                    Header for zigbee application 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_zigbee_endpoint.h                           Header for zigbee endpoint application   
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/ble_conf.h                                      Configuration file for BLE Middleware
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/ble_dbg_conf.h                               	Debug configuration file for BLE Middleware
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/dis.h                                           Header for service2.c 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/dis_app.h                                       Header for service2_app.c 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/hrs.h                                           Header for service1.c 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/hrs_app.h                                       Header for service1_app.c 
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/bpka.h                                       This file contains the interface of the BLE PKA module
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/host_stack_if.h                              This file contains the interface for the stack tasks 
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/ll_sys_if.h                                  Header file for ll_sys_if.c
  - BLE_HeartRate_Zigbee/System/Config/Debug_GPIO/app_debug.h                           Real Time Debug module application APIs and signal table 
  - BLE_HeartRate_Zigbee/System/Config/Debug_GPIO/app_debug_signal_def.h                Real Time Debug module application signal definition 
  - BLE_HeartRate_Zigbee/System/Config/Debug_GPIO/debug_config.h                        Real Time Debug module general configuration file 
  - BLE_HeartRate_Zigbee/System/Config/Flash/simple_nvm_arbiter_conf.h                  Configuration header for simple_nvm_arbiter.c module 
  - BLE_HeartRate_Zigbee/System/Config/LowPower/app_sys.h                               Header for app_sys.c 
  - BLE_HeartRate_Zigbee/System/Config/LowPower/user_low_power_config.h                 Header for user_low_power_config.c
  - BLE_HeartRate_Zigbee/System/Interfaces/stm32_lpm_if.h                               Header for stm32_lpm_if.c module (device specific LP management) 
  - BLE_HeartRate_Zigbee/Core/Src/app_entry.c                                           Entry point of the application 
  - BLE_HeartRate_Zigbee/Core/Src/main.c                                                Main program body 
  - BLE_HeartRate_Zigbee/Core/Src/stm32wbaxx_hal_msp.c                                  This file provides code for the MSP Initialization and de-Initialization codes
  - BLE_HeartRate_Zigbee/Core/Src/stm32wbaxx_it.c                                       Interrupt Service Routines
  - BLE_HeartRate_Zigbee/Core/Src/system_stm32wbaxx.c                                   CMSIS Cortex-M33 Device Peripheral Access Layer System Source File 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_bsp.c                                       Application to manage BSP
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_ble.c                                       BLE Application
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_zigbee.c                                    Zigbee Application
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/app_zigbee_endpoint.c                           Zigbee EndPoint Application  
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/dis.c                                           service2 definition 
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/dis_app.c                                       service2_app application definition
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/hrs.c                                           service1 definition
  - BLE_HeartRate_Zigbee/STM32_WPAN/App/hrs_app.c                                       service1_app application definition
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/bleplat.c                                    This file implements the platform functions for BLE stack library
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/bpka.c                                       This file implements the BLE PKA module
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/host_stack_if.c                              Source file for the stack tasks 
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/linklayer_plat.c                             Source file for the linklayer plateform adaptation layer 
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/ll_sys_if.c                                  Source file for initiating the system sequencer 
  - BLE_HeartRate_Zigbee/STM32_WPAN/Target/power_table.c                                This file contains supported power tables 
  - BLE_HeartRate_Zigbee/System/Config/Debug_GPIO/app_debug.c                           Real Time Debug module application side APIs 
  - BLE_HeartRate_Zigbee/System/Config/LowPower/user_low_power_config.c                 Low power related user configuration
  - BLE_HeartRate_Zigbee/System/Interfaces/stm32_lpm_if.c                               Low layer function to enter/exit low power modes (stop, sleep) 
  - BLE_HeartRate_Zigbee/System/Modules/ble_timer.c                                     This module implements the timer core functions 
### __Hardware and Software environment__

- This example runs on STM32WBA55xx devices.
- Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK).
- This example has been tested with an STMicroelectronics STM32WBA55CG-Nucleo board and can be easily 
	tailored to any other supported device and development board.

### __Hardware Requirements__

For this application it is requested to have at least:

* One STM32WBA55xx Nucleo board loaded with application **Zigbee_OnOff_Server_Coord**
* One STM32WBA55xx board loaded with application **BLE_HeartRate_Zigbee**
</br>

<pre>
	
			BLE_HeartRate_Zigbee							Zigbee_OnOff_Server_Coord                                                     
               +--------+                                      +--------+
               |        |                                      |        |
               | OnOff  |                                      | OnOff  |        
               | Client |                                      | Server | 
               |        |                                      |        |
 PushB SW3     |        |                                      |        |
 for Every 1   |        |                                      |        |
 Sec           |        |                                      |        |
 or            |        |                                      |        |
 PushB SW1   =>|        |    ZbZclOnOffClientToggleReq         |        |
 for once      |        | -----------------------------------> |        | => RED LED Toggle
               |        |                                      |        |			   
               +--------+                                      +--------+
				Router											Coord.
				
</pre> 

### __How to use it?__

In order to make the program work, you must do the following:

* First, open the projects, build them and load your generated applications on your STM32WBA55xx devices.
* To run the application :
	1. Start the first board. It must be the coordinator of the Zigbee network so in this demo application it is the device running Zigbee_OnOff_Server_Coord application.  

	2. Wait for the Blue LED ON.  

	3. Start the second board, flashed with BLE_HeartRate_Zigbee application. This board is configured as Zigbee router and will be attached to the network created by the coordinator.
At this stage, Blue LED blinks indicating that the Zigbee network is being created. This usually takes about 15 seconds. It is important to wait until Blue LED becomes ON to start pushing buttons.    

	4. It is now possible to send OnOff Cluster commands from the Client to the Server in multicast mode. It is done automatically every one second by pressing on the SW3 push button. 
You must see the Red LED toggling on the Server side.

During Zigbee activity, the BLE_HeartRate_Zigbee device is also configured in BLE with Heart Rate profile, performs advertising and is available to accept incoming connection in BLE.    

On the android/ios device, enable the Bluetooth communications, and if not done before:

- Install the ST BLE Toolbox application on the android device:
	- <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

- You can also install the ST BLE Sensor application on the android/ios device:
	- <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	- <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>

- Then, click on the App icon, ST BLE Toolbox (android/ios device),
   You can either open ST BLE Sensor application (android/ios device).

- In the Heart Rate interface, HearRate and energy measurement are launched and displayed in graphs,
  you can reset the energy measurement.
- After 60s of advertising, the application switch from fast advertising to low power advertising.

### __Traces__

* To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).  

* The UART must be configured as follows:  
<br>
BaudRate       = 115200 baud</br>
Word Length    = 8 Bits</br>
Stop Bit       = 1 bit</br>
Parity         = none</br>
Flow control   = none</br>
Terminal   "Go to the Line" : &lt;LF&gt;  
