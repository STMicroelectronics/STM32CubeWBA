## __BLE_p2pServer_TZ Application Description__

This example is similar to BLE_p2pServer with the TrustZone being activated and the Blue LED being connected to the Secure side of the MCU.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, TrustZone

### __Hardware and Software environment__

  This application runs on STM32WBA65xx Nucleo board.

### __How to use it?__

Please refer to dedicated wiki page describing how to setup and use this application:
 <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_BLE_%26_TrustZone"> Wiki: STM32WBA Bluetooth® LE & TrustZone</a>
 
With the following Option bytes configuration : 
TZEN = 1
SECBOOTADD0 = 0x180000
SECWM1_PSTRT = 0x0
SECWM1_PEND = 0x7F
SECWM2_PSTRT = 0x7F
SECWM2_PEND = 0x0

For more information, please refer to  STM32WBA Bluetooth LE Peer 2 Peer Applications ST wiki  
   - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Peer_To_Peer#STM32WBA_Peer_to_Peer_Server_application"> STM32WBA Peer To Peer Server Applications</a>
