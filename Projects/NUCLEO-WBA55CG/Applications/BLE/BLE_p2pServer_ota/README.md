## __BLE_p2pServer_ota Application Description__

How to use the BLE Peer to Peer Server with Over The Air Firmware Update feature.

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).
   - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications</a>

Peer to Peer Server device is part of Peer to Peer Profile.
It is widely used for direct connection and communication between a GATT-server of a Peer to Peer Server device, and a GATT-client Collector device, such as a smartphone, STM32WBA Peer to Peer Client or Peer to Peer Router devices.

BLE p2pServer application advertises and waits for a connection from either:

 - BLE p2pClient application running on STM32WBAxx devices
 - remote application

Once connected, p2pServer can receive messages from the Client and send notification to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE firmware update over the air, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - Another STM32WBAxx Nucleo board may be necessary to run BLE p2pClient or BLE p2pRouter application.
    
### __How to use it?__

In order to make the program work:

 - Open your prefered toolchain
 - Rebuild all files and flash the board:  
   - BLE_ApplicationInstallManager binary is downloaded at the memory address 0x08000000
   - BLE_p2pServer_ota binary is downloaded at the memory address 0x08006000
 - Run the example

Use one of the following remote interface to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>

 - Power on the STM32WBAxx Nucleo board. 

- Open remote interface:  
  - Select the p2pS_xx in the device list, where xx is the 2 last digits of the BD ADDRESS.  
  - Connect it.  
  - Select the P2P Server service.

 - On remote interface, click on the LED icon to switch On/Off LED1 (blue) of the STM32WBAxx Nucleo board.
 
Each button (B1, B2, B3) on STM32WBAxx Nucleo board,  is associated to the following functions:

B1 :
- not connected : launch fast advertising
- connected     : send notification message to the client

B2 :
- not connected : clear security database
- connected     : launch fast advertising to allow multi-connection

B3 :
- not connected : no action
- connected     : update connection interval

For more information, please refer to  STM32WBA Bluetooth LE - Peer 2 Peer Applications ST wiki  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer"> Peer To Peer Applications</a>

The firmware update over the air (FUOTA) service provides the possibility to receive a new application to be installed, while the current application is running, for more details please refer to 
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_FUOTA#Firmware_update_over_the_air_profile"> Firmware update over the air profile</a>  

To change the application downloading Firmware Other The Air 
- Select the Over The Air Update Server service panel in remote interface,  
- select the STM32WBA device type
- The STM32WBA interface offers the choice to download an Application Binary or a User Conf Binary
  - Memory mapping is defined as following:  
    - pages 0 to 2 are reserved for the BLE_ApplicationInstallManager application, 
    - pages 3 to 63 are dedicated to BLE_HeartRate_ota application,  
    - pages 64 to 124 are dedicated for the new application, 
    - page 125 is dedicated to User Configuration Data,
    - pages 126 to 127 reserved for NVM  

  - Offset configuration
    - if Application Binary is selected  
      Address offset is set to 0x100000 to download the new application at address 0x08100000.
    - if User Conf Binary is selected
      Address offset is set to 0x0F6000 to download the new application at address 0x080F6000.
  - Select the binary file to download
    - Number of pages to erase is displayed resulting of the size in pages of the file
    - The Number of pages may be over written is needed
  - Download the file
  - When done, a reboot of the application is done. 
