## __BLE_Audio_PBP_Source Application Description__

How to use the Public Broadcast profile in Source role as specified by the Bluetooth SIG.


The PBP Source application advertises and broadcasts audio streams to remote PBP Sinks.
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE profile, BLE audio, Auracast

### __Hardware and Software environment__

 - This example runs on STM32WBA55xx devices with SMPS.
   - Connect the Discovery Board to your PC with a USB cable type C
   - Connect an Audio Input on Jack Connector CN4.
 - Another Discovery board may be necessary to run PBP Sink application.

### __How to use it?__

To make the program work, you must do the following:

 - Open your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.
<br>
<br>
 - PBP Broadcast Name is configurable via the aPBPAPP_BroadcastName variable in the pbp_app.c file.
 - __At Startup__,
   - The PBP Source starts broadcasting the audio stream.
   - The audio is streamed through the Jack Connector CN4.
   - The Broadcast Name is displayed on the LCD interface.
 - Power up a PBP Sink device searching for the Broadcast Name that matches the aPBPAPP_BroadcastName value.
 - Once the PBP Source application starts and the Audio Stream Broadcast is streaming:
    - On the PBP Sink device, use the joystick to select the Broadcast Name of the remote PBP Source to synchronize to.
 - Once the PBP Sink device is synchronized with the PBP Source and the audio stream is received, the audio will be available through the Jack Connector.

### __Documentation__

Wiki pages related to the LE Audio solutions developed by STMicroelectronics are available here:
 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Public_Broadcast_Profile"> Bluetooth® Low Energy audio - STM32WBA Public broadcast profile</a>

