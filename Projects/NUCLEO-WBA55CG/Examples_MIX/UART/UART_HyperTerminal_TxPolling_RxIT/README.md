## <b>UART_HyperTerminal_TxPolling_RxIT Example Description</b>

Use of a UART to transmit data (transmit/receive)
between a board and an HyperTerminal PC application both in Polling and Interrupt
modes. This example describes how to use the USART peripheral through
the STM32WBAxx UART HAL and LL API, the LL API being used for performance improvement.

       Board: NUCLEO-WBA55CG
       - Tx Pin: PB.12 (available through VCP)
       - Rx Pin: PA.08 (available through VCP)

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz for STM32WBAxx Devices.

The UART peripheral configuration is ensured by the HAL_UART_Init() function.
This later is calling the HAL_UART_MspInit() function which core is implementing
the configuration of the needed UART resources according to the used hardware.
You may update this function to change UART configuration.

Reception under IT on UART/Hyperterminal interface is then enabled using LL APIs.
Each 0.5 sec, a predefined TX buffer is sent from MCU to PC reception of Data from Hyperterminal.

The Asynchronous communication aspect of the UART is clearly highlighted as the
data buffers transmission/reception to/from Hyperterminal are done simultaneously.

Transmit function which allow transmission of a predefined data buffer
in polling mode, is implemented using HAL UART API.
Receive function which allow the reception of Data from Hyperterminal in
IT mode, is implemented using LL USART API.

A double buffer is available for received data, allowing continuous reception.
Data received in Buffer A could be echoed on TX, while buffer B is used for reception.

When buffer B is full, it is then echoed, while buffer A becomes active buffer for next reception, and so on.

For this example the TX buffer (aTxStartMessage) is predefined and the RX buffers (aRXBufferA and aRXBufferB)
size is limited to 10 data by the mean of the RXBUFFERSIZE define in the main.c file.

Received data will be stored in the RX buffer.
When RX buffer is full, RX buffer is sent back to PC and led toggles.

The Asynchronous communication aspect of the UART is clearly highlighted as the
data buffers transmission/reception to/from Hyperterminal are done simultaneously.

STM32 Nucleo board's LEDs can be used to monitor the transfer status:

 - LD1 toggles each time transmission/reception of 10 characters is completed..
 - LD3 stays ON when there is an error in transmission/reception process.

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

Connectivity, UART/USART, baud rate, RS-232, HyperTerminal, full-duplex, Interrupt, Polling,
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_HyperTerminal_TxPolling_RxIT/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - UART/UART_HyperTerminal_TxPolling_RxIT/Inc/stm32wbaxx_it.h          IT interrupt handlers header file
  - UART/UART_HyperTerminal_TxPolling_RxIT/Inc/main.h                   Main program header file
  - UART/UART_HyperTerminal_TxPolling_RxIT/Src/stm32wbaxx_it.c          IT interrupt handlers
  - UART/UART_HyperTerminal_TxPolling_RxIT/Src/main.c                   Main program
  - UART/UART_HyperTerminal_TxPolling_RxIT/Src/stm32wbaxx_hal_msp.c     HAL MSP file
  - UART/UART_HyperTerminal_TxPolling_RxIT/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on NUCLEO-WBA55CG devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up
    Example is delivered for using Virtual Com port feature of STLINK for connection between NUCLEO-WBA55CG and PC,
    Please ensure that USART communication between the target MCU and ST-LINK MCU is properly enabled
    on HW board in order to support Virtual Com Port (Default HW SB configuration allows use of VCP)
    GPIOs connected to USART1 TX/RX (PB12 and PA8) are automatically mapped
    on RX and TX pins of PC UART Com port selected on PC side (please ensure VCP com port is selected).

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
    - Word Length = 7 Bits
    - One Stop Bit
    - Odd parity
    - BaudRate = 9600 baud
    - flow control: None


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

