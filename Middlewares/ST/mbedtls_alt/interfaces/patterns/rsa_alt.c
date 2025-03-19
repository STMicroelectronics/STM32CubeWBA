/**
  ******************************************************************************
  * @file    rsa_alt.c
  * @author  GPM Application Team
  * @brief   Implementation of mbedtls_alt RSA module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  * Copyright The Mbed TLS Contributors
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
  * The RSA public-key cryptosystem
  *
  * This file implements ST RSA based on STM32 PKA hardware crypto accelerator.
  *
  */

/*
 *  The following sources were referenced in the design of this implementation
 *  of the RSA algorithm:
 *
 *  [1] A method for obtaining digital signatures and public-key cryptosystems
 *      R Rivest, A Shamir, and L Adleman
 *      http://people.csail.mit.edu/rivest/pubs.html#RSA78
 *
 *  [2] Handbook of Applied Cryptography - 1997, Chapter 8
 *      Menezes, van Oorschot and Vanstone
 *
 *  [3] Malware Guard Extension: Using SGX to Conceal Cache Attacks
 *      Michael Schwarz, Samuel Weiser, Daniel Gruss, Clementine Maurice and
 *      Stefan Mangard
 *      https://arxiv.org/abs/1702.08719v2
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "common.h"

#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_RSA_ALT)

#include "mbedtls/rsa.h"
#include "bignum_core.h"
#include "bignum_internal.h"
#include "rsa_alt_helpers.h"
#include "rsa_internal.h"
#include "mbedtls/oid.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "constant_time_internal.h"
#include "mbedtls/constant_time.h"
#include "md_psa.h"

#include <string.h>

#if defined(MBEDTLS_PKCS1_V15) && !defined(__OpenBSD__) && !defined(__NetBSD__)
#include <stdlib.h>
#endif /* MBEDTLS_PKCS1_V15 and not __OpenBSD__ */

#include "mbedtls/platform.h"

#if defined(MBEDTLS_HAL_RSA_ALT)

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ST_PKA_TIMEOUT 5000      /* 5s timeout for the Public key accelerator */
#define RNG_MAX_LOOP    100    /* Maximum iteration to get random using f_rng */
/* Private macro -------------------------------------------------------------*/
/*
 * 32-bit integer manipulation macros (big endian)
 */
#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n,b,i)                            \
  do {                                                    \
    (n) = ( (uint32_t) (b)[(i)    ] << 24 )             \
          | ( (uint32_t) (b)[(i) + 1] << 16 )             \
          | ( (uint32_t) (b)[(i) + 2] <<  8 )             \
          | ( (uint32_t) (b)[(i) + 3]       );            \
  } while( 0 )
#endif /* !GET_UINT32_BE */

/**
  * @brief       Operate the PKA Arithmetic multiplication : AxB = A x B
  * @param[in]   A         Operand A
  * @param[in]   A_len     Operand A length
  * @param[in]   B         Operand B
  * @param[in]   B_len     Operand B length
  * @param[out]  AxB       Result
  * @retval      0                                       Ok
  * @retval      MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED    Error in the HW
  */
