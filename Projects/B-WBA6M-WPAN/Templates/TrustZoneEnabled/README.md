
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>Templates_TrustZoneEnabled Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=1).
- This project is targeted to run on STM32WBA6M device on B-WBA6M-WPAN board from STMicroelectronics.
- The reference template project configures the maximum system clock frequency at 100Mhz in non-secure
application.

#### <b>Description</b>

This project is composed of two sub-projects:

  - one for the secure application part (TrustZoneEnabled_S)
  - one for the non-secure application part (TrustZoneEnabled_NS).

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
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

    - TZEN=1
    - SECWM_PSTRT=0x0  SECWM_PEND=0x3F  meaning only first 64 pages of Flash set as secure

Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory, and reach the maximum performance.

### <b>Keywords</b>

Reference, Template, TrustZone enabled

### <b>Directory contents</b>

File | Description
 --- | ---
  Templates/TrustZoneEnabled/Secure/Src/main.c                      | Secure Main program
  Templates/TrustZoneEnabled/Secure/Src/secure_nsc.c                | Secure Non-Secure Callable (NSC) module
  Templates/TrustZoneEnabled/Secure/Src/stm32wbaxx_hal_msp.c        | Secure HAL MSP module
  Templates/TrustZoneEnabled/Secure/Src/stm32wbaxx_it.c             | Secure Interrupt handlers
  Templates/TrustZoneEnabled/Secure/Src/system_stm32wbaxx_s.c       | Secure STM32WBAxx system clock configuration file
  Templates/TrustZoneEnabled/Secure/Inc/main.h                      | Secure Main program header file
  Templates/TrustZoneEnabled/Secure/Inc/partition_stm32wba6mxx.h    | STM32WBA Device System Configuration file
  Templates/TrustZoneEnabled/Secure/Inc/stm32wbaxx_hal_conf.h       | Secure HAL Configuration file
  Templates/TrustZoneEnabled/Secure/Inc/stm32wbaxx_it.h             | Secure Interrupt handlers header file
  Templates/TrustZoneEnabled/Secure/Inc/b_wba6m_wpan_conf.h         | BSP Configuration file
  Templates/TrustZoneEnabled/Secure_nsclib/secure_nsc.h             | Secure Non-Secure Callable (NSC) module header file
  Templates/TrustZoneEnabled/NonSecure/Src/main.c                   | Non-secure Main program
  Templates/TrustZoneEnabled/NonSecure/Src/stm32wbaxx_hal_msp.c     | Non-secure HAL MSP module
  Templates/TrustZoneEnabled/NonSecure/Src/stm32wbaxx_it.c          | Non-secure Interrupt handlers
  Templates/TrustZoneEnabled/NonSecure/Src/system_stm32wbaxx_ns.c   | Non-secure STM32WBAxx system clock configuration file
  Templates/TrustZoneEnabled/NonSecure/Inc/main.h                   | Non-secure Main program header file
  Templates/TrustZoneEnabled/NonSecure/Inc/stm32wbaxx_hal_conf.h    | Non-secure HAL Configuration file
  Templates/TrustZoneEnabled/NonSecure/Inc/stm32wbaxx_it.h          | Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This template runs on STM32WBA6M devices with security enabled (TZEN=1).
  - This template has been tested with STMicroelectronics B-WBA6M-WPAN (MB2304)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = 1                               System with TrustZone-M enabled
        - SECWM_PSTRT=0x0   SECWM_PEND=0x3F      Only first 64 pages of Flash set as secure


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)
 - Open your preferred toolchain
 - Rebuild all files (Secure then Non-Secure)
 - Load your images (Non-Secure and Secure) into target memory
 - Run the example


:::
:::

