## __Mac_802_15_4_FFD Application Description__

How to use MAC 802.15.4 Association and Data exchange.

### __Keywords__

MAC, 802.15.4, association, data, STM32WBA 

### __Directory contents__

Mac_802_15_4/Mac_802_15_4_FFD/

   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/app_common.h                            Header for all modules with common definition
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/app_conf.h                              Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/app_entry.h                             Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/hw_conf.h                               Configuration file of the HW
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/main.h                                  Header for main.c module
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm_logging.h                           Application header file for logging
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm32wbxx_hal_conf.h                    STM32 HAL configuration file.
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm32wbxx_it.h                          Interrupt handlers header file
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/utilities_conf.h                        Header for Configuration of utilities
   - Mac_802_15_4/Mac_802_15_4_FFD/STM32_WPAN/App/app_mac_802_15_4_process.h        Header for FFD MAC 802.15.4 processing tasks
   - Mac_802_15_4/Mac_802_15_4_FFD/STM32_WPAN/App/app_mac.h                         Header for FFD MAC 802.15.4 Core Mac Device implementation
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/app_entry.c                             Parameters configuration file of the application
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm32wbaxx_hal_msp.c                    Driver configuration
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/hw_uart.c                               UART Driver
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/main.c                                  Main program
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm_logging.c                           Logging Implementation
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/stm32wbxx_it.c                          exceptions handler and peripherals interrupt service routine
   - Mac_802_15_4/Mac_802_15_4_FFD/Core/Inc/system_stm32wbaxx.c                     CMSIS Cortex Device Peripheral Access Layer implementation
   - Mac_802_15_4/Mac_802_15_4_FFD/STM32_WPAN/App/app_mac.c    			            FFD MAC 802.15.4 processing tasks implementation
   - Mac_802_15_4/Mac_802_15_4_FFD/STM32_WPAN/App/app_mac_802_15_4_process.c        FFD MAC 802.15.4 Core Mac Device implementation
 

### __Hardware and Software environment__

- This example runs on STM32WBA55xx devices.

- This example has been tested with an STMicroelectronics STM32WBA55CG-Nucleo 
  board and can be easily tailored to any other supported device 
  and development board.
  
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
802.15.4 coordinator ensuring association response and network establishment. 
The other device will act as a MAC 802.15.4 RFD - Node. The Node request 
association on startup and send data to the coordinator. 

Below example implements the FFD Devices.

Minimum requirements for the demo:

- 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 FFD mode 
using current generated binary.
    (This device will be referred as COORDINATOR )
- 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 Node - RFD 
(see MAC 802.14.4 RFD example)
    (This device will be referred as NODE )

In order to make the program work, you must do the following: 

  - Connect 2 STM32WBA55xx-Nucleo boards to your PC 
 
    - COORDINATOR :
      - Open your preferred toolchain 
      - Rebuild all files of Mac_802_15_4/Mac_802_15_4_FFD and load your image into COORDINATOR target memory 
      - The example run on Reset
 
    - NODE :
      - Refers to MAC 802.14.4 RFD example to build and load generated image
     
To get the traces in real time, you can connect an HyperTerminal to the STLink Virtual Com Port.
 
 The Serial interface must be configured as follows:

- BaudRate = 115200 baud  
- Word Length = 8 Bits 
- Stop Bit = 1 bit
- Parity = none
- Flow control = none

You can also use an MAC 802.15.4 over the air sniffer on channel 11 to spy MAC 802.15.4 frames exchanged between the devices.
  
__START DEMO__ 

By default, after a reset, both board starts. Coordinator must be started firstly to be able to answer to the node association request.

This MAC 802.15.4 example is to demonstrate Point-to-Point communication using MAC components between a FFD (ie.: the COORDINATOR) and an RFD (ie.: the NODE). 

Upon COORDINATOR start, the device initializes the MAC 802.15.4 and associates resources in non-beaconned enable mode. 

MAC PIB is then configured as follow :

  - PanID : 0x1AAA
  - Coordinator Extended Address : 0xACDE480000000001
  - Coordinator Short Address    : 0x1122

Once MAC PIB is initialized, FFD is started on channel 11 as an association capable coordinator.
At this stage the COORDINATOR is waiting for an association request. Blue LED (ID 1) is switch on once the coordinator is started.

On NODE side, the MAC layer is initialized on startup. In this state, blue LED (ID 1) is switch on.

RFD issues an association request to the COORDINATOR to retrieve its short address. 
On in coming association request FFD's green LED (ID 2) blinks.

Once the association between two devices is completed, RFD's green LED (ID 2) is switch on to indicate the node 
is associated to the coordinator. 

The NODE sends data to the coordinator that issues a Data Indication.
On both devices side, on data transmission red led (ID 3) blinked.

User can issue data from RFD to FFD, by pressing the button 1 (SW1). Data transmission can be then observe using OTA sniffer,
UART logs and LED status. 

If you want to restart the demo, simply reset the NODE.