
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}


## <b>Templates_OEMiROT_Appli Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=1).

- This project is targeted to run on STM32WBA65RIV device on STM32WBA65I-DK1 board from STMicroelectronics.

- The reference template project configures the maximum system clock frequency at 100Mhz in secure
application(By default is disabled).

#### <b>Description</b>

This project concerns only the secure application part (Project_s)

Please remember that on system with security enabled, the system always boots in secure

This project mainly shows how to run full secure application

User Option Bytes configuration:

Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
SECWM_PSTRT/SECWM_PEND should be set according to the application configuration.
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

    - TZEN=1
    - SECWM_PSTRT=0x0  SECWM_PEND=0x7F  meaning only first 64 pages of Flash set as secure

No non-secure application is provided for unauthorized access(illegal access).

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

Reference, Template

### <b>Directory contents</b>

  - Templates/OEMiROT_Appli/Src/main.c                      Secure Main program
  - Templates/OEMiROT_Appli/Src/secure_nsc.c                Secure Non-Secure Callable (NSC) module
  - Templates/OEMiROT_Appli/Src/stm32wbaxx_hal_msp.c        Secure HAL MSP module
  - Templates/OEMiROT_Appli/Src/stm32wbaxx_it.c             Secure Interrupt handlers
  - Templates/OEMiROT_Appli/Src/system_stm32wbaxx_s.c       Secure STM32WBAxx system clock configuration file
  - Templates/OEMiROT_Appli/Inc/main.h                      Secure Main program header file
  - Templates/OEMiROT_Appli/Inc/partition_stm32wba65xx.h    STM32WBA Device System Configuration file
  - Templates/OEMiROT_Appli/Inc/stm32wbaxx_hal_conf.h       Secure HAL Configuration file
  - Templates/OEMiROT_Appli/Inc/stm32wbaxx_it.h             Secure Interrupt handlers header file
  - Templates/OEMiROT_Appli/Secure_nsclib/secure_nsc.h      Secure Non-Secure Callable (NSC) module header file



### <b>Hardware and Software environment</b>

  - This template runs on STM32WBA65RIV devices with security enabled (TZEN=1).
  - This template has been tested with STMicroelectronics STM32WBA65I-DK1 (MB2143 and MB2130)/NUCLEO-WBA65RI (MB1801 and MB2130)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = 1                            System with TrustZone-M enabled
        - SECWM_PSTRT=0x0   SECWM_PEND=0x7F   Only first 64 pages of Flash set as secure


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)


<b>IAR</b>

- Open your toolchain
- Open Multi-projects workspace file Project.eww
- Rebuild xxxxx_S project
- Flash the secure binarie with Project->Download->Download active application
  (this shall download the \Secure_nsclib\xxxxx_S.out to flash memory)
- Run the example


<b>MDK-ARM</b>

- Open your toolchain
- Open Multi-projects workspace file Project.uvmpw
- Build xxxxx_S project
- Load the secure binary (F8)
  (this shall download the \MDK-ARM\xxxxx_s\Exe\Project_s.axf to flash memory)
 - Run the example


<b>STM32CubeIDE</b>

- Open STM32CubeIDE
- File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
- Build configuration: Set the same active build configuration: Debug (default) or Release for xxxxx_S
- Build xxxxx_S project
- Select "Debug configuration" or "Run configuration" in function of the active build configuration
  - Double click on "STM32 Cortex-M C/C++ Application"
  - Select  "Startup" >  "Add" >
    - Select the xxxxx_S project
- Click Debug/Run to debug/run the example


:::
:::

