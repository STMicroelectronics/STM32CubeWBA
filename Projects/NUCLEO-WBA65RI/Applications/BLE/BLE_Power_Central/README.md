## __BLE_Power_Central Application Description__

How to measure Central power consumption using BLE.  

BLE_Power_Central application usage:  
 - The BLE_Power_Central application needs to be used with BLE_Power_Peripheral application (2 Hardware boards needed).  
 - Configuration phase (Red led is active or toggling): buttons are active and user can select the desired configuration to measure.  
 - Power consumption measurement phase (Red led if off): buttons are disabled and power measurement can be done.  

 BLE_Power_Central can be used to measure:  

  - Central power consumption on Idle state.  
  - Central power consumption on Data transfer state.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA65 Nucleo board.  
  - Another Nucleo board may be necessary to run BLE_Power_Peripheral application.  
  - For power measurement: STLINK-V3PWR or STM32 Power shield can be used with STM32CubeMonitor-Power Graphical tool.  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  
 
Note: CFG_PERIPH_BD_ADDRESS must be configured in BLE_Power_Central; it must contain the BLE_Power_Peripheral BD address.  

Once powered, the application automatically connects to the BLE_Power_Peripheral and the configuration phase starts for 10sec. Buttons can be used to select the desired configuration:  

- B1 click : Start data transfer to the BLE_Power_Peripheral.  
- B3 click : Switch the Connection Interval.  
