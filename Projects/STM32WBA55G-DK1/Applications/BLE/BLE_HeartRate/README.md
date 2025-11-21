## __BLE_HeartRate Application Description__

The Heart Rate Profile, widely used in fitness applications, defines the communication process between a GATT-server of a Heart Rate Sensor device, such as a wrist band, and a GATT-client Collector device, such as a smartphone or tablet.  

How to use the Heart Rate profile as specified by the BLE SIG and described in the<a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:STM32WBA_HeartRate#Heart_Rate_Profile"> ST wiki.</a>  

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile.  

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx Discovery kit.  
  - Connect the Discovery Kit Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK).  

### __How to use it?__

In order to make the program work:  

- Power on the Discovery Kit board with the BLE_Heart Rate application.  
- Open your preferred toolchain.  
- Rebuild all files and flash the board at the address 0x08000000 with the binary file.  

On the android/ios device, enable the Bluetooth communications, and if not done before:  

- Install the ST BLE Toolbox application on the android device:  
  - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>  
  - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>  

- Or install the ST BLE Sensor application on the android/ios device:  
  - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>  
  - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>  

- Open ST BLE Toolbox or ST BLE Sensor application on android/ios device.  

Heart Rate value and Energy Expanded measurements are displayed in graphs.  
Energy Expanded can be reset.  

- Heart Rate and energy measurement are also displayed on LCD screen of the board.  
- Pairing is supported: right side of the joystick clears the security database when the device is not connected.  
When connected to a client, right side of the joystick sends the slave pairing request, here a popup asks you to associate your device.  

- You can either bond from the smartphone by clicking on Bond button in the ST BLE Toolbox application interface.  
- This example supports switch to 2Mbits PHY, pressing left side of the joystick while connected allows to switch between 1Mbits PHY and 2Mbits PHY.  
- After 60s of advertising, the application switches from fast advertising to low power advertising, pressing left side of the joystick while advertising allows to restart fast advertising.  
- Pressing uppder side of the joystick while connected allows to update the connection interval.  
