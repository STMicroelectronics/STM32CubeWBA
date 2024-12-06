## __BLE_HeartRate_Mac_RFD Application Description__

How to use the Concurrency mode BLE/MAC with BLE Heart Rate profile, combined with MAC RFD device.

This application demonstrates the BLE/MAC concurrency mode. The device is configured to act as a MAC 802.15.4 RFD device, 
requesting association and sending data to FFD device. This device is also configured in BLE with Heart Rate profile, 
starts  advertising and is available to accept incoming connection in BLE.

### __Keywords__

Connectivity, Concurrency, BLE, BLE protocol, MAC, 802.15.4, association, data, STM32WBA

### __Directory contents__

   - BLE_HeartRate_Mac_RFD/Core/Inc/app_common.h                         		Header for all modules with common definition
   - BLE_HeartRate_Mac_RFD/Core/Inc/app_conf.h                            		Parameters configuration file of the application
   - BLE_HeartRate_Mac_RFD/Core/Inc/app_entry.h                             	Parameters configuration file of the application
   - BLE_HeartRate_Mac_RFD/Core/Inc/main.h                                  	Header for main.c module
   - BLE_HeartRate_Mac_RFD/Core/Inc/stm32_rtos.h                            	Include file for all RTOS/Sequencer can be used on WBA
   - BLE_HeartRate_Mac_RFD/Core/Inc/stm32wbxx_hal_conf.h                    	STM32 HAL configuration file.
   - BLE_HeartRate_Mac_RFD/Core/Inc/stm32wbxx_it.h                          	Interrupt handlers header file
   - BLE_HeartRate_Mac_RFD/Core/Inc/stm32wbaxx_nucleo_conf.h   					STM32WBAXX nucleo board configuration file.
   - BLE_HeartRate_Mac_RFD/Core/Inc/utilities_conf.h                        	Header for Configuration of utilities
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_ble.h   							Header for ble application
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_mac_802_15_4_process.h        	Header for RFD MAC 802.15.4 processing tasks
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_mac.h                         	Header for RFD MAC 802.15.4 Core Mac Device implementation
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/ble_conf.h                            Configuration file for BLE Middleware
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/ble_dbg_conf.h                        Debug configuration file for BLE Middleware
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/dis.h                                 Header for service2.c 
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/dis_app.h                             Header for service2_app.c 
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/hrs.h                                 Header for service1.c 
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/hrs_app.h                             Header for service1_app.c 
   - BLE_HeartRate_Mac_RFD/Core/Src/app_entry.c                             	Parameters configuration file of the application
   - BLE_HeartRate_Mac_RFD/Core/Src/stm32wbaxx_hal_msp.c                    	Driver configuration
   - BLE_HeartRate_Mac_RFD/Core/Src/hw_uart.c                               	UART Driver
   - BLE_HeartRate_Mac_RFD/Core/Src/main.c                                  	Main program
   - BLE_HeartRate_Mac_RFD/Core/Src/stm32wbxx_it.c                          	exceptions handler and peripherals interrupt service routine
   - BLE_HeartRate_Mac_RFD/Core/Src/system_stm32wbaxx.c                     	CMSIS Cortex Device Peripheral Access Layer implementation
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_mac.c    			            	RFD MAC 802.15.4 processing tasks implementation
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_mac_802_15_4_process.c        	RFD MAC 802.15.4 Core Mac Device implementation
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/app_ble.c                             BLE Application 
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/dis.c                                 service2 definition 
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/dis_app.c                             service2_app application definition
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/hrs.c                                 service1 definition
   - BLE_HeartRate_Mac_RFD/STM32_WPAN/App/hrs_app.c                             service1_app application definition

### __Hardware and Software environment__
 
- This example runs on STM32WBA55xx devices.
- Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
- This example has been tested with an STMicroelectronics STM32WBA55CG-Nucleo board and 
  can be easily tailored to any other supported device and development board.
  
