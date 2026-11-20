/**
  ******************************************************************************
  * @file    mbedtls_threading_alt.c
  * @author  MCD Application Team
  * @brief   mbedTLS threading alternate backend for PSA integration
  ******************************************************************************
  */

#include "mbedtls_threading_alt.h"

#include "common_wpan_conf.h"

#if defined(CFG_USE_FREERTOS)
#include "FreeRTOS.h"  
#include "semphr.h"
#elif defined(CFG_USE_THREADX)
#include "tx_api.h"
#endif

#if defined(MBEDTLS_THREADING_C) && defined(MBEDTLS_THREADING_ALT)

#include "stm32wbaxx.h"
    
#if defined(CFG_USE_FREERTOS)
static void MBEDTLS_THREADING_ALT_MutexInit(mbedtls_threading_mutex_t *mutex)
{
  if (NULL != mutex)
  {
    mutex->mutex = NULL;
    mutex->is_valid = 0u;

    mutex->mutex = xSemaphoreCreateMutex();
    if (NULL != mutex->mutex)
    {
      mutex->is_valid = 1u;
    }
  }
}

static void MBEDTLS_THREADING_ALT_MutexFree(mbedtls_threading_mutex_t *mutex)
{
  if ((NULL != mutex) && (NULL != mutex->mutex) && (mutex->is_valid != 0u))
  {
    vSemaphoreDelete(mutex->mutex);
    
    mutex->mutex = NULL;
    mutex->is_valid = 0u;
  }
}

static int MBEDTLS_THREADING_ALT_MutexLock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u) || (mutex->mutex == NULL))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  if (xSemaphoreTake(mutex->mutex, portMAX_DELAY) == pdTRUE)
  {
    return 0;
  }
  
  return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

static int MBEDTLS_THREADING_ALT_MutexUnlock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u) || (mutex->mutex == NULL))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  if (xSemaphoreGive(mutex->mutex) == pdTRUE)
  {
    return 0;
  }

  return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

#elif defined(CFG_USE_THREADX)
extern TX_BYTE_POOL  * pBytePool;

static void MBEDTLS_THREADING_ALT_MutexInit(mbedtls_threading_mutex_t *mutex)
{
  if (NULL != mutex)
  {
    mutex->is_valid = 0u;
    
    if (tx_byte_allocate(pBytePool, (void **)&mutex->mutex, sizeof(TX_MUTEX), TX_NO_WAIT) == TX_SUCCESS)
    { 
      if (tx_mutex_create((TX_MUTEX *)mutex->mutex, "mbedtls", TX_INHERIT) == TX_SUCCESS)
      {
        mutex->is_valid = 1u;
      }
      else
      {
        (void)tx_byte_release(mutex->mutex);
      }
    }
    
    if (mutex->is_valid == 0u)
    {
      mutex->mutex = NULL;
    }
  }
}

static void MBEDTLS_THREADING_ALT_MutexFree(mbedtls_threading_mutex_t *mutex)
{
  if ((NULL != mutex) && (mutex->is_valid != 0u))
  {
    (void) tx_mutex_delete((TX_MUTEX *)mutex->mutex);
    mutex->is_valid = 0u;
    
    (void)tx_byte_release(mutex->mutex);
    mutex->mutex = NULL;
  }
}

static int MBEDTLS_THREADING_ALT_MutexLock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  if (tx_mutex_get((TX_MUTEX *)mutex->mutex, TX_WAIT_FOREVER) == TX_SUCCESS)
  {
    return 0;
  }
    
  return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

static int MBEDTLS_THREADING_ALT_MutexUnlock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }
  
  if (tx_mutex_put((TX_MUTEX *)mutex->mutex) == TX_SUCCESS)
  {
    return 0;
  }

  return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
}

#else

static void MBEDTLS_THREADING_ALT_MutexInit(mbedtls_threading_mutex_t *mutex)
{
  if (NULL != mutex)
  {
    mutex->mutex = NULL;
    mutex->is_valid = 0u;
    
    mutex->mutex = calloc(1, sizeof(uint8_t));
    
    if (NULL != mutex->mutex)
    {
      *((uint8_t *)(mutex->mutex)) = 0u;
      mutex->is_valid = 1u;
    }
  }
}

