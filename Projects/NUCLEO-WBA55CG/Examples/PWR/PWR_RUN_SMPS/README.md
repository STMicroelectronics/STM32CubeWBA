## <b>PWR_RUN_SMPS Example Description</b>

How to use the SMPS PWR regulator.

Switching to the SMPS regulator provides lower consumption in particular at high VDD
voltage level.

In the associated software, the system clock is set to 100 MHz and the SysTick is
programmed to generate an interrupt each 1 ms.

After BOR0 power-on reset and system reset, the LDO regulator is enabled, in range 2.
The example switches first to SMPS before changing to the voltage range 1.

The JP2 jumper can be remove of the board to current consumption
measurement Idd with ammeter.

Two leds LD1 and LD2 are used to monitor the system state as following:

 - LD2 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in Run mode with SMPS


#### <b>Notes</b>

 1. Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select
    the RTC clock source; in this case the Backup domain will be reset in
    order to modify the RTC Clock source, as consequence RTC registers (including
    the backup registers) and RCC_CSR register are set to their reset values.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Power, PWR, SMPS, LDO, Voltage range

### <b>Directory contents</b>

  - PWR/PWR_RUN_SMPS/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - PWR/PWR_RUN_SMPS/Inc/stm32wbaxx_conf.h         HAL Configuration file
  - PWR/PWR_RUN_SMPS/stm32wbaxx_it.h               Header for stm32wbaxx_it.c
  - PWR/PWR_RUN_SMPS/Inc/main.h                    Header file for main.c
  - PWR/PWR_RUN_SMPS/Src/system_stm32wbaxx.c       STM32WBAxx system clock configuration file
  - PWR/PWR_RUN_SMPS/Src/stm32wbaxx_it.c           Interrupt handlers
  - PWR/PWR_RUN_SMPS/Src/main.c                    Main program
  - PWR/PWR_RUN_SMPS/Src/stm32wbaxx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WBA55CG Set-up
    - Use LD1 and LD2 connected respectively to PB.04 and PA.09 pins

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

