## <b>DMA_FLASHToRAM Example Description</b>

How to use a DMA to transfer a word data buffer from Flash memory to embedded
SRAM through the STM32WBAxx DMA HAL and LL API. The LL API is used for 
performance improvement.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

LL_DMA_CHANNEL_1 is configured to transfer the contents of a 32-word data
buffer stored in Flash memory to the reception buffer declared in RAM.

The start of transfer is triggered by LL API. LL_DMA_CHANNEL_1 memory-to-memory
transfer is enabled. Source and destination addresses incrementing is also enabled.
The transfer is started by setting the channel enable bit for LL_DMA_CHANNEL_1.
At the end of the transfer a Transfer Complete interrupt is generated since it
is enabled and the callback function (customized by user) is called.

Board's LEDs can be used to monitor the transfer status:

 - LD1 is ON when the transfer is complete (into the Transfer Complete interrupt
   routine).
 - LD3 is ON when an Error_Handler is called

It is possible to select a different channel for the DMA transfer
example by modifying defines values in the file main.h.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, DMA, Data Transfer, Memory to memory, Flash, SRAM

### <b>Directory contents</b>

  - DMA/DMA_FLASHToRAM/Src/system_stm32wbaxx.c      STM32WBAxx system clock configuration file
  - DMA/DMA_FLASHToRAM/Src/stm32wbaxx_it.c          Interrupt handlers
  - DMA/DMA_FLASHToRAM/Src/main.c                   Main program
  - DMA/DMA_FLASHToRAM/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - DMA/DMA_FLASHToRAM/Src/stm32wbaxx_nucleo_conf.h BSP configuration file
  - DMA/DMA_FLASHToRAM/Inc/stm32wbaxx_hal_conf.h    HAL Configuration file
  - DMA/DMA_FLASHToRAM/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - DMA/DMA_FLASHToRAM/Inc/main.h                   Main program header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx Devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

