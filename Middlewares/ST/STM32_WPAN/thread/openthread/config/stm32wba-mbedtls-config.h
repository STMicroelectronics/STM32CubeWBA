/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef STM32WB_MBEDTLS_CONFIG_H
#define STM32WB_MBEDTLS_CONFIG_H

#include <inttypes.h>
#include <stdlib.h>

#include <openthread/platform/logging.h>
#include "stm32wbaxx.h"

/** @defgroup MBEDTLS_HAL_XXX_ALT
  * @brief    Alternative cryptography implementation based on STM32 hardware
  *           cryptographic accelerator
  * @{
  */

/**
  * @brief MBEDTLS_HAL_AES_ALT Enables ST AES alternative module to replace mbed
  *        TLS AES module by ST AES alternative implementation based on STM32
  *        AES hardware accelerator.
  *
  *        Uncomment a macro to enable ST AES hardware alternative module.
  *        Requires: MBEDTLS_AES_C, MBEDTLS_AES_ALT.
  */
#define MBEDTLS_AES_ALT
#define MBEDTLS_HAL_AES_ALT

/**
  * @brief HW_CRYPTO_DPA_AES Allows DPA resistance for AES modes by using secure
  *        crypto processor (SAES), when this option is enabled,
  *        AES-ECB/CBC modes become DPA-protected.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_AES_ALT
  */
//#define HW_CRYPTO_DPA_AES

/**
  * @brief MBEDTLS_HAL_GCM_ALT Enables ST GCM alternative module to replace mbed
  *        TLS GCM module by ST GCM alternative implementation based on STM32
  *        AES hardware accelerator.
  *
  *        Uncomment a macro to enable ST GCM hardware alternative module.
  *        Requires: MBEDTLS_AES_C, MBEDTLS_GCM_C, MBEDTLS_GCM_ALT.
  */
#define MBEDTLS_GCM_ALT
#define MBEDTLS_HAL_GCM_ALT

/**
  * @brief HW_CRYPTO_DPA_GCM Allows DPA resistance for GCM by using secure crypto
  *        processor (SAES) when this option is enabled, GCM becomes DPA-protected.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_GCM_ALT.
 */
//#define HW_CRYPTO_DPA_GCM

/**
  * @brief HW_CRYPTO_DPA_CTR_FOR_GCM Allows DPA resistance for GCM through CTR by 
  *        using secure crypto processor (SAES) when this option is enabled,
  *        CTR becomes DPA-protected.
  *        CTR protected mode is mixed with software to create GCM protected mode.
  *        This option can be enabled when the hardware don't support protected GCM.
  *
  * @note Using DPA resistance degrades the performance.
  *
  *       Comment this option if your system can run cryptographic services
  *       without DPA resistance for the highest performance benefit.
  *       Requires: MBEDTLS_HAL_GCM_ALT, HW_CRYPTO_DPA_GCM.
 */
//#define HW_CRYPTO_DPA_CTR_FOR_GCM

#if defined(HW_CRYPTO_DPA_GCM) && defined(HW_CRYPTO_DPA_CTR_FOR_GCM)
#error "HW_CRYPTO_DPA_GCM and HW_CRYPTO_DPA_CTR_FOR_GCM cannot be defined simultaneously"
#endif /* HW_CRYPTO_DPA_GCM && HW_CRYPTO_DPA_CTR_FOR_GCM */

#ifndef OT_LIGHT /* Issue with light configuration of mbedtls and ST hardware accelerator */
/**
  * @brief MBEDTLS_HAL_SHA256_ALT Enables ST SHA-224 and SHA-256 alternative
  *        modules to replace mbed TLS SHA-224 and SHA-256 modules by ST SHA-224
  *        and SHA-256 alternative implementation based on STM32 HASH hardware
  *        accelerator.
  *
  *        Uncomment a macro to enable ST SHA256 hardware alternative module.
  *        Requires: MBEDTLS_SHA256_C, MBEDTLS_SHA256_ALT.
  */
