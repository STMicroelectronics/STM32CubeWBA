## __BLE_HeartRateFreeRTOS Application Description__

The Heart Rate Profile, widely used in fitness applications, defines the communication process between a GATT-server of a Heart Rate Sensor device, such as a wrist band, and a GATT-client Collector device, such as a smartphone or tablet.

How to use the Heart Rate profile is specified by the BLE SIG and described in ST wiki.  

<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> Getting started with Heart Rate Sensor</a>

This example uses the X-CUBEFREERTOS pack.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, FreeRTOS

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx devices.
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 
  - This application is configured to support low power mode (No traces - No debugger) 
  - Traces and debugger could be acivated by setting in app_conf.h  
    - CFG_LPM_LEVEL (0)
    - CFG_DEBUGGER_LEVEL (1)
    - CFG_LOG_SUPPORTED (1)

### __How to use it?__

In order to make the program work:
 - Open your prefered toolchain
 - Rebuild all files and flash the board at the address 0x08000000 with the binary file

Use one of the following remote interface to interact with your device <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA</a>

Reset the STM32WBAxx Nucleo board and launch remote interface on PC/android/ios device. 

- Open remote interface:  
  - Select the HR__xx in the device list, where xx is the 2 last digits of the BD ADDRESS.  
  - Connect it.  
  - Select the Heart Rate service.
  
  HearRate value and Energy Expanded measurements are displayed in graphs.  
  Energy Expanded can be reset.

Each button (B1, B2, B3) on STM32WBAxx Nucleo board, is associated to the following functions:

B1 : 
- not connected : launch fast advertising
- connected     : switch between 1Mbits and 2Mbits PHY

B2 :
- not connected : clear security database
- connected     : security request, pairing

B3 :
- not connected : not
- connected     : update connection interval

For more informations, please refer to Bluetooth LE - Heart Rate Sensor ST wiki  
<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> Heart Rate Profile</a>  
