#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

#include "mbedtls/threading.h"

#ifdef __cplusplus
extern "C" {
#endif

void MBEDTLS_THREADING_ALT_EarlyInit(void);
void MBEDTLS_THREADING_ALT_EarlyDeInit(void);
void MBEDTLS_THREADING_ALT_Init(void);
void MBEDTLS_THREADING_ALT_DeInit(void);
void MBEDTLS_THREADING_ALT_EarlyMutexInit(mbedtls_threading_mutex_t *mutex);
void MBEDTLS_THREADING_ALT_EarlyMutexFree(mbedtls_threading_mutex_t *mutex);
int MBEDTLS_THREADING_ALT_EarlyMutexLock(mbedtls_threading_mutex_t *mutex);
int MBEDTLS_THREADING_ALT_EarlyMutexUnlock(mbedtls_threading_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_THREADING_ALT_H */
