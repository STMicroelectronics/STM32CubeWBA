## __BLE_Audio_HAP_Central Application Description__

How to use the Hearing Access Profile (HAP) in Unicast Client role (Hearing Aid Unicast Client / Hearing Aid Remote Controller / Immediate Alert Client) as specified by the Bluetooth SIG.


The HAP Central application, in __Unicast Client role__, performs the following actions:<br>
- Scans and connects to remote Unicast Servers (Hearing Aid).<br>
- Initiate Isochronous Connection.<br>
- Supports Volume Control, Call Control, and Microphone Control features.<br>
- Supports VHearing Head Preset Control features.<br>
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA55xx devices with SMPS.
  <br>Connect the Discovery Board to your PC with a USB cable type C
  <br>Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.
  <br>Connect Audio Input on Jack Connector CN4 for Media streaming

### __How to use it?__

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
<br>
<br>
 - __At Startup__
 <br>After pressing the Right direction on the joystick, the user accesses a menu offering following actions:
	- __Start Scan__ : the application starts the features associated to the HAP Unicast Client role (Unicast Client, Call Control, Volume Control, HA Preset Control).<br>
	- __Clear Sec. DB__ : clear all the information of the bonded devices
<br>
 - __Start Scan__
<br>Once "Start Scan" in HAP Central is executed, the HAP Central starts scanning of nearby Unicast Servers
	- Select the Unicast Server you want to connect with the Right direction on the joystick. Pairing and Link-up are initiated automatically.<br>
  If the remote HAP Peripheral is a Set member of a Coordinated Set, the application starts discovery and linkup of the other Set members of a Coordinated Set (if CFG_BLE_NUM_LINK > 1 and APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY = 1)
	- Once connected to the Unicast Server, use the Right direction on the joystick to access the menu:
		- __Select Audio Stream__: Access a submenu where you can :
          - Start Media (24KHz unidirectional stereo stream).
          - Start Telephony (16KHz bidirectional mono stream).
          - Stop the current stream.
	  - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Controller role (Volume Up / Volume Down / Mute)<br>
	  - __Select Local Volume Control__: Access a panel where the local volume can be adjusted (Volume Up / Volume Down / Mute)<br>
	  - __Select Microphone Control__: Access a panel where the microphone mute status of the remote device can be controlled and displayed using the MICP Microphone Controller (Mute)<br>
	  - __Select Call Control__: Access a panel where the remote active Hearing Aid preset can be selected using the HAP as Hearing Aid Remote Controller role (Next Preset / Previous Preset)<br>
	  - __Select Preset Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Server (Incoming Call / Answer Call / Terminate Call)<br>
	  - __Select Disconnect__: Disconnect the connected devices
<br>
 - __Clear Security Database__
<br>Once "Clear Sec. DB" in HAP Central is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developed by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Hearing_Access_Profile"> Bluetooth® Low Energy audio - Hearing Access Profile</a>