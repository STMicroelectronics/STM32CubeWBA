## __BLE_p2pClient Application Description__

Demonstrate STM32WBA acting as BLE central and GATT client.

BLE p2pClient application scans and connects to p2pServer device.

Once connected, BLE p2pClient can write a message to the p2pServer and receive notification from it.

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).
   - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Client_application"> STM32WBA Peer To Peer Client Applications</a>

### __Keywords__

Connectivity, BLE, BLE protocol

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx Nucleo board.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - Another STM32WBAxx Nucleo board is necessary to run BLE p2pServer application.
    
### __How to use it?__

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the binary file
 - Power up BLE p2pClient device next to a  BLE p2pServer device.
 - On BLE p2pClient device, click on button B1 to launch a scan.  
 - If a BLE p2pServer device is detected, a connection is initiated.
 - Once the BLE p2pClient application is connected to a BLE p2pServer (green led flashes faster):
    - On BLE p2pClient device, click on button B1, to write a message toward connected BLE p2pServer device, the blue LED on the Server device should switch on/off.
    - On BLE p2pServer device, click on button B1 to send a notification message toward connected BLE p2pClient device, the blue LED on the Client device should switch on/off.
 - Pressing button B2 while not connected clear the database.
 
 For more information, please refer to  STM32WBA Bluetooth LE - Peer 2 Peer Applications ST wiki  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer"> Peer To Peer Applications</a>
