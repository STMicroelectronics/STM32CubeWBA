## __BLE_p2pServer_ota Application Description__

How to use the BLE Peer to Peer Server with Firmware Update Over The Air feature.  

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).  
For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications.</a>  

The Peer to Peer Server device is part of the Peer to Peer Profile.  
It is widely used for direct connection and communication between a GATT-server of a Peer to Peer Server device, and a GATT-client Collector device, such as a smartphone, STM32WBA Peer to Peer Client or Peer to Peer Router devices.  

The BLE p2pServer application advertises and waits for a connection from either:  

 - The BLE p2pClient application running on STM32WBAxx devices.  
 - The remote application.  

Once connected, the BLE p2pServer can receive messages from the Client and send notification to it.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE Firmware Update Over The Air, BLE profile.  

### __Hardware and Software environment__

 - This application runs on STM32WBA65xx Nucleo board.  
 - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
 - Another STM32WBAxx Nucleo board may be necessary to run BLE p2pClient or BLE p2pRouter application.  

### __How to use it?__

  - This application runs on STM32WBA65xx Nucleo board.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
  - Another STM32WBAxx Nucleo board may be necessary to run the BLE p2pClient or BLE p2pRouter application.  
    
### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board:  
   - The BLE_ApplicationInstallManager binary is downloaded at the memory address 0x08000000  
   - The BLE_p2pServer_ota binary is downloaded at the memory address 0x08006000  
 - Run the example.

Use one of the following remote interface to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

- Power on the STM32WBAxx Nucleo board.  
- Open the remote interface:  
 - Select the p2pS_xx in the device list, where xx is the 2 last digits of the BD ADDRESS.  
 - Connect to it.  
 - Select the P2P Server service.  
 - On the remote interface, click on the LED icon to switch LED1 (blue) of the STM32WBAxx Nucleo board On/Off.  
 
Each button (B1, B2, B3) on STM32WBAxx Nucleo board is associated to the following functions:  

- B1 :  
  - Not connected : Launch fast advertising.  
  - Connected     : Send notification message to the client.  
- B2 :  
  - Not connected : Clear security database.  
  - Connected     : Launch fast advertising to allow multi-connection.  
- B3 :  
  - Not connected : Not used.  
  - Connected     : Update connection interval.  

For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer"> STM32WBA Bluetooth LE - Peer 2 Peer Applications ST wiki.</a>  

The Firmware Update Over The Air (FUOTA) service provides the possibility to receive a new application to be installed while the current application is running.  
For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_FUOTA#Firmware_update_over_the_air_profile"> Firmware Update over The Air profile.</a>  

To change the application using Firmware Other The Air:  

- Select the Over The Air Update Server service panel in the remote interface.  
- Select the STM32WBA device type.  
- The STM32WBA interface offers the choice to download an Application Binary or a User Conf Binary.  
  - Memory mapping is defined as follows:  
    - Pages 0 to 2 are reserved for the BLE_ApplicationInstallManager application.  
    - Pages 3 to 127 are dedicated to BLE_p2pServer_ota application.  
    - Pages 128 to 252 are dedicated for the new application.  
    - Page 253 is dedicated to User Configuration Data.  
    - Pages 254 to 255 reserved for NVM.  

  - Offset configuration:  
    - if Application Binary is selected:  
      Address offset is set to 0x100000 to download the new application at address 0x08100000.  
    - if User Conf Binary is selected:  
      Address offset is set to 0x0F6000 to download the new application at address 0x080F6000.  
  - Select the binary file to download:  
    - The number of pages to erase is displayed resulting of the size in pages of the file.  
    - The Number of pages may be over written is needed.  
  - Download the file.  
  - When done, a reboot of the application is performed.  
