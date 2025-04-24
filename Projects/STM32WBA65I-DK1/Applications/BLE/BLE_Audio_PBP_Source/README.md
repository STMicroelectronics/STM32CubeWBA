## __BLE_Audio_PBP_Source Application Description__

How to use the Public Broadcast profile in Source role as specified by the Bluetooth SIG.


The PBP Source application advertises and broadcasts audio streams to remote PBP Sinks.
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices with SMPS.
  <br>Connect the Discovery Board to your PC with a USB cable type C
  <br>Connect an Audio Input on Jack Connector CN4.

  - Another Discovery board may be necessary to run PBP Sink application.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
<br>
<br>
 - PBP Source ID is configurable thanks to BAP_BROADCAST_SOURCE_ID in pbp_app_conf.h file.
<br>
 - __At Startup__, PBP Source starts Audio Stream Broadcast<br>Stream Audio in the Jack Connector CN4.<br>Source ID is displayed on the LCD interface
 - Power up a PBP Sink device with Source ID matching with the BAP_BROADCAST_SOURCE_ID value
 - Once PBP Source application is started and Audio Stream Broadcast is launched :
    - On the PBP Sink device, select with the Joystick the Source ID of the remote PBP Source to synchronize to.
 - Once PBP Sink device is synchronized to PBP Source and Audio Stream is received, Audio is available on Jack Connector.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developped by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Public_Broadcast_Profile"> Bluetooth® Low Energy audio - STM32WBA Public broadcast profile</a>

