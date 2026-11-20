## <b>Ux_Device_Audio20_Recording Application Description</b>

This application provides an example of Azure RTOS USBX stack usage on STM32WBA65I-DK1 board,
it shows how to develop a USB Device AUDIO 2.0 class application for microphone recording (IN endpoint).

The application is designed to emulate an USB (In: Microphone). The code provides all required device descriptors framework
and associated Class descriptors to build a compliant USB AUDIO 2.0 recording device.

At the beginning ThreadX calls the entry function tx_application_define(). At this stage, all USBX resources
are initialized, the AUDIO Class driver is registered and the application creates 2 threads with the same priorities:

  - app_ux_device_thread_entry   (Prio: 20; PreemptionPrio: 20) used to initialize USB_OTG HAL PCD driver and start the device.
  - usbx_audio_record_app_thread (Prio: 20; PreemptionPrio: 20) used to start microphone capture and stream audio frames to the host.

The device supports the following audio features:
  - Pulse Coded Modulation (PCM) format
  - sampling rate: 48KHz.
  - Bit resolution: 16
  - Number of channels: 1
  - Asynchronous endpoints


#### <b>Expected success behavior</b>

When plugged to a PC host, the STM32WBA65I-DK1 must be properly enumerated as a USB Audio Microphone Device.
During the enumeration phase, the device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by host driver to identify the device capabilities. Once enumeration is complete,
start recording audio and stream frames to the host application (e.g., Audacity).

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Audio device enumeration failed) or recording frames are not received.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" specification and AUDIO 2.0 class specification.

#### <b>Known limitations</b>

 - This application does not support synchronization.
 - Footprint is not optimized.

### <b>Notes</b>

 1. A Product ID (PID) change is recommended when modifying the application to ensure the host treats it as a new device.
    This is especially critical when updating frequencies, as the host may otherwise retain stale descriptors.

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.s" file too.
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
    or modify the line below in "tx_low_level_initilize.s to match the memory region being used
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
       } >RAM_D1 AT> RAM_D1
	```

       The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
       In the example above the ThreadX heap size is set to 64KBytes.
       The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
       Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
       Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.s" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.


### <b>Keywords</b>

RTOS, ThreadX, USBXDevice, USB_OTG, High speed, SAI, Audio, Streaming, PCM, Microphone


### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA65RIVx devices.
  - This application has been tested with STMicroelectronics STM32WBA65I-DK1 boards revision MB2130-WBA65RI-A02 and can be easily tailored to any other supported device and development board.

  - STM32WBA65I-DK1 Set-up
    - Plug the USB Audio device into the STM32WBA65I-DK1 board through 'Type C  to A-Female' cable to the connector:
      - CN5 : to use USB High Speed OTG IP
    - Connect ST-Link cable to the PC USB port.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Open a recording application (e.g., Audacity) on the PC host and start recording from the "STM32 Audio Device"