#define MBEDTLS_SHA256_ALT
#define MBEDTLS_HAL_SHA256_ALT

/**
  * @brief ST_HW_CONTEXT_SAVING Enables ST HASH save context
  *        The HASH context of the interrupted task can be saved from the HASH 
  *        registers to memory, and then be restored from memory to the HASH
  *        registers.
  *
	*        Shall be activated if mbedtls_sha256_clone() is used => Y for OT comissionning
	*
  *        Uncomment a macro to enable ST HASH save context.
  *        Requires: MBEDTLS_SHA256_ALT.
  */
#define ST_HW_CONTEXT_SAVING
#endif /* OT_LIGHT */
#if defined(ST_HW_CONTEXT_SAVING) && (USE_HAL_HASH_SUSPEND_RESUME != 1U)
#error "Enable USE_HAL_HASH_SUSPEND_RESUME flag to save HASH context"
#endif /* ST_HW_CONTEXT_SAVING && USE_HAL_HASH_SUSPEND_RESUME */

/**
  * @brief MBEDTLS_HAL_ECDSA_ALT Enables ST ECDSA alternative module to replace
  *        mbed TLS ECDSA sign and  verify modules by ST ECDSA alternative
  *        implementation based on STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECDSA hardware alternative module.
  *        Requires: MBEDTLS_ECDSA_C, MBEDTLS_ECDSA_SIGN_ALT,
  *                  MBEDTLS_ECDSA_VERIFY_ALT, MBEDTLS_ECP_ALT.
  */
#define MBEDTLS_ECDSA_ALT
#define MBEDTLS_HAL_ECDSA_ALT

/**
  * @brief MBEDTLS_HAL_ECDH_ALT Enables ST ECDH alternative module to replace
  *        mbed TLS Compute shared secret module by ST Compute shared secret
  *        alternative implementation based on STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECDH hardware alternative module.
  *        Requires: MBEDTLS_ECDH_C, MBEDTLS_ECDH_COMPUTE_SHARED_ALT,
  *                  MBEDTLS_ECP_ALT!!!!!.
  */
#define MBEDTLS_ECDH_ALT
#define MBEDTLS_HAL_ECDH_ALT

/**
  * @brief MBEDTLS_HAL_ECP_ALT Enables ST ECP alternative modules to replace
  *        mbed TLS ECP module by ST ECP alternative implementation based on
  *        STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST ECP hardware alternative module.
  *        Requires: MBEDTLS_ECP_C, MBEDTLS_ECP_ALT.
  */
#define MBEDTLS_ECP_ALT
#define MBEDTLS_HAL_ECP_ALT

/**
  * @brief MBEDTLS_HAL_RSA_ALT Enables ST RSA alternative modules to replace
  *        mbed TLS RSA module by ST RSA alternative implementation based on
  *        STM32 PKA hardware accelerator.
  *
  *        Uncomment a macro to enable ST RSA hardware alternative module.
  *        Requires: MBEDTLS_RSA_C, MBEDTLS_RSA_ALT.
  */
#define MBEDTLS_RSA_ALT
#define MBEDTLS_HAL_RSA_ALT

/**
  * @brief MBEDTLS_HAL_ENTROPY_HARDWARE_ALT Enables ST entropy source modules
  *        to replace mbed TLS entropy module by ST entropy implementation
  *        based on STM32 RNG hardware accelerator.
  *
  *        Uncomment a macro to enable ST entropy hardware alternative module.
  *        Requires: MBEDTLS_ENTROPY_C, MBEDTLS_ENTROPY_HARDWARE_ALT.
  */
#define MBEDTLS_ENTROPY_HARDWARE_ALT
#define MBEDTLS_HAL_ENTROPY_HARDWARE_ALT

#endif /* STM32WB_MBEDTLS_CONFIG_H */
