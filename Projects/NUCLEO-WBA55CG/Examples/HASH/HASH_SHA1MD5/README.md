## <b>HASH_SHA1MD5 Description</b>

This example provides a short description of how to use the HASH peripheral to 
hash data using SHA-1 and MD5 Algorithms.

For this example, DMA is not used for data transfer, the CPU is using HASH peripheral in
polling mode.

The SHA-1 message digest result is a 160 bit data and the MD5 message digest result 
is a 128 bit data.

The expected HASH digests (for SHA1 and MD5) are already computed using an online
HASH tool. Those values are compared to those computed by the HASH peripheral.

- In case of digest computation mismatch or initialization issue the LD3 is blinking (200 ms period).
- In case the SHA1 digest is computed correctly the LD1 is turned ON.
- In case the MD5 digest is computed correctly the LD2 is turned ON.

### <b>Keywords</b>

System, Security, HASH, SHA1, MD5, digest

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Directory contents</b>

  - HASH/HASH_SHA1MD5/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - HASH/HASH_SHA1MD5/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - HASH/HASH_SHA1MD5/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - HASH/HASH_SHA1MD5/Inc/main.h                   Header for main.c module
  - HASH/HASH_SHA1MD5/Src/stm32wbaxx_it.c          Interrupt handlers
  - HASH/HASH_SHA1MD5/Src/main.c                   Main program
  - HASH/HASH_SHA1MD5/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - HASH/HASH_SHA1MD5/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with a STM32WBA55CGUx embedded on an
    NUCLEO-WBA55CG board and can be easily tailored to any other supported
    device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example 
