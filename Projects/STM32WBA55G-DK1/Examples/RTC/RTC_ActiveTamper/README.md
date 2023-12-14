## <b>RTC_ActiveTamper Example Description</b>

Configuration of the active tamper detection with backup registers erase.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The RTC peripheral configuration is ensured by the MX_RTC_Init() and MX_TAMP_RTC_Init functions.
HAL_RTC_MspInit()function which core is implementing the configuration of the needed RTC resources
according to the used hardware (CLOCK,PWR, RTC clock source and BackUp). 
You may update this function to change RTC configuration.

**Note** LSE oscillator clock is used as RTC clock source (32.768 kHz) by default.

This example performs the following:

1. Please connect the following pins together :
  - TAMP_IN1 / PA7 (Arduino A4 pin 5 connector CN11)
  - TAMP_OUT5 / PC13 (Arduino D4 pin 5 connector CN10)
  - (Optional) Oscilloscope probe to visualize the signal
2. Run the software
3. It configures the Active Tamper Input associated to an Output and enables the interrupt.
4. It writes  data to the RTC Backup registers, then check if the data are correctly written.
5. It updates the seed (optional).
6. Please disconnect the pins. 
7. The RTC backup register are reset and the Tamper interrupt is generated.
   The firmware then checks if the RTC Backup register are cleared.
8. LD3 turns ON, Test is OK.
   LD3 blinks, Test is KO.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, RTC, Tamper, Reset, LSE, Backup, Active

@par Directory contents
  - RTC/RTC_ActiveTamper/Inc/stm32wba55g_discovery_conf.h     BSP configuration file
  - RTC/RTC_ActiveTamper/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - RTC/RTC_ActiveTamper/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - RTC/RTC_ActiveTamper/Inc/main.h                  Header for main.c module
  - RTC/RTC_ActiveTamper/Src/stm32wbaxx_it.c          Interrupt handlers
  - RTC/RTC_ActiveTamper/Src/main.c                  Main program
  - RTC/RTC_ActiveTamper/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - RTC/RTC_ActiveTamper/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics STM32WBA55G-DK1
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


