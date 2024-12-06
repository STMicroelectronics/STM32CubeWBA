---
pagetitle: Readme for STM32 Mbed TLS Alt
lang: en
---

<center>

# STM32 Mbed TLS Alt
Copyright &copy; 2024 STMicroelectronics\
[![ST logo](doc/st_logo_2020.png)](https://www.st.com)

</center>

This component is an alternative cryptography implementation of Mbed TLS cryptography modules.

## __Summary__
STM32 Mbed TLS Alt provides a set of alternative functions to replace Mbed TLS cryptography modules
by alternative implementation based on STM32 hardware cryptographic accelerators.

## __Description__
* **This component repository contains two types of alternative cryptography implementation:**

  * _"HAL"_ means alternative cryptography implementation based on STM32 hardware cryptographic accelerator.
  * _"ITS"_ means alternative internal trusted storage implementation to replace PSA ITS over files module.

 **_NOTE:_**
    ITS alternative implementation is encrypted using user key imported with a specific unique ID.
    ITS alternative implementation needs a storage interface depending to user application.
    `Encrypted_ITS_KeyImport` example is provided to demonstrate how to store AES-CBC key using encrypted ITS.

* **Severals preprocessor symbols are created to distinguish cryptography modules:**

  * `MBEDTLS_HAL_AES_ALT` : to use STM32 hardware AES cryptographic accelerator for CBC implementation.
  * `MBEDTLS_HAL_GCM_ALT` : to use STM32 hardware AES cryptographic accelerator for GCM implementation.
  * `MBEDTLS_HAL_SHA256_ALT` : to use STM32 hardware HASH cryptographic accelerator for SHA256 implementation.
  * `MBEDTLS_HAL_ECDSA_ALT` : to use STM32 hardware PKA cryptographic accelerator for ECDSA implementation.
  * `MBEDTLS_HAL_ECDH_ALT` : to use STM32 hardware PKA cryptographic accelerator for ECDH implementation.
  * `MBEDTLS_HAL_RSA_ALT` : to use STM32 hardware PKA cryptographic accelerator for RSA implementation.
  * `MBEDTLS_HAL_ENTROPY_HARDWARE_ALT` : to use STM32 hardware RNG cryptographic accelerator for entropy source implementation.

* **Two preprocessor symbols are created to manage ITS alternative implementation:**
  * `PSA_USE_ITS_ALT` : to enable ITS alternative module to replace PSA ITS over files module by ITS alternative implementation.
  * `PSA_USE_ENCRYPTED_ITS` : to enable ITS encryption feature for ITS alternative module using imported user key.

 **_NOTE:_**
    AES-CBC and AES-GCM support protection against side-channel attacks using DPA preprocessor symbols:
  * `HW_CRYPTO_DPA_AES` : to use STM32 hardware SAES cryptographic accelerator for DPA AES CBC implementation.
  * `HW_CRYPTO_DPA_GCM` : to use STM32 hardware SAES cryptographic accelerator for DPA AES GCM implementation.

  There is a couple of methods for alternative implementations: specific function replacement and full module replacement.

  This version of middleware supports specific function replacement and module replacement, for example
  `MBEDTLS_HAL_SHA256_ALT` may be defined to replace the full Mbed TLS SHA256 module with a hardware accelerated SAH256 module.

* **Supported alternative cryptography implementation**

<center>

| Algorithms          | Mbed TLS  | STM32 Mbed TLS Alt  |
| ------------------- |:---------:| -------------------:|
| AES (GCM, CBC)      |    YES    |        YES          |
| ChachaPoly          |    YES    |        NO           |
| ECDH                |    YES    |        YES          |
| ECDSA               |    YES    |        YES          |
| HASH (SHA2)         |    YES    |        YES          |
| CMAC                |    YES    |        NO           |
| HMAC                |    YES    |        YES          |
| RSA PKCS(v1.5,v2.2) |    YES    |        YES          |

</center>

## __How to use it?__

 This can be achieved by :

  * Adding STM32 Mbed TLS Alt middleware to user application.

  * Adding Mbed TLS middleware to user application.

  * Defining the appropriate `MBEDTLS_*_ALT` preprocessor symbol in `mbedtls_config.h` for each module that needs to be replaced.

  * Defining the appropriate preprocessor `MBEDTLS_HAL_*_ALT` in mbedtls_alt_config.h to use STM32 hardware cryptographic accelerator alternative.

  **Use case : replace mbed TLS AES module by STM32 hardware accelerated AES module**

  * Add STM32 Mbed TLS Alt middleware to user application and uncomment the macro `MBEDTLS_HAL_AES_ALT` in `mbedtls_config.h` file.

  * Add Mbed TLS library to user application and uncomment the macro `MBEDTLS_AES_ALT` in `mbedtls_config.h` to let mbed TLS use the STM32 hardware accelerated AES module.

  * Add STM32CubeU3 HAL drivers for STM32 hardware cryptographic accelerator to user application as below:
    Add RNG HAL driver : `stm32u3xx_hal_rng.c` and `stm32u3xx_hal_rng_ex.c`
    Add AES HAL driver : `stm32u3xx_hal_cryp.c` and `stm32u3xx_hal_cryp_ex.c`

## __Examples__

   This component comes with a set of applications:

*  Cipher_AES_CBC_EncryptDecrypt
*  Cipher_AES_GCM_AuthEncrypt_VerifDecrypt
*  Cipher_ChachaPoly_AuthEnc_VerifDec
*  DRBG_RandomGeneration
*  ECC_ECDH_SharedSecretGeneration
*  ECC_ECDSA_SignVerify
*  Hash_SHA2_Digest
*  MAC_AES_CMAC_AuthenticateVerify
*  MAC_HMAC_SHA2_AuthenticateVerify
*  RSA_PKCS1v1.5_SignVerify
*  RSA_PKCS1v1.5_SignVerifyCRT
*  RSA_PKCS1v2.2_SignVerify
*  RSA_PKCS1v1.5_SignVerifyCRT
*  RSA_PKCS1v2.2_EncryptDecrypt
*  RSA_PKCS1v2.2_EncryptDecryptCRT
*  Encrypted_ITS_KeyImport

## __Keywords__

   AES, GCM, CBC, Chacha20-Poly1305, RNG, DRBG, ECC, ECDSA, ECDH, HASH, SHA2, CMAC, HMAC, RSA, PKCS#1 v1.5, PKCS#1 v2.2,
   ENCRYPT, DECRYPT, SIGN, VERIFY, Mbed TLS, PSA Crypto API, STM32 Mbed TLS Alt, Encrypted ITS.

## __Release note__

   Release v0.3.2

## __Compatibility information__

   This component depends on :

*  Mbed TLS cryptography library v3.6.1
