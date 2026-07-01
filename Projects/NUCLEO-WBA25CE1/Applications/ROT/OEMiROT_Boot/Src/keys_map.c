/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 * Modifications are Copyright (c) 2019 Arm Limited.
 */

#include <bootutil/sign_key.h>
#include "mcuboot_config/mcuboot_config.h"
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"
#include "flash_layout.h"
#include "mbedtls_config.h"
#include "region_defs.h"

/* Private define ------------------------------------------------------------*/
#if defined(MCUBOOT_SIGN_EC256)
#define EC256_PRIV_KEY_LENGTH     (70U)
#define EC256_PUB_KEY_LENGTH      (91U)
#endif

#define SECURE_AUTHENTICATION_PUBKEY_ADDRESS      (const uint8_t *)((uint32_t)(PERSO_START))
#if defined (OEMIROT_FIRST_BOOT_STAGE)
#define ENCRYPTION_PRIVKEY_ADDRESS                (const uint8_t *)((uint32_t)(PERSO_START | 0x5C))
#else
#define NONSECURE_AUTHENTICATION_PUBKEY_ADDRESS   (const uint8_t *)((uint32_t)(PERSO_START | 0x5C))
#define ENCRYPTION_PRIVKEY_ADDRESS                (const uint8_t *)((uint32_t)(PERSO_START | 0xB8))
#endif

#if defined(MCUBOOT_SIGN_EC256)
const unsigned int ecdsa_pub_key_len = EC256_PUB_KEY_LENGTH;
#if (MCUBOOT_APP_IMAGE_NUMBER == 2) || (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
const unsigned int ecdsa_pub_key_len_1 = EC256_PUB_KEY_LENGTH;
#endif
const struct bootutil_key bootutil_keys[]= {
    {
        .key = SECURE_AUTHENTICATION_PUBKEY_ADDRESS,
        .len = &ecdsa_pub_key_len,
    },
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
    {
        .key = NONSECURE_AUTHENTICATION_PUBKEY_ADDRESS,
        .len = &ecdsa_pub_key_len_1,
    },
#endif
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
    {
        .key = SECURE_AUTHENTICATION_PUBKEY_ADDRESS,
        .len = &ecdsa_pub_key_len,
    },
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
    {
        .key = NONSECURE_AUTHENTICATION_PUBKEY_ADDRESS,
        .len = &ecdsa_pub_key_len_1,
    },
#endif
};
#else
#error "No public key available for given signing algorithm."
#endif
const int bootutil_key_cnt = MCUBOOT_IMAGE_NUMBER;
#if defined(MCUBOOT_ENC_IMAGES)
#if defined(MCUBOOT_ENCRYPT_EC256)

const unsigned int enc_ec256_priv_key_len = EC256_PRIV_KEY_LENGTH;
#if defined(__ICCARM__)
#pragma location=".bootutil_enc_key"
#endif /* __ICCARM__ */
const struct bootutil_key bootutil_enc_key = {
    .key = ENCRYPTION_PRIVKEY_ADDRESS,
    .len = &enc_ec256_priv_key_len,
};
#endif /* MCUBOOT_ENCRYPT_RSA */
#else
const struct bootutil_key bootutil_enc_key = {
    .key = (const unsigned char *)0,
    .len = (const unsigned int *)0,
};
#endif/* MCUBOOT_ENC_IMAGES */
