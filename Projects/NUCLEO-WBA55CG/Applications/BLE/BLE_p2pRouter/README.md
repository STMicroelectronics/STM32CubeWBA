## __BLE_p2pRouter Application Description__

Demonstrate STM32WBA acting at the same time as both: BLE central and peripheral, GATT server and client.

p2pRouter application scans to connect to p2pServer devices and accept connection of ST BLE Toolbox smartphone app.  
It routes BLE messages received from both side.

<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer_Router
"> STM32WBA Bluetooth LE - P2P router</a>


### __Keywords__

Connectivity, BLE

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - At least another STM32WBA Nucleo board is necessary to run p2pServer application.
    
### __How to use it?__

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file

Use one of the following remote application to interact with your device  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>

Reset the STM32WBAxx Nucleo board and launch remote interface application on PC/android/ios device. 

 - Connect to STM32WBA BLE p2pRouter device.
 - Open P2pRouter service  
  1 - Switch on a STM32WBAxx Nucleo board with p2pServer application  
  2 - Click on B1 to start BLE p2pRouter scanning and then connect to a p2pServer device surrounding 
 - once connected blue Led is switched off on BLE p2pRouter STM32WBAxx Nucleo board and coonected p2pS-xx is displayed on the remote interface.

 - Up to 7 p2pServer devices can be connected to BLE p2pRouter device:  
 if needed, do step 1 and 2 to connect a new p2pServer  

 - On p2pServer device, a click on B1 send a notification to the remote interface through BLE p2pRouter .
 - On remote interface turning on/off the Led send a write message to selected p2pServer device through BLE p2pRouter to switch on/off the blue Led on the STM32WBAxx Nucleo board.
