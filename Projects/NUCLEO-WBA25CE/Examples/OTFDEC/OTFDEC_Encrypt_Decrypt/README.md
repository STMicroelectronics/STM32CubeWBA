## <b>OTFDEC_Encrypt_Decrypt Example Description</b>

How to use a OTFDEC peripheral to decrypt data stored in the external memory.

This example describes how to encrypt data using AES cipher algorithm and to decrypt  the cipherred data stored NOR memory

 - LED1 is on when the data have been checked successfully
 - LED3 toogles when an error occured

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, OTFDEC, AES, XSPI, NOR, Memory Mapped, QSPI, MX25R3235F

### <b>Directory contents</b>

  - OTFDEC/OTFDEC_Encrypt_Decrypt/Src/main.c                 	Main program
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Src/system_stm32wbaxx.c     	STM32WBA2xx system clock configuration file
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Src/stm32wbaxx_it.c         	Interrupt handlers
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Src/stm32wbaxx_hal_msp.c    	HAL MSP module
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Inc/main.h                 	Main program header file
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Inc/stm32wbaxx_hal_conf.h   	HAL Configuration file
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Inc/stm32wbaxx_it.h         	Interrupt handlers header file
  - OTFDEC/OTFDEC_Encrypt_Decrypt/Inc/stm32wbaxx_hal_conf.h     HAL configuration file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA25CEUx devices.

  - This example has been tested with NUCLEO-WBA25CE board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

