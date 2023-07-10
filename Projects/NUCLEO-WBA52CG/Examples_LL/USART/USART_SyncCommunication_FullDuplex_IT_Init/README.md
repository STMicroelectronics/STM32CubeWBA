## <b>USART_SyncCommunication_FullDuplex_IT_Init Example Description</b>

Configuration of GPIO, USART, DMA and SPI peripherals to transmit 
bytes between a USART and an SPI (in slave mode) in Interrupt mode. This example is based on the STM32WBAxx USART LL API 
(the SPI uses the DMA to receive/transmit characters sent from/received by the USART). The peripheral 
initialization uses LL unitary service functions for optimization purposes (performance and size).

This example works with only one NUCLEO-WBA52CG.

USART1 Peripheral is acts as Master using IT.
SPI1 Peripheral is configured in Slave mode using DMA.
GPIO associated to USER push-button is linked with EXTI.

Example execution:

LD2 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD start a Full-Duplex communication from USART using IT.
On USART1 side, Clock will be generated on SCK line, Transmission done on TX, reception on RX.
On SPI1 side, reception is done through the MOSI Line, transmission on MISO line.

At end of transmission, both received buffers are compared to expected ones.
In case of both transfers successfully completed, LD2 is turned on.
In case of errors, LD2 is blinking Slowly (1s).

**Note:** that LD1 remains ON cause it shares the same pin with SPI1 SCK (PB.04).

### <b>Keywords</b>

Connectivity, UART/USART, Synchronous, Interrupt, SPI interface, Receiver, Transmitter, full-duplex

### <b>Directory contents</b>

  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Inc/stm32wbaxx_it.h        Interrupt handlers header file
  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Inc/main.h                 Header for main.c module
  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Inc/stm32_assert.h         Template file to include assert_failed function
  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Src/stm32wbaxx_it.c        Interrupt handlers
  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Src/main.c                 Main program
  - USART/USART_SyncCommunication_FullDuplex_IT_Init/Src/system_stm32wbaxx.c    STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
    - Connect USART1 SCK PA.05 to Slave SPI1 SCK PB.04
    - Connect USART1 TX PB.12 to Slave SPI1 MOSI PA.15
    - Connect USART1 RX PA.08 to Slave SPI1 MISO PB.03

Board connector:

 - PA.05 :   connected to pin 36 of CN3 connector
 - PB.12 :   connected to pin 37 of CN3 connector
 - PA.08 :   connected to pin 35 of CN3 connector
 - PB.04 :   connected to pin 6 of CN4 connector
 - PA.15 :   connected to pin 15 of CN4 connector
 - PB.03 :   connected to pin 10 of CN4 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Launch the program. Press on USER push-button on board to initiate data transfers.

