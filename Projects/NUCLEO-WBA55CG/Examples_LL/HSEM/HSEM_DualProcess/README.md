## <b>HSEM_DualProcess Example Description</b>

How to use the low-layer HSEM API to initialize, lock, and unlock hardware 
semaphore in the context of two processes accessing the same resource.

Example execution:
After startup from reset and system configuration, HSEM configuration is performed.

Two processes are used to demonstrate the features:

 - PROCESS_A with an identifier of 0xA (This can be changed at will).
 - PROCESS_B with an identifier of 0xB (This can be changed at will).

The hardware semaphore used is 0x2 (This can be changed at will).

 - PROCESS_A takes the semaphore with success.
 - PROCESS_B takes the semaphore and fail (as already taken by PROCESS_A).
 - PROCESS_B releases the semaphore and fail (as owned by PROCESS_A).
 - PROCESS_A releases the semaphore with success.

After successful sequence, LD1 is turned On.
 
In case of errors, LD1 is slowly blinking (1sec period).

Additionally, this example demonstrate how to:

 - Retrieve the current process locking a semaphore.
 - Retrieve the current core locking a semaphore.

**Remarks:**
  As this example uses two processes to demonstrate the lock/unlock mechanism, it is not
  possible to use the function LL_HSEM_1StepLock which does not use process identifier. Those
  identifier are set to 0. This function is only interesting in the context of multicore where
  the core identifiers are used to determine the owner of the semaphore.

### <b>Keywords</b>

Hardware semaphore, Semaphore, HSEM, Lock, Unlock, Take, Release, Process

### <b>Directory contents</b> 

  - HSEM/HSEM_DualProcess/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - HSEM/HSEM_DualProcess/Inc/main.h                  Header for main.c module
  - HSEM/HSEM_DualProcess/Inc/stm32_assert.h          Template file to include assert_failed function
  - HSEM/HSEM_DualProcess/Src/stm32wbaxx_it.c         Interrupt handlers
  - HSEM/HSEM_DualProcess/Src/main.c                  Main program
  - HSEM/HSEM_DualProcess/Src/system_stm32wbaxx.c     STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example and observe the LED status

