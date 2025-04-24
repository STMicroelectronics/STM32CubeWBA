## __BLE_Audio_PBP_Sink Application Description__

How to use the Public Broadcast profile in Sink role as specified by the Bluetooth SIG.


The PBP Sink application scans and synchronizes audio streams from remote PBP Sources.
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices with SMPS.
  <br>Connect the Discovery Board to your PC with a USB cable type C
  <br>Connect a headset on Jack Connector CN3.

  - Another Discovery board may be necessary to run PBP Source application.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
<br>
<br>
 - Target PBP Source ID is configurable thanks to the aSourceIdList[] table in pbp_app.c.
<br>
 - __At Startup__, PBP Sink starts BLE Scanning and look for remote PBP Source Device associated to the first Source ID (SCAN_INTERVAL and SCAN_WINDOW of the scanning are configurable in pbp_app.c file.).<br>PBP Sink starts automatically the Audio Synchronization process once PBP Source is found.
 - On the PBP Sink device, use Up/Down direction of the Joystick to change the Source ID to synchronize to. Target Source ID is displayed on the LCD interface.
 - Power up PBP Source device with Source ID matching with one of the ID listed in PBP Sink application
 - Once PBP Source application is started, on the PBP Sink device, select with the joystick the Source ID of the remote PBP Source to synchronize to.
 - Once PBP Sink device is synchronized to PBP Source and Audio Stream is received, Audio is available on Jack Connector.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developped by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Public_Broadcast_Profile"> Bluetooth® Low Energy audio - STM32WBA Public broadcast profile</a>

