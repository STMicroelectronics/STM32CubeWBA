## __BLE_Long_Range_Peripheral Application Description__

Demonstrate STM32WBA2 using BLE stack full version to establish a link at LE coded PHY.
Based on P2PServer and P2PClient, write command and notification are sent to switch ON/OFF the blue led.

BLE_Long_Range_Peripheral application advertises one extended advertising set at primary and secondary LE coded PHY.

LE coded feature is described in the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Long_Range"> ST wiki.</a>

This set is configurable in file adv_ext_app.c.

### __Keywords__
  
Connectivity, BLE, extended, advertising, LE coded

### __Hardware and Software environment__
  
  - This example runs on STM32WBA2xx devices.
    Connect the Nucleo Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK). 
- Another STM32WBA2xx Nucleo board may be necessary to run BLE_Long_Range_Central application.

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

In order to make the program work, you must do the following :
 __You can interact with BLE_App_Long_Range_Peripheral application with a smartphone:__

 - Install and launch ST BLE Toolbox application on android or iOS smartphone
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

 - Enable extended advertising in scan panel
 - You can see data advertised by BLE_Long_Range_Peripheral application.

 __You can interact with another STM32WBA2xx Nucleo board running BLE_App_Long_Range_Central:__

 - Power up BLE_Long_Range_Central device next to BLE_Long_Range_Peripheral device.
 - On BLE_Long_Range_Central device push B1 button to start scan and connection.

 - When idle mode:
   B1 on BLE_Long_Range_Peripheral: start LE coded advertising
   B1 on BLE_Long_Range_Central: start LE CODED scan

 - When connection is established:
   B1 on BLE_Long_Range_Peripheral: send notification to switch ON/switch OFF blue led on BLE_Long_Range_Central.
   B1 on BLE_Long_Range_Central: write characteristic to switch ON/switch OFF blue led on BLE_Long_Range_Peripheral.
   B2 on central toggles LE coded PHY options (S=2 or S=8) RX from peripheral point of view. If S=8, red led is ON.
   B2 on peripheral toggles LE coded PHY options (S=2 or S=8) TX from peripheral point of view. If S=8, red led is ON.

   Change in PHY Coded option (S2 or S8) is not notified to remote device.
