## __BLE_p2pRouter Application Description__

Demonstrates STM32WBA acting simultaneously as both a BLE central and a BLE peripheral, as well as a GATT server and client.  

The BLE_p2pRouter application scans to connect to p2pServer devices and accepts connections of ST BLE Toolbox smartphone app.  
It routes BLE messages received from both sides.  

For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer_Router"> STM32WBA Bluetooth LE - P2P router.</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing.  

### __Hardware and Software environment__

 - This application runs on STM32WBA55xx Nucleo board.  
 - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
 - At least another STM32WBA Nucleo board is necessary to run the BLE_p2pServer application.  
    
### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file.  

Use one of the following remote applications to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the STM32WBAxx Nucleo board and launch remote interface application on PC/android/ios device.  

- Connect to STM32WBA BLE_p2pRouter device.  
- Open BLE_P2pRouter service:  
  - 1: Switch on an STM32WBAxx Nucleo board with the BLE_p2pServer application.  
  - 2: Click on B1 to start BLE_p2pRouter scanning and then connect to a BLE_p2pServer device nearby.  
- Once connected blue Led is switched off on BLE_p2pRouter STM32WBAxx Nucleo board and coonected p2pS-xx is displayed on the remote interface.  
- Up to 7 BLE_p2pServer devices can be connected to BLE p2pRouter device:  
if needed, repeat steps 1 and 2 to connect a new BLE_p2pServer.  
- On the BLE_p2pServer device, a click on B1 sends a notification to the remote interface through BLE_p2pRouter.  
- On remote interface, turning on/off the Led sends a write message to selected BLE_p2pServer device through the BLE_p2pRouter to switch the blue Led on/off on the STM32WBAxx Nucleo board.  

