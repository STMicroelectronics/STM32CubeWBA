## __BLE_Beacon Application Description__  

How to advertise 5 types of beacon (Eddystone uid, Eddystone url, Eddystone uid+tlm, Eddystone url+tlm, iBeacon).  

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE profile.  

### __Hardware and Software environment__
  
  - This application runs on STM32WBA25xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

   5 Beacon types are available and have to be defined in app_con.h file :  
   - #define CFG_BEACON_TYPE must be set at one of the following value :  
     - CFG_EDDYSTONE_UID_BEACON_TYPE  
     - CFG_EDDYSTONE_URL_BEACON_TYPE  
     - CFG_EDDYSTONE_UID_BEACON_TYPE | CFG_EDDYSTONE_TLM_BEACON_TYPE  
     - CFG_EDDYSTONE_URL_BEACON_TYPE | CFG_EDDYSTONE_TLM_BEACON_TYPE  
     - CFG_IBEACON_TYPE  

 - Use one of the following remote interfaces to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a> 
   On the smartphone, download the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a> application.  
 - Enable Bluetooth communications.  
 - Run the smartphone application and start scanning.  
 - The Beacon information are displayed on the smartphone.  

For more information on Beacon profile, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Beacon"> STM32WBA Bluetooth Low Energy Beacon.</a>  

