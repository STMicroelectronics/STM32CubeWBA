## <b>SPI_OneBoard_HalfDuplex_IT_Init Example Description</b>

Configuration of GPIO and SPI peripherals to transmit bytes
from an SPI Master device to an SPI Slave device in Interrupt mode. This example
is based on the STM32WBAxx SPI LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

This example works with only one NUCLEO-WBA52CG.

- SPI1 Peripheral is configured in Master mode Half-Duplex Tx.
- SPI3 Peripheral is configured in Slave mode Half-Duplex Rx.
- GPIO associated to User push-button is linked with EXTI.

Example execution:

LD2 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD start a Half-Duplex communication through IT.
On MASTER side (SPI1), Clock will be generated on SCK line, Transmission done on MOSI Line.
On SLAVE side (SPI3) reception is done through the MISO Line.

LD2 is On if data is well received.

In case of errors, LD2 is blinking Slowly (1s).

### <b>Keywords</b>

Connectivity, SPI, Half-duplex, Transmission, Reception, Master, Slave, MISO, MOSI, Interrupt

### <b>Directory contents</b>

  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Inc/main.h                  Header for main.c module
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Src/stm32wbaxx_it.c         Interrupt handlers
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Src/main.c                  Main program
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Src/system_stm32wbaxx.c     STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
    - Connect Master SCK  PB4 to Slave SCK  PA0
    - Connect Master MOSI PA15 to Slave MISO PB9

Relation with Board connector:

  - PB4  is connected to pin 6 of CN6 connector
  - PA15  is connected to pin 4 of CN6 connector
  - PA0 is connected to pin 6 of CN7 connector
  - PB9 is connected to pin 8 of CN8 connector

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

