## <b>PKA_ModularExponentiationCRT Example Description</b>

How to compute the Chinese Remainder Theorem (CRT) optimization.

For this example, two .pem file have been created using openssl external tool. (https://www.openssl.org).
Src/rsa_pub_2048.pem and Src/rsa_priv_2048.pem, contains respectively the public and private key used in
this example.

To ease the usage of this .pem files, 2 set of files, Src/rsa_pub_2048.c and Inc/rsa_pub_2048.h and
Src/rsa_priv_2048.c and Inc/rsa_priv_2048.h are present to reflect their content. The explanation of 
how to create those file from a .pem are embedded inside both .c files.

The selected algorithm is RSA.

A ciphered buffer is deciphered using the CRT modular exponentiation and the private key.
The result is stored in the decipheredBuffer. After this, the buffer is identical to the plain text.

In case of success, the LD1 (BLUE) is ON.

In case of any error, the LD1 (BLUE) is toggling slowly.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
   based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
   a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
   than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
   To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
   to have correct HAL operation.

### <b>Keywords</b>

Security, PKA, RSA, modular exponentiation, CRT ,decipher

### <b>Directory contents</b>
  
  - PKA/PKA_ModularExponentiationCRT/Inc/stm32wbaxx_hal_conf.h   HAL configuration file
  - PKA/PKA_ModularExponentiationCRT/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - PKA/PKA_ModularExponentiationCRT/Inc/main.h                  Header for main.c module
  - PKA/PKA_ModularExponentiationCRT/Src/stm32wbaxx_it.c         Interrupt handlers
  - PKA/PKA_ModularExponentiationCRT/Src/main.c                  Main program
  - PKA/PKA_ModularExponentiationCRT/Src/stm32wbaxx_hal_msp.c    HAL MSP module 
  - PKA/PKA_ModularExponentiationCRT/Src/system_stm32wbaxx.c     STM32WBAxx system source file
  - PKA/PKA_ModularExponentiationCRT/Src/plaintext.bin           Random 2048 bits
  - PKA/PKA_ModularExponentiationCRT/Src/plaintext.c             Reflect the content of plaintext.bin in array format
  - PKA/PKA_ModularExponentiationCRT/Inc/plaintext.h             Header for plaintext.c
  - PKA/PKA_ModularExponentiationCRT/Src/ciphertext.bin          Ciphered plaintext.bin using openssl
  - PKA/PKA_ModularExponentiationCRT/Src/ciphertext.c            Reflect the content of ciphertext.bin in array format
  - PKA/PKA_ModularExponentiationCRT/Inc/ciphertext.h            Header for ciphertext.c
  - PKA/PKA_ModularExponentiationCRT/Src/rsa_priv_2048.pem       RSA private key generated using openssl
  - PKA/PKA_ModularExponentiationCRT/Src/rsa_priv_2048.c         Reflect the content of rsa_priv_2048.pem in array format
  - PKA/PKA_ModularExponentiationCRT/Inc/rsa_priv_2048.h         Header for rsa_priv_2048.c
  - PKA/PKA_ModularExponentiationCRT/Src/rsa_pub_2048.pem        RSA public key extracted from rsa_priv_2048.pem using openssl
  - PKA/PKA_ModularExponentiationCRT/Src/rsa_pub_2048.c          Reflect the content of rsa_pub_2048.pem in array format
  - PKA/PKA_ModularExponentiationCRT/Inc/rsa_pub_2048.h          Header for rsa_pub_2048.c

### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA55CGUx devices.
  
  - This example has been tested with an STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device 
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 
