## <b>HAL_TimeBase_TIM Example Description</b>

How to customize HAL using a general-purpose timer as main source of time base 
instead of Systick.

In this example the used timer is TIM17.

Time base duration is kept unchanged: 1ms  since PPP_TIMEOUT_VALUEs are defined 
and handled in milliseconds basis.

The example brings, in user file, a new implementation of the following HAL weak functions:

	HAL_InitTick() 
	HAL_SuspendTick()
	HAL_ResumeTick()

This implementation will overwrite native implementation in stm32wbaxx_hal.c
and so user functions will be invoked instead when called.

The following time base functions are kept as implemented natively:

	HAL_IncTick()
	HAL_Delay()

When user pushes the USER push-button, the Tick increment is suspended if it is already
enabled, else it will be resumed.
In an infinite loop, LD1 toggles spaced out over 1s delay.

### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in TIM17 ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the TIM17 interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the TIM17 interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 2. The application needs to ensure that the TIM17 time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, TIM, Time base, HAL

### <b>Directory contents</b>

  - HAL/HAL_TimeBase_TIM/Inc/stm32wbaxx_nucleo_conf.h       BSP configuration file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbaxx_hal_conf.h          HAL configuration file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbaxx_it.h                Interrupt handlers header file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbaxx.h                   Header for main.c module  
  - HAL/HAL_TimeBase_TIM/Src/stm32wbaxx_it.c                Interrupt handlers
  - HAL/HAL_TimeBase_TIM/Src/main.c                         Main program
  - HAL/HAL_TimeBase_TIM/Src/stm32wbaxx_hal_msp.c           HAL MSP file
  - HAL/HAL_TimeBase_TIM/Src/stm32wbaxx_hal_timebase_tim.c  HAL time base functions
  - HAL/HAL_TimeBase_TIM/Src/system_stm32wbaxx.c            STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

