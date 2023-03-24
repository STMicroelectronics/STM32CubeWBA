## <b>I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init Example Description</b>

How to transmit data bytes from an I2C master device using DMA mode
to an I2C slave device using DMA mode. The peripheral is initialized
with LL unitary service functions to optimize for performance and size.

This example guides you through the different configuration steps by mean of LL API
to configure GPIO, DMA and I2C peripherals using two NUCLEO-WBA52CG.

       Boards: NUCLEO-WBA52CG (embeds a STM32WBA52CGUx device)
       - SCL Pin: PB.2 (CN4, pin 3)
       - SDA Pin: PB.1 (CN4, pin 5)

The project is split in two parts the Master Board and the Slave Board.

- Master Board :
  I2C1 Peripheral is configured in Master mode with DMA (Clock 400Khz).
  And GPIO associated to USER push-button is linked with EXTI.

- Slave Board :
  I2C1 Peripheral is configured in Slave mode with DMA (Clock 400Khz, Own address 7-bit enabled).

The user can choose between Master and Slave through "#define SLAVE_BOARD"
in the "main.h" file:

- Comment "#define SLAVE_BOARD" to select Master board.
- Uncomment "#define SLAVE_BOARD" to select Slave board.

The user can disable internal pull-up by opening ioc file.

For that, user can follow the procedure :

 1. Double click on the I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init.ioc file
 2. When CUBEMX tool is opened, select System Core category
 3. Then in the configuration of GPIO/I2C1, change Pull-up to No pull-up and no pull-down for the both pins
 4. Last step, generate new code thanks to button "GENERATE CODE"
 
The example is updated with no pull on each pin used for I2C communication

A first program launch, BOARD SLAVE waiting Address Match code through Handle_I2C_Slave() routine.
LD1 blinks quickly on BOARD MASTER to wait for user-button press.

Example execution:

Press the USER push-button on BOARD MASTER to initiate a write request by Master through Handle_I2C_Master() routine.
This action will generate an I2C start condition with the Slave address and a write bit condition.
When address Slave match code is received on I2C1 of BOARD SLAVE, an ADDR event occurs.
Handle_I2C_Slave() routine is then checking Address Match Code and direction Write.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the bytes through DMA.
When acknowledge is received on I2C1 (Master), DMA transfer the data from flash memory buffer to I2C1 TXDR register (Master).
This will allow Master to transmit a byte to the Slave.
Each time a byte is received on I2C1 (Slave), DMA transfer the data from I2C1 RXDR register to RAM memory buffer (Slave).
And so each time the Slave acknowledge the byte received,
DMA transfer the next data from flash memory buffer to I2C1 TXDR register (Master) until Transfer completed.
Master auto-generate a Stop condition when DMA transfer is achieved.

The STOP condition generate a STOP event and initiate the end of reception on Slave side.
Handle_I2C_Slave() and Handle_I2C_Master() routine are then clearing the STOP flag in both side.

LD1 is On :

- Slave side if data are well received.
- Master side if transfer sequence is completed.

In case of errors, LD1 is blinking slowly (1s).

### <b>Keywords</b>

Connectivity, Communication, I2C, DMA, Master Rx, Slave Tx, Transmission, Reception, Fast mode, SDA, SCL


### <b>Directory contents</b>

  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Inc/main.h                  Header for main.c module
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Src/stm32wbaxx_it.c         Interrupt handlers
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Src/main.c                  Main program
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_DMA_Init/Src/system_stm32wbaxx.c     STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
    - Connect GPIOs connected to Board Slave I2C1 SCL/SDA (PB.2 and PB.1)
    to respectively Board Master SCL and SDA pins of I2C1 (PB.2 and PB.1).
      - I2C1_SCL  PB.2 (CN4, pin 3) : connected to I2C1_SCL PB.2 (CN4, pin 3)
      - I2C1_SDA  PB.1 (CN4, pin 5) : connected to I2C1_SDA PB.1 (CN4, pin 5)
    - Connect Master board GND to Slave Board GND

  - Launch the program. Press USER push-button to initiate a write request by Master
      then Slave receive bytes.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory (The user can choose between Master
   and Slave target through "#define SLAVE_BOARD" in the "main.h" file)
    - Comment "#define SLAVE_BOARD" and load the project in Master Board
    - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
 - Run the example

