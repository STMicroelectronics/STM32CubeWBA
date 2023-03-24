## <b>CRYP_SAES_SharedKey Example Description</b>

How to use the Secure AES co-processor (SAES) peripheral to share application keys
with AES peripheral.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32WBA52CGUx Devices :
The CPU at 100 MHz.

The Secure AES instance is configured in shared mode and hardware secret key DHUK is selected.
The goal here to share application key with AES peripheral without being exposed in clear-text.
First, application key is encrypted (the result is stored in Encryptedkey buffer).
Then, user decrypt and share Encryptedkey, now application key is loaded in AES_KEYRx registers
of AES peripheral(without knowing him), immediately usable by users for any AES operation.
Finally, ECB encryption/decryption is performed using key already in AES_KEYRx registers
and results are compared with expected ones.

The Secure AES mode requires:

1. Application keys (AESKey256) to be encrypted and shared using DHUK(derived hardware unique key).
2. Plain text (Plaintext) which will be encrypted with AES peripheral.
3. Cipher text (CiphertextAESECB256)  which is the encryption result of the Plaint text.

In this example, LD1 (led BLUE) is turned on when the operation finish with success. 
and LD3 (led RED) is turned on when the operation generate an error.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

System, Security, SAES, AES, SharedKey

### <b>Directory contents</b>

  - CRYP/CRYP_SAES_SharedKey/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - CRYP/CRYP_SAES_SharedKey/Src/main.c                    Main program
  - CRYP/CRYP_SAES_SharedKey/Src/system_stm32wbaxx.c       STM32WBAxx system clock configuration file
  - CRYP/CRYP_SAES_SharedKey/Src/stm32wbaxx_it.c           Interrupt handlers
  - CRYP/CRYP_SAES_SharedKey/Src/stm32wbaxx_hal_msp.c      HAL MSP module
  - CRYP/CRYP_SAES_SharedKey/Inc/main.h                    Main program header file
  - CRYP/CRYP_SAES_SharedKey/Inc/stm32wbaxx_hal_conf.h     HAL Configuration file
  - CRYP/CRYP_SAES_SharedKey/Inc/stm32wbaxx_it.h           Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

