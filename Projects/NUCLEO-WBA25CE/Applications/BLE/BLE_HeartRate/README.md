## __BLE_HeartRate Application Description__  

The Heart Rate Profile, widely used in fitness applications, defines the communication process between a GATT-server of a Heart Rate Sensor device, such as a wrist band, and a GATT-client Collector device, such as a smartphone or tablet.  

How to use the Heart Rate profile as specified by the BLE SIG and described in the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> ST wiki.</a>  

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__
  
  - This application runs on STM32WBA25xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
  - This application is configured to support low power mode (No traces - No debugger).  
  - Traces and debugger could be acivated by setting the following in app_conf.h:  
    - CFG_LPM_LEVEL to 0  
    - CFG_DEBUGGER_LEVEL to 1  
    - CFG_LOG_SUPPORTED to 1  

### __How to use it?__
  
In order to make the program work:  

 - Open the project with your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  

