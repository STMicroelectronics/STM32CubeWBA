## <b>PWR_ModesSelection Example Description</b>

This program shows how to configure the system to measure the current consumption in 9 different low-power modes.
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
The measured values are from the NUCLEO-WBA55CG at 3.3 V and 25 °C.
The datasheet values are provided for comparison.

For datasheet values, see Section 5.3.8, ‘Supply current characteristics,’ in the 'stm32wba55cg.pdf' datasheet.

| Power mode | Measured | Datasheet |
|---|---:|---:|
| RUN - 32 MHz | 1.26 mA | 1.47 mA |
| RUN - 100 MHz | 3.76 mA | 3.35 mA |
| SLEEP - 16 MHz | 0.25 mA | 0.22 mA |
| STOP0 - SRAM2 + CACHE | 11.37 uA | 11 uA |
| STOP0 - ALL RAMs + CACHE | 12.05 uA | 12 uA |
| STOP1 - SRAM1 + ULPMEN=1 | 6.36 uA | 6.5 uA |
| STANDBY(all peripherals disabled) - ULPMEN=1 | 0.49 uA | 0.37 uA |
| STANDBY(with RTC) - Clocked by LSI1 + ULPMEN=0 | 2.79 uA | 2.64 uA |
| STANDBY(Retention Mode) - ALL RAMs + Radio(BLE) + ULPMEN=1 | 1.37 uA | 1.37 uA |

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

  - This example runs on STM32WBA55CGUx devices
  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG board
    embedding a high-speed internal clock HSI and a low-speed clock LSI and can be easily tailored to any other
    supported device and development board.

  - NUCLEO-WBA55CG set-up:
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


