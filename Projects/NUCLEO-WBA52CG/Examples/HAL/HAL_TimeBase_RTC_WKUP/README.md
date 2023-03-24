## <b>HAL_TimeBase_RTC_WKUP Example Description</b>

How to customize HAL using RTC wakeup as main source of time base, 
instead of Systick.

The USER push-button is used to suspend or resume tick increment. 

Each time the button is pressed; an interrupt is generated (External line 13)
and in the ISR the uwIncrementState is checked:

  1. If the uwIncrementState = 0: the tick increment is suspended by calling 
     HAL_SuspendTick() API (RTC wakeup timer interrupt is disabled).
  2. If the uwIncrementState = 1: the tick increment is Resumed by calling 
     HAL_ResumeTick() API(RTC wakeup timer interrupt is enabled).

The wakeup feature is configured to assert an interrupt each 1ms.

The example brings, in user file, a new implementation of the following HAL weak functions:

    HAL_InitTick() 
    HAL_SuspendTick()
    HAL_ResumeTick()

This implementation will overwrite native implementation in stm32wbaxx_hal.c
and so user functions will be invoked instead when called.

The following time base functions are kept as implemented natively:

    HAL_IncTick()
    HAL_Delay()

In an infinite loop, LD1 toggles spaced out over 500ms delay, except when tick increment is suspended.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in HAL time base ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the HAL time base interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the HAL time base interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 2. The application needs to ensure that the HAL time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, RTC Wakeup, Time base, HAL

### <b>Directory contents</b>

  - HAL/HAL_TimeBase_RTC_WKUP/Inc/stm32wbaxx_nucleo_conf.h             BSP configuration file
  - HAL/HAL_TimeBase_RTC_WKUP/Inc/stm32wbaxx_hal_conf.h                HAL configuration file
  - HAL/HAL_TimeBase_RTC_WKUP/Inc/stm32wbaxx_it.h                      Interrupt handlers header file
  - HAL/HAL_TimeBase_RTC_WKUP/Inc/main.h                               Header for main.c module  
  - HAL/HAL_TimeBase_RTC_WKUP/Src/stm32wbaxx_it.c                      Interrupt handlers
  - HAL/HAL_TimeBase_RTC_WKUP/Src/main.c                               Main program
  - HAL/HAL_TimeBase_RTC_WKUP/Src/stm32wbaxx_hal_msp.c                 HAL MSP file
  - HAL/HAL_TimeBase_RTC_WKUP/Src/stm32wbaxx_hal_timebase_rtc_wakeup.c HAL time base functions
  - HAL/HAL_TimeBase_RTC_WKUP/Src/system_stm32wbaxx.c                  STM32WBAxx system source file

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA52CGUx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.      

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 