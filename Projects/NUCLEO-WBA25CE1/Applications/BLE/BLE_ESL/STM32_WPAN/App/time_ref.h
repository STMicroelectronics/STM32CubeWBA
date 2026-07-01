/**
  ******************************************************************************
  * @file         time_ref.h
  * @brief        Header file of TIME_REF library.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef TIME_REF_H
#define TIME_REF_H

/**
 * @brief Set the current absolute time in milliseconds.
 * @param absolute_time The current absolute time (ms).
 *
 */
void TIMEREF_SetAbsoluteTime(uint32_t absolute_time);

/**
 * @brief Get the current absolute time.
 * @retval The current absolute time (ms).
 *
 */
uint32_t TIMEREF_GetCurrentAbsTime(void);

#endif /* TIME_REF_H */