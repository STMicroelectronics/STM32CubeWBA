---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>Templates_TrustZoneEnabled_NoIsolation Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used to build any firmware application
when security is enabled (TZEN=1) and all MCU resources (memories, peripherals...) are configured as secure.
- This project is targeted to run on STM32WBA65RI device on STM32WBA65I-DK1 board from STMicroelectronics.
- At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals and initialize
the systick used as 1ms HAL timebase.
- The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 100 MHz.

### <b>Description</b>

Please remember that on system with security enabled, the system always boots in secure.

User Option Bytes configuration:

Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes Security Watermarks
should be set according to the application configuration.
Here the proper User Option Bytes setup in line with the project linker/scatter file is as follows:

    - TZEN=1
    - Bank 1 watermark : SECWM1R1 : SECWM1_PSTRT=0x00  SECWM1_PEND=0x7F  meaning all pages of Flash bank 1 set as secured
    - Bank 2 watermark : SECWM2R1 : SECWM2_PSTRT=0x00  SECWM2_PEND=0x7F  meaning all pages of Flash bank 2 set as secured

### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds) based on variable
    incremented in SysTick ISR. This implies that if HAL_Delay() is called from a peripheral ISR process, then the SysTick
    interrupt must have higher priority (numerically lower) than the peripheral interrupt. Otherwise the caller ISR process
    will be blocked. To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.

 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution from Flash memory,
    and reach the maximum performance.

### <b>Keywords</b>

Reference, Template, TrustZone

### <b>Directory contents</b>

File | Description
 --- | ---
  Templates/TrustZoneEnabled_NoIsolation/Src/main.c                      | Main program
  Templates/TrustZoneEnabled_NoIsolation/Src/system_stm32wbaxx.c         | STM32WBAxx system source file
  Templates/TrustZoneEnabled_NoIsolation/Src/stm32wbaxx_it.c             | Interrupt handlers
  Templates/TrustZoneEnabled_NoIsolation/Src/stm32wbaxx_hal_msp.c        | HAL MSP module
  Templates/TrustZoneEnabled_NoIsolation/Inc/main.h                      | Main program header file
  Templates/TrustZoneEnabled_NoIsolation/Inc/stm32wbaxx_discovery_conf.h | BSP Configuration file
  Templates/TrustZoneEnabled_NoIsolation/Inc/stm32wbaxx_hal_conf.h       | HAL Configuration file
  Templates/TrustZoneEnabled_NoIsolation/Inc/stm32wbaxx_it.h             | Interrupt handlers header file

### <b>Hardware and Software environment</b>

- This template runs on STM32WBA65RI devices with security enabled (TZEN=1).
- This template has been tested with STMicroelectronics STM32WBA65I-DK1 (MB1801 and MB2143) board and can be easily
  tailored to any other supported device and development board.

User Option Bytes requirement (with STM32CubeProgrammer tool) :

- TZEN = 1 : System with TrustZone-M enabled
- Secure boot base address 0 : SECBOOTADD0R = 0x0C000000
- Bank 1 secure watermark : SECWM1R1 : SECWM1_PSTRT=0x00  SECWM1_PEND=0x7F  meaning all pages of Flash bank 1 set as secured
- Bank 2 secure watermark : SECWM2R1 : SECWM2_PSTRT=0x00  SECWM2_PEND=0x7F  meaning all pages of Flash bank 2 set as secured

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Make sure that the system is configured with the security enable (TZEN=1) (option byte)
- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example

:::
:::
