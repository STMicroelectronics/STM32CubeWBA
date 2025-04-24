## __BLE_Audio_HAP_Peripheral Application Description__

How to use the Hearing Access Profile in Unicast Server role (Hearing Aid role) as specified by the Bluetooth SIG.


The HAP Peripheral application, in __Unicast Server role__, performs the following actions:<br>
- Advertises and accept connection from remote devices.<br>
- Accepts Isochronous Connection from a remote HAP Central (Hearing Aid Unicast Client).<br>
- Supports Volume Control, Call Control, and Microphone Control features.<br>
- Supports Hearing Head Preset Control features.<br>
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices with SMPS.
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
	- __Start Adv__ : the application starts the features associated to the HAP Unicast Server role (Unicast Server, Call Control, Microphone Control, Volume Control).<br>
	- __Audio Config__ : configure Audio device type of the HAP Peripheral <br>
	- __Clear Sec. DB__ : clear all the information of the bonded devices
<br>
 - __Start Adv__
<br>Once "Start Adv" in HAP Peripheral is executed, the HAP Peripheral starts advertising with a name displayed on the screen and wait for an incoming connection from a remote Unicast Client.
	- Using a device compatible with BLE Audio Unicast Client role (Smartphone supporting LE Audio Feature for example), connect to the HAP Peripheral and perform Link-Up of available services.
	- Once the Unicast Client is connected, it can start a streaming procedure (Media Playing or Call) to send audio to the HAP Peripheral. High Quality Media unidirectional streams can be established, as well as conversational bidirectional streams
	- Once connected to the Unicast Client, use the Right direction on the joystick to access the menu:
	  - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Renderer role (Volume Up / Volume Down / Mute)<br>
	  - __Select Microphone Control__: Access a panel where the microphone mute state can be adjusted and displayed using the MICP as MICP Device role (Microphone Mute)<br>
	  - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Client (Incoming Call / Answer Call / Terminate Call)<br>
	  - __Select Preset Control__: Access a panel where the active Hearing Aid preset can be selected using the HAP as Hearing Aid role (Next Preset / Previous Preset)<br>
	  - __Select Disconnect__: Disconnect the connected devices
<br>
 - __Audio Config__
<br>Once "Audio Config" in HAP Peripheral is executed, use the Joystck Up/Down direction to select one of the Hearing Head Audio device configuration:
	  - __Banded Hearing Aid__<br>
	  - __Right Binaural Hearing Aid__<br>
	  - __Left Binaural Hearing Aid__<br>
	  - __Monaural Hearing Aid__<br>

    Push Left direction on the joystick to validate the Audio Configuration
<br>
 - __Clear Security Database__
<br>Once "Clear Sec. DB" in HAP Peripheral is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developped by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Hearing_Access_Profile"> Bluetooth® Low Energy audio - Hearing Access Profile</a>

