/**
  ******************************************************************************
  * @file    psa_plat_crypto.h
  * @author  MCD Application Team
  * @brief   PSA crypto interface source
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef _PSA_PLAT_CRYPTO_H_
#define _PSA_PLAT_CRYPTO_H_

#include <stdint.h>

void PSA_PLAT_CRYPTO_RngGet(uint8_t n, uint32_t* a_Val);

void PSA_PLAT_CRYPTO_AesEcbEncryt(const uint8_t* a_Key, const uint8_t* a_Input, uint8_t* a_Output);

void PSA_PLAT_CRYPTO_AesCmacSetKey(const uint8_t* a_Key);

void PSA_PLAT_CRYPTO_AesCmacCompute(const uint8_t* a_Input, uint32_t InputLenght, uint8_t* a_OutputTag);

int PSA_PLAT_CRYPTO_CcmCrypt(uint8_t mode,
                             const uint8_t* a_Key,
                             uint8_t iv_length,
                             const uint8_t* iv,
                             uint16_t add_length,
                             const uint8_t* add,
                             uint32_t input_length,
                             const uint8_t* input,
                             uint8_t tag_length,
                             uint8_t* tag,
                             uint8_t* output);

int PSA_PLAT_CRYPTO_PkaStartP256Key(const uint32_t* a_LocalPrivateKey);

void PSA_PLAT_CRYPTO_PkaReadP256Key(uint32_t* a_LocalPublicKey);

int PSA_PLAT_CRYPTO_PkaStartDhKey(const uint32_t* a_LocalPrivateKey, const uint32_t* a_RemotePublicKey);

int PSA_PLAT_CRYPTO_PkaReadDhKey(uint32_t* a_DhKey);

#endif /*_PSA_PLAT_CRYPTO_H_*/