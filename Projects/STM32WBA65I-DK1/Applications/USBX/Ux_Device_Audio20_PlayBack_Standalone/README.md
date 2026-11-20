## <b>Ux_Device_Audio20_PlayBack_Standalone Application Description</b>

This application provides an example of standalone USBX stack usage on STM32WBA65I-DK1 board, it shows how to develop USB Device "AUDIO" Class based application.

The application is designed to emulate an USB (Out: Speaker/Headset), the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB audio 2.0 device.

This application supports the feedback endpoint feature.

This is a typical application on how to use the STM32WBA6xx USB OTG Device peripheral and I2S peripheral to stream audio data from USB Host to the audio codec implemented on the STM32WBA65I-DK1 board.

The device supports the following audio features:
  - Pulse Coded Modulation (PCM) format
  - Sampling rate: 48 kHz
  - Bit resolution: 16
  - Number of channels: 2
  - Mute capability
  - Asynchronous endpoints
  - Feedback endpoint

#### <b>Expected success behavior</b>

When plugged into the PC host, the STM32WBA65I-DK1 must be properly enumerated as an USB speaker device.
During the enumeration phase, the device must provide the host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).
These descriptors are used by host driver to identify the device capabilities. Once STM32WBA65I-DK1 USB device successfully completed the enumeration phase,
start streaming audio.

#### <b>Error behaviors</b>

The host PC indicates that the USB device does not operate as designed (Audio device enumeration failed).

#### <b>Assumptions if any</b>

User is familiar with the USB 2.0 "Universal Serial Bus" specification and the Audio 2.0 class specification.

#### <b>Known limitations</b>

None

#### <b>Notes</b>

 1. A Product ID (PID) change is recommended when modifying the application to ensure the host treats it as a new device.
    This is especially critical when updating frequencies, as the host may otherwise retain stale descriptors.
 2. To run application at a frequency of 44.1 kHz, set the following value in your descriptors:
        #define USBD_AUDIO_PLAY_FREQ_MAX  USBD_AUDIO_FREQ_44_1_K

#### <b>USBX usage hints</b>

- None

### <b>Keywords</b>

USBXDevice, USB_OTG, High speed, SAI, Audio, Standalone, Streaming, PCM, Feedback

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA65RIVx devices.
  - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

- STM32WBA65I-DK1 Set-up
    - Plug the USB HID device into the STM32WBA65I-DK1 board through 'Type C  to A-Female' cable to the connector:
      - CN5 : to use USB High Speed OTG IP
    - Plug headset on CN3 connector (Headset).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Open an audio player application (Windows Media Player) and play music on the PC host
