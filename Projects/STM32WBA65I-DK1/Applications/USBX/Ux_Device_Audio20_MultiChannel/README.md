## <b>Ux_Device_Audio20_MultiChannel Application Description</b>

This application provides an example of Azure RTOS USBX device stack usage on the STM32WBA65I-DK1 board.
It demonstrates a USB Audio Class 2.0 loopback device based on USBX.

The application exposes both audio playback and recording streaming interfaces.
Audio data received from the host on the playback OUT endpoint is buffered by the application and sent back to the host on the recording IN endpoint.

At startup, ThreadX calls the entry function tx_application_define(). At this stage, all USBX resources
are initialized, the AUDIO Class driver is registered, and the application creates two threads:

  - app_ux_device_thread_entry (Prio: 20; PreemptionPrio: 20) used to initialize the USB_DRD HAL PCD driver and start the device.
  - playback_monitor_thread    (Prio: 31; PreemptionPrio: 31) used to monitor and log execution profile of system threads.

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
 - The UART terminal periodically displays execution profile percentage values, providing a direct indication of CPU load during execution.
 - The log includes entries such as `ux_device_class_audio_stream_thread`, `Idle System`, and `Interrupt`, typically updated about every 10 seconds.

#### <b>Error behaviors</b>

The host PC shows that the USB device does not operate as designed, or the audio device enumeration fails.
The test should also be considered failed if the user cannot see the audio loopback result or if the UART terminal does not show the expected execution profile percentage values.

#### <b>Assumptions if any</b>

User is familiar with the USB 2.0 Universal Serial Bus specification and the Audio 2.0 class specification.

#### <b>Known limitations</b>

 - Footprint is not optimized.

### <b>Notes</b>

 1. A Product ID (PID) change is recommended when modifying the application to ensure the host treats it as a new device.
    This is especially critical when updating frequencies, as the host may otherwise retain stale descriptors.

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default. This should be taken into account when using delays or timeouts at application level. It can be reconfigured in `tx_user.h` through `TX_TIMER_TICKS_PER_SECOND`, but this should also be reflected in `tx_initialize_low_level.s`.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the `tx_application_define()` function, which is automatically called by the `tx_kernel_enter()` API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the `tx_application_define()` function using the `first_unused_memory` argument.
   This requires changes in the linker files to expose this memory location.
   - For EWARM, add the following section into the `.icf` file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```
   - For MDK-ARM:
     ```
     either define the RW_IRAM1 region in the `.sct` file
     or modify the line below in `tx_low_level_initilize.s` to match the memory region being used
         LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
     ```
   - For STM32CubeIDE, add the following section into the `.ld` file:
     ```
     ._threadx_heap :
       {
          . = ALIGN(8);
          __RAM_segment_used_end__ = .;
          . = . + 64K;
          . = ALIGN(8);
        } >RAM_D1 AT> RAM_D1
     ```

     The simplest way to provide memory for ThreadX is to define a new section, see `._threadx_heap` above.
     In the example above the ThreadX heap size is set to 64 Kbytes.
     The `._threadx_heap` must be located between the `.bss` and the `._user_heap_stack` sections in the linker script.
     Caution: Make sure that ThreadX does not need more than the provided heap memory in this example.
     Read more in STM32CubeIDE User Guide, chapter `Linker script`.

   - The `tx_initialize_low_level.s` file should also be modified to enable the `USE_DYNAMIC_MEMORY_ALLOCATION` flag.

### <b>Keywords</b>

RTOS, ThreadX, USBX, Device, USB_OTG, High speed, Feedback, Audio, Microphone, Streaming, PCM, TX_EXECUTION_PROFILE, UART/USART


### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA65RIVx devices.
  - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

  - STM32WBA65I-DK1 Set-up
    - Plug the USB HID device into the STM32WBA65I-DK1 board through 'Type C  to A-Female' cable to the connector:
      - CN5 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port.

  - A virtual COM port will then appear in the HyperTerminal:
     - Hyperterminal configuration:
       - Data Length = 8 Bits
       - One Stop Bit
       - No parity
       - BaudRate = 115200 baud
       - Flow control: None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Open the serial terminal and check that execution profile percentage values are printed periodically
 - Verify that the PC detects the STM32 board as one speaker and one microphone
 - Use the provided loopback validation script for the recommended host-side test

#### <b>Validation tool usage</b>

 - Open a Windows command prompt in the `Ux_Device_Audio20_MultiChannel` folder and run the following commands.
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
 - This file matches the standard test configuration: 48 kHz, 4 channels, 16-bit PCM.
 - The script can also be used with other reference WAV files, provided they are 16-bit PCM and compatible with the selected device format.
 - Before running the script command, ensure that “STM32 AUDIO20 STREAMING” is not selected in the Windows sound settings (Speakers/Microphone) or in Audacity.

#### <b>Available commands</b>

 - `validate`: recommended full test command.
 - `playrec`: recording only.
 - `analyze`: analysis only.
 - `--output-file`: path to the generated output WAV file.
 - `STM32 AUDIO20 STREAMING` is the USB product string.
 - `--monitor-rms` is optional and prints live input RMS values during recording.
 - `PASS` means the recorded WAV matches the expected STM32 loopback behavior.
 - `FAIL` usually indicates a wrong device selection, missing loopback audio, or a format mismatch.