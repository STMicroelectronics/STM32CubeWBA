## __BLE_p2pServer Application Description__

The BLE Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).  
For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications.</a>  

The BLE Peer to Peer Server device is part of the BLE Peer to Peer Profile.  
It is widely used for direct connection and communication between a GATT-server of a BLE Peer to Peer Server device, and a GATT-client Collector device, such as a smartphone, STM32WBA BLE Peer to Peer Client or BLE Peer to Peer Router devices.  

The BLE p2pServer application advertises and waits for a connection from either:  

 - The BLE p2pClient application running on STM32WBAxx devices.  
 - A remote interface.  

Once connected, the BLE_p2pServer can receive messages from the Client and send notification to it.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
  - Another STM32WBAxx Nucleo board may be necessary to run the BLE p2pClient or BLE p2pRouter application.  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file.  

Use one of the following remote interface to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the STM32WBAxx Nucleo board and launch the remote interface on PC/android/ios device.  
 
- Open the remote interface:  
  - Select the p2pS_xx in the device list, where xx are the last 2 digits of the BD ADDRESS.  
  - Connect to it.  
  - Select the P2P Server service.  

 - On remote interface, click on the LED icon to switch On/Off LED1 (blue) of the STM32WBAxx Nucleo board.  
 
Each button (B1, B2, B3) on the STM32WBAxx Nucleo board is associated to the following functions:  

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
