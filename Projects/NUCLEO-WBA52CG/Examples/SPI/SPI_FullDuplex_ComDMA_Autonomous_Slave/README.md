## <b>SPI_FullDuplex_ComDMA_Autonomous_Slave Example Description</b>

Data buffer transmission/reception between two boards via SPI using DMA.

	Board: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
	CLK Pin: PB.04 (6 on CN4 connector)
	MISO Pin: PB.03 (10 on CN4 connector)
	MOSI Pin: PA.15 (15 on CN4 connector)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The SPI peripheral configuration is ensured by the HAL_SPI_Init() function.
This later is calling the HAL_SPI_MspInit()function which core is implementing
the configuration of the needed SPI resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change SPI configuration.

The SPI communication is then initiated.
The HAL_SPI_TransmitReceive_DMA() function allows the reception and the
transmission of a predefined data buffer at the same time (Full Duplex Mode).

If the Master board is used, the project SPI_FullDuplex_ComDMA_Autonomous_Master must be used.
If the Slave board is used, the project SPI_FullDuplex_ComDMA_Slave must be used.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button, SPI Master starts the
communication, after the trigger event, by sending aTxBuffer and receiving aRxBuffer
through HAL_SPI_TransmitReceive_DMA(), at the same time SPI Slave transmits
aTxBuffer and receives aRxBuffer through HAL_SPI_TransmitReceive_DMA().
The callback functions (HAL_SPI_TxRxCpltCallback and HAL_SPI_ErrorCallbackand) update
the variable wTransferState used in the main function to check the transfer status.
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

STM32 board's LEDs can be used to monitor the transfer status:

 - LD2 toggles quickly on master board waiting USER push-button to be pressed.
 - LD2 turns ON when the transmission/reception process is complete.
 - LD3 turns ON when there is an error in transmission/reception process.

**Note** that LD1 remains ON because it shares the same pin with SPI1_SCK (pin PB4).

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

### <b>Keywords</b>

Connectivity, SPI, full-duplex, DMA, baud rate, data size, Trigger Mode, Master, Slave, Transmitter, Receiver

### <b>Directory contents</b>

  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Inc/main.h                   Header for main.c module
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Src/stm32wbaxx_it.c          Interrupt handlers
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Src/main.c                   Main program
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Src/system_stm32wbaxx.c      stm32wbaxx system source file
  - SPI/SPI_FullDuplex_ComDMA_Autonomous_Slave/Src/stm32wbaxx_hal_msp.c     HAL MSP file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up:

    - Connect Master board PB.04 (6 on CN4 connector) to Slave Board PB.04 (6 on CN4 connector)
    - Connect Master board PB.03 (10 on CN4 connector) to Slave Board PB.03 (10 on CN4 connector)
    - Connect Master board PA.15 (15 on CN4 connector) to Slave Board PA.15 (15 on CN4 connector)
    - Connect Master board GND  to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files (master project) and load your image into target memory
    - Load the project in Master Board
 - Rebuild all files (slave project) and load your image into target memory
    - Load the project in Slave Board
 - Run the example

