/**
 ******************************************************************************
 * @file    ble_audio_plat.h
 * @author  MCD Application Team
 * @brief   Bluetooth Low Energy Audio Stack Interface for Audio Stack
 *          integration.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef BLE_AUDIO_PLAT_H
#define BLE_AUDIO_PLAT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"

/* Defines ---------------------------------------------------------------------*/


/* Types ---------------------------------------------------------------------*/

/* Non Volatile Memory (NVM) interface:
 *
 * This interface is only called from BLE Audio stack context
 */
extern void BLE_AUDIO_PLAT_NvmStore( const uint32_t* ptr,uint32_t size );

/* Advanced Encryption Standard (AES) interface:
 */
extern void BLE_AUDIO_PLAT_AesEcbEncrypt( const uint8_t* key,
                                        const uint8_t* input,
                                        uint8_t* output );

extern void BLE_AUDIO_PLAT_AesCmacSetKey( const uint8_t* key );

extern void BLE_AUDIO_PLAT_AesCmacCompute( const uint8_t* input,
                                           uint32_t input_length,
                                           uint8_t* output_tag );

/* Random Number Generation (RNG) interface:
 */
extern void BLE_AUDIO_PLAT_RngGet( uint8_t n, uint32_t* val );

/* Timer interface:
 */
/**
 * @brief BLE_AUDIO_PLAT_TimerStart
 * This function is used to initialize and start the oneshot timer used by the BLE Audio Stack.
 *
 * @param pCallbackFunc : pointer to a function to call when the timer expires
 * @param pParam : param to pass in func when the timer expire
 * @param Timeout : time in milliseconds to wait before the timer expiration
 *
 */
extern void BLE_AUDIO_PLAT_TimerStart(void *pCallbackFunc, void *pParam, uint32_t Timeout);

/**
 * @brief BLE_AUDIO_PLAT_TimerStop
 * This function is used by the BLE Audio Stack to stop timer. This function
 * requests destruction of the timer created by BLE_AUDIO_PLAT_TimerStart()
 *
 */
extern void BLE_AUDIO_PLAT_TimerStop(void);

/**
 * @brief BLE_AUDIO_PLAT_TimerGetRemainMs
 * This function is used by the Audio Stack to get the remaining time
 * of the timer started thanks to the BLE_AUDIO_PLAT_TimerStart() function.
 *
 * @return remaining time in millisecond.
 */
extern uint32_t BLE_AUDIO_PLAT_TimerGetRemainMs(void);

/* Debug Trace interface:
 */
/**
 * @brief BLE_AUDIO_PLAT_DbgLog
 * This function is used by the Audio Stack to print some Debug Traces
 *
 * @param format: a list of printable arguments
 *
 */
extern void BLE_AUDIO_PLAT_DbgLog(char *format,...);

#ifdef __cplusplus
}
#endif

#endif /*BLE_AUDIO_PLAT_H*/
