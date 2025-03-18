## <b>RAMCFG_Parity_Error Example Description</b>

How to configure and use the RAMCFG HAL API to enable parity error detection and generate parity error interruption.

This project is targeted to run on STM32WBA55CGUx devices on NUCLEO-WBA55CG board from STMicroelectronics.

The project configures the maximum system clock frequency at 100 MHz.

The example performs the following actions:

 1. First, enable parity detection feature with SRAM2_PE bit inside FLASH_OPTR.
    LD 1 toggles 10 times at 100 ms to indicate the software step.
    A software reset is carried out to launch the option bytes reload.

 2. Upon reset, erase and fill all SRAM2 memory with known data and then
    disable parity detection feature with SRAM2_PE bit inside FLASH_OPTR.
    LD 1 toggles 10 times at 100 ms to indicate the software step. 
    A software reset is carried out to launch the option bytes reload.

 3. Upon reset, change just one bit inside each SRAM2 words and then
    enable again the parity detection feature with SRAM2_PE bit inside FLASH_OPTR.
    LD 1 toggles 10 times at 100 ms to indicate the software step.
    A software reset is carried out to launch the option bytes reload.

 4. Upon reset, enable latch parity error address and parity Error Interruption.
    The software accesses to different SRAM2 areas thru CPU or thru DMA accesses and checks that PED interrupt is triggered.

There are 3 different read access types at several SRAM2 addresses that can chose to be performed or not with ArraySRAMParityError array:
 - Parity Error generated @ 0x20014000 on byte position 2 with CPU read access.
 - Parity Error generated @ 0x2001FFF0 on byte position 4 with DMA read access with port 0.
 - Parity Error generated @ 0x20018654 on byte position 1 with DMA read access with port 1.

NUCLEO-WBA55CG board's LED can be used to monitor the project operation status:
 - LD1 toggles 10 times at 100 ms to indicate each software step.
 - LD2 toggles when no error detected.
 - LD3 is ON when any project error was occurred.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

RAMCFG, Memory, SRAM, Parity Error Detection, DMA

### <b>Directory contents</b>

  - RAMCFG/RAMCFG_ParityError/Src/main.c                        Main program
  - RAMCFG/RAMCFG_ParityError/Src/system_stm32wbaxx.c           STM32WBAxx system clock configuration file
  - RAMCFG/RAMCFG_ParityError/Src/stm32wbaxx_it.c               Interrupt handlers
  - RAMCFG/RAMCFG_ParityError/Src/stm32wbaxx_hal_msp.c          HAL MSP module
  - RAMCFG/RAMCFG_ParityError/Inc/main.h                        Main program header file
  - RAMCFG/RAMCFG_ParityError/Inc/stm32wbaxx_hal_conf.h         HAL Configuration file
  - RAMCFG/RAMCFG_ParityError/Inc/stm32wbaxx_it.h               Interrupt handlers header file
  - RAMCFG/RAMCFG_ParityError/Inc/stm32wbaxx_nucleo_conf.h      HAL configuration file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Because of software reset during the example, the initial debugger connection will be lost, so performs an 'attach to running target' 
   to be able to get the debugger connection back after the step 3.
