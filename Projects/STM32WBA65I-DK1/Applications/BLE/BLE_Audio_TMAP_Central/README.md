## __BLE_Audio_TMAP_Central Application Description__

How to use the Telephony and Media Audio Profile (TMAP) in Unicast Client role (Call Gateway and/or Unicast Media Sender) and Broadcast Source role (Broadcast Media Sender) as specified by the Bluetooth SIG.

The TMAP Central application, in __Unicast Client role__, performs the following actions:

- Scans and connects to remote Unicast Servers (Call Terminal and/or Unicast Media Receiver).
- Initiates Isochronous Connection.
- Supports Volume Control, Call Control, and Media Control features.

The TMAP Central application, in __Broadcast Source role__, advertises and broadcasts audio streams to remote Broadcast Sinks.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This application runs on STM32WBA65xx Discovery kit.

  - Connect the Discovery Board to your PC with a USB cable type C.
  - Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.
  - Connect Audio Input on Jack Connector CN4 for Media streaming.

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

- __At Startup__
  - After pressing the Right direction on the joystick, the user accesses a menu offering following actions:
    - __Start Unicast__: the application starts the features associated to the TMAP Unicast Client role (Unicast Client, Media Control, Call Control, Volume Control).
    - __Start Broadcast__: the application starts the features associated to the TMAP Broadcast Source role.
    - __Clear Sec. DB__: clear all the information of the bonded devices.

- __Start Unicast__
  - Once "Start Unicast" in TMAP Central is executed, the TMAP Central starts scanning of nearby Unicast Servers.
    - Select the Unicast Server you want to connect with the Right direction on the joystick. Pairing and Link-up are initiated automatically.
    - If the remote TMAP Peripheral is a Set member of a Coordinated Set, the application starts discovery and linkup of the other Set members of a Coordinated Set (if CFG_BLE_NUM_LINK > 1 and APP_CSIP_AUTOMATIC_SET_MEMBERS_DISCOVERY = 1)
  - Once connected to the Unicast Server, use the Right direction on the joystick to access the menu:
    - __Select Audio Stream__: Access a submenu where you can :
      - Start Media (48KHz unidirectional stereo stream).
      - Start Telephony (32KHz bidirectional mono stream if connected to one audio device ; 16KHz when connected to 2 Set members of a Coordinated Set).
      - Stop the current stream.
    - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Controller role (Volume Up / Volume Down / Mute).
    - __Select Local Volume Control__: Access a panel where the local volume can be adjusted (Volume Up / Volume Down / Mute).
    - __Select Media Control__: Access a panel where the local media tracks can be controlled and displayed using the MCP as Media Control Server (Next Track / Previous Track / Play|Pause).
    - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Server (Incoming Call / Answer Call / Terminate Call).
    - __Select Disconnect__: Disconnect the connected devices.

- __Start Broadcast__
  - Once "Start Broadcast" in TMAP Central is executed, the application first offers to select a Streaming frequency configuration in a menu.
  - Once Streaming frequency selected by using Up/Down direction, push the joystick in the Right direction to start Audio Stream Broadcast.
  - Once Broadcasting is started, to stop broadcasting, push the joystick in the Left direction.

- __Clear Security Database__
  - Once "Clear Sec. DB" in TMAP Central is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

Wiki pages related to the LE Audio solutions developed by STMicroelectronics are available here:

  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth Low Energy Audio</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth Low Energy Audio - STM32WBA LC3 codec and audio data path</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth Low Energy Audio - STM32WBA Architecture and Integration</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth Low Energy Audio - Content Control</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Telephony_%26_Media_Audio_Profile"> Bluetooth Low Energy Audio - Telephony & Media Audio Profile</a>
