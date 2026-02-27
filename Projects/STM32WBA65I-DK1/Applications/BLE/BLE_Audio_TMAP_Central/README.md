## __BLE_Audio_TMAP_Central Application Description__

How to use the Telephony and Media Audio Profile (TMAP) in Unicast Client role (Call Gateway and/or Unicast Media Sender) and Broadcast Source role (Broadcast Media Sender) as specified by the Bluetooth SIG.


The TMAP Central application, in __Unicast Client role__, performs the following actions:<br>
- Scans and connects to remote Unicast Servers (Call Terminal and/or Unicast Media Receiver).<br>
- Initiate Isochronous Connection.<br>
- Supports Volume Control, Call Control, and Media Control features.<br>

The TMAP Central application, in __Broadcast Source role__, advertises and broadcasts audio streams to remote Broadcast Sinks.<br>
<br>

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This example runs on STM32WBA65xx devices with SMPS.
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
	- __Start Unicast__ : the application starts the features associated to the TMAP Unicast Client role (Unicast Client, Media Control, Call Control, Volume Control).<br>
	- __Start Broadcast__ : the application starts the features associated to the TMAP Broadcast Source role.<br>
	- __Clear Sec. DB__ : clear all the information of the bonded devices
<br>
 - __Start Unicast__
<br>Once "Start Unicast" in TMAP Central is executed, the TMAP Central starts scanning of nearby Unicast Servers
	- Select the Unicast Server you want to connect with the Right direction on the joystick. Pairing and Link-up are initiated automatically.<br>
  If the remote TMAP Peripheral is a Set member of a Coordinated Set, the application starts discovery and linkup of the other Set members of a Coordinated Set (if CFG_BLE_NUM_LINK > 1 and APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY = 1)
	- Once connected to the Unicast Server, use the Right direction on the joystick to access the menu:
		- __Select Audio Stream__: Access a submenu where you can :
          - Start Media (48KHz unidirectional stereo stream).
          - Start Telephony (32KHz bidirectional mono stream if connected to one audio device ; 16KHz when connected to 2 Set members of a Coordinated Set).
          - Stop the current stream.
	  - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Controller role (Volume Up / Volume Down / Mute)<br>
	  - __Select Local Volume Control__: Access a panel where the local volume can be adjusted (Volume Up / Volume Down / Mute)<br>
	  - __Select Media Control__: Access a panel where the local media tracks can be controlled and displayed using the MCP as Media Control Server (Next Track / Previous Track / Play|Pause)<br>
	  - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Server (Incoming Call / Answer Call / Terminate Call)<br>
	  - __Select Disconnect__: Disconnect the connected devices
<br>
 - __Start Broadcast__
<br>Once "Start Broadcast" in TMAP Central is executed, the application first offers to select a Streaming frequency configuration in a menu.
    - Once Streaming frequency selected by using Up/Down direction, push the joystick in the Right direction to start Audio Stream Broadcast.
    - Once Broadcasting is started, to stop broadcasting, push the joystick in the Left direction.
<br>
 - __Clear Security Database__
<br>Once "Clear Sec. DB" in TMAP Central is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

   - Wiki pages related to the LE Audio solutions developed by STMicroelectronics are available here:
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth® Low Energy Audio</a>
	 - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth® Low Energy audio - STM32WBA LC3 codec and audio data path</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth® Low Energy audio - STM32WBA Architecture and Integration</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth® Low Energy audio - Content Control</a>
     - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Telephony_%26_Media_Audio_Profile"> Bluetooth® Low Energy audio - Telephony & Media Audio Profile</a>
