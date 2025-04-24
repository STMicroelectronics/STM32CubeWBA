## __BLE_HeartRate Application Description__

How to use the Heart Rate profile as specified by the BLE SIG.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx Discovery kit.
  - Connect the Discovery Kit Board to your PC with a USB cable type A to type C to ST-LINK connector (USB_STLINK). 

### __How to use it?__

In order to make the program work, you must do the following:
 - Open IAR toolchain
 - Rebuild all files and flash the board with the executable file

 On the android/ios device, enable the Bluetooth communications, and if not done before:

   - Install the ST BLE Toolbox application on the android device:
     - <a href="https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox"> ST BLE Toolbox Android</a>
     - <a href="https://apps.apple.com/us/app/st-ble-toolbox/id1531295550"> ST BLE Toolbox iOS</a>

   - You can also install the ST BLE Sensor application on the android/ios device:
       - <a href="https://play.google.com/store/apps/details?id=com.st.bluems"> ST BLE Sensor Android</a>
       - <a href="https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8"> ST BLE Sensor iOS</a>

 Power on the Discovery Kit board with the BLE_HeartRate application.

- Then, click on the App icon, ST BLE Toolbox (android/ios device),
   You can either open ST BLE Sensor application (android/ios device).

- In the Heart Rate interface, HearRate and energy measurement are launched and displayed in graphs,
  you can reset the energy measurement.
   - HeartRate and energy measurement are also displayed on LCD screen of the board.
   - Pairing is supported: right side of the joystick clears the security database when the device is not connected. 
   When connected with a client, right side of the joystick send the slave pairing request, here a popup asks you to associate your device.
   You can either bond from the smartphone by clicking on Bond button in the ST BLE Toolbox application interface.
   - This example supports switch to 2Mbits PHY, pressing left side of the joystick while connected allows to switch between 1Mbits PHY and 2Mbits PHY.
   - After 60s of advertising, the application switch from fast advertising to low power advertising, pressing left side of the joystick while advertising allows to restart fast advertising.
   - Pressing uppder side of the joystick while connected allows to update the connection interval.

