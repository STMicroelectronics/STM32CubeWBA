## <b>PWR_SMPS_64MHZ_HSI_PLL Example Description</b>

How to use the SMPS PWR regulator with system clock at 64MHz with HSI and PLL.

In the associated software, the system clock is set to 64MHz.

After start-up, the LDO regulator is enabled in range 2. 
The software switch first to the SMPS regulator and then switch
to voltage range 1 before to configure the system clock to 64MHz with HSI and 
the PLL.

Switching to the SMPS regulator provides lower consumption in particular at high VDD
voltage level.

The JP2 jumper can be remove of the board to current consumption
measurement Idd with ammeter.

### <b>Keywords</b>

Power, PWR, LDO, SMPS, Voltage range 1, HSI, PLL, 64MHz

### <b>Directory contents</b>

  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Inc/main.h                  Header for main.c module
  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Inc/stm32_assert.h          Template file to include assert_failed function
  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Src/stm32wbaxx_it.c         Interrupt handlers
  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Src/main.c                  Main program
  - PWR/PWR_SMPS_64MHZ_HSI_PLL/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b>
  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board with SMPS.

  - NUCLEO-WBA55CG Set-up
    - LD1 connected to pin PB.04

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

