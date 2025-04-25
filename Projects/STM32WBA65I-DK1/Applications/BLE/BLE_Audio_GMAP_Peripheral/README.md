## __BLE_Audio_GMAP_Peripheral Application Description__

How to use the Gaming Audio Profile profile in Unicast Server role (Unicast Game Terminal) and Broadcast Sink role (Broadcast Game Receiver) as specified by the Bluetooth SIG.


The GMAP Peripheral application, in __Unicast Server role__, performs the following actions:<br>
- Advertises and accept connection from remote devices.<br>
- Accepts Isochronous Connection from a remote GMAP Central (Unicast Game Gateway).<br>
- Supports Volume Control, and Microphone Control features.<br>

The GMAP Peripheral application, in __Broadcast Sink role__, scans and synchronizes audio streams from remote Broadcast Sources.<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices with SMPS.
  <br>Connect the Discovery Board to your PC with a USB cable type C
  <br>Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
<br>
<br>
 - __At Startup__
 <br>After pressing the Right direction on the joystick, the user accesses a menu offering following actions:
	- __Start Unicast__ : the application starts the features associated to the GMAP Unicast Server role (Unicast Server, Media Control, Call Control, Volume Control).<br>
	- __Start Broadcast__ : the application starts the features associated to the GMAP Broadcast Sink role.<br>
   __Audio Config__ : configure Audio device type of the GMAP Peripheral <br>
	- __Clear Sec. DB__ : clear all the information of the bonded devices
<br>
 - __Start Unicast __
<br>Once "Start Unicast" in GMAP Peripheral is executed, the GMAP Peripheral starts advertising with a name displayed on the screen and wait for an incoming connection from a remote Unicast Client.
	- Using a device compatible with BLE Audio Unicast Client role (Smartphone supporting LE Audio Feature for example), connect to the GMAP Peripheral and perform Link-Up of available services.
	- Once the Unicast Client is connected, it can start a streaming procedure to send audio to the GMAP Peripheral. High Quality Media unidirectional streams can be established, as well as conversational bidirectional streams
	- Once connected to the Unicast Client, use the Right direction on the joystick to access the menu:
	  - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Renderer role (Volume Up / Volume Down / Mute)<br>
	  - __Select Microphone Control__: Access a panel where the mute state of the microphone can be set (Microphone Mute / Microphone Unmute)<br>
	  - __Select Disconnect__: Disconnect the connected devices
<br>
 - __Start Broadcast__
<br>Once "Start Broadcast" in GMAP Peripheral is executed, the application scans the remote Broadcast Sources and display them on the LCD.
    - Select with the Joystick the Broadcast Source to synchronize to.
    - Once the GMAP Peripheral is synchronized to the Broadcast Source and Audio Stream is received, Audio is available on Jack Connector.
    - Once synchronization with the Broadcast Source is complete, using the Right direction on the joystick, local volume can be modified and Audio Sink can be stopped (Broadcast desynchronization).
<br>
 - __Audio Config__
<br>Once "Audio Config" in GMAP Peripheral is executed, use the Joystck Up/Down direction to select one of the Audio device configuration:
	  - __Headphone__<br>
	  - __Earbud Right__<br>
	  - __Earbud Left__<br>

    Push Left direction on the joystick to validate the Audio Configuration
<br>
 - __Clear Security Database__
<br>Once "Clear Sec. DB" in GMAP Peripheral is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developped by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Gaming_Audio_Profile"> Bluetooth® Low Energy audio - Gaming Audio Profile</a>