static int rsa_pka_arithmetic_mul(const unsigned char *A,
                                  size_t A_len,
                                  const unsigned char *B,
                                  size_t B_len,
                                  uint32_t *AxB)
{
  int ret = 0;
  PKA_HandleTypeDef hpka = {0};
  PKA_MulInTypeDef in = {0};
  uint32_t *input_A = NULL;
  uint32_t *input_B = NULL;
  size_t i = 0;
  size_t op_len = 0;

  if (A_len != B_len)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  op_len = A_len;

  input_A = mbedtls_calloc(1, op_len);
  MBEDTLS_MPI_CHK((input_A == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  for (i = op_len / 4 ; i > 0; i--)
  {
    GET_UINT32_BE(input_A[(op_len / 4) - i], A, 4 * (i - 1));
  }

  input_B = mbedtls_calloc(1, op_len);
  MBEDTLS_MPI_CHK((input_B == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  for (i = op_len / 4 ; i > 0; i--)
  {
    GET_UINT32_BE(input_B[(op_len / 4) - i], B, 4 * (i - 1));
  }

  in.size = op_len / 4;
  in.pOp1 = input_A;
  in.pOp2 = input_B;

  /* Enable HW peripheral clock */
  __HAL_RCC_PKA_CLK_ENABLE();

  /* Initialize HW peripheral */
  hpka.Instance = PKA;
  MBEDTLS_MPI_CHK((HAL_PKA_Init(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  /* Reset PKA RAM */
  HAL_PKA_RAMReset(&hpka);

  MBEDTLS_MPI_CHK((HAL_PKA_Mul(&hpka, &in, ST_PKA_TIMEOUT) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  HAL_PKA_Arithmetic_GetResult(&hpka, (uint32_t *)AxB);

cleanup:
  /* De-initialize HW peripheral */
  HAL_PKA_DeInit(&hpka);

  /* Disable HW peripheral clock */
  __HAL_RCC_PKA_CLK_DISABLE();

  if (input_A != NULL)
  {
    mbedtls_platform_zeroize(input_A, op_len);
    mbedtls_free(input_A);
  }

  if (input_B != NULL)
  {
    mbedtls_platform_zeroize(input_B, op_len);
    mbedtls_free(input_B);
  }

  return ret;
}

/**
  * @brief       Call the PKA Arithmetic multiplication : AxB = A x B
  * @param[out]  AxB       Result in mpi format
  * @param[in]   A         Operand A in mpi format
  * @param[in]   B         Operand B in mpi format
  * @retval      0                                       Ok
  * @retval      MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED    Error in the HW
  */
static int rsa_mpi2pka_mul(mbedtls_mpi *AxB,
                           const mbedtls_mpi *A,
                           const mbedtls_mpi *B)
{
  int ret = 0;

  size_t A_len = 0;
  size_t B_len = 0;
  size_t AxB_len = 0;
  uint8_t *A_binary = NULL;
  uint8_t *B_binary = NULL;
  uint8_t *AxB_binary = NULL;

  A_len = mbedtls_mpi_size(A);
  A_binary = mbedtls_calloc(1, A_len);
  MBEDTLS_MPI_CHK((A_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(A, A_binary, A_len));

  B_len = mbedtls_mpi_size(B);
  B_binary = mbedtls_calloc(1, B_len);
  MBEDTLS_MPI_CHK((B_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(B, B_binary, B_len));

  AxB_len = A_len + B_len;
  AxB_binary = mbedtls_calloc(1, AxB_len);
  MBEDTLS_MPI_CHK((AxB_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  MBEDTLS_MPI_CHK(rsa_pka_arithmetic_mul(A_binary,
                                         A_len,
                                         B_binary,
                                         B_len,
                                         (uint32_t *)AxB_binary));

  MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary_le(AxB, AxB_binary, AxB_len));

cleanup:
  if (A_binary != NULL)
  {
    mbedtls_platform_zeroize(A_binary, A_len);
    mbedtls_free(A_binary);
  }

  if (B_binary != NULL)
  {
    mbedtls_platform_zeroize(B_binary, B_len);
    mbedtls_free(B_binary);
  }

  if (AxB_binary != NULL)
  {
    mbedtls_platform_zeroize(AxB_binary, AxB_len);
    mbedtls_free(AxB_binary);
  }

  return ret;
}

/**
  * @brief       Compute Euler totient function of n
  * @param[in]   p            prime
  * @param[in]   q            prime
  * @param[out]  output       phi = ( p - 1 )*( q - 1 )
  * @retval      0            Ok
  */
static int rsa_deduce_phi(const mbedtls_mpi *p,
                          const mbedtls_mpi *q,
                          mbedtls_mpi *phi)
{
  int ret = 0;

  /* Temporaries holding P-1, Q-1 */
  mbedtls_mpi P1, Q1;

  mbedtls_mpi_init(&P1);
  mbedtls_mpi_init(&Q1);

  /* P1 = p - 1 */
  MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&P1, p, 1));

  /* Q1 = q - 1 */
  MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&Q1, q, 1));

  /* phi = ( p - 1 ) * ( q - 1 ) */
  MBEDTLS_MPI_CHK(rsa_mpi2pka_mul(phi, &P1, &Q1));

cleanup:

  mbedtls_mpi_free(&P1);
  mbedtls_mpi_free(&Q1);

  return ret;
}

/**
  * @brief       Call the PKA modular exponentiation : output = input^e mod n
  * @param[in]   input        Input of the modexp
  * @param[in]   ctx          RSA context
  * @param[in]   is_private   public (0) or private (1) exponentiation
  * @param[in]   is_protected normal (0) or protected (1) exponentiation
  * @param[out]  output       Output of the ModExp (with length of the modulus)
  * @retval      0                                       Ok
  * @retval      MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED    Error in the HW
  */
static int rsa_pka_modexp(mbedtls_rsa_context *ctx,
                          int is_private,
                          int is_protected,
                          const unsigned char *input,
                          unsigned char *output)
{
  int ret = 0;
  size_t nlen = 0;
  size_t elen = 0;
  PKA_HandleTypeDef hpka = {0};
  PKA_ModExpInTypeDef in = {0};
  uint8_t *e_binary = NULL;
  uint8_t *n_binary = NULL;
  /* parameters for exponentiation in protected mode */
  size_t philen = 0;
  PKA_ModExpProtectModeInTypeDef in_protected = {0};
  uint8_t *phi_binary = NULL;

  if (is_private != 0)
  {
    elen = mbedtls_mpi_size(&ctx->D);
  }
  else
    elen = mbedtls_mpi_size(&ctx->E);

  /* exponent aligned on 4 bytes */
  elen = ((elen + 3) / 4) * 4;

  e_binary = mbedtls_calloc(1, elen);
  MBEDTLS_MPI_CHK((e_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  if (is_private != 0)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->D, e_binary, elen));
  }
  else
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->E, e_binary, elen));

  nlen = ctx->len;
  n_binary = mbedtls_calloc(1, nlen);
  MBEDTLS_MPI_CHK((n_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->N, n_binary, nlen));

  if (is_protected)
  {
    philen = mbedtls_mpi_size(&ctx->Phi);

    /* first phi computation */
    if (0 == philen)
    {
      MBEDTLS_MPI_CHK(rsa_deduce_phi(&ctx->P, &ctx->Q, &ctx->Phi));
      philen = mbedtls_mpi_size(&ctx->Phi);
    }

    phi_binary = mbedtls_calloc(1, philen);
    MBEDTLS_MPI_CHK((phi_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->Phi, phi_binary, philen));

    in_protected.expSize = elen;           /* Exponent length */
    in_protected.OpSize  = nlen;           /* modulus length */
    in_protected.pOp1    = input;
    in_protected.pExp    = e_binary;       /* Exponent */
    in_protected.pMod    = n_binary;       /* modulus */
    in_protected.pPhi    = phi_binary;     /* Euler tolient function */
  }
  else
    /* exponention in normal mode */
  {
    in.expSize = elen;           /* Exponent length */
    in.OpSize  = nlen;           /* modulus length */
    in.pOp1    = input;
    in.pExp    = e_binary;       /* Exponent */
    in.pMod    = n_binary;       /* modulus */
  }

  /* Enable HW peripheral clock */
  __HAL_RCC_PKA_CLK_ENABLE();

  /* Initialize HW peripheral */
  hpka.Instance = PKA;
  MBEDTLS_MPI_CHK((HAL_PKA_Init(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  /* Reset PKA RAM */
  HAL_PKA_RAMReset(&hpka);

  if (is_protected)
  {
    /* output = input ^ e_binary mod n (protected mode) */
    MBEDTLS_MPI_CHK((HAL_PKA_ModExpProtectMode(&hpka, &in_protected,
                                               ST_PKA_TIMEOUT) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  }
  else
  {
    /* output = input ^ e_binary mod n (normal mode) */
    MBEDTLS_MPI_CHK((HAL_PKA_ModExp(&hpka, &in, ST_PKA_TIMEOUT) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  }

  HAL_PKA_ModExp_GetResult(&hpka, (uint8_t *)output);

cleanup:

  /* De-initialize HW peripheral */
  HAL_PKA_DeInit(&hpka);

  /* Disable HW peripheral clock */
  __HAL_RCC_PKA_CLK_DISABLE();

  if (e_binary != NULL)
  {
    mbedtls_platform_zeroize(e_binary, elen);
    mbedtls_free(e_binary);
  }

  if (n_binary != NULL)
  {
    mbedtls_platform_zeroize(n_binary, nlen);
    mbedtls_free(n_binary);
  }

  if (phi_binary != NULL)
  {
    mbedtls_platform_zeroize(phi_binary, philen);
    mbedtls_free(phi_binary);
  }

  return ret;
}

#if !defined(MBEDTLS_RSA_NO_CRT)
/**
  * @brief       Call the PKA CRT exponentiation :
  *              m1 = input ^ dP mod p
  *              m2 = input ^ dQ mod q
  *              h =  (qp)*(m1 - m2) mod p
  *              output = m2 + h*q
  *
  * @param[in]   input        Input of the modexp
  * @param[in]   dP           CRT exponent for the prime factor p
  * @param[in]   dQ           CRT exponent for the prime factor q
  * @param[in]   p            first precomputed prime factor
  * @param[in]   q            second precomputed prime factor
  * @param[in]   qp           qinv = q^-1 mod p
  * @param[out]  output       Output of the ModExp (with length of the modulus)
  * @retval      0                                       Ok
  * @retval      MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED    Error in the HW
  */
static int rsa_crt_pka_modexp(const mbedtls_mpi *dp,
                              const mbedtls_mpi *dq,
                              const mbedtls_mpi *p,
                              const mbedtls_mpi *q,
                              const mbedtls_mpi *qp,
                              const unsigned char *input,
                              size_t input_len,
                              unsigned char *output)
{
  int ret = 0;
  size_t dplen = 0;
  size_t dqlen = 0;
  size_t plen = 0;
  size_t qlen = 0;
  size_t qplen = 0;
  PKA_HandleTypeDef hpka = {0};
  PKA_RSACRTExpInTypeDef in = {0};
  uint8_t *dp_binary = NULL;
  uint8_t *dq_binary = NULL;
  uint8_t *p_binary = NULL;
  uint8_t *q_binary = NULL;
  uint8_t *qp_binary = NULL;

  dplen = mbedtls_mpi_size(dp);
  dp_binary = mbedtls_calloc(1, dplen);
  MBEDTLS_MPI_CHK((dp_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(dp, dp_binary, dplen));

  dqlen = mbedtls_mpi_size(dq);
  dq_binary = mbedtls_calloc(1, dqlen);
  MBEDTLS_MPI_CHK((dq_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(dq, dq_binary, dqlen));

  plen = mbedtls_mpi_size(p);
  p_binary = mbedtls_calloc(1, plen);
  MBEDTLS_MPI_CHK((p_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(p, p_binary, plen));

  qlen = mbedtls_mpi_size(q);
  q_binary = mbedtls_calloc(1, qlen);
  MBEDTLS_MPI_CHK((q_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(q, q_binary, qlen));

  qplen = mbedtls_mpi_size(qp);
  qp_binary = mbedtls_calloc(1, qplen);
  MBEDTLS_MPI_CHK((qp_binary == NULL) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);
  MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(qp, qp_binary, qplen));

  in.size    = input_len;
  in.pOpDp   = dp_binary;
  in.pOpDq   = dq_binary;
  in.pOpQinv = qp_binary;
  in.pPrimeP = p_binary;
  in.pPrimeQ = q_binary;
  in.popA    = input;

  /* Enable HW peripheral clock */
  __HAL_RCC_PKA_CLK_ENABLE();

  /* Initialize HW peripheral */
  hpka.Instance = PKA;
  MBEDTLS_MPI_CHK((HAL_PKA_Init(&hpka) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  /* Reset PKA RAM */
  HAL_PKA_RAMReset(&hpka);

  MBEDTLS_MPI_CHK((HAL_PKA_RSACRTExp(&hpka, &in, ST_PKA_TIMEOUT) != HAL_OK) ? MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED : 0);

  HAL_PKA_RSACRTExp_GetResult(&hpka, (uint8_t *)output);

cleanup:

  /* De-initialize HW peripheral */
  HAL_PKA_DeInit(&hpka);

  /* Disable HW peripheral clock */
  __HAL_RCC_PKA_CLK_DISABLE();

  if (dp_binary != NULL)
  {
    mbedtls_platform_zeroize(dp_binary, dplen);
    mbedtls_free(dp_binary);
  }

  if (dq_binary != NULL)
  {
    mbedtls_platform_zeroize(dq_binary, dqlen);
    mbedtls_free(dq_binary);
  }

  if (p_binary != NULL)
  {
    mbedtls_platform_zeroize(p_binary, plen);
    mbedtls_free(p_binary);
  }

  if (q_binary != NULL)
  {
    mbedtls_platform_zeroize(q_binary, qlen);
    mbedtls_free(q_binary);
  }

  if (qp_binary != NULL)
  {
    mbedtls_platform_zeroize(qp_binary, qplen);
    mbedtls_free(qp_binary);
  }

  return ret;
}
#endif /* !MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_PKCS1_V15) && defined(MBEDTLS_RSA_C)

/** This function performs the unpadding part of a PKCS#1 v1.5 decryption
  *  operation (EME-PKCS1-v1_5 decoding).
  *
  * @note The return value from this function is a sensitive value
  *       (this is unusual). #MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE shouldn't happen
  *       in a well-written application, but 0 vs #MBEDTLS_ERR_RSA_INVALID_PADDING
  *       is often a situation that an attacker can provoke and leaking which
  *       one is the result is precisely the information the attacker wants.
  *
  * @param input          The input buffer which is the payload inside PKCS#1v1.5
  *                       encryption padding, called the "encoded message EM"
  *                       by the terminology.
  * @param ilen           The length of the payload in the \p input buffer.
  * @param output         The buffer for the payload, called "message M" by the
  *                       PKCS#1 terminology. This must be a writable buffer of
  *                       length \p output_max_len bytes.
  * @param olen           The address at which to store the length of
  *                       the payload. This must not be \c NULL.
  * @param output_max_len The length in bytes of the output buffer \p output.
  *
  * @return      \c 0 on success.
  * @return      #MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE
  *              The output buffer is too small for the unpadded payload.
  * @return      #MBEDTLS_ERR_RSA_INVALID_PADDING
  *              The input doesn't contain properly formatted padding.
  */
static int mbedtls_ct_rsaes_pkcs1_v15_unpadding(unsigned char *input,
                                                size_t ilen,
                                                unsigned char *output,
                                                size_t output_max_len,
                                                size_t *olen)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t i, plaintext_max_size;

  /* The following variables take sensitive values: their value must
   * not leak into the observable behavior of the function other than
   * the designated outputs (output, olen, return value). Otherwise
   * this would open the execution of the function to
   * side-channel-based variants of the Bleichenbacher padding oracle
   * attack. Potential side channels include overall timing, memory
   * access patterns (especially visible to an adversary who has access
   * to a shared memory cache), and branches (especially visible to
   * an adversary who has access to a shared code cache or to a shared
   * branch predictor). */
  size_t pad_count = 0;
  mbedtls_ct_condition_t bad;
  mbedtls_ct_condition_t pad_done;
  size_t plaintext_size = 0;
  mbedtls_ct_condition_t output_too_large;

  plaintext_max_size = (output_max_len > ilen - 11) ? ilen - 11
                       : output_max_len;

  /* Check and get padding length in constant time and constant
   * memory trace. The first byte must be 0. */
  bad = mbedtls_ct_bool(input[0]);


  /* Decode EME-PKCS1-v1_5 padding: 0x00 || 0x02 || PS || 0x00
   * where PS must be at least 8 nonzero bytes. */
  bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_ne(input[1], MBEDTLS_RSA_CRYPT));

  /* Read the whole buffer. Set pad_done to nonzero if we find
   * the 0x00 byte and remember the padding length in pad_count. */
  pad_done = MBEDTLS_CT_FALSE;
  for (i = 2; i < ilen; i++)
  {
    mbedtls_ct_condition_t found = mbedtls_ct_uint_eq(input[i], 0);
    pad_done   = mbedtls_ct_bool_or(pad_done, found);
    pad_count += mbedtls_ct_uint_if_else_0(mbedtls_ct_bool_not(pad_done), 1);
  }

  /* If pad_done is still zero, there's no data, only unfinished padding. */
  bad = mbedtls_ct_bool_or(bad, mbedtls_ct_bool_not(pad_done));

  /* There must be at least 8 bytes of padding. */
  bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_gt(8, pad_count));

  /* If the padding is valid, set plaintext_size to the number of
   * remaining bytes after stripping the padding. If the padding
   * is invalid, avoid leaking this fact through the size of the
   * output: use the maximum message size that fits in the output
   * buffer. Do it without branches to avoid leaking the padding
   * validity through timing. RSA keys are small enough that all the
   * size_t values involved fit in unsigned int. */
  plaintext_size = mbedtls_ct_uint_if(
                     bad, (unsigned) plaintext_max_size,
                     (unsigned)(ilen - pad_count - 3));

  /* Set output_too_large to 0 if the plaintext fits in the output
   * buffer and to 1 otherwise. */
  output_too_large = mbedtls_ct_uint_gt(plaintext_size,
                                        plaintext_max_size);

  /* Set ret without branches to avoid timing attacks. Return:
   * - INVALID_PADDING if the padding is bad (bad != 0).
   * - OUTPUT_TOO_LARGE if the padding is good but the decrypted
   *   plaintext does not fit in the output buffer.
   * - 0 if the padding is correct. */
  ret = mbedtls_ct_error_if(
          bad,
          MBEDTLS_ERR_RSA_INVALID_PADDING,
          mbedtls_ct_error_if_else_0(output_too_large, MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE)
        );

  /* If the padding is bad or the plaintext is too large, zero the
   * data that we're about to copy to the output buffer.
   * We need to copy the same amount of data
   * from the same buffer whether the padding is good or not to
   * avoid leaking the padding validity through overall timing or
   * through memory or cache access patterns. */
  mbedtls_ct_zeroize_if(mbedtls_ct_bool_or(bad, output_too_large), input + 11, ilen - 11);

  /* If the plaintext is too large, truncate it to the buffer size.
   * Copy anyway to avoid revealing the length through timing, because
   * revealing the length is as bad as revealing the padding validity
   * for a Bleichenbacher attack. */
  plaintext_size = mbedtls_ct_uint_if(output_too_large,
                                      (unsigned) plaintext_max_size,
                                      (unsigned) plaintext_size);

  /* Move the plaintext to the leftmost position where it can start in
   * the working buffer, i.e. make it start plaintext_max_size from
   * the end of the buffer. Do this with a memory access trace that
   * does not depend on the plaintext size. After this move, the
   * starting location of the plaintext is no longer sensitive
   * information. */
  mbedtls_ct_memmove_left(input + ilen - plaintext_max_size,
                          plaintext_max_size,
                          plaintext_max_size - plaintext_size);

  /* Finally copy the decrypted plaintext plus trailing zeros into the output
   * buffer. If output_max_len is 0, then output may be an invalid pointer
   * and the result of memcpy() would be undefined; prevent undefined
   * behavior making sure to depend only on output_max_len (the size of the
   * user-provided output buffer), which is independent from plaintext
   * length, validity of padding, success of the decryption, and other
   * secrets. */
  if (output_max_len != 0)
  {
    memcpy(output, input + ilen - plaintext_max_size, plaintext_max_size);
  }

  /* Report the amount of data we copied to the output buffer. In case
   * of errors (bad padding or output too large), the value of *olen
   * when this function returns is not specified. Making it equivalent
   * to the good case limits the risks of leaking the padding validity. */
  *olen = plaintext_size;

  return ret;
}

#endif /* MBEDTLS_PKCS1_V15 && MBEDTLS_RSA_C */

int mbedtls_rsa_import(mbedtls_rsa_context *ctx, const mbedtls_mpi *N,
                       const mbedtls_mpi *P, const mbedtls_mpi *Q,
                       const mbedtls_mpi *D, const mbedtls_mpi *E)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

  if ((N != NULL && (ret = mbedtls_mpi_copy(&ctx->N, N)) != 0) ||
      (P != NULL && (ret = mbedtls_mpi_copy(&ctx->P, P)) != 0) ||
      (Q != NULL && (ret = mbedtls_mpi_copy(&ctx->Q, Q)) != 0) ||
      (D != NULL && (ret = mbedtls_mpi_copy(&ctx->D, D)) != 0) ||
      (E != NULL && (ret = mbedtls_mpi_copy(&ctx->E, E)) != 0))
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
  }

  if (N != NULL)
  {
    ctx->len = mbedtls_mpi_size(&ctx->N);
  }

  return 0;
}

int mbedtls_rsa_import_raw(mbedtls_rsa_context *ctx,
                           unsigned char const *N, size_t N_len,
                           unsigned char const *P, size_t P_len,
                           unsigned char const *Q, size_t Q_len,
                           unsigned char const *D, size_t D_len,
                           unsigned char const *E, size_t E_len)
{
  int ret = 0;

  if (N != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->N, N, N_len));
    ctx->len = mbedtls_mpi_size(&ctx->N);
  }

  if (P != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->P, P, P_len));
  }

  if (Q != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->Q, Q, Q_len));
  }

  if (D != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->D, D, D_len));
  }

  if (E != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&ctx->E, E, E_len));
  }

cleanup:

  if (ret != 0)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
  }

  return 0;
}

/*
 * Checks whether the context fields are set in such a way
 * that the RSA primitives will be able to execute without error.
 * It does *not* make guarantees for consistency of the parameters.
 */
static int rsa_check_context(mbedtls_rsa_context const *ctx, int is_priv,
                             int blinding_needed)
{
#if !defined(MBEDTLS_RSA_NO_CRT)
  /* blinding_needed is only used for NO_CRT to decide whether
   * P,Q need to be present or not. */
  ((void) blinding_needed);
#endif /* !MBEDTLS_RSA_NO_CRT */

  if (ctx->len != mbedtls_mpi_size(&ctx->N) ||
      ctx->len > MBEDTLS_MPI_MAX_SIZE)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /*
   * 1. Modular exponentiation needs positive, odd moduli.
   */

  /* Modular exponentiation wrt. N is always used for
   * RSA public key operations. */
  if (mbedtls_mpi_cmp_int(&ctx->N, 0) <= 0 ||
      mbedtls_mpi_get_bit(&ctx->N, 0) == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

#if !defined(MBEDTLS_RSA_NO_CRT)
  /* Modular exponentiation for P and Q is only
   * used for private key operations and if CRT
   * is used. */
  if (is_priv &&
      (mbedtls_mpi_cmp_int(&ctx->P, 0) <= 0 ||
       mbedtls_mpi_get_bit(&ctx->P, 0) == 0 ||
       mbedtls_mpi_cmp_int(&ctx->Q, 0) <= 0 ||
       mbedtls_mpi_get_bit(&ctx->Q, 0) == 0))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
#endif /* !MBEDTLS_RSA_NO_CRT */

  /*
   * 2. Exponents must be positive
   */

  /* Always need E for public key operations */
  if (mbedtls_mpi_cmp_int(&ctx->E, 0) <= 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

#if defined(MBEDTLS_RSA_NO_CRT)
  /* For private key operations, use D or DP & DQ
   * as (unblinded) exponents. */
  if (is_priv && mbedtls_mpi_cmp_int(&ctx->D, 0) <= 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
#else
  if (is_priv &&
      (mbedtls_mpi_cmp_int(&ctx->DP, 0) <= 0 ||
       mbedtls_mpi_cmp_int(&ctx->DQ, 0) <= 0))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
#endif /* MBEDTLS_RSA_NO_CRT */

  /* Blinding shouldn't make exponents negative either,
   * so check that P, Q >= 1 if that hasn't yet been
   * done as part of 1. */
#if defined(MBEDTLS_RSA_NO_CRT)
  if (is_priv && blinding_needed &&
      (mbedtls_mpi_cmp_int(&ctx->P, 0) <= 0 ||
       mbedtls_mpi_cmp_int(&ctx->Q, 0) <= 0))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
#endif /* MBEDTLS_RSA_NO_CRT */

  /* It wouldn't lead to an error if it wasn't satisfied,
   * but check for QP >= 1 nonetheless. */
#if !defined(MBEDTLS_RSA_NO_CRT)
  if (is_priv &&
      mbedtls_mpi_cmp_int(&ctx->QP, 0) <= 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
#endif /* !MBEDTLS_RSA_NO_CRT */

  return 0;
}

int mbedtls_rsa_complete(mbedtls_rsa_context *ctx)
{
  int ret = 0;
  int have_N = 0;
  int have_P = 0;
  int have_Q = 0;
  int have_D = 0;
  int have_E = 0;
  int n_missing = 0;
  int pq_missing = 0;
  int d_missing = 0;
  int is_pub = 0;
  int is_priv = 0;

  have_N = (mbedtls_mpi_cmp_int(&ctx->N, 0) != 0);
  have_P = (mbedtls_mpi_cmp_int(&ctx->P, 0) != 0);
  have_Q = (mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0);
  have_D = (mbedtls_mpi_cmp_int(&ctx->D, 0) != 0);
  have_E = (mbedtls_mpi_cmp_int(&ctx->E, 0) != 0);

  /*
   * Check whether provided parameters are enough
   * to deduce all others. The following incomplete
   * parameter sets for private keys are supported:
   *
   * (1) P, Q missing.
   * (2) D and potentially N missing.
   *
   */

  n_missing  =              have_P &&  have_Q &&  have_D && have_E;
  pq_missing =   have_N && !have_P && !have_Q &&  have_D && have_E;
  d_missing  =              have_P &&  have_Q && !have_D && have_E;
  is_pub     =   have_N && !have_P && !have_Q && !have_D && have_E;

  /* These three alternatives are mutually exclusive */
  is_priv = n_missing || pq_missing || d_missing;

  if (!is_priv && !is_pub)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /*
   * Step 1: Deduce N if P, Q are provided.
   */

  if (!have_N && have_P && have_Q)
  {
    if ((ret = rsa_mpi2pka_mul(&ctx->N, &ctx->P,
                               &ctx->Q)) != 0)
    {
      return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }

    ctx->len = mbedtls_mpi_size(&ctx->N);
  }

  /*
   * Step 2: Deduce and verify all remaining core parameters.
   */

  if (pq_missing)
  {
    ret = mbedtls_rsa_deduce_primes(&ctx->N, &ctx->E, &ctx->D,
                                    &ctx->P, &ctx->Q);
    if (ret != 0)
    {
      return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }

  }
  else if (d_missing)
  {
    if ((ret = mbedtls_rsa_deduce_private_exponent(&ctx->P,
                                                   &ctx->Q,
                                                   &ctx->E,
                                                   &ctx->D)) != 0)
    {
      return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }
  }

  /*
   * Step 3: Deduce all additional parameters specific
   *         to our current RSA implementation.
   */
#if !defined(MBEDTLS_RSA_NO_CRT)
  if (is_priv != 0)
  {
    ret = mbedtls_rsa_deduce_crt(&ctx->P,  &ctx->Q,  &ctx->D,
                                 &ctx->DP, &ctx->DQ, &ctx->QP);
    if (ret != 0)
    {
      return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
    }
  }
#endif /* MBEDTLS_RSA_NO_CRT */

  /*
   * Step 3: Basic sanity checks
   */

  return rsa_check_context(ctx, is_priv, 1);
}

int mbedtls_rsa_export_raw(const mbedtls_rsa_context *ctx,
                           unsigned char *N, size_t N_len,
                           unsigned char *P, size_t P_len,
                           unsigned char *Q, size_t Q_len,
                           unsigned char *D, size_t D_len,
                           unsigned char *E, size_t E_len)
{
  int ret = 0;
  int is_priv = 0;

  /* Check if key is private or public */
  is_priv =
    mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

  if (is_priv == 0)
  {
    /* If we're trying to export private parameters for a public key,
     * something must be wrong. */
    if (P != NULL || Q != NULL || D != NULL)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

  }

  if (N != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->N, N, N_len));
  }

  if (P != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->P, P, P_len));
  }

  if (Q != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->Q, Q, Q_len));
  }

  if (D != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->D, D, D_len));
  }

  if (E != NULL)
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_write_binary(&ctx->E, E, E_len));
  }

