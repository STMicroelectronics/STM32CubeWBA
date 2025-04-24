## __BLE_p2pServer_Simplest Application Description__

How to demonstrate Point-to-Point communication using BLE as simple as possible.

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).
   - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications</a>

Peer to Peer Server device is part of Peer to Peer Profile.
It is widely used for direct connection and communication between a GATT-server of a Peer to Peer Server device, and a GATT-client Collector device, such as a smartphone, STM32WBA Peer to Peer Client or Peer to Peer Router devices.

BLE p2pServer application advertises and waits for a connection from either:

 - BLE p2pClient application running on STM32WBA65xx devices
 - remote interface

Once connected, BLE p2pServer can receive messages from the Client and send notification to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WBA65 Nucleo board.
    
### __How to use it?__

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file

Use one of the following remote interface to interact with your device  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>
	
Reset the STM32WBAxx Nucleo board and launch remote interface on PC/android/ios device. 

- Open remote interface:  
  - Select the p2pS_SI in the device list.  
  - Connect it.  
  - Select the P2P Server service.

 - On remote interface, click on the LED icon to switch On/Off LED1 (blue) of the STM32WBAxx Nucleo board.

 - On the STM32WBAxx Nucleo board click on B1 to send a notification to the remote interface
