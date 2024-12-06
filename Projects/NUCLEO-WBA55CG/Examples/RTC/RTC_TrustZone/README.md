## <b>RTC_TrustZone Example Description</b>

How to configure the TrustZone-aware RTC peripheral when TrustZone security is activated (Option bit TZEN=1): some features
of the RTC can be secure while the others are non-secure.

In this example, all features are secure except alarm B and zone 2 / zone 3 of backup registers.

**LD1** is ON after few seconds if programmed features are OK. 
**LD3** will turn ON if any error occurs.

This project is composed of two sub-projects:

- one for the secure application part (Project_s)
- one for the non-secure application part (Project_ns).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

This project mainly shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:

 - the first half for the secure application and
 - the second half for the non-secure application.

User Option Bytes configuration:

Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes 
SECWM_PSTRT/SECWM_PEND should be set according to the application configuration. 
Here the proper User Option Bytes setup in line with the project linker/scatter file is as follows:

        - TZEN = 1                            System with TrustZone-M enabled
        - SECWM_PSTRT=0x0   SECWM_PEND=0x3F   Only first 64 pages of Flash set as secure

Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault as demonstrated in non secure application by commenting some
code instructions in Secure/main.c (LED I/O release).

This project is targeted to run on STM32WBA55CGUx device on NUCLEO-WBA55CG boards from STMicroelectronics.

This project configures the maximum system clock frequency at 100 MHz in secure application.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. The following sequence is needed to disable TrustZone:

      - **Boot from user Flash memory**:
         a. Make sure that secure and non-secure applications are well loaded and executed (jump done on non-secure application).
         b. If not yet done, set RDP to level 1 through STM32CubeProgrammer. Then only Hotplug connection is possible during non-secure application execution.
         c. Use a power supply different from ST-LINK in order to be able to connect to the target.
         d. Uncheck the TZEN box and set RDP to level 0 (option byte value 0xAA), then click on Apply.

     - **Boot from RSS**:
         a. Make sure to apply a high level on BOOT0 pin (make sure that nSWBOOT0 Option Byte is checked).
         b. If not yet done, set RDP to level 1 through STM32CubeProgrammer. Then only Hotplug connection is possible during non-secure application execution.
         c. Use a power supply different from ST-LINK in order to be able to connect to the target.
         d. Uncheck the TZEN box and set RDP to level 0 (option byte value 0xAA), then click on Apply.

    Please refer to AN5347 for more details.

### <b>Keywords</b>

RTC, TrustZone, LSE, Backup, Alarm, Wake-up timer, Time base

### <b>Directory contents</b>

  -  RTC/RTC_TrustZone/Secure/Src/main.c                                       Secure Main program
  -  RTC/RTC_TrustZone/Secure/Src/secure_nsc.c                                 Secure Non-Secure Callable (NSC) module
  -  RTC/RTC_TrustZone/Secure/Src/stm32wbaxx_hal_msp.c                    Secure HAL MSP module
  -  RTC/RTC_TrustZone/Secure/Src/stm32wbaxx_it.c                         Secure Interrupt handlers
  -  RTC/RTC_TrustZone/Secure/Src/system_stm32wbaxx_s.c                   Secure STM32WBAxx system clock configuration file
  -  RTC/RTC_TrustZone/Secure/Inc/main.h                                       Secure Main program header file
  -  RTC/RTC_TrustZone/Secure/Inc/partition_stm32wba55xx.h      STM32WBA Device System Configuration file
  -  RTC/RTC_TrustZone/Secure/Inc/stm32wbaxx_hal_conf.h                   Secure HAL Configuration file
  -  RTC/RTC_TrustZone/Secure/Inc/stm32wbaxx_it.h                         Secure Interrupt handlers header file
  -  RTC/RTC_TrustZone/Secure_nsclib/secure_nsc.h                              Secure Non-Secure Callable (NSC) module header file
  -  RTC/RTC_TrustZone/NonSecure/Src/main.c                                    Non-secure Main program
  -  RTC/RTC_TrustZone/NonSecure/Src/stm32wbaxx_hal_msp.c                 Non-secure HAL MSP module
  -  RTC/RTC_TrustZone/NonSecure/Src/stm32wbaxx_it.c                      Non-secure Interrupt handlers
  -  RTC/RTC_TrustZone/NonSecure/Src/system_stm32wbaxx_ns.c               Non-secure STM32WBAxx system clock configuration file
  -  RTC/RTC_TrustZone/NonSecure/Inc/main.h                                    Non-secure Main program header file
  -  RTC/RTC_TrustZone/NonSecure/Inc/stm32wbaxx_nucleo_conf.h             BSP Configuration file
  -  RTC/RTC_TrustZone/NonSecure/Inc/stm32wbaxx_hal_conf.h                Non-secure HAL Configuration file
  -  RTC/RTC_TrustZone/NonSecure/Inc/stm32wbaxx_it.h                      Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices with security enabled (TZEN=1).

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = 1                            System with TrustZone-M enabled
        - SECWM_PSTRT=0x0   SECWM_PEND=0x3F   Only first 64 pages of Flash set as secure

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

<b>IAR</b>

 - Open your toolchain
 - Open Multi-projects workspace file Project.eww
 - Set the "xxxxx_S" as active application (Set as Active)
 - Rebuild xxxxx_S project
 - Rebuild xxxxx_NS project
 - Load the secure and non-secures images into target memory (Ctrl + D)
 - Run the example