static void MBEDTLS_THREADING_ALT_MutexFree(mbedtls_threading_mutex_t *mutex)
{
  if ((NULL != mutex) && (NULL != mutex->mutex) && (mutex->is_valid != 0u))
  {
    free(mutex->mutex);
    mutex->mutex = NULL;
    mutex->is_valid = 0u;
  }
}

static int MBEDTLS_THREADING_ALT_MutexLock(mbedtls_threading_mutex_t *mutex)
{
  uint32_t primask;
  
  if ((mutex == NULL) || (mutex->is_valid == 0u) || (mutex->mutex == NULL))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  primask = __get_PRIMASK();
  __disable_irq();

  if (*((uint8_t *)(mutex->mutex)) != 0u)
  {
    __set_PRIMASK(primask);
    return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }

  *((uint8_t *)(mutex->mutex)) = 1u;

  __set_PRIMASK(primask);

  return 0;
}

static int MBEDTLS_THREADING_ALT_MutexUnlock(mbedtls_threading_mutex_t *mutex)
{
  uint32_t primask;
  
  if ((mutex == NULL) || (mutex->is_valid == 0u) || (mutex->mutex == NULL))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  primask = __get_PRIMASK();
  __disable_irq();

  if (*((uint8_t *)(mutex->mutex)) == 0u)
  {
    __set_PRIMASK(primask);
    return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
  }

  *((uint8_t *)(mutex->mutex)) = 0u;

  __set_PRIMASK(primask);

  return 0;
}

#endif

void MBEDTLS_THREADING_ALT_EarlyMutexInit(mbedtls_threading_mutex_t *mutex)
{
  if (NULL != mutex)
  {
    mutex->is_valid = 1u;
  }
}

void MBEDTLS_THREADING_ALT_EarlyMutexFree(mbedtls_threading_mutex_t *mutex)
{
  if ((NULL != mutex) && (mutex->is_valid != 0u))
  {
    mutex->is_valid = 0u;
  }
}

int MBEDTLS_THREADING_ALT_EarlyMutexLock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }

  return 0;
}

int MBEDTLS_THREADING_ALT_EarlyMutexUnlock(mbedtls_threading_mutex_t *mutex)
{
  if ((mutex == NULL) || (mutex->is_valid == 0u))
  {
    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
  }
  
  return 0;
}

void MBEDTLS_THREADING_ALT_EarlyInit(void)
{
  mbedtls_threading_set_alt(MBEDTLS_THREADING_ALT_EarlyMutexInit,
                            MBEDTLS_THREADING_ALT_EarlyMutexFree,
                            MBEDTLS_THREADING_ALT_EarlyMutexLock,
                            MBEDTLS_THREADING_ALT_EarlyMutexUnlock);
}

void MBEDTLS_THREADING_ALT_EarlyDeInit(void)
{
  mbedtls_threading_free_alt();
}

void MBEDTLS_THREADING_ALT_Init(void)
{
  mbedtls_threading_set_alt(MBEDTLS_THREADING_ALT_MutexInit,
                            MBEDTLS_THREADING_ALT_MutexFree,
                            MBEDTLS_THREADING_ALT_MutexLock,
                            MBEDTLS_THREADING_ALT_MutexUnlock);
}

void MBEDTLS_THREADING_ALT_DeInit(void)
{
  mbedtls_threading_free_alt();
}

#else

void MBEDTLS_THREADING_ALT_Init(void)
{
}

void MBEDTLS_THREADING_ALT_DeInit(void)
{
}

void MBEDTLS_THREADING_ALT_EarlyInit(void)
{
}

void MBEDTLS_THREADING_ALT_EarlyDeInit(void)
{
}

#endif /* MBEDTLS_THREADING_C && MBEDTLS_THREADING_ALT */
