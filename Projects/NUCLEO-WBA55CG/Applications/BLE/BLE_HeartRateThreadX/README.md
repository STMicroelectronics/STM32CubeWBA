## __BLE_HeartRateThreadX Application Description__
The Heart Rate Profile, widely used in fitness applications, defines the communication process between a GATT-server of a Heart Rate Sensor device, such as a wrist band, and a GATT-client Collector device, such as a smartphone or tablet.  

How to use the Heart Rate profile is specified by the BLE SIG and described in <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> ST wiki.</a>  

This example uses the ThreadX OS.  

### __Notes__

 1. Care must be taken when using HAL_Delay(). This function provides accurate delay (in milliseconds)
    based on variable incremented in TIM2 ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the TIM2 interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.  
    To change the TIM2 interrupt priority, you have to use HAL_NVIC_SetPriority() function.  

 2. The application needs to ensure that the TIM2 time base is always set to 1 millisecond
    to have correct HAL operation.  

 3. SysTick is used as the timebase by the ThreadX OS.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, ThreadX.  

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx devices.  
  - Connect the Nucleo Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  
  - This application is configured to support low power mode (No traces - No debugger).  
  - Traces and debugger could be acivated by setting the following in app_conf.h.  
    - CFG_LPM_LEVEL to 0  
    - CFG_DEBUGGER_LEVEL to 1  
    - CFG_LOG_SUPPORTED to 1  

### __How to use it?__

In order to make the program work:  

- Open your preferred toolchain.  
- Rebuild all files and flash the board at the address 0x08000000 with the binary file.  

Use one of the following remote interfaces to interact with your device: <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:BLE_smartphone_applications#Bluetooth-C2-AE_LE_collector_applications_for_STM32WBA
"> Bluetooth LE collector applications for STM32WBA.</a>  

Reset the STM32WBAxx Nucleo board and launch remote interface on PC/android/ios device.  

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
  - Connected     : Update connection interval.  

For more information, please refer to the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> STM32WBA Bluetooth LE - Heart Rate Sensor ST wiki.</a>   
