## __BLE_Power_Peripheral Application Description__

How to measure Peripheral power consumption using BLE.  

BLE_Power_Peripheral application usage:  

 - Configuration phase (Red led is active or toggling): buttons are active and user can select the desired configuration to measure.  
 - Power consumption measurement phase (Red led if off): buttons are disabled, and power measurement can be done.  

 BLE_Power_Peripheral can be used to measure:  

 - Advertising phase.  
 - Connected phase with the ST BLE Toolbox smartphone application.  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This application runs on STM32WBA65 Nucleo board.  
  - Another Nucleo board may be necessary to run BLE_Power_Central application.  
  - For power measurement: STLINK-V3PWR or STM32 Power shield can be used with STM32CubeMonitor-Power Graphical tool.  

### __How to use it?__

In order to make the program work:  

 - Open your preferred toolchain.  
 - Rebuild all files and load your image into target memory.  
 - Run the example.  
 
Once powered the application starts advertising and the configuration phase begin for 10sec. Buttons can be used to select the desired configuration:  

- B2 click : Switch the advertising interval.  
- B3 click : Switch the Advertising type.  
 
 __You can interact with BLE_Power_Peripheral application with a smartphone:__  

 - Install and launch ST BLE Toolbox application on android or iOS smartphone:  
   - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android.</a>  
   - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS.</a>  

 - Open ST BLE Toolbox application:  
   - Select the PWR in the device list.  
   - Once connected, service and characteristics discovered; enable notification.  

- B1 click : Start/stop Notification.  
- B2 click : Toggle PHY 1M/2M by HCI_LE_SET_PHY.  
- B3 click : Update connection interval by ACI_L2CAP_CONNECTION_UPDATE_REQ_EVENT.  
