## __BLE_p2pServer_TZ Application Description__

This example is similar to the BLE_p2pServer with the TrustZone being activated and the Blue LED being connected to the Secure side of the MCU.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, TrustZone.

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx Nucleo board.

  - Connect the Nucleo Board to your PC with a USB cable to USB type C to ST-LINK connector (USB_STLINK).

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

Please refer to the wiki page describing how to setup and use this application: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_BLE_%26_TrustZone"> Wiki: STM32WBA Bluetooth LE & TrustZone.</a>

With the following Option bytes configuration :<br />
TZEN = 1<br />
SECBOOTADD0 = 0x180000<br />
SECWM1_PSTRT = 0x0<br />
SECWM1_PEND = 0x7F<br />
SECWM2_PSTRT = 0x7F<br />
SECWM2_PEND = 0x0

For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Bluetooth LE Peer 2 Peer Applications ST wiki.</a>
