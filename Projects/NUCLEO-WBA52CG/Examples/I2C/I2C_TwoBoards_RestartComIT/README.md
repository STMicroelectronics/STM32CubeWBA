## <b>I2C_TwoBoards_RestartComIT Example Description</b>

How to handle single I2C data buffer transmission/reception between two boards, 
in interrupt mode and with restart condition.

       Board: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
       - SCL Pin: PB2 (CN4, pin3)
       - SDA Pin: PB1 (CN4, pin5)

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.
The project is split in two parts the Master Board and the Slave Board.

 - Master Board: 
   The HAL_I2C_Master_Sequential_Transmit_IT() and the HAL_I2C_Master_Sequential_Receive_IT() functions 
   allow respectively the transmission and the reception of a predefined data buffer
   in Master mode.

 - Slave Board: 
   The HAL_I2C_EnableListen_IT(), HAL_I2C_Slave_Sequential_Receive_IT() and the HAL_I2C_Slave_Sequential_Transmit_IT() functions 
   allow respectively the "Listen" the I2C bus for address match code event, reception and the transmission of a predefined data buffer
   in Slave mode.

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:

- If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
- If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

On Master board side:

 - Wait USER push-button to be pressed.
 - Request write communication after a Start condition by sending aTxBuffer through HAL_I2C_Master_Sequential_Transmit_IT() to I2C Slave 
 - LD1 is turned ON when the transmission process is completed.
 - Then wait USER push-button to be pressed.
 - Request read communication after a Repeated Start condition through HAL_I2C_Master_Sequential_Receive_IT() to I2C Slave.
 - Receive data from Slave in aRxBuffer.
 - LD1 is turned OFF when the reception process is completed.

Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.
LD3 is slowly blinking (1 sec. period) when there is an error in transmission/reception process,
or buffers compared error. (communication is stopped if any, using infinite loop)

On Slave board side:

 - Go Listen mode by calling HAL_I2C_EnableListen_IT().
 - Acknowledge Address match code.
 - Slave receives data into aRxBuffer through HAL_I2C_Slave_Sequential_Receive_IT().
 - LD1 is turned ON when the reception process is completed.
 - Wait for read communication request from I2C Master (by pressing the USER push-button on Master board.)
 - Then slave transmits data by sending aTxBuffer through HAL_I2C_Slave_Sequential_Transmit_IT().
 - LD1 is turned OFF when the transmission process is completed.

Finally, aRxBuffer and aTxBuffer are compared through Buffercmp() in order to 
check buffers correctness.
LD3 is slowly blinking (1 sec. period) when there is an error in transmission/reception process,
or buffers compared error. (communication is stopped if any, using infinite loop)

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 1. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, I2C, Communication, Transmission, Reception, SCL, SDA, Interrupt

### <b>Directory contents</b> 

  - I2C/I2C_TwoBoards_RestartComIT/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - I2C/I2C_TwoBoards_RestartComIT/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - I2C/I2C_TwoBoards_RestartComIT/Inc/stm32wbaxx_it.h          I2C interrupt handlers header file
  - I2C/I2C_TwoBoards_RestartComIT/Inc/main.h                   Header for main.c module  
  - I2C/I2C_TwoBoards_RestartComIT/Src/stm32wbaxx_it.c          I2C interrupt handlers
  - I2C/I2C_TwoBoards_RestartComIT/Src/main.c                   Main program
  - I2C/I2C_TwoBoards_RestartComIT/Src/system_stm32wbaxx.c      STM32WBAxx system source file
  - I2C/I2C_TwoBoards_RestartComIT/Src/stm32wbaxx_hal_msp.c     HAL MSP file    

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA5xx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.    

  - NUCLEO-WBA52CG Set-up

    - Connect I2C_SCL line of Master board (PB2, CN4, pin3) to I2C_SCL line of Slave Board (PB2, CN4, pin3).
    - Connect I2C_SDA line of Master board (PB1, CN4, pin5) to I2C_SDA line of Slave Board (PB1, CN4, pin5).
    - Connect GND of Master board to GND of Slave Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
    - Uncomment "#define MASTER_BOARD" and load the project in Master Board
    - Comment "#define MASTER_BOARD" and load the project in Slave Board
 - Run the example

