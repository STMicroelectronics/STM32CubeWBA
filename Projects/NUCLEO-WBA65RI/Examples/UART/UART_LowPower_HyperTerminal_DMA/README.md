## <b>UART_LowPower_HyperTerminal_DMA Example Description</b>

Low power UART transmission (transmit/receive) in DMA mode between a board and
an HyperTerminal PC application.

    Board: NUCLEO-WBA65RI (Connector : CN7)
      Tx Pin PA2: (A2 pin 3 CN7)
      Rx Pin PA1: (A3 pin 4 CN7)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware (CLOCK,
GPIO, DMA and NVIC). You may update this function to change UART configuration.

The UART/Hyperterminal communication is then initiated.
The HAL_UART_Receive_DMA() and the HAL_UART_Transmit_DMA() functions allow respectively
the reception of Data from Hyperterminal and the transmission of a predefined data
buffer.

The Asynchronous communication aspect of the UART is clearly highlighted as the
data buffers transmission/reception to/from Hyperterminal are done simultaneously.

For this example the TxBuffer is predefined and the RxBuffer size is limited to
10 data by the mean of the RXBUFFERSIZE define in the main.c file.

In a first step the received data will be stored in the RxBuffer buffer and the
TxBuffer buffer content will be displayed in the Hyperterminal interface.
In a second step the received data in the RxBuffer buffer will be sent back to
Hyperterminal and displayed.

The end of this two steps are monitored through the HAL_UART_GetState() function
result.

NUCLEO-WBA65RI board LEDs are used to monitor the transfer status:

 - LD1 is ON when the transmission process is complete.
 - LD2 is ON when the reception process is complete.
 - LD3 is ON when there is an error in transmission/reception process.

The UART is configured as follows:

    - BaudRate = 9600 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

### <b>Keywords</b>

Connectivity, LPUART, baud rate, RS-232, HyperTerminal, full-duplex, DMA
Transmitter, Receiver, Asynchronous, Low Power, Wake Up

### <b>Directory contents</b>

  - UART/UART_LowPower_HyperTerminal_DMA/Inc/stm32wbaxx_nucleo_conf.h    BSP configuration file
  - UART/UART_LowPower_HyperTerminal_DMA/Inc/stm32wbaxx_hal_conf.h       HAL configuration file
  - UART/UART_LowPower_HyperTerminal_DMA/Inc/stm32wbaxx_it.h             DMA interrupt handlers header file
  - UART/UART_LowPower_HyperTerminal_DMA/Inc/main.h                      Header for main.c module
  - UART/UART_LowPower_HyperTerminal_DMA/Src/stm32wbaxx_it.c             DMA interrupt handlers
  - UART/UART_LowPower_HyperTerminal_DMA/Src/main.c                      Main program
  - UART/UART_LowPower_HyperTerminal_DMA/Src/stm32wbaxx_hal_msp.c        HAL MSP module
  - UART/UART_LowPower_HyperTerminal_DMA/Src/system_stm32wbaxx.c         STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RIVx devices.

  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.

  - Connect LPUART1 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter):
    - Connect STM32 MCU board LPUART1 TX pin PA2 (A2 pin 3 CN7)
      to PC COM port RX signal
    - Connect STM32 MCU board LPUART1 RX pin PA1 (A3 pin 4 CN7)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Hyperterminal configuration:
    - Word Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - Flow control: None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example