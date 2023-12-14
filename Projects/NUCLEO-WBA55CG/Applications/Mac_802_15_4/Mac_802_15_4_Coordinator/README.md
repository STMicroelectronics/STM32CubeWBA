## __Mac_802_15_4_Coordinator Application Description__

How to use MAC 802.15.4 Association and Data exchange.

### __Keywords__

MAC, 802.15.4, association, data, STM32WBA, Coordinator, ED scan, diassociate


### __Directory contents__

Mac_802_15_4/Mac_802_15_4_FFD/

  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/app_common.h                            Header for all modules with common definition
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/app_conf.h                              Parameters configuration file of the application
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/app_entry.h                             Parameters configuration file of the application
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/hw_conf.h                               Configuration file of the HW
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/main.h                                  Header for main.c module
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm_logging.h                           Application header file for logging
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm32wbxx_hal_conf.h                    STM32 HAL configuration file.
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm32wbxx_it.h                          Interrupt handlers header file
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/utilities_conf.h                        Header for Configuration of utilities
  - Mac_802_15_4/Mac_802_15_4_Coordinator/STM32_WPAN/App/app_mac_802_15_4_process.h        Header for Coordinator MAC 802.15.4 processing tasks
  - Mac_802_15_4/Mac_802_15_4_Coordinator/STM32_WPAN/App/app_mac.h                         Header for Coordinator MAC 802.15.4 Core Mac Device implementation
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/app_entry.c                             Parameters configuration file of the application
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm32wbaxx_hal_msp.c                    Driver configuration
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/hw_uart.c                               UART Driver
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/main.c                                  Main program
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm_logging.c                           Logging Implementation
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/stm32wbxx_it.c                          exceptions handler and peripherals interrupt service routine
  - Mac_802_15_4/Mac_802_15_4_Coordinator/Core/Inc/system_stm32wbaxx.c                     CMSIS Cortex Device Peripheral Access Layer implementation
  - Mac_802_15_4/Mac_802_15_4_Coordinator/STM32_WPAN/App/app_mac.c    			           Coordinator MAC 802.15.4 processing tasks implementation
  - Mac_802_15_4/Mac_802_15_4_Coordinator/STM32_WPAN/App/app_mac_802_15_4_process.c        CoordinatorMAC 802.15.4 Core Mac Device implementation


### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices.

  - This example has been tested with an STMicroelectronics STM32WBA55CG-Nucleo 
    board and can be easily tailored to any other supported device 
    and development board.
    
  - On STM32WBA52CG-Nucleo, the jumpers must be configured as described
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
The other device will act as a MAC 802.15.4 Node. The Node request 
association on startup and send data to the coordinator. 

Below example implements the CoordinatorDevices.

Minimum requirements for the demo:

  - 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 Coordinatormode using current generated binary.
  - 1 STM32WBA55xx-Nucleo board in MAC 802.15.4 Node (see MAC 802.14.4 Node example)

In order to make the program work, you must do the following:

  - Connect 2 STM32WBA55xx-Nucleo boards to your PC 
 
    - COORDINATOR :
 
      - Open your preferred toolchain 
      - Rebuild all files of Mac_802_15_4/Mac_802_15_4_Coordinator and load your image into COORDINATOR target memory 
      - The example run on Reset
 
    - NODE :
    
      - Refers to MAC 802.14.4 Node example to build and load generated image
     
To get the traces in real time, you can connect an HyperTerminal to the LPUART CN3-32.
 
 
 The Serial interface must be configured as follows:
 
- BaudRate = 115200 baud  
- Word Length = 8 Bits 
- Stop Bit = 1 bit
- Parity = none
- Flow control = none

You can also use an MAC 802.15.4 over the air sniffer on channel write on HyperTerminal to spy MAC 802.15.4 frames exchanged between the devices.
  
__START DEMO__ 

By default, after a reset, both board starts. Coordinator must be started firstly to be able to answer
to the node association request.

This MAC 802.15.4 example is to demonstrate Point-to-Point communication using MAC components between 
a FFD (ie.: the COORDINATOR) and an RFD (ie.: the NODE). 

Upon COORDINATOR start, the device initializes the MAC 802.15.4 and associates resources in non-beaconned
enable mode. 

MAC PIB is then configured as follow :

  - PanID 						 : 0xBEEF, can be define in app_conf.h
  - Coordinator Extended Address : depending to the device but begin by 00:80:E1
  - Coordinator Short Address    : 0x2000, can be define in app_conf.h
  - Poll Short Address           : 0x2001 - 0x2010
  - Channel                      : take the lest noisy channel, can be define in app_conf.h

Once MAC PIB is initialized, Coordinator is started on least noisy channel with ED scan as an association capable coordinator.
At this stage the Coordinator is waiting for an association request. Blue LED (ID 1) is switch on once 
the coordinator is started.

On Node side, the MAC layer is initialized on startup. In this state, blue LED (ID 1) is switch on.

Node issues an association request to the Coodinator to retrieve its short address. 
On in coming association request Coordinator's green LED (ID 2) blinks.

Once the association between two devices is completed, Node's green LED (ID 2) is switch on to indicate the node 
is associated to the coordinator. 

The Node sends data to the coordinator that issues a Data Indication.
On both devices side, on data transmission red led (ID 3) blinked.

User can issue display all associated devices, by pressing the button 1 (SW1). 

User can issue check if device is not associated, by pressing the button 2 (SW2). Data transmission with ACK, if the node associated don't response, then the device is remove of associated device table, can be then observe using OTA sniffer,
UART logs and LED status. 

If you want to restart the demo, simply reset the NODE.









