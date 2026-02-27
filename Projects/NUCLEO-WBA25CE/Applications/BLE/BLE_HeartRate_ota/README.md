## __BLE_HeartRate_ota Application Description__  

How to use the BLE Heart Rate with Over The Air Firmware Update feature.

### __Keywords__
  
Connectivity, BLE, BLE protocol, BLE Firmware Update Over The Air, BLE profile.  

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

The BLE_ApplicationInstallManager application must run with BLE applications embedding OTA service like: BLE_HeartRate_ota application.  

__BLE_ApplicationInstallManager__ is loaded at the memory address __0x08000000__ and __BLE_HeartRate_ota__ application is loaded at the memory address __0x08006000__.  

For more details on OTA process please refers to associated readme.html.  

Use one of the following remote applications to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

- Power on the STM32WBAxx Nucleo board.  
- Open remote interface:  
  - Select the HR__xx in the device list, where xx are the last 2 digits of the BD ADDRESS.  
  - Connect to it.  
  - Select the Heart Rate service.  
  
Heart Rate value and Energy Expanded measurements are displayed in graphs.  
Energy Expanded can be reset.  

Each button (B1, B2, B3) on STM32WBAxx Nucleo board, is associated with the following functions:  

- B1 :  
  - Not connected : Launch fast advertising.  
  - Connected     : Switch between 1Mbit and 2Mbit PHY.  
- B2 :  
  - Not connected : Clear security database.  
  - Connected     : Security request, pairing.  
- B3 :  
  - Not connected : Not used.  
  - Connected     : update connection interval.  

For more information, please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> Bluetooth LE - Heart Rate Sensor ST wiki.</a>  

The Firmware Update Over The Air (FUOTA) service provides the possibility to receive a new application to be installed while the current application is running.  
For more information please refer to the <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_FUOTA#Firmware_update_over_the_air_profile"> Firmware Update Over The Air profile.</a>  
 
