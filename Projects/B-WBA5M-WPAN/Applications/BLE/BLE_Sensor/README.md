## __BLE_Sensor Application Description__

How to use the BLE sensor application.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile

### __Hardware and Software environment__

* This example runs on STM32WBA5mxx device.
* Connect the B-WBA5M-WPAN Board to your PC with a USB cable type C. To flash and debug, connect an STLink v3 to the STDC14 connector.

### __How to use it?__

In order to make the program work, you must do the following: 
 - Open IAR toolchain 
 - Rebuild all files and flash the board with the executable file

On the android/ios device, enable the Bluetooth communications, and if not done before:

* Install the ST BLE Toolbox application on the android device:
    * [ST BLE Toolbox Android](https://play.google.com/store/apps/details?id=com.st.dit.stbletoolbox)
    * [ST BLE Toolbox iOS](https://apps.apple.com/us/app/st-ble-toolbox/id1531295550)
* You can also install the ST BLE Sensor application on the android/ios device:
    * [ST BLE Sensor Android](https://play.google.com/store/apps/details?id=com.st.bluems)
    * [ST BLE Sensor iOS](https://itunes.apple.com/us/App/st-bluems/id993670214?mt=8)

Power on the B-WBA5M-WPAN board with the BLE_Sensor application.

* Then, click on the App icon, ST BLE Sensor (android/ios device), You can either open application ST BLE Toolbox (android/ios device).
    
* Open Plot data panel to display the sensor's values
* Open Enrironmental panel to display available temperature and humidity