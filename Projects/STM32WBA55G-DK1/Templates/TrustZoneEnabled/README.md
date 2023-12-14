
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>Templates_TrustZoneEnabled Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=1).

- This project is targeted to run on STM32WBA55CGU device on STM32WBA55G-DK1 board from STMicroelectronics.

- The reference template project configures the maximum system clock frequency at 100Mhz in non-secure
application.

#### <b>Description</b>

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
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

    - TZEN=1
    - SECWM_PSTRT=0x0  SECWM_PEND=0x3F  meaning only first 64 pages of Flash set as secure

Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault as demonstrated in non secure application by uncommenting some
code instructions in Secure/main.c (LED) and NonSecure/main.c (illegal access).

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

Reference, Template, TrustZone

### <b>Directory contents</b>

  - Templates/TrustZoneEnabled/Secure/Src/main.c                          Secure Main program
  - Templates/TrustZoneEnabled/Secure/Src/secure_nsc.c                    Secure Non-Secure Callable (NSC) module
  - Templates/TrustZoneEnabled/Secure/Src/stm32wbaxx_hal_msp.c            Secure HAL MSP module
  - Templates/TrustZoneEnabled/Secure/Src/stm32wbaxx_it.c                 Secure Interrupt handlers
  - Templates/TrustZoneEnabled/Secure/Src/system_stm32wbaxx_s.c           Secure STM32WBAxx system clock configuration file
  - Templates/TrustZoneEnabled/Secure/Inc/main.h                          Secure Main program header file
  - Templates/TrustZoneEnabled/Secure/Inc/partition_stm32wba55xx.h        STM32WBA Device System Configuration file
  - Templates/TrustZoneEnabled/Secure/Inc/stm32wbaxx_hal_conf.h           Secure HAL Configuration file
  - Templates/TrustZoneEnabled/Secure/Inc/stm32wbaxx_it.h                 Secure Interrupt handlers header file
  - Templates/TrustZoneEnabled/Secure_nsclib/secure_nsc.h                 Secure Non-Secure Callable (NSC) module header file
  - Templates/TrustZoneEnabled/NonSecure/Src/main.c                       Non-secure Main program
  - Templates/TrustZoneEnabled/NonSecure/Src/stm32wbaxx_hal_msp.c         Non-secure HAL MSP module
  - Templates/TrustZoneEnabled/NonSecure/Src/stm32wbaxx_it.c              Non-secure Interrupt handlers
  - Templates/TrustZoneEnabled/NonSecure/Src/system_stm32wbaxx_ns.c       Non-secure STM32WBAxx system clock configuration file
  - Templates/TrustZoneEnabled/NonSecure/Inc/main.h                       Non-secure Main program header file
  - Templates/TrustZoneEnabled/NonSecure/Inc/stm32wba55g_discovery_conf.h BSP Configuration file
  - Templates/TrustZoneEnabled/NonSecure/Inc/stm32wbaxx_hal_conf.h        Non-secure HAL Configuration file
  - Templates/TrustZoneEnabled/NonSecure/Inc/stm32wbaxx_it.h              Non-secure Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This template runs on STM32WBA55CGU devices with security enabled (TZEN=1).
  - This template has been tested with STMicroelectronics STM32WBA55G-DK1 (MB1802 and MB1803)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = 1                            System with TrustZone-M enabled
        - SECWM_PSTRT=0x0   SECWM_PEND=0x3F   Only first 64 pages of Flash set as secure


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)


<b>IAR</b>

- Open your toolchain
- Open Multi-projects workspace file Project.eww
- Rebuild xxxxx_S project
- Rebuild xxxxx_NS project
- Set the "xxxxx_S" as active application (Set as Active)
- Flash the non-seure and secure binaries with Project->Download->Download active application
  (this shall download the \Secure_nsclib\xxxxx_S.out and \EWARM\xxxxx_NS\Exe\xxxxx_NS.out to flash memory)
- Run the example


<b>MDK-ARM</b>

- Open your toolchain
- Open Multi-projects workspace file Project.uvmpw
- Select the xxxxx_S project as Active Project (Set as Active Project)
- Build xxxxx_S project
- Select the xxxxx_NS project as Active Project (Set as Active Project)
- Build xxxxx_NS project
- Load the non-secure binary (F8)
  (this shall download the \MDK-ARM\xxxxx_ns\Exe\Project_ns.axf to flash memory)
- Select the xxxxx_S project as Active Project (Set as Active Project)
- Load the secure binary (F8)
  (this shall download the \MDK-ARM\xxxxx_s\Exe\Project_s.axf to flash memory)
 - Run the example


<b>STM32CubeIDE</b>

- Open STM32CubeIDE
- File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
- Build configuration: Set the same active build configuration: Debug (default) or Release for both projects xxxxx_S & xxxxx_NS
- Select and build the xxxxx_NS project, this will automatically trigger the build of xxxxx_S project
- Select the xxxxx_S project and select "Debug configuration" or "Run configuration" in function of the active build configuration
  - Double click on "STM32 Cortex-M C/C++ Application"
  - Select  "Startup" >  "Add" >
    - Select the xxxxx_NS project
- Click Debug/Run to debug/run the example


:::
:::