- On STM32WBA55CG-Nucleo, the jumpers must be configured as described
  in this section. Starting from the top left position up to the bottom 
  right position, the jumpers on the Board must be set as follows:

  - CN11:    GND         [OFF]
  - JP4:     VDDRF       [ON]
  - JP6:     VC0         [ON]
  - JP2:     +3V3        [ON] 
  - JP1:     USB_STL     [ON]   All others [OFF]
  - CN12:    GND         [OFF]
  - CN7:     <All>       [OFF]
  - JP3:     VDD_MCU     [ON]
  - JP5:     GND         [OFF]  All others [ON]
  - CN10:    <All>       [OFF]

### __How to use it?__

This application requires two STM32WBA55xx-Nucleo boards. One device will act as 
802.15.4 FFD coordinator ensuring association response and network establishment. 
The other device will act as a MAC 802.15.4 RFD - Node. The Node request 
association on startup and send data to the coordinator. 

Minimum requirements for the demo:

- 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 FFD mode (see BLE_HeartRate_Mac_FFD example)
  (This device will be referred as COORDINATOR )
- 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 RFD mode using current generated binary.
  (This device will be referred as NODE )

In order to make the program work, you must do the following: 

- Connect 2 STM32WBA55xx-Nucleo boards to your PC 
	
    - COORDINATOR :
      - Refers to BLE_HeartRate_Mac_FFD example to build and load generated image
 
    - NODE :
	  - Open your preferred toolchain 
	  - Rebuild all files of BLE_HeartRate_Mac_RFD application and load your image into NODE target memory 
      - The example run on Reset

     
To get the traces in real time, you can connect an HyperTerminal to the STLink Virtual Com Port.
The Serial interface must be configured as follows:

- BaudRate = 115200 baud  
- Word Length = 8 Bits 
- Stop Bit = 1 bit
- Parity = none
- Flow control = none

You can also use an MAC 802.15.4 over the air sniffer on channel 11 to spy MAC 802.15.4 frames exchanged between the devices.

__MAC 802.15.4 DEMO__ 

By default, after a reset, both board starts. Coordinator must be started firstly to be able to answer to the node association request.

This MAC 802.15.4 example is to demonstrate Point-to-Point communication using MAC components between a FFD (ie.: the COORDINATOR) and an RFD (ie.: the NODE). 

Upon COORDINATOR start, the device initializes the MAC 802.15.4 and associates resources in non-beaconned enable mode. 

MAC PIB is then configured as follow :

- PanID : 0x1AAA
- Coordinator Extended Address : 0xACDE480000000001
- Coordinator Short Address    : 0x1122

Once MAC PIB is initialized, FFD is started on channel 11 as an association capable coordinator.
At this stage the COORDINATOR is waiting for an association request. Blue LED of FFD device is switched on once the coordinator is started.

On NODE side, the MAC layer is initialized on startup. RFD issues an association request to the COORDINATOR to retrieve its short address. 

Once the association between two devices is completed, RFD's blue LED is switched on to indicate the node is associated to the coordinator.
In the meantime, blue LED of FFD device is switched off.

The NODE sends data to the coordinator that issues a Data Indication.
Every time a data is transmitted, red LED of RFD device blinks and red LED of FFD device is toggled.

After association, RFD sends data to FFD every 1 second.
User can also issue data from RFD to FFD, by pressing the button 1 (SW1) of RFD device.
Data transmission can be then observed using OTA sniffer, UART logs and LED status. 

If you want to restart the demo, simply reset the NODE.

__BLE DEMO__ 

During MAC 802.15.4 activity, the BLE_HeartRate_Mac_RFD device is also configured in BLE with Heart Rate profile, performs advertising and is available
to accept incoming connection in BLE.    

The BLE activity (advertising or connection events) can be observed on green LED.

On the android/ios device, enable the Bluetooth communications, and if not done before:

- Install the ST BLE Toolbox application on the android device:
	- <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
    - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

- You can also install the ST BLE Sensor application on the android/ios device:
	- <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
	- <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>

- Then, click on the App icon, ST BLE Toolbox (android/ios device).
You can either open ST BLE Sensor application (android/ios device).

- In the Heart Rate interface, HearRate and energy measurement are launched and displayed in graphs, you can reset the energy measurement.
- After 60s of advertising, the application switch from fast advertising to low power advertising.
Pressing button B2 while advertising allows to restart fast advertising.