cleanup:

  return ret;
}

int mbedtls_rsa_export(const mbedtls_rsa_context *ctx,
                       mbedtls_mpi *N, mbedtls_mpi *P, mbedtls_mpi *Q,
                       mbedtls_mpi *D, mbedtls_mpi *E)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  int is_priv = 0;

  /* Check if key is private or public */
  is_priv =
    mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

  if (is_priv == 0)
  {
    /* If we're trying to export private parameters for a public key,
     * something must be wrong. */
    if (P != NULL || Q != NULL || D != NULL)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

  }

  /* Export all requested core parameters. */

  if ((N != NULL && (ret = mbedtls_mpi_copy(N, &ctx->N)) != 0) ||
      (P != NULL && (ret = mbedtls_mpi_copy(P, &ctx->P)) != 0) ||
      (Q != NULL && (ret = mbedtls_mpi_copy(Q, &ctx->Q)) != 0) ||
      (D != NULL && (ret = mbedtls_mpi_copy(D, &ctx->D)) != 0) ||
      (E != NULL && (ret = mbedtls_mpi_copy(E, &ctx->E)) != 0))
  {
    return ret;
  }

  return 0;
}

/*
 * Export CRT parameters
 * This must also be implemented if CRT is not used, for being able to
 * write DER encoded RSA keys. The helper function mbedtls_rsa_deduce_crt
 * can be used in this case.
 */
