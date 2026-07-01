## __BLE_Audio_TMAP_Peripheral Application Description__

How to use the Telephony and Media Audio Profile (TMAP) in Unicast Server role (Call Terminal and/or Unicast Media Receiver), Broadcast Sink role (Broadcast Media Receiver) and Scan Delegator as specified by the Bluetooth SIG.

The TMAP Peripheral application, in __Unicast Server role__, performs the following actions:

- Advertises and accepts connection from remote devices.
- Accepts Isochronous Connection from a remote TMAP Central (Call Gateway and/or Unicast Media Sender).
- Supports Volume Control, Call Control, and Media Control features.

The TMAP Peripheral application, in __Broadcast Sink role__, scans and synchronizes audio streams from remote Broadcast Sources.

The TMAP Peripheral application, in __Scan Delegator role__, performs the following actions:

- Advertises and accepts connection from remote devices.
- Accepts, scans and synchronizes audio streams from remote Broadcast Sources added by remote Broadcast Assistants.
- Supports Volume Control, Call Control, and Media Control features.

### __Keywords__

Connectivity, BLE, BLE protocol, BLE pairing, BLE profile, BLE audio

### __Hardware and Software environment__

  - This application runs on STM32WBA55xx Discovery kit.

  - Connect the Discovery Board to your PC with a USB cable type C.
  - Connect a headset (optionally with microphone to enable bidirectional streams) on Jack Connector CN3.

### __How to use it?__

In order to make the program work:

 - Open the project with your preferred toolchain.
 - Rebuild all files and load your image into target memory.
 - Run the example.

- __At Startup__
  - After pressing the Right direction on the joystick, the user accesses a menu offering following actions:
    - __Start Unicast__: the application starts the features associated to the TMAP Unicast Server role (Unicast Server, Media Control, Call Control, Volume Control).
    - __Start Broadcast__: the application starts the features associated to the TMAP Broadcast Sink role.
    - __Start Scan Delegator__: the application starts the features associated to the Scan Delegator role.
    - __Audio Config__: configure Audio device type of the TMAP Peripheral.
    - __Clear Sec. DB__: clear all the information of the bonded devices.

- __Start Unicast__
  - Once "Start Unicast" in TMAP Peripheral is executed, the TMAP Peripheral starts advertising with a name displayed on the screen and wait for an incoming connection from a remote Unicast Client.
  - Using a device compatible with BLE Audio Unicast Client role (Smartphone supporting LE Audio Feature for example), connect to the TMAP Peripheral and perform Link-Up of available services.
  - Once the Unicast Client is connected, it can start a streaming procedure (Media Playing or Call) to send audio to the TMAP Peripheral. High Quality Media unidirectional streams can be established, as well as conversational bidirectional streams.
  - Once connected to the Unicast Client, use the Right direction on the joystick to access the menu:
    - __Select Remote Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Renderer role (Volume Up / Volume Down / Mute).
    - __Select Media Control__: Access a panel where the local media tracks can be controlled and displayed using the MCP as Media Control Client (Next Track / Previous Track / Play|Pause).
    - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Client (Incoming Call / Answer Call / Terminate Call).
    - __Select Disconnect__: Disconnect the connected devices.

- __Start Broadcast__
  - Once "Start Broadcast" in TMAP Peripheral is executed, the application scans the remote Broadcast Sources and display them on the LCD.
  - Select with the Joystick the Broadcast Source to synchronize to.
  - Once the TMAP Peripheral is synchronized to the Broadcast Source and Audio Stream is received, Audio is available on Jack Connector.
  - Once synchronization with the Broadcast Source is complete, using the Right direction on the joystick, local volume can be modified and Audio Sink can be stopped (Broadcast desynchronization).

- __Start Scan Delegator__
  - Once "Start Scan Delegator" in TMAP Peripheral is executed, starts advertising with a name displayed on the screen and wait for an incoming connection from a remote Broadcast Assistant.
  - Using a device compatible with BLE Audio Broadcast Assistant role (Specific smartphones supporting LE Audio Feature for example), connect to the TMAP Peripheral and perform Link-Up of available services.
  - Once the Broadcast Assistant is connected, it can perform an Audio Reception Start procedure to send Broadcast Source information to the TMAP Peripheral. The TMAP Peripheral will try to synchronize to the Broadcast Source if asked by the Broadcast Assistant.
  - Once connected to the Broadcast Assistant, use the Right direction on the joystick to access the menu:
    - __Select Volume__: Access a panel where the volume of the remote device can be adjusted using the VCP as VCP Renderer role (Volume Up / Volume Down / Mute).
    - __Select Media Control__: Access a panel where the local media tracks can be controlled and displayed using the MCP as Media Control Client (Next Track / Previous Track / Play|Pause).
    - __Select Call Control__: Access a panel where the local calls can be controlled and displayed using the CCP as Call Control Client (Incoming Call / Answer Call / Terminate Call).
    - __Select Disconnect__: Disconnect the connected devices and stops ongoing synchronization.

- __Audio Config__
  - Once "Audio Config" in TMAP Peripheral is executed, use the Joystck Up/Down direction to select one of the Audio device configuration:
    - __Headphones__
    - __Earbud Right__
    - __Earbud Left__
  - Push Left direction on the joystick to validate the Audio Configuration.

- __Clear Security Database__
  - Once "Clear Sec. DB" in TMAP Peripheral is executed, all the information stored in Non-Volatile Memory related to bonded devices are removed.

### __Documentation__

Wiki pages related to the LE Audio solutions developed by STMicroelectronics are available here:

  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Introduction_to_Bluetooth_LE_Audio"> Introduction to Bluetooth Low Energy Audio</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_LC3_Codec"> Bluetooth Low Energy Audio - STM32WBA LC3 codec and audio data path</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Architecture_and_Integration"> Bluetooth Low Energy Audio - STM32WBA Architecture and Integration</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_Content_Control"> Bluetooth Low Energy Audio - Content Control</a>
  - <a href="https://wiki.st.com/stm32mcu/wiki/Connectivity:Bluetooth_LE_Audio_-_STM32WBA_Telephony_%26_Media_Audio_Profile"> Bluetooth Low Energy Audio - Telephony & Media Audio Profile</a>
