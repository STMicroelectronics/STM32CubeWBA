
## <b>Ux_Host_Audio_20 Application Description</b>

This application provides an example of Azure RTOS USBX stack usage. It shows how to develop USB Host Audio able to enumerate and communicate with a device audio speaker 2.0.

The main entry function tx_application_define() is called by ThreadX during kernel start, at this stage, all USBx resources are initialized, the AUDIO Class driver is registered.
The application creates two threads :

  - app_ux_host_thread_entry     (Priority : 10; Preemption threshold : 10) used to initialize USB OTG HAL HCD driver and start the Host.
  - audio_playback_thread_entry  (Priority : 30; Preemption threshold : 30) used to proceed to audio once the device is properly enumerated.

####  <b>Expected success behavior</b>

When a USB speaker/microphone device is plugged into the STM32WBA65I-DK1 board, the USB host displays a message in the UART HyperTerminal showing the Vendor ID and the Product ID of the attached device.
After enumeration phase :

  - "Audio file in Flash" operation uses a WAV file stored in internal Flash memory.
  - "Once user presses the user button, audio Player and Recorder start simultaneously" operation starts playing the audio.

#### <b>Error behaviors</b>

Errors are detected such as (Unsupported device, Enumeration Fail, unsupported WAV sampling) and the corresponding message is displayed on the HyperTerminal.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and audio2.0/audio1.0 class Specification.

#### <b>Known limitations</b>

None

#### <b>Notes</b>

To play another audio file, replace the WAV data in USBX/App/audio_data.c, rebuild the project, and program the new firmware. The file must be a PCM WAV format supported by the connected USB speaker

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it, by updating the “TX_TIMER_TICKS_PER_SECOND” define in the “tx_user.h” file. The update should be reflected in “tx_initialize_low_level.S” file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This require changes in the linker files to expose this memory location.
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

### <b>Keywords</b>

Connectivity, USBXHost, ThreadX, AUDIO 2.0, UAC, Speaker, WAV, Internal Flash, User Button, UART/USART

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA65RIVx devices.
  - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

- STM32WBA65I-DK1 Set-up
    - Plug the USB key into the STM32WBA65I-DK1 board through 'Type C' cable to the connector:
      - CN5 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal.

  - A virtual COM port will then appear in the HyperTerminal:
    - Hyperterminal configuration
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
 
### <b>Note</b>

The user has to check the list of the COM ports in Device Manager to find out the number of the COM ports that have been assigned (by OS) to the Stlink VCP.

To support audio class 2.0 this flag "UX_HOST_CLASS_AUDIO_2_SUPPORT" should be defined in ux_user.h.

It is mandatory to check that the Jumpers below are fitted: JP1 : 5V_USB Jumper is fitted in order to provide Vbus 5V.
