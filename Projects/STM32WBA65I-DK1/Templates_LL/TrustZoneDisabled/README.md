
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>Templates_LL_TrustZoneDisabled Example Description</b>

- This project provides a reference template based on the STM32Cube LL API that can be used
to build any firmware application.
- This project is targeted to run on STM32WBA65RI device on STM32WBA65I-DK1 board from STMicroelectronics.
- The reference template project configures the maximum system clock frequency at 100Mhz.

#### <b>Description</b>

This project LL template provides:

 - Inclusion of all LL drivers (include files in "main.h" and LL sources files in IDE environment, with option "USE_FULL_LL_DRIVER" in IDE environment)
   Note: If optimization is needed afterwards, user can perform a cleanup by removing unused drivers.
 - Definition of LED and joystick (file: main.h)
 - Clock configuration (file: main.c)

This project LL template does not provide:

 - Functions to initialize and control LED and joystick
 - Functions to manage IRQ handler of joystick

To port a LL example to the targeted board:

1. Select the LL example to port.
   To find the board on which LL examples are deployed, refer to LL examples list in "STM32CubeProjectsList.html", table section "Examples_LL"
   or Application Note: STM32Cube firmware examples for STM32WBA Series

2. Replace source files of the LL template by the ones of the LL example, except code specific to board.
   Note: Code specific to board is specified between tags:

   - /* ==============   BOARD SPECIFIC CONFIGURATION CODE BEGIN    ============== */
   - /* ==============   BOARD SPECIFIC CONFIGURATION CODE END      ============== */


  - Replace file main.h, with updates:
    - Keep LED and joystick definition of the LL template under tags
  - Replace file main.c, with updates:
    - Keep clock configuration of the LL template: function "SystemClock_Config()"
    - Depending of LED availability, replace LEDx_PIN by another LEDx (number) available in file main.h
  - Replace file stm32wbaxx_it.h
  - Replace file stm32wbaxx_it.c

#### <b>Notes</b>

  1. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

Reference, Template, TrustZone disabled

### <b>Directory contents</b>

File | Description
 --- | ---
  Templates_LL/TrustZoneDisabled/Inc/stm32wbaxx_it.h     | Interrupt handlers header file
  Templates_LL/TrustZoneDisabled/Inc/main.h              | Header for main.c module
  Templates_LL/TrustZoneDisabled/Inc/stm32_assert.h      | Template file to include assert_failed function
  Templates_LL/TrustZoneDisabled/Src/stm32wbaxx_it.c     | Interrupt handlers
  Templates_LL/TrustZoneDisabled/Src/main.c              | Main program
  Templates_LL/TrustZoneDisabled/Src/system_stm32wbaxx.c | STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This template runs on STM32WBA65RI devices without security enabled (TZEN=0).
  - This template has been tested with STMicroelectronics STM32WBA65I-DK1 (MB1801 and MB2143)
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


:::
:::

