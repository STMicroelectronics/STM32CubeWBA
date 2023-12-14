## <b>SPI_TwoBoards_FullDuplex_DMA_Slave_Init Example Description</b>

Data buffer transmission and receptionvia SPI using DMA mode. This example is
based on the STM32WBAxx SPI LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

The communication is done with 2 boards through SPI.

This example shows how to configure GPIO and SPI peripherals
to use a Full-Duplex communication using DMA Transfer mode through the STM32WBAxx SPI LL API.

This example is split in two projects, Master board and Slave board:

- Master Board :
  - SPI1 Peripheral is configured in Master mode.
  - DMA1_Channel2 and DMA1_Channel1 configured to transfer Data via SPI peripheral
  - GPIO associated to USER push-button is linked with EXTI.

- Slave Board :
  - SPI1 Peripheral is configured in Slave mode.
  - DMA1_Channel2 and DMA1_Channel1 configured to transfer Data via SPI peripheral


Example execution:

On BOARD MASTER, LD2 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD MASTER start a Full-Duplex communication through DMA.
On MASTER side, Clock will be generated on SCK line, Transmission(MOSI Line) and reception (MISO Line)
will be done at the same time.
SLAVE SPI will received  the Clock (SCK Line), so Transmission(MISO Line) and reception (MOSI Line) will be done also.

LD2 is On on both boards if data is well received.

In case of errors, LD2 is blinking Slowly (1s).

#### <b>Notes</b>

 - You need to perform a reset on Master board, then perform it on Slave board
   to have the correct behaviour of this example.
 - LD1 remains ON cause it shares the same pin with SPI1 SCK (PB.04).

### <b>Keywords</b>

Connectivity, SPI, Full-duplex, Interrupt, Transmission, Reception, Master, Slave, MISO, MOSI, DMA

### <b>Directory contents</b>

  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Inc/main.h                  Header for main.c module
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Src/stm32wbaxx_it.c         Interrupt handlers
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Src/main.c                  Main program
  - SPI/SPI_TwoBoards_FullDuplex_DMA_Slave_Init/Src/system_stm32wbaxx.c     STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on NUCLEO-WBA55CG devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up
    - Connect Master board PB4 to Slave Board PB4 (connected to pin 6 of CN6 connector)
    - Connect Master board PB3 to Slave Board PB3 (connected to pin 5 of CN6 connector)
    - Connect Master board PA15 to Slave Board PA15 (connected to pin 4 of CN6 connector)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
    - Load the Master project in Master Board
    - Load the Slave project in Slave Board
 - Run the example

