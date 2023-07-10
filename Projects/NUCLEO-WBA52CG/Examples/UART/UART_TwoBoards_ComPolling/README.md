## <b>UART_TwoBoards_ComPolling Example Description</b>

UART transmission (transmit/receive) in Polling mode 
between two boards.

 - Board: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
 - Tx Pin:  CN6 pin 3
 - Rx Pin:  CN4 pin 24

Two identical boards are connected :

 - Board 1: transmitting then receiving board
 - Board 2: receiving then transmitting board

The user presses the USER push-button on board 1.
Then, board 1 sends in polling mode a message to board 2 that sends it back to 
board 1 in polling mode as well.
Finally, board 1 and 2 compare the received message to that sent.
If the messages are the same, the test passes.

WARNING: as both boards do not behave the same way, "TRANSMITTER_BOARD" compilation
switch is defined in /Src/main.c and must be enabled
at compilation time before loading the executable in the board that first transmits
then receives.
The receiving then transmitting board needs to be loaded with an executable
software obtained with TRANSMITTER_BOARD disabled. 

NUCLEO-WBA52CG board LEDs are used to monitor the transfer status:

- While board 1 is waiting for the user to press the USER push-button, its LD1 is
  blinking rapidly (100 ms period).
- When the test passes, LD1 is turned on.
- If there is an initialization or transfer error, LD3 is turned on.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The UART is configured as follows:

    - BaudRate = 9600 baud  
    - Word Length = 8 bits (8 data bits, no parity bit)
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Reception and transmission are enabled in the time

**Note:**
When the parity is enabled, the computed parity is inserted at the MSB
position of the transmitted data.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, baud rate, RS-232, full-duplex, Polling, parity, stop bit,
Transmitter, Receiver, Asynchronous

### <b>Directory contents</b>

  - UART/UART_TwoBoards_ComPolling/Inc/stm32wbaxx_nucleo_conf.h         BSP configuration file
  - UART/UART_TwoBoards_ComPolling/Inc/stm32wbaxx_hal_conf.h            HAL configuration file
  - UART/UART_TwoBoards_ComPolling/Inc/stm32wbaxx_it.h                  interrupt handlers header file
  - UART/UART_TwoBoards_ComPolling/Inc/main.h                           Header for main.c module  
  - UART/UART_TwoBoards_ComPolling/Src/stm32wbaxx_it.c                  interrupt handlers
  - UART/UART_TwoBoards_ComPolling/Src/main.c                           Main program
  - UART/UART_TwoBoards_ComPolling/Src/stm32wbaxx_hal_msp.c             HAL MSP module
  - UART/UART_TwoBoards_ComPolling/Src/system_stm32wbaxx.c              STM32WBAxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA52CGUx devices.    
  - This example has been tested with two NUCLEO-WBA52CG boards embedding
    a STM32WBA52CGUx device and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-WBA52CG set-up
    - Connect a wire between 1st board PA12 pin (Uart Tx) and 2nd board PA11 pin (Uart Rx)
    - Connect a wire between 1st board PA11 pin (Uart Rx) and 2nd board PA12 pin (Uart Tx)
    - Connect 1st board GND to 2nd Board GND    

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
