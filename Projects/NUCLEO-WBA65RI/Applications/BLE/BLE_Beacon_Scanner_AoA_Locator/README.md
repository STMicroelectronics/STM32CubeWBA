## __BLE_Beacon_Scanner_AoA_Locator Application Description__

It shows how to receive advertising beacon packets containing Constant Tone Extension data and collect the IQ samples.

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE Direction Finding

### __Hardware and Software environment__
  
  - This application runs on STM32WBA device.
  - Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.

  - Another STM32WBA Nucleo board is necessary to run BLE_Beacon_AoA_Tag application.

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

 - Place BLE_Beacon_Scanner_AoA_Locator device next to a BLE_Beacon_Aoa_Tag device.
 - The BLE_Beacon_Scanner_AoA_Locator device synchronizes with periodic advertising from BLE_Beacon_Aoa_Tag device and begin receiving periodic advertising packets.
 - The BLE_Beacon_Scanner_AoA_Locator device enables capturing of IQ samples from the Constant Tone Extension of periodic advertising packets and it gets these IQ samples. 

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
