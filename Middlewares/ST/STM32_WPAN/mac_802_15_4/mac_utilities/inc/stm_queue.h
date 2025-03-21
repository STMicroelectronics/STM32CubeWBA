/**
  ******************************************************************************
  * @file    stm_queue.h
  * @author  MCD Application Team
  * @brief   Header for stm_queue.c
  ******************************************************************************
   * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the 
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
 */



/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM_QUEUE_H
#define __STM_QUEUE_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* Exported define -----------------------------------------------------------*/
/* Basic definitions */
#undef NULL
#define NULL                    0

#undef FALSE
#define FALSE                   0

#undef TRUE
#define TRUE                    (!0)

/* Some useful macro definitions */
#undef MAX
#define MAX( x, y )          (((x)>(y))?(x):(y))

#undef MIN
#define MIN( x, y )          (((x)<(y))?(x):(y))

/* Options flags */
#define CIRCULAR_QUEUE_NO_FLAG 0
#define CIRCULAR_QUEUE_NO_WRAP_FLAG 1
#define CIRCULAR_QUEUE_SPLIT_IF_WRAPPING_FLAG 2


/* Exported types ------------------------------------------------------------*/
typedef struct {
   uint8_t* qBuff;        /* queue buffer, , provided by init fct */
   uint32_t queueMaxSize;   /* size of the queue, provided by init fct (in bytes)*/
   uint16_t elementSize;    /* -1 variable. If variable element size the size is stored in the 4 first of the queue element */
   uint32_t first;          /* position of first element */
   uint32_t last;           /* position of last element */
   uint32_t byteCount;      /* number of bytes in the queue */
   uint32_t elementCount;   /* number of element in the queue */
   uint8_t  optionFlags;     /* option to enable specific features */
} stm_wbl_queue_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
int CircularQueue_Init(stm_wbl_queue_t *q, uint8_t* queueBuffer, uint32_t queueSize, uint16_t elementSize, uint8_t optionlags);
uint8_t* CircularQueue_Add(stm_wbl_queue_t *q, uint8_t* x, uint16_t elementSize, uint32_t nbElements);
uint8_t* CircularQueue_Remove(stm_wbl_queue_t *q, uint16_t* elementSize);
uint8_t* CircularQueue_Sense(stm_wbl_queue_t *q, uint16_t* elementSize);
int CircularQueue_Empty(stm_wbl_queue_t *q);
int CircularQueue_NbElement(stm_wbl_queue_t *q);
uint8_t* CircularQueue_Remove_Copy(stm_wbl_queue_t *q, uint16_t* elementSize, uint8_t* buffer);
uint8_t* CircularQueue_Sense_Copy(stm_wbl_queue_t *q, uint16_t* elementSize, uint8_t* buffer);

/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
#endif /* __STM_QUEUE_H */
