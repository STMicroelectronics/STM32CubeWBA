## __BLE_Beacon_AoA_Tag Application Description__

It allows to configure a STM32WBA device as an AoA tag for connectionless scenario.
Device advertises beacon packets containing Constant Tone Extension data. 

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE Direction Finding

### __Hardware and Software environment__
  
  - This application runs on STM32WBA device.
  - Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.

  - Another STM32WBA Nucleo board is necessary to run BLE_Beacon_Scanner_AoA_Locator application.

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

 - The application will advertise beacon packets with Constant Tone Extension, to let a Aoa Locator device to locate the Aoa tag.
 
### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
