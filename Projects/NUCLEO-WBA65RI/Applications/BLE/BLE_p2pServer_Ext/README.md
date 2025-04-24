## __BLE_p2pServer_Ext Application Description__

Demonstrate STM32WBA using BLE basic plus stack version to use several extended advertising sets.

BLE p2pServer_Ext application advertises several extended advertising sets:

- A legacy advertising
- A connectable extended advertising
- A scannable extended advertising
- A not connectable and not scannable extended advertising

These sets are configurable in file adv_ext_app.c.

For legacy advertising set please refer to :    - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications</a>

### __Keywords__

Connectivity, BLE, extended, advertising

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - Another STM32WBAxx Nucleo board  may be necessary to run p2pClient_Ext application.
    
### __How to use it?__

In order to make the program work, you must do the following :

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the STM32WBAxx Nucleo board at the address 0x08000000 with the executable file
 - Run the example
 
 __You can interact with p2pServer_Ext application with a smartphone:__

Use one of the following remote interface to interact with your device  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>

 - Enable extended advertising in scan panel
 - You can see data advertised by BLE p2pServer_Ext application.
 - Moreover, you can connect to the legacy adverting set, your device acts like in BLE p2pServer application.

 __You can interact with BLE p2pServer_Ext application with another Nucleo board running BLE p2pClient_Ext:__

 - Power up BLE p2pClient_Ext devices next to BLE p2pServer_Ext device.
 - On BLE p2pClient_Ext device button B1 click, starts scanning
   - BLE p2pServer_Ext connectable extended advertising in discovered, BD address is registered.
   - Then, on BLE p2pClient_Ext device button B3 click, connection is performed.

__You can interact with BLE p2pServer_Ext application with another Nucleo board running BLE p2pClient:__

 - Power up BLE p2pClient devices next to BLE p2pServer_Ext device.
 - On BLE p2pClient device button B1 click, to start scanning
   - p2pServer_Ext connectable legacy advertising in discovered, BD address is registered.
   - Then, connection is performed.
