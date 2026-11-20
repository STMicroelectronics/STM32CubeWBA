## <b>Ux_Device_Audio20_PlayBack Application Description</b>

This application provides an example of Azure RTOS USBX stack usage on STM32WBA65I-DK1 board.
It shows how to develop a USB Device Audio Class application.

The application is designed to emulate a USB output device (speaker/headset). The code provides all required device descriptor frameworks
and associated class-specific descriptors to build a compliant USB audio 2.0 device.

This application supports the feedback endpoint feature.

This application supports the ThreadX Execution Profile for logging thread, ISR, and idle execution time by UART on the HyperTerminal.

At startup, ThreadX calls the entry function tx_application_define(). At this stage, all USBX resources
are initialized, the audio class driver is registered, and the application creates three threads:

  - app_ux_device_thread_entry (Prio: 20; PreemptionPrio: 20): initializes the USB_OTG HAL PCD driver and starts the device.
  - usbx_audio_play_app_thread (Prio: 20; PreemptionPrio: 20): starts the audio streaming from the PC host machine.
  - playback_monitor_thread (Prio: 31; PreemptionPrio: 31): monitors and logs the execution profile of system threads.

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
When ThreadX execution profiling is enabled, the UART log on the HyperTerminal must also print, by default every 10 seconds (configurable by the user), the execution profile information for the audio stream thread, feedback thread, idle time, and interrupts.
For the CPU load check, the expected result is that the reported percentages remains valid and changes according to the streaming activity.

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

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the "TX_TIMER_TICKS_PER_SECOND" define in the "tx_user.h" file. The update should be reflected in "tx_initialize_low_level.S" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This requires changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
     place in RAM_region    { last section FREE_MEM };
     ```
    + For MDK-ARM:
    ```
    either define the RW_IRAM1 region in the ".sct" file
    or modify the line below in "tx_initialize_low_level.S to match the memory region being used
        LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
    ```
    + For STM32CubeIDE add the following section into the .ld file:
    ```
    ._threadx_heap :
      {
         . = ALIGN(8);
         __RAM_segment_used_end__ = .;
         . = . + 64K;
         . = ALIGN(8);
       } >RAM AT> RAM
    ```

    The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
    In the example above the ThreadX heap size is set to 64KBytes.
    The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
    Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
    Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

#### <b>USBX usage hints</b>

- None

### <b>Keywords</b>

RTOS, ThreadX, USBXDevice, USB_OTG, High speed, SAI, Audio, Streaming, PCM, Feedback, TX_EXECUTION_PROFILE, UART/USART

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA65RIVx devices.
  - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

- STM32WBA65I-DK1 Set-up
    - Plug the USB HID device into the STM32WBA65I-DK1 board through 'Type C  to A-Female' cable to the connector:
      - CN5 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.
    - Plug headset on CN3 connector (Headset).

  - The USART1 interface available on PA8 and PB12 of the microcontroller are connected to ST-LINK MCU.
  - By default the USART1 communication between the target MCU and ST-LINK MCU is enabled. It's configuration is as following:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Open an audio player application (Windows Media Player) and play music on the PC host
