## <b>FLASH_DualBoot Example Description</b>
This example guides you through the different configuration steps by mean of HAL API
how to program bank1 and bank2 of the STM32WBAxx internal Flash memory mounted on NUCLEO-WBA65RI
and swap between both of them.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 100 MHz.

Below are the steps to run this example:

1- Select bank2 configuration by commenting FLASH_BANK1 and uncommenting FLASH_BANK2 defines
   in "main.h", and generate its binary (ie: FLASH_DualBoot.bin)
##### <b>note:</b>
      You have to configure your preferred toolchain in order to generate the binary
      file after compiling the project.
	  
2- Load this binary at the bank2 of the flash(at the address 0x08100000) using
   STM32 CubeProgrammer.


3- Select bank1 configuration by uncommenting FLASH_BANK1 and commenting FLASH_BANK2 defines
   in "main.h", and run it, this project will be loaded in the bank1 of the flash: at the
   address 0x08000000

4- Click the USER push-button to swap between the two banks

- If program in bank1 is selected, LD1 is turn on.

- If program in bank2 is selected, LD2 is turn on.

- If error occurs LD3 is turn on.

#### <b>Notes</b>
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. After executing this example , the SWAP_BANK option byte will be set ,
    Use STM32CubeProgrammer to change this option byte back to "Not swapped" if needed. 

### <b>Keywords</b>

Memory, Flash, Dual boot, Dual Bank, Execute, Binary, Option bytes

### <b>Directory contents</b>

  - FLASH/FLASH_DualBoot/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32wbaxx_hal_conf.h        HAL Configuration file
  - FLASH/FLASH_DualBoot/Inc/stm32wbaxx_it.h              Header for stm32wbaxx_it.c
  - FLASH/FLASH_DualBoot/Inc/main.h                       Header for main.c module
  - FLASH/FLASH_DualBoot/Src/stm32wbaxx_it.c              Interrupt handlers
  - FLASH/FLASH_DualBoot/Src/main.c                       Main program
  - FLASH/FLASH_DualBoot/Src/stm32wbaxx_msp_hal.c         MSP initialization and de-initialization
  - FLASH/FLASH_DualBoot/Src/system_stm32wbaxx.c          STM32WBAxx system clock configuration file
  - FLASH/FLASH_DualBoot/Binary/FLASH_DualBoot.bin        Binary file to load at bank2

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RIVx devices.

  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
