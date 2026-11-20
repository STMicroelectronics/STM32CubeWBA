## <b>PWR_ModesSelection Example Description</b>

This program shows how to configure the system to measure the current consumption in 14 different low-power modes.
The objective is to provide a means to measure the power consumption using an ammeter on the IDD connector.

The SysTick is programmed to generate an interrupt every 1 ms.

### <b>Notes</b>

  1. To measure the current consumption while running in the selected mode, remove jumper JP2
      and connect an ammeter to measure the IDD current.

  2. This example can not be used in DEBUG mode due to the fact
      that the Cortex-M33 core is no longer clocked during low power mode
      so debugging features are disabled.

  3. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

  4. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Current Consumption(Typical @3.3V, 25°C)</b>

The supported scenarios are the ones listed in `lowpower_mode_msg`.
The measured values are from the NUCLEO-WBA65RI at 3.3 V and 25 °C.
The datasheet values are provided for comparison.

For datasheet values, see Section 5.3.8, ‘Supply current characteristics,’ in the 'stm32wba65ri.pdf' datasheet.

| Power mode | Measured | Datasheet |
|---|---:|---:|
| RUN - 32 MHz | 0.96 mA | 2.01 mA |
| RUN - 100 MHz | 3.47 mA | 4.40 mA |
| SLEEP - 16 MHz | 0.32 mA | 0.25 mA |
| STOP0 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG | 16.24 µA | 16.70 µA |
| STOP0 - 64KB SRAM2 | 14.15 µA | 14.49 µA |
| STOP1 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG + ULPMEN=1 | 12.61 µA | 11.90 µA |
| STOP1 - 64KB SRAM2 + ULPMEN=1 | 10.08 µA | 9.72 µA |
| STOP2 - Full SRAM (SRAM 1 + 2 = 512KB) + PKA + OTG + ULPMEN=1 | 7.56 µA | 7.58 µA |
| STOP2 - 64KB SRAM2 + ULPMEN=1 | 5.51 µA | 5.86 µA |
| STANDBY(with RTC) - Clocked by LSI1 + ULPMEN=1 | 0.93 µA | 0.79 µA |
| STANDBY(with RTC) - Clocked by LSI1 + ULPMEN=0 | 2.93 µA | 2.70 µA |
| STANDBY(Retention Mode) - Full SRAM (SRAM 1 + 2 = 512KB) + Radio + ULPMEN=1 | 3.26 µA | 3.42 µA |
| STANDBY(Retention Mode) - 64KB SRAM2 + ULPMEN=1 | 1.15 µA | 1.15 µA |
| STANDBY(Retention Mode) - Radio + ULPMEN=1 | 0.92 µA | 0.83 µA |

### <b>Keywords</b>

Power, PWR, EXTI, Sleep mode, Stop mode, Standby mode, Interrupt, RTC, External reset, Memory retention, Cache, USART, SMPS

### <b>Directory contents</b>

        - PWR/PWR_ModesSelection/Inc/stm32wbaxx_hal_conf.h       HAL Configuration file
        - PWR/PWR_ModesSelection/Inc/stm32wbaxx_nucleo_conf.h    STM32WBAxx_Nucleo board configuration header file
        - PWR/PWR_ModesSelection/Inc/stm32wbaxx_it.h             Header for stm32wbaxx_it.c
        - PWR/PWR_ModesSelection/Inc/main.h                      Header file for main.c
        - PWR/PWR_ModesSelection/Inc/console.h                   Header for console.c module
        - PWR/PWR_ModesSelection/Inc/lowpower_scenarios.h        Header for lowpower_scenarios.c module
        - PWR/PWR_ModesSelection/Inc/system_config.h             Header for system_config.c module
        - PWR/PWR_ModesSelection/Src/main.c                      Main program body
        - PWR/PWR_ModesSelection/Src/lowpower_scenarios.c        Low-power scenario module
        - PWR/PWR_ModesSelection/Src/console.c                   Console output input module
        - PWR/PWR_ModesSelection/Src/stm32wbaxx_hal_msp.c        HAL MSP module
        - PWR/PWR_ModesSelection/Src/stm32wbaxx_it.c             Interrupt handlers
        - PWR/PWR_ModesSelection/Src/system_config.c             System configuration

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RI devices
  - This example has been tested with STMicroelectronics NUCLEO-WBA65RI board
    embedding a high-speed internal clock HSI and a low-speed clock LSI and can be easily tailored to any other
    supported device and development board.

  - NUCLEO-WBA65RI set-up:
	  - USART1 interface connected to ST-LINK's USB port

  - HyperTerminal setup of COM port associated to STLink Virtual COM Port:
    - Baud rate = 115200
    - Data = 8 bit
    - Parity = none
    - Stop = 1 bit
    - No hardware flow control

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

  - Open your preferred toolchain
  - Rebuild all files and load your image into target memory
  - Run the example
  - Select the desired low-power scenario on the serial terminal
  - UnPlug then Plug STLINK connection to perform a power-on-reset

