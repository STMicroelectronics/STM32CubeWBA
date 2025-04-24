## __BLE_p2pServerThreadX Application Description__

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).
   - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications</a>

Peer to Peer Server device is part of Peer to Peer Profile.
It is widely used for direct connection and communication between a GATT-server of a Peer to Peer Server device, and a GATT-client Collector device, such as a smartphone, STM32WBA Peer to Peer Client or Peer to Peer Router devices.

BLE p2pServer application advertises and waits for a connection from either:

 - BLE p2pClient application running on STM32WBAxx devices
 - remote interface

Once connected, BLE p2pServer can receive messages from the Client and send notification to it.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - Another STM32WBAxx Nucleo board may be necessary to run BLE p2pClient or BLE p2pRouter application.
    
### __How to use it?__

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file

Use one of the following remote interface to interact with your device  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>
	
Reset the STM32WBAxx Nucleo board and launch remote interface on PC/android/ios device. 
 
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
