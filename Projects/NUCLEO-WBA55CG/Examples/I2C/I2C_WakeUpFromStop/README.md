## <b>I2C_WakeUpFromStop Example Description</b>

How to handle I2C data buffer transmission/reception between two boards, 
using an interrupt when the device is in Stop mode.

       Board: NUCLEO-WBA55CG (embeds a STM32WBA55CGUx device)
       - SCL Pin: PB2 (CN4, pin 3)
       - SDA Pin: PB1 (CN4, pin 5)

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 100 MHz. When The system is wakeup from Stop mode,
system clock is 16 Mhz. The I2C peripheral is directly clocked by HSI.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.
The project is split in two parts the Master Board and the Slave Board

- Master Board: 
  The HAL_I2C_Master_Receive_IT() and the HAL_I2C_Master_Transmit_IT() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Master mode using interrupt.

- Slave Board: 
  The HAL_I2C_Slave_Receive_IT() and the HAL_I2C_Slave_Transmit_IT() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Slave mode using interrupt.

The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:

- If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
- If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user presses the USER push-button on the Master Board, I2C Master
starts the communication by sending aTxBuffer through HAL_I2C_Master_Transmit_IT() to 
I2C Slave which wakes up from Stop mode and receives aRxBuffer through HAL_I2C_Slave_Receive_IT(). 

The second step starts when the user presses the USER push-button on the Master Board,
the I2C Slave after wake up from Stop mode at address match, sends aTxBuffer through HAL_I2C_Slave_Transmit_IT()
to the I2C Master which receives aRxBuffer through HAL_I2C_Master_Receive_IT().

The end of this two steps are monitored through the HAL_I2C_GetState() function
result.

Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

NUCLEO-WBA55CG's LEDs can be used to monitor the transfer status on the Slave Board :

 - LD1 is ON when the Transmission process is complete.
 - LD1 is OFF when the Reception process is complete.
 - LD3 is ON when there is an error in transmission/reception process.
 - LD2 is ON when Slave enters Stop mode.
 - LD2 is OFF when Slave wakes up from Stop.    

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Connectivity, I2C, Communication, Transmission, Reception, SCL, SDA, Wake up, Interrupt

### <b>Directory contents</b> 

  - I2C/I2C_WakeUpFromStop/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - I2C/I2C_WakeUpFromStop/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - I2C/I2C_WakeUpFromStop/Inc/stm32wbaxx_it.h          I2C interrupt handlers header file
  - I2C/I2C_WakeUpFromStop/Inc/main.h                  Header for main.c module  
  - I2C/I2C_WakeUpFromStop/Src/stm32wbaxx_it.c          I2C interrupt handlers
  - I2C/I2C_WakeUpFromStop/Src/main.c                  Main program
  - I2C/I2C_WakeUpFromStop/Src/system_stm32wbaxx.c      STM32WBAxx system source file
  - I2C/I2C_WakeUpFromStop/Src/stm32wbaxx_hal_msp.c     HAL MSP file    

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA5xx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA55CG Set-up

    - Connect I2C_SCL line of Master board (PB2, CN4, pin 3) to I2C_SCL line of Slave Board (PB2, CN4, pin 3).
    - Connect I2C_SDA line of Master board (PB1, CN4, pin 5) to I2C_SDA line of Slave Board (PB1, CN4, pin 5).
    - Connect GND of Master board to GND of Slave Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
    o Uncomment "#define MASTER_BOARD" and load the project in Master Board
    o Comment "#define MASTER_BOARD" and load the project in Slave Board
 - Run the example

