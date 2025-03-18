## <b>SPI_FullDuplex_ComIT_Master example Description</b>

Data buffer transmission/reception between two boards via SPI using Interrupt mode.

Board: NUCLEO-WBA65RI (embeds a STM32WBA65RIVx device)
CLK Pin: PA7 (CN4, pin 22)
MISO Pin: PA1 (CN4, pin 1)
MOSI Pin: PD5 (CN7, pin 4)

HAL architecture allows user to easily change code to move to DMA or Polling 
mode. To see others communication modes please check following examples:

- SPI\SPI_FullDuplex_ComPolling_Master and SPI\SPI_FullDuplex_ComPolling_Slave
- SPI\SPI_FullDuplex_ComDMA_Master and SPI\SPI_FullDuplex_ComDMA_Slave

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive_IT() function allows the reception and the 
transmission of a predefined data buffer at the same time (Full Duplex Mode).
If the Master board is used, the project SPI_FullDuplex_ComIT_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComIT_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button, SPI Master starts the
communication by sending aTxBuffer and receiving aRxBuffer through 
HAL_SPI_TransmitReceive_IT(), at the same time SPI Slave transmits aTxBuffer 
and receives aRxBuffer through HAL_SPI_TransmitReceive_IT(). 
The callback functions (HAL_SPI_TxRxCpltCallback and HAL_SPI_ErrorCallbackand) update 
the variable wTransferState used in the main function to check the transfer status.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

STM32 board's LEDs can be used to monitor the transfer status:

 - LD1 toggles quickly on master board waiting USER push-button to be pressed.
 - LD1 turns ON when the transmission process is complete.
 - LD2 turns ON when the reception process is complete.
 - LD3 turns ON when there is an error in transmission/reception process.  

**Note**  You need to perform a reset on Slave board, then perform it on Master board
to have the correct behaviour of this example.

#### <b>Notes</b>
 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Interrupt, Transmission, Reception, Master, Slave, MISO, MOSI

### <b>Directory contents</b>

  - SPI/SPI_FullDuplex_ComIT_Master/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - SPI/SPI_FullDuplex_ComIT_Master/Inc/stm32wbaxx_hal_conf.h        HAL configuration file
  - SPI/SPI_FullDuplex_ComIT_Master/Inc/stm32wbaxx_it.h              Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComIT_Master/Inc/main.h                       Header for main.c module  
  - SPI/SPI_FullDuplex_ComIT_Master/Src/stm32wbaxx_it.c              Interrupt handlers
  - SPI/SPI_FullDuplex_ComIT_Master/Src/main.c                       Main program
  - SPI/SPI_FullDuplex_ComIT_Master/Src/system_stm32wbaxx.c          stm32wbaxx system source file
  - SPI/SPI_FullDuplex_ComIT_Master/Src/stm32wbaxx_hal_msp.c         HAL MSP file

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA65RIVx devices.

  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA65RI Set-up
    - Connect Master board PA7 (CN4, pin 22) to Slave Board PA7 (CN4, pin 22)
    - Connect Master board PA1 (CN4, pin 1) to Slave Board PA1 (CN4, pin 1)
    - Connect Master board PD5 (CN7, pin 4) to Slave Board PD5 (CN7, pin 4)
    - Connect Master board GND  to Slave Board GND

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files (master project) and load your image into target memory
    o Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    o Load the project in Slave Board
 - Run the example

 