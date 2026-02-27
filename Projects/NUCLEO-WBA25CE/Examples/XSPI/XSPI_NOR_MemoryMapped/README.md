## <b>XSPI_NOR_MemoryMapped Example Description</b>

How to use a XSPI peripheral to access NOR memory (MX25R3235F) in memory-mapped mode.

This example describes how to erase a part of a QSPI NOR memory, write data in indirect mode and
access it in memory-mapped mode to check the data in an infinite loop.

 - LED1 toggles each time the data have been checked
 - LED2 is on as soon as a a data is wrong
 - LED3 is on as soon as an error is returned by HAL API

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Memory, XSPI, NOR, Memory Mapped, QSPI, MX25R3235F

### <b>Directory contents</b>

  - XSPI/XSPI_NOR_MemoryMapped/Src/main.c                 	Main program
  - XSPI/XSPI_NOR_MemoryMapped/Src/system_stm32wbaxx.c     	STM32WBA2xx system clock configuration file
  - XSPI/XSPI_NOR_MemoryMapped/Src/stm32wbaxx_it.c         	Interrupt handlers 
  - XSPI/XSPI_NOR_MemoryMapped/Src/stm32wbaxx_hal_msp.c    	HAL MSP module
  - XSPI/XSPI_NOR_MemoryMapped/Inc/main.h                 	Main program header file  
  - XSPI/XSPI_NOR_MemoryMapped/Inc/stm32wbaxx_hal_conf.h   	HAL Configuration file
  - XSPI/XSPI_NOR_MemoryMapped/Inc/stm32wbaxx_it.h         	Interrupt handlers header file
  - XSPI/XSPI_NOR_MemoryMapped/Inc/stm32wbaxx_hal_conf.h    HAL configuration file  

        
### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA25CEUx devices.

  - This example has been tested with NUCLEO-WBA25CE board and can be
    easily tailored to any other supported device and development board.
  
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

