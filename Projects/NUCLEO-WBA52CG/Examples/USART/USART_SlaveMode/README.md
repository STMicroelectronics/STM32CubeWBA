## <b>USART_SlaveMode Example Description</b>

This example describes an USART-SPI communication (transmit/receive) between two
boards where the USART is configured as a slave.

       Boards: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
       - Board 1: SPI Master board
       - Board 2: USART Slave board

In a first step, after the user presses the User push-button, Master (SPI) starts
the communication by sending aTxBuffer and receiving aRxBuffer through
HAL_SPI_TransmitReceive(), at the same time Slave (USART) transmits aTxBuffer
and receives aRxBuffer through HAL_USART_TransmitReceive().
Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to
check buffers correctness.

WARNING: as both boards do not behave the same way, "SLAVE_BOARD" compilation
switch is defined in Src/main.c and must be enabled at compilation time before
loading the executable in the USART slave board.
The SPI master board needs to be loaded with an executable software obtained
with SLAVE_BOARD disabled.

NUCLEO-WBA52CG board LEDs are used to monitor the transfer status:

- While board 1 is waiting for the user to press the User push-button, its
  LD2 is blinking rapidly (100 ms period).
- After the transmission/reception processes begin LD2 is turned off.
- When the test passes, LD2 on both boards is turned on, otherwise the
  test has failed.
- If there is an initialization or transfer error, LD3 blinks slowly (500 ms).

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WBA52CGUx Devices.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to
    1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Connectivity, UART/USART, SPI interface, full-duplex, Polling, Receiver, Transmitter, Synchronous, Slave,

### <b>Directory contents</b> 

  - USART/USART_SlaveMode/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - USART/USART_SlaveMode/Src/main.c                   Main program
  - USART/USART_SlaveMode/Src/system_stm32wbaxx.c      STM32WBAxx system clock configuration file
  - USART/USART_SlaveMode/Src/stm32wbaxx_it.c          Interrupt handlers
  - USART/USART_SlaveMode/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - USART/USART_SlaveMode/Inc/main.h                   Main program header file
  - USART/USART_SlaveMode/Inc/stm32wbaxx_hal_conf.h    HAL Configuration file
  - USART/USART_SlaveMode/Inc/stm32wbaxx_it.h          Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with two NUCLEO-WBA52CG boards embedding
    a STM32WBA52CGUx device and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-WBA52CG set-up
    - Connect Master board PB4 (CN4.6) (SCK) to Slave Board PA5 (CN3.36) (USART CK)
    - Connect Master board PB3 (CN4.10) (MISO) to Slave Board PB12 (CN3.37) (USART TX)
    - Connect Master board PA15 (CN4.15) (MOSI) to Slave Board PA8 (CN3.35) (USART RX)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
   - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
   - Comment "#define SLAVE_BOARD" and load the project in Master Board
 - Run the example

