## <b>HSEM_ReadLock Example Description</b>

How to enable, take then release semaphore using 2 different Process.

First, Enable HW semaphore using __HAL_RCC_HSEM_CLK_ENABLE macro.
Secondly, Semaphore 9 with Process1(ID=12) is locked in 2-step(Write) lock procedure.

    ==> LD1 is ON when lock with Process1 is done correctly.

Third, Lock the same semaphore 9  with Process1(ID 13) via the same semaphore 9 using 2-step(Write) lock procedure,

    ==> No lock is done, because the semaphore 9 is already locked. otherwise LD2 is ON.

Finally, semaphore 9 with Process1(ID=12) is released and take the same semaphore 9 with Process2(ID=13)

    ==> Check take status, if equal to 0, release done otherwise, LD2 is ON.
    ==> LD1 is ON, when lock with Process2 is done correctly.  

In every step, if any mismatch with right result, LD2 is ON.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Hardware semaphore, Semaphore, HSEM, synchronize, Lock, Unlock, Take, Release, Process

### <b>Directory contents</b>

  - HSEM/HSEM_ReadLock/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - HSEM/HSEM_ReadLock/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - HSEM/HSEM_ReadLock/Inc/main.h                   Header for main.c module  
  - HSEM/HSEM_ReadLock/Src/stm32wbaxx_it.c          Interrupt handlers
  - HSEM/HSEM_ReadLock/Src/stm32wbaxx_hal_msp.c     HAL MSP file
  - HSEM/HSEM_ReadLock/Src/main.c                   Main program
  - HSEM/HSEM_ReadLock/Src/system_stm32wbaxx.c      stm32wbaxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx device.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