int mbedtls_rsa_export_crt(const mbedtls_rsa_context *ctx,
                           mbedtls_mpi *DP, mbedtls_mpi *DQ, mbedtls_mpi *QP)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  int is_priv = 0;

  /* Check if key is private or public */
  is_priv =
    mbedtls_mpi_cmp_int(&ctx->N, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->P, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->Q, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->D, 0) != 0 &&
    mbedtls_mpi_cmp_int(&ctx->E, 0) != 0;

  if (is_priv == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

#if !defined(MBEDTLS_RSA_NO_CRT)
  /* Export all requested blinding parameters. */
  if ((DP != NULL && (ret = mbedtls_mpi_copy(DP, &ctx->DP)) != 0) ||
      (DQ != NULL && (ret = mbedtls_mpi_copy(DQ, &ctx->DQ)) != 0) ||
      (QP != NULL && (ret = mbedtls_mpi_copy(QP, &ctx->QP)) != 0))
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
  }
#else
  if ((ret = mbedtls_rsa_deduce_crt(&ctx->P, &ctx->Q, &ctx->D,
                                    DP, DQ, QP)) != 0)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_BAD_INPUT_DATA, ret);
  }
#endif /* !MBEDTLS_RSA_NO_CRT */

  return 0;
}

/*
 * Initialize an RSA context
 */
void mbedtls_rsa_init(mbedtls_rsa_context *ctx)
{
  memset(ctx, 0, sizeof(mbedtls_rsa_context));

  ctx->padding = MBEDTLS_RSA_PKCS_V15;
  ctx->hash_id = MBEDTLS_MD_NONE;

#if defined(MBEDTLS_THREADING_C)
  /* Set ctx->ver to nonzero to indicate that the mutex has been
   * initialized and will need to be freed. */
  ctx->ver = 1;
  mbedtls_mutex_init(&ctx->mutex);
#endif /* MBEDTLS_THREADING_C */
}

/*
 * Set padding for an existing RSA context
 */
int mbedtls_rsa_set_padding(mbedtls_rsa_context *ctx, int padding,
                            mbedtls_md_type_t hash_id)
{
  switch (padding)
  {
#if defined(MBEDTLS_PKCS1_V15)
    case MBEDTLS_RSA_PKCS_V15:
      break;
#endif /* MBEDTLS_PKCS1_V15 */

#if defined(MBEDTLS_PKCS1_V21)
    case MBEDTLS_RSA_PKCS_V21:
      break;
#endif /* MBEDTLS_PKCS1_V21 */

    default:
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }

#if defined(MBEDTLS_PKCS1_V21)
  if ((padding == MBEDTLS_RSA_PKCS_V21) &&
      (hash_id != MBEDTLS_MD_NONE))
  {
    /* Just make sure this hash is supported in this build. */
    if (mbedtls_md_info_from_type(hash_id) == NULL)
    {
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
    }
  }
#endif /* MBEDTLS_PKCS1_V21 */

  ctx->padding = padding;
  ctx->hash_id = hash_id;
  return 0;
}

/*
 * Get padding mode of initialized RSA context
 */
int mbedtls_rsa_get_padding_mode(const mbedtls_rsa_context *ctx)
{
  return ctx->padding;
}

/*
 * Get hash identifier of mbedtls_md_type_t type
 */
int mbedtls_rsa_get_md_alg(const mbedtls_rsa_context *ctx)
{
  return ctx->hash_id;
}

/*
 * Get length in bits of RSA modulus
 */
size_t mbedtls_rsa_get_bitlen(const mbedtls_rsa_context *ctx)
{
  return mbedtls_mpi_bitlen(&ctx->N);
}

/*
 * Get length in bytes of RSA modulus
 */
size_t mbedtls_rsa_get_len(const mbedtls_rsa_context *ctx)
{
  return ctx->len;
}

#if defined(MBEDTLS_GENPRIME)

/*
 * Generate an RSA key pair
 *
 * This generation method follows the RSA key pair generation procedure of
 * FIPS 186-4 if 2^16 < exponent < 2^256 and nbits = 2048 or nbits = 3072.
 */
int mbedtls_rsa_gen_key(mbedtls_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng,
                        unsigned int nbits, int exponent)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  mbedtls_mpi H;
  mbedtls_mpi G;
  mbedtls_mpi L;

  int prime_quality = 0;

  /*
   * If the modulus is 1024 bit long or shorter, then the security strength of
   * the RSA algorithm is less than or equal to 80 bits and therefore an error
   * rate of 2^-80 is sufficient.
   */
  if (nbits > 1024)
  {
    prime_quality = MBEDTLS_MPI_GEN_PRIME_FLAG_LOW_ERR;
  }

  mbedtls_mpi_init(&H);
  mbedtls_mpi_init(&G);
  mbedtls_mpi_init(&L);

  if (exponent < 3 || nbits % 2 != 0)
  {
    ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    goto cleanup;
  }

  if (nbits < MBEDTLS_RSA_GEN_KEY_MIN_BITS)
  {
    ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    goto cleanup;
  }

  /*
   * find primes P and Q with Q < P so that:
   * 1.  |P-Q| > 2^( nbits / 2 - 100 )
   * 2.  GCD( E, (P-1)*(Q-1) ) == 1
   * 3.  E^-1 mod LCM(P-1, Q-1) > 2^( nbits / 2 )
   */
  MBEDTLS_MPI_CHK(mbedtls_mpi_lset(&ctx->E, exponent));

  do
  {
    MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime(&ctx->P, nbits >> 1,
                                          prime_quality, f_rng, p_rng));

    MBEDTLS_MPI_CHK(mbedtls_mpi_gen_prime(&ctx->Q, nbits >> 1,
                                          prime_quality, f_rng, p_rng));

    /* make sure the difference between p and q is not too small (FIPS 186-4 par. B.3.3 step 5.4) */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_mpi(&H, &ctx->P, &ctx->Q));
    if (mbedtls_mpi_bitlen(&H) <= ((nbits >= 200) ? ((nbits >> 1) - 99) : 0))
    {
      continue;
    }

    /* not required by any standards, but some users rely on the fact that P > Q */
    if (H.s < 0)
    {
      mbedtls_mpi_swap(&ctx->P, &ctx->Q);
    }

    /* Temporarily replace P,Q by P-1, Q-1 */
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->P, &ctx->P, 1));
    MBEDTLS_MPI_CHK(mbedtls_mpi_sub_int(&ctx->Q, &ctx->Q, 1));
    MBEDTLS_MPI_CHK(rsa_mpi2pka_mul(&H, &ctx->P, &ctx->Q));

    /* check GCD( E, (P-1)*(Q-1) ) == 1 (FIPS 186-4 par. B.3.1 criterion 2(a)) */
    MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->E, &H));
    if (mbedtls_mpi_cmp_int(&G, 1) != 0)
    {
      continue;
    }

    /* compute smallest possible D = E^-1 mod LCM(P-1, Q-1) (FIPS 186-4 par. B.3.1 criterion 3(b)) */
    MBEDTLS_MPI_CHK(mbedtls_mpi_gcd(&G, &ctx->P, &ctx->Q));
    MBEDTLS_MPI_CHK(mbedtls_mpi_div_mpi(&L, NULL, &H, &G));
    MBEDTLS_MPI_CHK(mbedtls_mpi_inv_mod(&ctx->D, &ctx->E, &L));

    if (mbedtls_mpi_bitlen(&ctx->D) <= ((nbits + 1) / 2))  /* (FIPS 186-4 par. B.3.1 criterion 3(a)) */
    {
      continue;
    }

    break;
  } while (1);

  /* Restore P,Q */
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->P,  &ctx->P, 1));
  MBEDTLS_MPI_CHK(mbedtls_mpi_add_int(&ctx->Q,  &ctx->Q, 1));

  MBEDTLS_MPI_CHK(rsa_mpi2pka_mul(&ctx->N, &ctx->P, &ctx->Q));

  ctx->len = mbedtls_mpi_size(&ctx->N);

#if !defined(MBEDTLS_RSA_NO_CRT)
  /*
   * DP = D mod (P - 1)
   * DQ = D mod (Q - 1)
   * QP = Q^-1 mod P
   */
  MBEDTLS_MPI_CHK(mbedtls_rsa_deduce_crt(&ctx->P, &ctx->Q, &ctx->D,
                                         &ctx->DP, &ctx->DQ, &ctx->QP));
#endif /* !MBEDTLS_RSA_NO_CRT */

  /* Double-check */
  MBEDTLS_MPI_CHK(mbedtls_rsa_check_privkey(ctx));

cleanup:

  mbedtls_mpi_free(&H);
  mbedtls_mpi_free(&G);
  mbedtls_mpi_free(&L);

  if (ret != 0)
  {
    mbedtls_rsa_free(ctx);

    if ((-ret & ~0x7f) == 0)
    {
      ret = MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_KEY_GEN_FAILED, ret);
    }
    return ret;
  }

  return 0;
}

#endif /* MBEDTLS_GENPRIME */

