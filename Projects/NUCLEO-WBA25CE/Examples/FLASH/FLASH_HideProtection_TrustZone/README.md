::: {.row}
::: {.col}

## <b>FLASH_HideProtection_TrustZone Example Description</b>

How to configure and use the FLASH HAL API for the secure hide protection and extended
secure hide protection of internal Flash memory.

This project is composed of two sub-projects:

 - one for the secure application part (FLASH_HideProtection_TrustZone_S)
 - one for the non-secure application part (FLASH_HideProtection_TrustZone_NS).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

The system isolation splits the internal Flash and internal SRAM memories into two halves:

 - the first half of Flash is used for the secure application code,
 - the second half of Flash is used for the non-secure application code,
 - the first half of each SRAM is used for the secure application data,
 - the second half of each SRAM is used for the non-secure application data.

For this specific example, the secure part of the FLASH memory is divided into several areas
with different Hide Protection (HDP) properties:

          Page               Area                    Address
               ___________________________________
              |                                   |
              .     Non Secure Callable Area      .
           62 |___________________________________| 0x0C03E000
              |                                   |
              |                                   |
              .                                   .
              |                                   |
           20 |___________________________________| 0x0C014000
              |                                   |
              |        Non HDP Area - Data        |
           17 |___________________________________| 0x0C011000
              |                                   |
              |     Extended HDP Area - Code      |
           16 |___________________________________| 0x0C010000
              |                                   |
              |     Extended HDP Area - Data      |
           15 |___________________________________| 0x0C00F000
              |                                   |
              .          HDP Area - Data          .
           10 |___________________________________| 0x0C00A000
              |                                   |
              |                                   |
              .          HDP Area - Code          .
              |                                   |
            0 |___________________________________| 0x0C000000


**Secure Application :**

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 64 MHz.

After Reset, the Flash memory Program/Erase Controller is locked. A dedicated function
is used to enable the FLASH control register access.
Before programming the desired secured addresses (some of them are in the secure hide
area, extended secure hide area and some other not), an erase operation is performed using
the flash erase page feature. The erase procedure is done by filling the erase init
structure giving the starting erase page and the number of pages to erase.
At this stage, all these pages will be erased one by one separately.

**Note:** If problem occurs on a page, erase will be stopped and faulty page will
be returned to user (through variable 'PageError').

Once this operation is finished, page double-word programming operation will be performed
at these addresses.

The access of the secure hide protection area is then denied. A check of the written data is
performed to verify that the data in the secure hide protection area are read as zero
where as the other areas are correctly read.

Then the access of the extended secure hide protection area is denied. A check of the
written data is performed to verify that the data in the extended secure hide protection
area are read as zero where as the other areas are correctly read.

Finally the non-secure application is launched.


**Non-Secure Application :**

A non-secure callable function is called to get the result of the data check done by secure
application.

Nothing else is done in the application, user can add desired code


**User Option Bytes configuration:**

Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
Security Watermarks should be set according to the application configuration.
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

 - TZEN=1
 - Watermark :
   - SECWMR1 : STRT=0x00  END=0x3F  meaning first half of Flash set as secured
   - SECWMR2 : END=0x0E  EN=0x0FF  meaning HDP area includes pages 0 to 15 of Flash
 - Boot addresses :
   - SBOOT0 : 0x0C000000 to set secure VTOR
   - BOOT0 : 0x08040000 to set non-secure VTOR


Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault.

The NUCLEO-WBA25CE board LED can be used to monitor the transfer status:

  - LD3 is ON when there is an issue during erase or program procedure
  - LD2 is blinking when there are no errors detected
  - LD3 is blinking when there is an issue during programmed data check or
    when an error occurred in non-secure application

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

Memory, FLASH, Option Bytes, Hide Protection

### <b>Directory contents</b>

