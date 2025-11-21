## __BLE_p2pClient_Ext Application Description__

Demontrates a BLE scanner with connections from extended and legacy advertising.  

The Peer to Peer Profile is a Generic Attribute Profile (GATT) based low-energy profile defined by STMicroelectronics with proprietary UUIDs(128 bits).  
For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Client_application"> STM32WBA Peer To Peer Client Applications.</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, extended.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Nucleo board.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
  - Another STM32WBAxx Nucleo board may be necessary to run BLE p2pServer_Ext application.  

### __How to use it?__

In order to make the program work, you must do the following:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the binary file.  
 - Power up BLE p2pClient_Ext device next to a BLE p2pServer_Ext device.  

Open a terminal on both the Central and the Peripheral side (ST Link Com Port, @115200 bauds).  

At startup on the BLE_p2pServer_Ext side, depending on the chosen configuration in app_conf.h, traces show the started advertising sets.  

The central device (BLE_p2pClient_Ext) scans and receives HCI_LE_EXTENDED_ADVERTISING_REPORT_EVENT from BLE_p2pServer_Ext device.  

Result is printed over the UART and presented in a tab format with following information:  

  - BD address  
  - Type  
  - Rssi  
  - Property  
  - Data length  
  - SID  
  - Some flags information  
 
Pressing the buttons will perform the following actions:  

  - B1: Start/stop the scan and list the BLE_p2pServer_Ext recorded for connection.  
  - B2: Toggles a filter on extended results only.  
  - B3: Initiates a connection request extended. 

When link is established:
  - B1: writes a message to the connected BLE p2pServer_Ext device. The blue LED on the Server device should switch on/off.
  - B3: writes a message of 509 bytes thanks to aci_gatt_write_without_esp_ext command 


 For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer"> STM32WBA Bluetooth LE Peer 2 Peer Applications ST wiki.</a>  