/*
 * Check a public RSA key
 */
int mbedtls_rsa_check_pubkey(const mbedtls_rsa_context *ctx)
{
  if (rsa_check_context(ctx, 0 /* public */, 0 /* no blinding */) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  if (mbedtls_mpi_bitlen(&ctx->N) < 128)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  if (mbedtls_mpi_get_bit(&ctx->E, 0) == 0 ||
      mbedtls_mpi_bitlen(&ctx->E)     < 2  ||
      mbedtls_mpi_cmp_mpi(&ctx->E, &ctx->N) >= 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  return 0;
}

/*
 * Check for the consistency of all fields in an RSA private key context
 */
int mbedtls_rsa_check_privkey(const mbedtls_rsa_context *ctx)
{
  if (mbedtls_rsa_check_pubkey(ctx) != 0 ||
      rsa_check_context(ctx, 1 /* private */, 1 /* blinding */) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  if (mbedtls_rsa_validate_params(&ctx->N, &ctx->P, &ctx->Q,
                                  &ctx->D, &ctx->E, NULL, NULL) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

#if !defined(MBEDTLS_RSA_NO_CRT)
  else if (mbedtls_rsa_validate_crt(&ctx->P, &ctx->Q, &ctx->D,
                                    &ctx->DP, &ctx->DQ, &ctx->QP) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }
#endif /* !MBEDTLS_RSA_NO_CRT */

  return 0;
}

/*
 * Check if contexts holding a public and private key match
 */
int mbedtls_rsa_check_pub_priv(const mbedtls_rsa_context *pub,
                               const mbedtls_rsa_context *prv)
{
  if (mbedtls_rsa_check_pubkey(pub)  != 0 ||
      mbedtls_rsa_check_privkey(prv) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  if (mbedtls_mpi_cmp_mpi(&pub->N, &prv->N) != 0 ||
      mbedtls_mpi_cmp_mpi(&pub->E, &prv->E) != 0)
  {
    return MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
  }

  return 0;
}

/*
 * Do an RSA public key operation
 */
int mbedtls_rsa_public(mbedtls_rsa_context *ctx,
                       const unsigned char *input,
                       unsigned char *output)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  mbedtls_mpi T;

  if (rsa_check_context(ctx, 0 /* public */, 0 /* no blinding */))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  mbedtls_mpi_init(&T);

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&ctx->mutex)) != 0)
  {
    return ret;
  }
#endif /* MBEDTLS_THREADING_C */

  MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&T, input, ctx->len));

  if (mbedtls_mpi_cmp_mpi(&T, &ctx->N) >= 0)
  {
    ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    goto cleanup;
  }

  /* output = input ^ E mod N */
  MBEDTLS_MPI_CHK(rsa_pka_modexp(ctx, 0 /* public */, 0 /* unprotected mode */, input, output));

cleanup:

#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&ctx->mutex) != 0)
  {
    return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  mbedtls_mpi_free(&T);

  if (ret != 0)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_PUBLIC_FAILED, ret);
  }

  return 0;
}

/*
 * Exponent blinding supposed to prevent side-channel attacks using multiple
 * traces of measurements to recover the RSA key. The more collisions are there,
 * the more bits of the key can be recovered. See [3].
 *
 * Collecting n collisions with m bit long blinding value requires 2^(m-m/n)
 * observations on average.
 *
 * For example with 28 byte blinding to achieve 2 collisions the adversary has
 * to make 2^112 observations on average.
 *
 * (With the currently (as of 2017 April) known best algorithms breaking 2048
 * bit RSA requires approximately as much time as trying out 2^112 random keys.
 * Thus in this sense with 28 byte blinding the security is not reduced by
 * side-channel attacks like the one in [3])
 *
 * This countermeasure does not help if the key recovery is possible with a
 * single trace.
 */
#define RSA_EXPONENT_BLINDING 28

/*
 * Do an RSA private key operation
 */
int mbedtls_rsa_private(mbedtls_rsa_context *ctx,
                        int (*f_rng)(void *, unsigned char *, size_t),
                        void *p_rng,
                        const unsigned char *input,
                        unsigned char *output)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

  /* Temporary holding the result */
  mbedtls_mpi T;

#if !defined(MBEDTLS_RSA_NO_CRT)
  /* Pointers to actual exponents to be used - either the unblinded
   * or the blinded ones, depending on the presence of a PRNG. */
  mbedtls_mpi *DP = &ctx->DP;
  mbedtls_mpi *DQ = &ctx->DQ;
#endif /* !MBEDTLS_RSA_NO_CRT */

  if (rsa_check_context(ctx, 1        /* private key checks */,
                        1 /* blinding on       */) != 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

#if defined(MBEDTLS_THREADING_C)
  if ((ret = mbedtls_mutex_lock(&ctx->mutex)) != 0)
  {
    return ret;
  }
#endif /* MBEDTLS_THREADING_C */

  /* MPI Initialization */
  mbedtls_mpi_init(&T);

  /* End of MPI initialization */

  MBEDTLS_MPI_CHK(mbedtls_mpi_read_binary(&T, input, ctx->len));
  if (mbedtls_mpi_cmp_mpi(&T, &ctx->N) >= 0)
  {
    ret = MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
    goto cleanup;
  }

#if defined(MBEDTLS_RSA_NO_CRT)
  /*
   * Protected decryption
   */
  /* T = T ^ D mod N */
  MBEDTLS_MPI_CHK(rsa_pka_modexp(ctx, 1 /* private */, 1 /* protected mode */, input, output));
#else
  /*
   * Faster decryption using the CRT
   */
  MBEDTLS_MPI_CHK(rsa_crt_pka_modexp(DP, DQ, &ctx->P, &ctx->Q, &ctx->QP, input, ctx->len, output));
#endif /* MBEDTLS_RSA_NO_CRT */

cleanup:
#if defined(MBEDTLS_THREADING_C)
  if (mbedtls_mutex_unlock(&ctx->mutex) != 0)
  {
    return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }
#endif /* MBEDTLS_THREADING_C */

  mbedtls_mpi_free(&T);

  if (ret != 0 && ret >= -0x007f)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_PRIVATE_FAILED, ret);
  }

  return ret;
}

#if defined(MBEDTLS_PKCS1_V21)
/**
  * Generate and apply the MGF1 operation (from PKCS#1 v2.1) to a buffer.
  *
  * @param dst       buffer to mask
  * @param dlen      length of destination buffer
  * @param src       source of the mask generation
  * @param slen      length of the source buffer
  * @param md_alg    message digest to use
  */
int mgf_mask(unsigned char *dst, size_t dlen, unsigned char *src,
             size_t slen, mbedtls_md_type_t md_alg)
{
  unsigned char counter[4] = {0};
  unsigned char *p;
  unsigned int hlen = 0;
  size_t i, use_len = 0;
  unsigned char mask[MBEDTLS_MD_MAX_SIZE] = {0};
  int ret = 0;
  const mbedtls_md_info_t *md_info = NULL;
  mbedtls_md_context_t md_ctx;

  md_info = mbedtls_md_info_from_type(md_alg);
  if (md_info == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  mbedtls_md_init(&md_ctx);
  if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0)
  {
    goto exit;
  }

  hlen = mbedtls_md_get_size(md_info);

  memset(mask, 0, sizeof(mask));
  memset(counter, 0, 4);

  /* Generate and apply dbMask */
  p = dst;

  while (dlen > 0)
  {
    use_len = hlen;
    if (dlen < hlen)
    {
      use_len = dlen;
    }

    if ((ret = mbedtls_md_starts(&md_ctx)) != 0)
    {
      goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, src, slen)) != 0)
    {
      goto exit;
    }
    if ((ret = mbedtls_md_update(&md_ctx, counter, 4)) != 0)
    {
      goto exit;
    }
    if ((ret = mbedtls_md_finish(&md_ctx, mask)) != 0)
    {
      goto exit;
    }

    for (i = 0; i < use_len; ++i)
    {
      *p++ ^= mask[i];
    }

    counter[3]++;

    dlen -= use_len;
  }

exit:
  mbedtls_platform_zeroize(mask, sizeof(mask));
  mbedtls_md_free(&md_ctx);

  return ret;
}

/**
  * Generate Hash(M') as in RFC 8017 page 43 points 5 and 6.
  *
  * @param hash      the input hash
  * @param hlen      length of the input hash
  * @param salt      the input salt
  * @param slen      length of the input salt
  * @param out       the output buffer - must be large enough for \p md_alg
  * @param md_alg    message digest to use
  */
