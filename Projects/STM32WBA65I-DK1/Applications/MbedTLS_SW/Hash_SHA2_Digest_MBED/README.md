## <b>Hash_SHA2_Digest_MBED Application Description</b>

This application describes how to use the PSA Ref API to digest a message
using the SHA256 algorithm.

This application is configured to use the Hardware library.

This application demonstrates two way of using the API:

  - The single call method: the configuration of the algorithm, and the operation is done
in one single API call
  - The multiple calls method: the configuration of the algorithm, and the operation is done
in several API calls, allowing in particular a piecemeal injection of data to process.

Each way is composed of:

  - A hashing of a known message, followed by the verification of the generated digest

####  <b>Expected success behavior</b>

- The green led will be turned on
- The global variable glob_status will be set to PASSED

#### <b>Error behaviors</b>

- The green led will be toggled each 250 milliseconds in an infinity loop.
- The global variable glob_status will be set to FAILED

### <b>Notes</b>
NIST vectors applications for SHA2 are taken from:
"SHA Test Vectors for Hashing Byte-Oriented Messages"
Available at:
 https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/secure-hashing

### <b>Keywords</b>

Cryptography, digest, SHA256, Cryptographic

### <b>Directory contents</b>

  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/stm32wba65i_discovery_conf.h     BSP configuration file
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/main.h                        Header for main.c module
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/mbedtls_config.h              Mbed TLS configuration file
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Inc/mbedtls_alt_config.h          Mbed TLS Alt configuration file
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Src/stm32wbaxx_it.c          Interrupt handlers
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Src/main.c                        Main program
  - MbedTLS_SW/Hash_SHA2_Digest_MBED/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBAxx devices.

  - This application has been tested with STM32WBA65I-DK1 board and can be
    easily tailored to any other supported device and development board.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

