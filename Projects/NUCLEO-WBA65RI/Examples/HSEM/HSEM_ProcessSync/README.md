## <b>HSEM_ProcessSync Example Description</b>

How to use a HW semaphore to synchronize 2 process.

In this example, HW semaphore (9) is used to synchronize 2 process.

process 1 : Toggle LD1 every 500 ms
process 2 : Set LD2 On.

First, semaphore (9) is taken using HAL API HAL_HSEM_Take with process ID  set to "12". 
If the semaphore is taken LD1 every 500 ms .
The semaphore is released after 5 seconds, using HAL API HAL_HSEM_Release. During this period, the LD1 
is toggling to mention that the semaphore is not available.

Note that HAL_HSEM_Take use the 2-Step semaphore take method (refer to the HW Semaphore
description in the reference manual)

Pushing the USER push-button will generate an EXTI interrupt. 
The corresponding EXTI callback will try to take the same semaphore (9) using 
HAL API HAL_HSEM_FastTake. 
If the semaphore has been released by first process , then semaphore can be taken 
immediately and it will set LD2 On.

If not, HW semaphore 9 not released yet by the first process , then the HAL_HSEM_FastTake API 
will return HAL_ERROR meaning that the semaphore is already taken by another process.
In this case the HAL API HAL_HSEM_ActivateNotification enable the notification receive once 
the semaphore will be released. 
Treatment will be postponed until the semaphore is released.
Once done, a HSEM interrupt is generated (semaphore 9), and HAL_HSEM_FreeCallback is asserted 
to set LD2 On.

Note that the API HAL_HSEM_FastTake use the 1-Step semaphore take method (refer to the HW Semaphore
description in the reference manual) where the process ID is assumed to be zero.

As a result both process will be synchronized, second process will toggle LD2
only when first process has ended toggle LD1 10 times.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Hardware semaphore, Semaphore, HSEM, EXTI, synchronize, Lock, Unlock, Take, Release, Process

### <b>Directory contents</b>

  - HSEM/HSEM_ProcessSync/Inc/stm32wbaxx_hal_conf.h   HAL configuration file
  - HSEM/HSEM_ProcessSync/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - HSEM/HSEM_ProcessSync/Inc/main.h                  Header for main.c module  
  - HSEM/HSEM_ProcessSync/Src/stm32wbaxx_it.c         Interrupt handlers
  - HSEM/HSEM_ProcessSync/Src/stm32wbaxx_hal_msp.c    HAL MSP file
  - HSEM/HSEM_ProcessSync/Src/main.c                  Main program
  - HSEM/HSEM_ProcessSync/Src/system_stm32wbaxx.c     stm32wbaxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RIVx device.
    
  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