static int hash_mprime(const unsigned char *hash, size_t hlen,
                       const unsigned char *salt, size_t slen,
                       unsigned char *out, mbedtls_md_type_t md_alg)
{
  const unsigned char zeros[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

  mbedtls_md_context_t md_ctx;
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

  const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(md_alg);
  if (md_info == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  mbedtls_md_init(&md_ctx);
  if ((ret = mbedtls_md_setup(&md_ctx, md_info, 0)) != 0)
  {
    goto exit;
  }
  if ((ret = mbedtls_md_starts(&md_ctx)) != 0)
  {
    goto exit;
  }
  if ((ret = mbedtls_md_update(&md_ctx, zeros, sizeof(zeros))) != 0)
  {
    goto exit;
  }
  if ((ret = mbedtls_md_update(&md_ctx, hash, hlen)) != 0)
  {
    goto exit;
  }
  if ((ret = mbedtls_md_update(&md_ctx, salt, slen)) != 0)
  {
    goto exit;
  }
  if ((ret = mbedtls_md_finish(&md_ctx, out)) != 0)
  {
    goto exit;
  }

exit:
  mbedtls_md_free(&md_ctx);

  return ret;
}

/**
  * Compute a hash.
  *
  * @param md_alg    algorithm to use
  * @param input     input message to hash
  * @param ilen      input length
  * @param output    the output buffer - must be large enough for \p md_alg
  */
static int compute_hash(mbedtls_md_type_t md_alg,
                        const unsigned char *input, size_t ilen,
                        unsigned char *output)
{
  const mbedtls_md_info_t *md_info;

  md_info = mbedtls_md_info_from_type(md_alg);
  if (md_info == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  return mbedtls_md(md_info, input, ilen, output);
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-ENCRYPT function
 */
int mbedtls_rsa_rsaes_oaep_encrypt(mbedtls_rsa_context *ctx,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng,
                                   const unsigned char *label, size_t label_len,
                                   size_t ilen, const unsigned char *input,
                                   unsigned char *output)
{
  size_t olen = 0;
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  unsigned char *p = output;
  unsigned int hlen = 0;

  if (ctx->padding != MBEDTLS_RSA_PKCS_V21)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  hlen = mbedtls_md_get_size_from_type((mbedtls_md_type_t) ctx->hash_id);
  if (hlen == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  olen = ctx->len;

  /* first comparison checks for overflow */
  if (ilen + 2 * hlen + 2 < ilen || olen < ilen + 2 * hlen + 2)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  memset(output, 0, olen);

  *p++ = 0;

  /* Generate a random octet string seed */
  if ((ret = f_rng(p_rng, p, hlen)) != 0)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_RNG_FAILED, ret);
  }

  p += hlen;

  /* Construct DB */
  ret = compute_hash((mbedtls_md_type_t) ctx->hash_id, label, label_len, p);
  if (ret != 0)
  {
    return ret;
  }
  p += hlen;
  p += olen - 2 * hlen - 2 - ilen;
  *p++ = 1;
  if (ilen != 0)
  {
    memcpy(p, input, ilen);
  }

  /* maskedDB: Apply dbMask to DB */
  if ((ret = mgf_mask(output + hlen + 1, olen - hlen - 1, output + 1, hlen,
                      (mbedtls_md_type_t) ctx->hash_id)) != 0)
  {
    return ret;
  }

  /* maskedSeed: Apply seedMask to seed */
  if ((ret = mgf_mask(output + 1, hlen, output + hlen + 1, olen - hlen - 1,
                      (mbedtls_md_type_t) ctx->hash_id)) != 0)
  {
    return ret;
  }

  return mbedtls_rsa_public(ctx, output, output);
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-ENCRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_encrypt(mbedtls_rsa_context *ctx,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng, size_t ilen,
                                        const unsigned char *input,
                                        unsigned char *output)
{
  size_t nb_pad = 0;
  size_t olen  = 0;
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  unsigned char *p = output;

  olen = ctx->len;

  /* first comparison checks for overflow */
  if (ilen + 11 < ilen || olen < ilen + 11)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  nb_pad = olen - 3 - ilen;

  *p++ = 0;


  if (f_rng == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  *p++ = MBEDTLS_RSA_CRYPT;

  while (nb_pad-- > 0)
  {
    int rng_dl = RNG_MAX_LOOP;

    do
    {
      ret = f_rng(p_rng, p, 1);
    } while (*p == 0 && --rng_dl && ret == 0);

    /* Check if RNG failed to generate data */
    if (rng_dl == 0 || ret != 0)
    {
      return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_RNG_FAILED, ret);
    }

    p++;
  }

  *p++ = 0;
  if (ilen != 0)
  {
    memcpy(p, input, ilen);
  }

  return mbedtls_rsa_public(ctx, output, output);
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Add the message padding, then do an RSA operation
 */
int mbedtls_rsa_pkcs1_encrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng, size_t ilen,
                              const unsigned char *input,
                              unsigned char *output)
{
  switch (ctx->padding)
  {
#if defined(MBEDTLS_PKCS1_V15)
    case MBEDTLS_RSA_PKCS_V15:
      return mbedtls_rsa_rsaes_pkcs1_v15_encrypt(ctx, f_rng, p_rng, ilen,
                                                 input, output);
#endif /* MBEDTLS_PKCS1_V15 */

#if defined(MBEDTLS_PKCS1_V21)
    case MBEDTLS_RSA_PKCS_V21:
      return mbedtls_rsa_rsaes_oaep_encrypt(ctx, f_rng, p_rng, NULL, 0,
                                            ilen, input, output);
#endif /* MBEDTLS_PKCS1_V21 */

    default:
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }
}

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-OAEP-DECRYPT function
 */
int mbedtls_rsa_rsaes_oaep_decrypt(mbedtls_rsa_context *ctx,
                                   int (*f_rng)(void *, unsigned char *, size_t),
                                   void *p_rng,
                                   const unsigned char *label, size_t label_len,
                                   size_t *olen,
                                   const unsigned char *input,
                                   unsigned char *output,
                                   size_t output_max_len)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t ilen = 0;
  size_t i = 0;
  size_t pad_len = 0;
  unsigned char *p;
  mbedtls_ct_condition_t bad = 0;
  mbedtls_ct_condition_t in_padding = MBEDTLS_CT_TRUE;
  unsigned char buf[MBEDTLS_MPI_MAX_SIZE] = {0};
  unsigned char lhash[MBEDTLS_MD_MAX_SIZE] = {0};
  unsigned int hlen = 0;

  /*
   * Parameters sanity checks
   */
  if (ctx->padding != MBEDTLS_RSA_PKCS_V21)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  ilen = ctx->len;

  if (ilen < 16 || ilen > sizeof(buf))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  hlen = mbedtls_md_get_size_from_type((mbedtls_md_type_t) ctx->hash_id);
  if (hlen == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /* checking for integer underflow */
  if (2 * hlen + 2 > ilen)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /*
   * RSA operation
   */
  ret = mbedtls_rsa_private(ctx, f_rng, p_rng, input, buf);

  if (ret != 0)
  {
    goto cleanup;
  }

  /*
   * Unmask data and generate lHash
   */

  /* seed: Apply seedMask to maskedSeed */
  if ((ret = mgf_mask(buf + 1, hlen, buf + hlen + 1, ilen - hlen - 1,
                      (mbedtls_md_type_t) ctx->hash_id)) != 0 ||
      /* DB: Apply dbMask to maskedDB */
      (ret = mgf_mask(buf + hlen + 1, ilen - hlen - 1, buf + 1, hlen,
                      (mbedtls_md_type_t) ctx->hash_id)) != 0)
  {
    goto cleanup;
  }

  /* Generate lHash */
  ret = compute_hash((mbedtls_md_type_t) ctx->hash_id,
                     label, label_len, lhash);
  if (ret != 0)
  {
    goto cleanup;
  }

  /*
   * Check contents, in "constant-time"
   */
  p = buf;

  bad = mbedtls_ct_bool(*p++); /* First byte must be 0 */

  p += hlen; /* Skip seed */

  /* Check lHash */
  bad = mbedtls_ct_bool_or(bad, mbedtls_ct_bool(mbedtls_ct_memcmp(lhash, p, hlen)));
  p += hlen;

  /* Get zero-padding len, but always read till end of buffer
   * (minus one, for the 01 byte) */
  pad_len = 0;
  for (i = 0; i < ilen - 2 * hlen - 2; i++)
  {
    in_padding = mbedtls_ct_bool_and(in_padding, mbedtls_ct_uint_eq(p[i], 0));
    pad_len += mbedtls_ct_uint_if_else_0(in_padding, 1);
  }

  p += pad_len;
  bad = mbedtls_ct_bool_or(bad, mbedtls_ct_uint_ne(*p++, 0x01));

  /*
   * The only information "leaked" is whether the padding was correct or not
   * (eg, no data is copied if it was not correct). This meets the
   * recommendations in PKCS#1 v2.2: an opponent cannot distinguish between
   * the different error conditions.
   */
  if (bad != MBEDTLS_CT_FALSE)
  {
    ret = MBEDTLS_ERR_RSA_INVALID_PADDING;
    goto cleanup;
  }

  if (ilen - ((size_t)(p - buf)) > output_max_len)
  {
    ret = MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE;
    goto cleanup;
  }

  *olen = ilen - ((size_t)(p - buf));
  if (*olen != 0)
  {
    memcpy(output, p, *olen);
  }
  ret = 0;

cleanup:
  mbedtls_platform_zeroize(buf, sizeof(buf));
  mbedtls_platform_zeroize(lhash, sizeof(lhash));

  return ret;
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSAES-PKCS1-V1_5-DECRYPT function
 */
int mbedtls_rsa_rsaes_pkcs1_v15_decrypt(mbedtls_rsa_context *ctx,
                                        int (*f_rng)(void *, unsigned char *, size_t),
                                        void *p_rng, size_t *olen,
                                        const unsigned char *input,
                                        unsigned char *output,
                                        size_t output_max_len)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t ilen = 0;
  unsigned char buf[MBEDTLS_MPI_MAX_SIZE] = {0};

  ilen = ctx->len;

  if (ctx->padding != MBEDTLS_RSA_PKCS_V15)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  if (ilen < 16 || ilen > sizeof(buf))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  ret = mbedtls_rsa_private(ctx, f_rng, p_rng, input, buf);

  if (ret != 0)
  {
    goto cleanup;
  }

  ret = mbedtls_ct_rsaes_pkcs1_v15_unpadding(buf, ilen,
                                             output, output_max_len, olen);

cleanup:
  mbedtls_platform_zeroize(buf, sizeof(buf));

  return ret;
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation, then remove the message padding
 */
int mbedtls_rsa_pkcs1_decrypt(mbedtls_rsa_context *ctx,
                              int (*f_rng)(void *, unsigned char *, size_t),
                              void *p_rng, size_t *olen,
                              const unsigned char *input,
                              unsigned char *output,
                              size_t output_max_len)
{
  switch (ctx->padding)
  {
#if defined(MBEDTLS_PKCS1_V15)
    case MBEDTLS_RSA_PKCS_V15:
      return mbedtls_rsa_rsaes_pkcs1_v15_decrypt(ctx, f_rng, p_rng, olen,
                                                 input, output, output_max_len);
#endif /* MBEDTLS_PKCS1_V15 */

#if defined(MBEDTLS_PKCS1_V21)
    case MBEDTLS_RSA_PKCS_V21:
      return mbedtls_rsa_rsaes_oaep_decrypt(ctx, f_rng, p_rng, NULL, 0,
                                            olen, input, output,
                                            output_max_len);
#endif /* MBEDTLS_PKCS1_V21 */

    default:
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }
}

#if defined(MBEDTLS_PKCS1_V21)
static int rsa_rsassa_pss_sign_no_mode_check(mbedtls_rsa_context *ctx,
                                             int (*f_rng)(void *, unsigned char *, size_t),
                                             void *p_rng,
                                             mbedtls_md_type_t md_alg,
                                             unsigned int hashlen,
                                             const unsigned char *hash,
                                             int saltlen,
                                             unsigned char *sig)
{
  size_t olen = 0;
  unsigned char *p = sig;
  unsigned char *salt = NULL;
  size_t slen = 0;
  size_t min_slen = 0;
  size_t hlen = 0;
  size_t offset = 0;
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t msb = 0;
  mbedtls_md_type_t hash_id;
  size_t exp_hashlen = 0;

  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  if (f_rng == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  olen = ctx->len;

  if (md_alg != MBEDTLS_MD_NONE)
  {
    /* Gather length of hash to sign */
    exp_hashlen = mbedtls_md_get_size_from_type(md_alg);
    if (exp_hashlen == 0)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (hashlen != exp_hashlen)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
  }

  hash_id = (mbedtls_md_type_t) ctx->hash_id;
  if (hash_id == MBEDTLS_MD_NONE)
  {
    hash_id = md_alg;
  }
  hlen = mbedtls_md_get_size_from_type(hash_id);
  if (hlen == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  if (saltlen == MBEDTLS_RSA_SALT_LEN_ANY)
  {
    /* Calculate the largest possible salt length, up to the hash size.
    * Normally this is the hash length, which is the maximum salt length
    * according to FIPS 185-4 par. 5.5 (e) and common practice. If there is not
    * enough room, use the maximum salt length that fits. The constraint is
    * that the hash length plus the salt length plus 2 bytes must be at most
    * the key length. This complies with FIPS 186-4 par. 5.5 (e) and RFC 8017
    * (PKCS#1 v2.2) par. 9.1.1 step 3. */
    min_slen = hlen - 2;
    if (olen < hlen + min_slen + 2)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
    else if (olen >= hlen + hlen + 2)
    {
      slen = hlen;
    }
    else
    {
      slen = olen - hlen - 2;
    }
  }
  else if ((saltlen < 0) || (saltlen + hlen + 2 > olen))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
  else
  {
    slen = (size_t) saltlen;
  }

  memset(sig, 0, olen);

  /* Note: EMSA-PSS encoding is over the length of N - 1 bits */
  msb = mbedtls_mpi_bitlen(&ctx->N) - 1;
  p += olen - hlen - slen - 2;
  *p++ = 0x01;

  /* Generate salt of length slen in place in the encoded message */
  salt = p;
  if ((ret = f_rng(p_rng, salt, slen)) != 0)
  {
    return MBEDTLS_ERROR_ADD(MBEDTLS_ERR_RSA_RNG_FAILED, ret);
  }

  p += slen;

  /* Generate H = Hash( M' ) */
  ret = hash_mprime(hash, hashlen, salt, slen, p, hash_id);
  if (ret != 0)
  {
    return ret;
  }

  /* Compensate for boundary condition when applying mask */
  if (msb % 8 == 0)
  {
    offset = 1;
  }

  /* maskedDB: Apply dbMask to DB */
  ret = mgf_mask(sig + offset, olen - hlen - 1 - offset, p, hlen, hash_id);
  if (ret != 0)
  {
    return ret;
  }

  msb = mbedtls_mpi_bitlen(&ctx->N) - 1;
  sig[0] &= 0xFF >> (olen * 8 - msb);

  p += hlen;
  *p++ = 0xBC;

  return mbedtls_rsa_private(ctx, f_rng, p_rng, sig, sig);
}

static int rsa_rsassa_pss_sign(mbedtls_rsa_context *ctx,
                               int (*f_rng)(void *, unsigned char *, size_t),
                               void *p_rng,
                               mbedtls_md_type_t md_alg,
                               unsigned int hashlen,
                               const unsigned char *hash,
                               int saltlen,
                               unsigned char *sig)
{
  if (ctx->padding != MBEDTLS_RSA_PKCS_V21)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  if ((ctx->hash_id == MBEDTLS_MD_NONE) && (md_alg == MBEDTLS_MD_NONE))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  return rsa_rsassa_pss_sign_no_mode_check(ctx, f_rng, p_rng, md_alg, hashlen, hash, saltlen,
                                           sig);
}

int mbedtls_rsa_rsassa_pss_sign_no_mode_check(mbedtls_rsa_context *ctx,
                                              int (*f_rng)(void *, unsigned char *, size_t),
                                              void *p_rng,
                                              mbedtls_md_type_t md_alg,
                                              unsigned int hashlen,
                                              const unsigned char *hash,
                                              unsigned char *sig)
{
  return rsa_rsassa_pss_sign_no_mode_check(ctx, f_rng, p_rng, md_alg,
                                           hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, sig);
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function with
 * the option to pass in the salt length.
 */
int mbedtls_rsa_rsassa_pss_sign_ext(mbedtls_rsa_context *ctx,
                                    int (*f_rng)(void *, unsigned char *, size_t),
                                    void *p_rng,
                                    mbedtls_md_type_t md_alg,
                                    unsigned int hashlen,
                                    const unsigned char *hash,
                                    int saltlen,
                                    unsigned char *sig)
{
  return rsa_rsassa_pss_sign(ctx, f_rng, p_rng, md_alg,
                             hashlen, hash, saltlen, sig);
}

/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-SIGN function
 */
int mbedtls_rsa_rsassa_pss_sign(mbedtls_rsa_context *ctx,
                                int (*f_rng)(void *, unsigned char *, size_t),
                                void *p_rng,
                                mbedtls_md_type_t md_alg,
                                unsigned int hashlen,
                                const unsigned char *hash,
                                unsigned char *sig)
{
  return rsa_rsassa_pss_sign(ctx, f_rng, p_rng, md_alg,
                             hashlen, hash, MBEDTLS_RSA_SALT_LEN_ANY, sig);
}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-V1_5-SIGN function
 */

/* Construct a PKCS v1.5 encoding of a hashed message
 *
 * This is used both for signature generation and verification.
 *
 * Parameters:
 * - md_alg:  Identifies the hash algorithm used to generate the given hash;
 *            MBEDTLS_MD_NONE if raw data is signed.
 * - hashlen: Length of hash . Must match md_alg if that's not NONE.
 * - hash:    Buffer containing the hashed message or the raw data.
 * - dst_len: Length of the encoded message.
 * - dst:     Buffer to hold the encoded message.
 *
 * Assumptions:
 * - hash has size hashlen.
 * - dst points to a buffer of size at least dst_len.
 *
 */
int rsa_rsassa_pkcs1_v15_encode(mbedtls_md_type_t md_alg,
                                unsigned int hashlen,
                                const unsigned char *hash,
                                size_t dst_len,
                                unsigned char *dst)
{
  size_t oid_size  = 0;
  size_t nb_pad    = dst_len;
  unsigned char *p = dst;
  const char *oid  = NULL;
  unsigned char md_size = 0;

  /* Are we signing hashed or raw data? */
  if (md_alg != MBEDTLS_MD_NONE)
  {
    md_size = mbedtls_md_get_size_from_type(md_alg);
    if (md_size == 0)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (mbedtls_oid_get_oid_by_md(md_alg, &oid, &oid_size) != 0)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (hashlen != md_size)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    /* Double-check that 8 + hashlen + oid_size can be used as a
     * 1-byte ASN.1 length encoding and that there's no overflow. */
    if (8 + hashlen + oid_size  >= 0x80
        || 10 + hashlen            <  hashlen
        || 10 + hashlen + oid_size <  10 + hashlen)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    /*
     * Static bounds check:
     * - Need 10 bytes for five tag-length pairs.
     *   (Insist on 1-byte length encodings to protect against variants of
     *    Bleichenbacher's forgery attack against lax PKCS#1v1.5 verification)
     * - Need hashlen bytes for hash
     * - Need oid_size bytes for hash alg OID.
     */
    if (nb_pad < (10 + hashlen + oid_size))
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
    nb_pad -= 10 + hashlen + oid_size;
  }
  else
  {
    if (nb_pad < hashlen)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    nb_pad -= hashlen;
  }

  /* Need space for signature header and padding delimiter (3 bytes),
   * and 8 bytes for the minimal padding */
  if (nb_pad < (3 + 8))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
  nb_pad -= 3;

  /* Now nb_pad is the amount of memory to be filled
   * with padding, and at least 8 bytes long. */

  /* Write signature header and padding */
  *p++ = 0;
  *p++ = MBEDTLS_RSA_SIGN;
  memset(p, 0xFF, nb_pad);
  p += nb_pad;
  *p++ = 0;

  /* Are we signing raw data? */
  if (md_alg == MBEDTLS_MD_NONE)
  {
    memcpy(p, hash, hashlen);
    return 0;
  }

  /* Signing hashed data, add corresponding ASN.1 structure
   *
   * DigestInfo ::= SEQUENCE {
   *   digestAlgorithm DigestAlgorithmIdentifier,
   *   digest Digest }
   * DigestAlgorithmIdentifier ::= AlgorithmIdentifier
   * Digest ::= OCTET STRING
   *
   * Schematic:
   * TAG-SEQ + LEN [ TAG-SEQ + LEN [ TAG-OID  + LEN [ OID  ]
   *                                 TAG-NULL + LEN [ NULL ] ]
   *                 TAG-OCTET + LEN [ HASH ] ]
   */
  *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
  *p++ = (unsigned char)(0x08 + oid_size + hashlen);
  *p++ = MBEDTLS_ASN1_SEQUENCE | MBEDTLS_ASN1_CONSTRUCTED;
  *p++ = (unsigned char)(0x04 + oid_size);
  *p++ = MBEDTLS_ASN1_OID;
  *p++ = (unsigned char) oid_size;
  memcpy(p, oid, oid_size);
  p += oid_size;
  *p++ = MBEDTLS_ASN1_NULL;
  *p++ = 0x00;
  *p++ = MBEDTLS_ASN1_OCTET_STRING;
  *p++ = (unsigned char) hashlen;
  memcpy(p, hash, hashlen);
  p += hashlen;

  /* Just a sanity-check, should be automatic
   * after the initial bounds check. */
  if (p != (dst + dst_len))
  {
    mbedtls_platform_zeroize(dst, dst_len);
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  return 0;
}

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_rsassa_pkcs1_v15_sign(mbedtls_rsa_context *ctx,
                                      int (*f_rng)(void *, unsigned char *, size_t),
                                      void *p_rng,
                                      mbedtls_md_type_t md_alg,
                                      unsigned int hashlen,
                                      const unsigned char *hash,
                                      unsigned char *sig)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  unsigned char *sig_try = NULL;
  unsigned char *verif = NULL;

  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  if (ctx->padding != MBEDTLS_RSA_PKCS_V15)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /*
   * Prepare PKCS1-v1.5 encoding (padding and hash identifier)
   */

  if ((ret = rsa_rsassa_pkcs1_v15_encode(md_alg, hashlen, hash,
                                         ctx->len, sig)) != 0)
  {
    return ret;
  }

  /* Private key operation
   *
   * In order to prevent Lenstra's attack, make the signature in a
   * temporary buffer and check it before returning it.
   */

  sig_try = mbedtls_calloc(1, ctx->len);
  if (sig_try == NULL)
  {
    return MBEDTLS_ERR_MPI_ALLOC_FAILED;
  }

  verif = mbedtls_calloc(1, ctx->len);
  if (verif == NULL)
  {
    mbedtls_free(sig_try);
    return MBEDTLS_ERR_MPI_ALLOC_FAILED;
  }

  MBEDTLS_MPI_CHK(mbedtls_rsa_private(ctx, f_rng, p_rng, sig, sig_try));
  MBEDTLS_MPI_CHK(mbedtls_rsa_public(ctx, sig_try, verif));

  if (mbedtls_ct_memcmp(verif, sig, ctx->len) != 0)
  {
    ret = MBEDTLS_ERR_RSA_PRIVATE_FAILED;
    goto cleanup;
  }

  memcpy(sig, sig_try, ctx->len);

cleanup:
  mbedtls_zeroize_and_free(sig_try, ctx->len);
  mbedtls_zeroize_and_free(verif, ctx->len);

  if (ret != 0)
  {
    memset(sig, '!', ctx->len);
  }
  return ret;
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation to sign the message digest
 */
int mbedtls_rsa_pkcs1_sign(mbedtls_rsa_context *ctx,
                           int (*f_rng)(void *, unsigned char *, size_t),
                           void *p_rng,
                           mbedtls_md_type_t md_alg,
                           unsigned int hashlen,
                           const unsigned char *hash,
                           unsigned char *sig)
{
  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  switch (ctx->padding)
  {
#if defined(MBEDTLS_PKCS1_V15)
    case MBEDTLS_RSA_PKCS_V15:
      return mbedtls_rsa_rsassa_pkcs1_v15_sign(ctx, f_rng, p_rng, md_alg,
                                               hashlen, hash, sig);
#endif /* MBEDTLS_PKCS1_V15 */

#if defined(MBEDTLS_PKCS1_V21)
    case MBEDTLS_RSA_PKCS_V21:
      return mbedtls_rsa_rsassa_pss_sign(ctx, f_rng, p_rng, md_alg,
                                         hashlen, hash, sig);
#endif /* MBEDTLS_PKCS1_V21 */

    default:
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }
}

#if defined(MBEDTLS_PKCS1_V21)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify_ext(mbedtls_rsa_context *ctx,
                                      mbedtls_md_type_t md_alg,
                                      unsigned int hashlen,
                                      const unsigned char *hash,
                                      mbedtls_md_type_t mgf1_hash_id,
                                      int expected_salt_len,
                                      const unsigned char *sig)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
  size_t siglen = 0;
  unsigned char *p = NULL;
  unsigned char *hash_start = NULL;
  unsigned char result[MBEDTLS_MD_MAX_SIZE] = {0};
  unsigned int hlen = 0;
  size_t observed_salt_len = 0;
  size_t msb = 0;
  unsigned char buf[MBEDTLS_MPI_MAX_SIZE] = {0};
  size_t exp_hashlen = 0;

  if (((md_alg != MBEDTLS_MD_NONE) || (hashlen != 0)) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  siglen = ctx->len;

  if ((siglen < 16) || (siglen > sizeof(buf)))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  ret = mbedtls_rsa_public(ctx, sig, buf);

  if (ret != 0)
  {
    return ret;
  }

  p = buf;

  if (buf[siglen - 1] != 0xBC)
  {
    return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }

  if (md_alg != MBEDTLS_MD_NONE)
  {
    /* Gather length of hash to sign */
    exp_hashlen = mbedtls_md_get_size_from_type(md_alg);
    if (exp_hashlen == 0)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }

    if (hashlen != exp_hashlen)
    {
      return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
    }
  }

  hlen = mbedtls_md_get_size_from_type(mgf1_hash_id);
  if (hlen == 0)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /*
   * Note: EMSA-PSS verification is over the length of N - 1 bits
   */
  msb = mbedtls_mpi_bitlen(&ctx->N) - 1;

  if (buf[0] >> (8 - siglen * 8 + msb))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  /* Compensate for boundary condition when applying mask */
  if ((msb % 8) == 0)
  {
    p++;
    siglen -= 1;
  }

  if (siglen < (hlen + 2))
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }
  hash_start = p + siglen - hlen - 1;

  ret = mgf_mask(p, siglen - hlen - 1, hash_start, hlen, mgf1_hash_id);
  if (ret != 0)
  {
    return ret;
  }

  buf[0] &= 0xFF >> (siglen * 8 - msb);

  while (p < hash_start - 1 && *p == 0)
  {
    p++;
  }

  if (*p++ != 0x01)
  {
    return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }

  observed_salt_len = (size_t)(hash_start - p);

  if (expected_salt_len != MBEDTLS_RSA_SALT_LEN_ANY &&
      observed_salt_len != (size_t) expected_salt_len)
  {
    return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }

  /*
   * Generate H = Hash( M' )
   */
  ret = hash_mprime(hash, hashlen, p, observed_salt_len,
                    result, mgf1_hash_id);
  if (ret != 0)
  {
    return ret;
  }

  if (memcmp(hash_start, result, hlen) != 0)
  {
    return MBEDTLS_ERR_RSA_VERIFY_FAILED;
  }

  return 0;
}

/*
 * Simplified PKCS#1 v2.1 RSASSA-PSS-VERIFY function
 */
int mbedtls_rsa_rsassa_pss_verify(mbedtls_rsa_context *ctx,
                                  mbedtls_md_type_t md_alg,
                                  unsigned int hashlen,
                                  const unsigned char *hash,
                                  const unsigned char *sig)
{
  mbedtls_md_type_t mgf1_hash_id;
  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  mgf1_hash_id = (ctx->hash_id != MBEDTLS_MD_NONE)
                 ? (mbedtls_md_type_t) ctx->hash_id
                 : md_alg;

  return mbedtls_rsa_rsassa_pss_verify_ext(ctx,
                                           md_alg, hashlen, hash,
                                           mgf1_hash_id, MBEDTLS_RSA_SALT_LEN_ANY,
                                           sig);

}
#endif /* MBEDTLS_PKCS1_V21 */

#if defined(MBEDTLS_PKCS1_V15)
/*
 * Implementation of the PKCS#1 v2.1 RSASSA-PKCS1-v1_5-VERIFY function
 */
int mbedtls_rsa_rsassa_pkcs1_v15_verify(mbedtls_rsa_context *ctx,
                                        mbedtls_md_type_t md_alg,
                                        unsigned int hashlen,
                                        const unsigned char *hash,
                                        const unsigned char *sig)
{
  int ret = 0;
  size_t sig_len = 0;
  unsigned char *encoded = NULL;
  unsigned char *encoded_expected = NULL;

  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  sig_len = ctx->len;

  /*
   * Prepare expected PKCS1 v1.5 encoding of hash.
   */

  if ((encoded          = mbedtls_calloc(1, sig_len)) == NULL
      || (encoded_expected = mbedtls_calloc(1, sig_len)) == NULL)
  {
    ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
    goto cleanup;
  }
  ret = rsa_rsassa_pkcs1_v15_encode(md_alg, hashlen, hash, sig_len, encoded_expected);
  if (ret != 0)
  {
    goto cleanup;
  }

  /*
   * Apply RSA primitive to get what should be PKCS1 encoded hash.
   */

  ret = mbedtls_rsa_public(ctx, sig, encoded);
  if (ret != 0)
  {
    goto cleanup;
  }

  /*
   * Compare
   */
  ret = mbedtls_ct_memcmp(encoded, encoded_expected, sig_len);
  if (ret != 0)
  {
    ret = MBEDTLS_ERR_RSA_VERIFY_FAILED;
    goto cleanup;
  }

cleanup:

  if (encoded != NULL)
  {
    mbedtls_zeroize_and_free(encoded, sig_len);
  }

  if (encoded_expected != NULL)
  {
    mbedtls_zeroize_and_free(encoded_expected, sig_len);
  }

  return ret;
}
#endif /* MBEDTLS_PKCS1_V15 */

/*
 * Do an RSA operation and check the message digest
 */
int mbedtls_rsa_pkcs1_verify(mbedtls_rsa_context *ctx,
                             mbedtls_md_type_t md_alg,
                             unsigned int hashlen,
                             const unsigned char *hash,
                             const unsigned char *sig)
{
  if ((md_alg != MBEDTLS_MD_NONE || hashlen != 0) && hash == NULL)
  {
    return MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
  }

  switch (ctx->padding)
  {
#if defined(MBEDTLS_PKCS1_V15)
    case MBEDTLS_RSA_PKCS_V15:
      return mbedtls_rsa_rsassa_pkcs1_v15_verify(ctx, md_alg,
                                                 hashlen, hash, sig);
#endif /* MBEDTLS_PKCS1_V15 */

#if defined(MBEDTLS_PKCS1_V21)
    case MBEDTLS_RSA_PKCS_V21:
      return mbedtls_rsa_rsassa_pss_verify(ctx, md_alg,
                                           hashlen, hash, sig);
#endif /* MBEDTLS_PKCS1_V21 */

    default:
      return MBEDTLS_ERR_RSA_INVALID_PADDING;
  }
}

/*
 * Copy the components of an RSA key
 */
int mbedtls_rsa_copy(mbedtls_rsa_context *dst, const mbedtls_rsa_context *src)
{
  int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

  dst->len = src->len;

  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->N, &src->N));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->E, &src->E));

  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->D, &src->D));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->P, &src->P));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Q, &src->Q));

