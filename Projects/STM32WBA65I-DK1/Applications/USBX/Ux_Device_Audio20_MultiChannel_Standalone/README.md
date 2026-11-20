## <b>Ux_Device_Audio20_MultiChannel_Standalone Application Description</b>

This application provides an example of standalone USBX device stack usage on the STM32WBA65I-DK1 board.
It demonstrates a USB Audio Class 2.0 loopback device based on USBX.

The application exposes both audio playback and recording streaming interfaces.
Audio data received from the host on the playback OUT endpoint is buffered by the application and sent back to the host on the recording IN endpoint.

The application's main function calls `MX_USBX_Init()` to initialize USBX, then repeatedly calls `USBX_Device_Process()` in the infinite loop to handle the device stack.
The device supports the following audio features:

  - Pulse Coded Modulation (PCM) format
  - Sampling rate: 48 kHz
  - Bit resolution: 16 bits
  - Number of channels: 4 IN and 4 OUT
  - Playback and recording audio streams
  - Asynchronous endpoints
  - Feedback Endpoint

#### <b>Expected success behavior</b>

When connected to the PC host, the STM32WBA65I-DK1 must be properly enumerated as a USB Audio Class 2.0 device exposing one speaker interface and one microphone interface.
During enumeration, the device provides the required device, configuration, class, and string descriptors.
Once enumeration is complete, the host can start audio streaming and the playback data received by the STM32 is returned on the recording stream.
Under normal operating conditions, the user should observe the following:

 - The PC detects the STM32 board as one speaker device and one microphone device.
 - Audio sent by the host to the STM32 speaker stream is returned on the STM32 microphone stream.
 - The recorded output stream remains adequate to the host input stream in format, cadence, and channel mapping in case of successful loopback operation.

#### <b>Error behaviors</b>

The host PC shows that the USB device does not operate as designed, or the audio device enumeration fails.
The test should also be considered failed if the user cannot observe the expected audio loopback result on the host side.

#### <b>Assumptions if any</b>

User is familiar with the USB 2.0 Universal Serial Bus specification and the Audio 2.0 class specification.

#### <b>Known limitations</b>

 - Footprint is not optimized.

### <b>Notes</b>

 1. A Product ID (PID) change is recommended when modifying the application to ensure the host treats it as a new device. This is especially important when updating frequencies, as the host may retain stale descriptors.

#### <b>USBX usage hints</b>

 - Configure the USBX memory pool and streaming buffers in SRAM accessible by the USB peripheral.
 - Keep audio buffer alignment and endpoint packet sizing consistent with the active stream format.

### <b>Keywords</b>

USBX, Device, USB_OTG, High speed, Feedback, Audio, Microphone, Streaming, PCM, Standalone


### <b>Hardware and Software environment</b>

 - This application runs on STM32WBA65RIVx devices.
 - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

 - STM32WBA65I-DK1 Set-up
   - Plug the USB HID device into the STM32WBA65I-DK1 board through 'Type C to A-Female' cable to the connector:
     - CN5: to use USB High Speed OTG IP
   - Connect ST-Link cable to the PC USB port.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Verify that the PC detects the STM32 board as one speaker and one microphone
 - Use the provided loopback validation script for the recommended host-side test

#### <b>Validation tool usage</b>

 - Open a Windows command prompt in the `Ux_Device_Audio20_MultiChannel_Standalone` folder and run the following commands.
 - Validation script: `Loopback_Validation\run_loopback_validation.py`
 - The script uses an existing reference WAV file. It does not generate a synthetic reference automatically.

#### <b>Recommended test sequence</b>

 - Install the Python packages with: `python -m pip install -r .\Loopback_Validation\requirements.txt`
 - List the host audio devices with: `python .\Loopback_Validation\run_loopback_validation.py devices`
 - Identify the STM32 speaker name and the STM32 microphone name reported by the PC.
 - Run the validation command shown below.
 - Wait for `Recording complete`, then `Analyzing recording`, then the final `PASS` or `FAIL` result.

#### <b>Example validation command</b>
```
python .\Loopback_Validation\run_loopback_validation.py validate --input-file .\Loopback_Validation\loopback_reference_4ch.wav --output-file .\Loopback_Validation\loopback_capture.wav --input-device "STM32 AUDIO20 STREAMING" --output-device "STM32 AUDIO20 STREAMING" --monitor-rms
```

#### <b>Notes for use</b>

 - The reference WAV file must exist before running the test.
 - The default reference file is `Loopback_Validation\loopback_reference_4ch.wav`.
 - This file matches the standard standalone test configuration: 48 kHz, 4 channels, 16-bit PCM.
 - The script can also be used with other reference WAV files, provided they are 16-bit PCM and compatible with the selected device format.
 - Before running the script command, ensure that “STM32 AUDIO20 STREAMING” is not selected in the Windows sound settings (Speakers/Microphone) or in Audacity.

#### <b>Available commands</b>

 - `devices`: list host audio devices.
 - `validate`: recommended full test command.
 - `playrec`: recording only.
 - `analyze`: analysis only.
 - `--output-file`: path to the generated output WAV file.
 - `--monitor-rms` is optional and prints live input RMS values during recording.
 - `PASS` means the recorded WAV matches the expected STM32 loopback behavior.
 - `FAIL` usually indicates a wrong device selection, missing loopback audio, or a format mismatch.