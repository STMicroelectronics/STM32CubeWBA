## __BLE_DirectionFinding_Peripheral_Tag Application Description__
This demo implements a basic Direction Finding scenario using tag and locator roles (connection CTE mode) to demonstrate how to utilize the related Bluetooth LE stack capabilities.

When connected, the locator device sends CTE (Constant Tone Extension) requests to the peer device to receive packets containing the CTE field. This field is used to collect IQ samples necessary for direction finding.

The demo focuses on the configuration and collection of IQ samples, while the actual calculation of the direction from the received IQ samples is beyond the scope of this demonstration application and requires specific algorithms.

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, Direct Finding, CTE

### __Hardware and Software environment__
  
  - This example runs on STM32WBA65xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to USB type C to ST-LINK USB connector.
    
### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

 - Place BLE_DirectionFinding_Peripheral_Tag device next to a BLE_DirectionFinding_Central_Locator device.
 - A connection is initiated if a BLE_DirectionFinding_Peripheral_Tag device is detected.
 - Once the BLE_DirectionFinding_Peripheral_Tag application is connected to a BLE_DirectionFinding_Central_Locator:
    - On BLE_DirectionFinding_Peripheral_Tag device, LED2 is turned On.
    - On BLE_DirectionFinding_Central_Locator device, LED2 is turned On.

 If you want see debug message, you can open a serial terminal on Central and Peripheral side (ST Link Com Port, @115200 bauds).

### __Notes__
                                            
 - On Keil framework, the following compilation setting are applied:
   - diag_suppress L6312W          (Hide "Empty < type> region description for region < region>" warning)
   - diag_suppress L6314W          (Hide "No section matches pattern < module>(< section>" warning)
   - diag_suppress L6329W          (Hide "Pattern < module>(< section>) only matches removed unused sections" warning)
