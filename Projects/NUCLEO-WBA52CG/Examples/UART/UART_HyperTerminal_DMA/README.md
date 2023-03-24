## <b>UART_HyperTerminal_DMA Example Description</b>

UART transmission (transmit/receive) in DMA mode
between a board and an HyperTerminal PC application.

    Board: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
    Tx Pin: PB12(available through VCP)
    Rx Pin: PA8(available through VCP)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz for STM32WBAxx Devices.

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

For this example the TxBuffer (aTxStartMessage) is predefined and the RxBuffer (aRxBuffer)
size is limited to 10 data by the mean of the RXBUFFERSIZE define in the main.c file.

In a first step the TxBuffer buffer content will be displayed in the Hyperterminal
interface and the received data will be stored in the RxBuffer buffer.
In a second step the received data in the RxBuffer buffer will be sent back to
Hyperterminal and displayed.

The end of this two steps are monitored through the HAL_UART_GetState() function
result.

NUCLEO-WBA52CG board LEDs are used to monitor the transfer status:

 - LED2 turns ON if transmission/reception is complete and OK.
 - LD3 turns ON when when there is an error in transmission/reception process.

The UART is configured as follows:

    - BaudRate = 9600 baud
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - One Stop Bit
    - Odd parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

#### <b>Notes</b>

 1. When the parity is enabled, the computed parity is inserted at the MSB
    position of the transmitted data.
   
 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, Printf, baud rate, RS-232, HyperTerminal, full-duplex, DMA,
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_HyperTerminal_DMA/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - UART/UART_HyperTerminal_DMA/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - UART/UART_HyperTerminal_DMA/Inc/stm32wbaxx_it.h          DMA interrupt handlers header file
  - UART/UART_HyperTerminal_DMA/Inc/main.h                   Header for main.c module
  - UART/UART_HyperTerminal_DMA/Src/stm32wbaxx_it.c          DMA interrupt handlers
  - UART/UART_HyperTerminal_DMA/Src/main.c                   Main program
  - UART/UART_HyperTerminal_DMA/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - UART/UART_HyperTerminal_DMA/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
     - Connect ST-Link cable to the PC USB port to display data on the HyperTerminal (and to receive data).
       A virtual COM port will then appear in the HyperTerminal.
     

  - Hyperterminal configuration:
    - Data Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - Flow control: None

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

