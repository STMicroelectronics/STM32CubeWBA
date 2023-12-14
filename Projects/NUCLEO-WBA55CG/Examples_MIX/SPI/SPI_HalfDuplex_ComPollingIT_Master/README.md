## <b>SPI_HalfDuplex_ComPollingIT_Master Example Description</b>

Data buffer transmission/reception between 
two boards via SPI using Polling (LL driver) and Interrupt modes (HAL driver).

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK & 
GPIO). You may update this function to change SPI configuration.
The Half-Duplex SPI transmission (8bit) is done using LL Driver on Master board (Tx) by using function 
LL_SPI_TransmitData8.
The The Half-Duplex SPI reception (8bit) is done using HAL Driver on Slave board (Rx) by using function 
HAL_SPI_Receive_IT.

Example execution:
First step, press the USER push-button, this action initiates a Half-Duplex transfer 
between Master and Slave.
After end of transfer, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.

STM32 board's LEDs can be used to monitor the transfer status:

 - LD2 toggles quickly on master board waiting USER push-button to be pressed.
 - LD2 turns ON if reception is complete and OK.
 - LD3 turns ON when there is an error in transmission/reception process.  

#### <b>Notes</b>

 1. You need to perform a reset on Slave board, then perform it on Master board
    to have the correct behaviour of this example.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 4. Note that LD1 remains ON cause it shares the same pin with SPI1 SCK (PB.04).

### <b>Keywords</b>

Connectivity, SPI, Half-duplex, Transmission, Reception, Master, MISO, MOSI, Polling, Interrupt

### <b>Directory contents</b>

  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/stm32wbaxx_hal_conf.h   HAL configuration file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/stm32wbaxx_it.h         SPI interrupt handlers header file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Inc/main.h                  Header for main.c module  
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/stm32wbaxx_it.c         SPI interrupt handlers
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/main.c                  Main program
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/system_stm32wbaxx.c     STM32WBAxx system source file
  - Examples_MIX/SPI/SPI_HalfDuplex_ComPollingIT_Master/Src/stm32wbaxx_hal_msp.c    HAL MSP file    


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - Take care to cable connection between Master and Slave Board:
    Cable shall be smaller than 5 cm and rigid if possible.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up
    - Connect Master board PB4 (CN6, pin 6) to Slave Board PB4 (CN6, pin 6)
    - Connect Master board PA15 (CN6, pin 4) to Slave Board PB3 (CN6, pin 5)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files (master project) and load your image into target memory
    - Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    - Load the project in Slave Board
 - Run the example

