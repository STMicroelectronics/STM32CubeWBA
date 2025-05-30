## <b>Tx_CMSIS_Wrapper Application Description</b>

This application provides an example of CMSIS RTOS adaptation layer for Azure RTOS ThreadX, it shows how to develop an application using the CMSIS RTOS 2 APIs.
It demonstrates how to create multiple threads using CMSIS RTOS 2 for ThreadX APIs.
At the main function, the application creates 2 threads with the same priority which execute in a periodic cycle of 15 seconds :

  - 'ThreadOne' (Priority : osPriorityNormal)
  - 'ThreadTwo' (Priority : osPriorityNormal)

The function "Led_Toggle()" is the entry function for both threads to toggle the leds.Therefore it is considered as a "critical section" that needs protection with a 'SyncObject' flag in the file "app_tx_cmsisrtos.h"
Each thread is running in an infinite loop as following:

- 'ThreadOne':
  + Try to acquire the 'SyncObject' immediately.
  + On Success toggle the 'LED_BLUE' each 500ms for 5 seconds.
  + Release the 'SyncObject'
  + Sleep for 10ms.
  + Repeat the steps above

- 'ThreadTwo':
  + Try to acquire the 'SyncObject' immediately.
  + On Success toggle the 'LED_GREEN' each 500ms for 5 seconds.
  + Release the 'SyncObject'
  + Sleep for 10ms.
  + Repeat the steps above.

By default the 'SyncObject' is defined as  "osMutexId_t" .It is possible to use a semaphore "osSemaphoreId_t" by tuning
the compile flags in the file "app_tx_cmsisrtos.h".

####  <b>Expected success behavior</b>

  - 'LED_BLUE' toggles every 500ms for 5 seconds
  - 'LED_GREEN' toggles every 500ms for 5 seconds
  - Messages on HyperTerminal :
     + "** ThreadXXX : waiting for SyncObject !! **" : When thread is waiting for the SyncObject.
     + "** ThreadXXX : SyncObject released **" : When thread put the SyncObject.
     + "** ThreadXXX : SyncObject acquired **" : When thread get the SyncObject.

#### <b>Error behaviors</b>

  - On failure, the LED_RED toggles every 1 second while the LED_GREEN and LED_BLUE are switched OFF.

#### <b>Assumptions if any</b>

None

#### <b>Known limitations</b>

None

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
    or modify the line below in "tx_initialize_low_level.S" to match the memory region being used
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

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

### <b>Keywords</b>

RTOS, ThreadX, Threading, CMSIS RTOS, Semaphore, Mutex

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBAxx devices
  - This application has been tested with STMicroelectronics NUCLEO-WBA55CG boards revision MB1803-WBA55CGA-B01
    and can be easily tailored to any other supported device and development board.
  - A virtual COM port appears in the HyperTerminal:
    - Hyperterminal configuration:
        + Data Length = 8 Bits
        + One Stop Bit
        + No parity
        + BaudRate = 115200 baud
        + Flow control: None


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