File | Description
 --- | ---
  FLASH/FLASH_HideProtection_TrustZone/Secure/Src/main.c                      |  Secure Main program
  FLASH/FLASH_HideProtection_TrustZone/Secure/Src/secure_nsc.c                |  Secure Non-Secure Callable (NSC) module
  FLASH/FLASH_HideProtection_TrustZone/Secure/Src/stm32wbaxx_hal_msp.c        |  Secure HAL MSP module
  FLASH/FLASH_HideProtection_TrustZone/Secure/Src/stm32wbaxx_it.c             |  Secure Interrupt handlers
  FLASH/FLASH_HideProtection_TrustZone/Secure/Src/system_stm32wbaxx_s.c       |  Secure STM32WBAxx system init file
  FLASH/FLASH_HideProtection_TrustZone/Secure/Inc/main.h                      |  Secure Main program header file
  FLASH/FLASH_HideProtection_TrustZone/Secure/Inc/partition_stm32wba25xx.h    |  Secure core configuration file
  FLASH/FLASH_HideProtection_TrustZone/Secure/Inc/stm32wbaxx_hal_conf.h       |  Secure HAL configuration file
  FLASH/FLASH_HideProtection_TrustZone/Secure/Inc/stm32wbaxx_it.h             |  Secure Interrupt handlers header file
  FLASH/FLASH_HideProtection_TrustZone/Secure/Inc/stm32wbaxx_nucleo_conf.h    |  Secure BSP configuration file
  FLASH/FLASH_HideProtection_TrustZone/Secure_nsclib/secure_nsc.h             |  Secure Non-Secure Callable (NSC) header file
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Src/main.c                   |  Non-secure Main program
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Src/stm32wbaxx_hal_msp.c     |  Non-secure HAL MSP module
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Src/stm32wbaxx_it.c          |  Non-secure Interrupt handlers
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Src/system_stm32wbaxx_ns.c   |  Non-secure STM32WBAxx system init file
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Inc/main.h                   |  Non-secure Main program header file
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Inc/stm32wbaxx_hal_conf.h    |  Non-secure HAL Configuration file
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Inc/stm32wbaxx_it.h          |  Non-secure Interrupt handlers header file
  FLASH/FLASH_HideProtection_TrustZone/NonSecure/Inc/stm32wbaxx_nucleo_conf.h |  Non-secure BSP configuration file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA25CEx devices with security enabled (TZEN=1).
  - This example has been tested with NUCLEO-WBA25CE board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

Option Byte | Value | Description
 --- | --- | ---
  OPTR_TZEN         | 1          | System with TrustZone-M enabled
  SBOOT0R_ADD       | 0x0C000000 | SBOOT0 address for secure automatic VTOR update
  BOOT0R_ADD        | 0x08040000 | BOOT0 address for non-secure automatic VTOR update
  SECWMR1_STRT      | 0x00       | First half of Flash set as secured
  SECMWR1_END       | 0x3F       | First half of Flash set as secured
  SECWMR2_HDP_END   | 0x0E       | HDP area from page 0 to 15 of Flash
  SECWMR2_HDPEN     | 0xFF       | HDP area enabled in Flash


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

<b>IAR</b>

 - Open your toolchain
 - Open Multi-projects workspace file Project.eww
 - Set the FLASH_HideProtection_TrustZone_S as active application (Set as Active)
 - Rebuild FLASH_HideProtection_TrustZone_S project
 - Rebuild FLASH_HideProtection_TrustZone_NS project
 - Load the secure and non-secures images into target memory (Ctrl + D)
 - Run the example

<b>MDK-ARM</b>

 - Open your toolchain
 - Open Multi-projects workspace file Project.uvmpw
 - Select the FLASH_HideProtection_TrustZone_S project as Active Project (Set as Active Project)
 - Build FLASH_HideProtection_TrustZone_S project
 - Select the FLASH_HideProtection_TrustZone_NS project as Active Project (Set as Active Project)
 - Build FLASH_HideProtection_TrustZone_NS project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FLASH_HideProtection_TrustZone_NS.axf to flash memory)
 - Select the FLASH_HideProtection_TrustZone_S project as Active Project (Set as Active Project)
 - Load the secure binary (F8)
   (this shall download the \MDK-ARM\Objects\FLASH_HideProtection_TrustZone_S.axf to flash memory)
 - Run the example

<b>STM32CubeIDE</b>

 - Open STM32CubeIDE
 - File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
 - Build configuration: Set the same active build configuration: Debug (default) or Release for both projects FLASH_HideProtection_TrustZone_S & FLASH_HideProtection_TrustZone_NS
 - Select and build the FLASH_HideProtection_TrustZone_NS project, this will automatically trigger the build of FLASH_HideProtection_TrustZone_S project
 - Select the FLASH_HideProtection_TrustZone_S project and select "Debug configuration" or "Run configuration" in function of the active build configuration
   - Double click on “STM32 Cortex-M C/C++ Application”
   - Click on "Debugger" and select "ST-LINK (OpenOCD)" as Debug probe
   - Select  “Startup” >  “Add” >
   - Select the FLASH_HideProtection_TrustZone_NS project
 - Click Debug/Run to debug/run the example

:::
:::
