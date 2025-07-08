## __BLE_Beacon Application Description__

How to advertise 4 types of beacon (tlm, uuid, url, iBeacon).  
For more details on Beacon profile, please refer to <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Beacon"> STM32WBA Bluetooth Low Energy - Beacon</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__

In order to make the program work:  

* Open your preferred toolchain.  
* Rebuild all files and flash the board at the address 0x08000000 with the binary file.  
* 4 Beacon types are available and have to be defined in app_con.h file :  
  * #define CFG_BEACON_TYPE must be set at one of the following value :  
    * CFG_EDDYSTONE_UID_BEACON_TYPE  
    * CFG_EDDYSTONE_URL_BEACON_TYPE  
    * CFG_EDDYSTONE_TLM_BEACON_TYPE  
    * CFG_IBEACON  

* Run the example.  
* On the smartphone, download a Beacon Scanner application.  
* Enable Bluetooth communications.  
* Run Beacon Scanner application and start scanning.  
* The Beacon information is displayed on the smartphone.  

