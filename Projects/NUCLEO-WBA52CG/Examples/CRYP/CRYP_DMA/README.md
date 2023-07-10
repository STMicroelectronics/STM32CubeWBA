## <b>CRYP_DMA Example Description</b>

How to use the AES peripheral to encrypt and decrypt data using AES 128
Algorithm with ECB chaining mode in DMA mode.

DMA is used to transfer data from memory to the AES processor
input as well as to transfer data from AES processor output to memory.

64-byte buffers are ciphered and deciphered (4 AES blocks)
Ciphering/Deciphering with a 128-bit long key is used with data type set to 8-bit (byte swapping).

This example unrolls as follows:

 - AES Encryption (Plain Data --> Encrypted Data)
 - AES Decryption with key derivation (Encrypted Data --> Decrypted Data)

When resorting to IAR Embedded workbench, plain data, encrypted data
and decrypted data are displayed on debugger terminal IO.

When resorting to MDK-ARM KEIL IDE, plain data, encrypted data and decrypted
data are displayed on debugger as follows:  View --> Serial Viewer --> Debug (printf) Viewer.

When resorting to STM32CubeIDE:
Command Code is displayed on debugger as follows: Window--> Show View--> Console.

 - In Debug configuration :
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup, add the command "monitor arm semihosting enable"

When all ciphering and deciphering operations are successful, LD1 is turned on.
In case of ciphering or deciphering issue, LD3 toggle each 200ms.

The Plain data, encrypted data and decrypted data can be displayed on a PC HyperTerminal using the
UART by enabling USE_VCP_CONNECTION compilation switch with adding (#define USE_VCP_CONNECTION  1) in
stm32wbaxx_nucleo_conf.h.

### <b>Keywords</b>

Security, Cryptography, CRYPT, AES, ECB, DMA, cipher, UART


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Directory contents</b>

  - CRYP/CRYP_DMA/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - CRYP/CRYP_DMA/Inc/stm32wbaxx_hal_conf.h     HAL configuration file
  - CRYP/CRYP_DMA/Inc/stm32wbaxx_it.h           Interrupt handlers header file
  - CRYP/CRYP_DMA/Inc/main.h                    Header for main.c module
  - CRYP/CRYP_DMA/Src/stm32wbaxx_it.c           Interrupt handlers
  - CRYP/CRYP_DMA/Src/main.c                    Main program
  - CRYP/CRYP_DMA/Src/stm32wbaxx_hal_msp.c      HAL MSP module
  - CRYP/CRYP_DMA/Src/system_stm32wbaxx.c       STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board with socket using STM32WBA52CGUx plugged-in
    and can be easily tailored to any other supported device and development board.

  - To be able to display data on MDK-ARM debugger please make sure to close the SB12.

  - Hyperterminal configuration:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
