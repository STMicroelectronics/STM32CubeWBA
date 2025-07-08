## __BLE_HealthThermometer Application Description__

The Health Thermometer Sensor is a GATT server:  

Measures the temperature and exposes it via the Health Thermometer Service.  
Contains the Device Information Service to be identified by the remote device.  

How to use the Health Thermometer profile as specified by the BLE SIG and described in the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Health_Thermometer#Health_Thermometer_Profile"> ST wiki.</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and flash the board at the address 0x08000000 with the binary file.  

Use one of the following remote interfaces to interact with your device:  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the STM32WBAxx Nucleo board and launch remote interface on PC/android/ios device.  

- Open the remote interface:  
  - Select the HT_xx in the device list, where xx are the last 2 digits of the BD ADDRESS.  
  - Connect to it.  
  - Select the Health Thermometer service.  

- Open Health Thermometer service:  
  The temperature measurement is displayed on the remote interface.  
  Temperature min/max values are also displayed.  

For more information, please refer to the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_Health_Thermometer#Health_Thermometer_Profile"> STM32WBA Bluetooth LE - Health Thermometer Sensor ST wiki.</a>  