#if !defined(MBEDTLS_RSA_NO_CRT)
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->DP, &src->DP));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->DQ, &src->DQ));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->QP, &src->QP));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RP, &src->RP));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RQ, &src->RQ));
#endif /* !MBEDTLS_RSA_NO_CRT */

  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->RN, &src->RN));

  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Vi, &src->Vi));
  MBEDTLS_MPI_CHK(mbedtls_mpi_copy(&dst->Vf, &src->Vf));

  dst->padding = src->padding;
  dst->hash_id = src->hash_id;

cleanup:
  if (ret != 0)
  {
    mbedtls_rsa_free(dst);
  }

  return ret;
}

/*
 * Free the components of an RSA key
 */
void mbedtls_rsa_free(mbedtls_rsa_context *ctx)
{
  if (ctx == NULL)
  {
    return;
  }

  mbedtls_mpi_free(&ctx->Vi);
  mbedtls_mpi_free(&ctx->Vf);
  mbedtls_mpi_free(&ctx->RN);
  mbedtls_mpi_free(&ctx->D);
  mbedtls_mpi_free(&ctx->Q);
  mbedtls_mpi_free(&ctx->P);
  mbedtls_mpi_free(&ctx->Phi);
  mbedtls_mpi_free(&ctx->E);
  mbedtls_mpi_free(&ctx->N);

#if !defined(MBEDTLS_RSA_NO_CRT)
  mbedtls_mpi_free(&ctx->RQ);
  mbedtls_mpi_free(&ctx->RP);
  mbedtls_mpi_free(&ctx->QP);
  mbedtls_mpi_free(&ctx->DQ);
  mbedtls_mpi_free(&ctx->DP);
#endif /* MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_THREADING_C)
  /* Free the mutex, but only if it hasn't been freed already. */
  if (ctx->ver != 0)
  {
    mbedtls_mutex_free(&ctx->mutex);
    ctx->ver = 0;
  }
#endif /* MBEDTLS_THREADING_C */
}

#endif /* MBEDTLS_HAL_RSA_ALT */

#endif /* MBEDTLS_RSA_ALT */
#endif /* MBEDTLS_RSA_C */
