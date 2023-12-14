/*
 * app_thread.h
 *
 *  Created on: May 6, 2021
 *      Author: frq02098
 */

#ifndef APP_THREAD_H
#define APP_THREAD_H
#include <stdint.h>

void Thread_Init(void);
void ProcessLinkLayer(void);
void ProcessTasklets(void);
void ProcessAlarm(void);
void ProcessUsAlarm(void);
void ProcessOpenThreadTasklets(void);

void APP_THREAD_Init(void);
void APP_THREAD_ScheduleUART(void);
void APP_THREAD_Error(uint32_t ErrId, uint32_t ErrCode);


#define APP_READ32_REG(base_addr) \
    (*(volatile uint32_t *)(base_addr))
#define APP_WRITE32_REG(base_addr, data) \
      (*(volatile uint32_t *)(base_addr) = (data))

/*
 *  List of all errors tracked by the Thread application
 *  running on M4. Some of these errors may be fatal
 *  or just warnings
 */
typedef enum
{
  ERR_REC_MULTI_MSG_FROM_M0,
  ERR_THREAD_SET_STATE_CB,
  ERR_THREAD_SET_CHANNEL,
  ERR_THREAD_SET_PANID,
  ERR_THREAD_SET_THRESHOLD,
  ERR_THREAD_SET_MASTERKEY,
  ERR_THREAD_IPV6_ENABLE,
  ERR_THREAD_START,
  ERR_THREAD_ERASE_PERSISTENT_INFO,
/* USER CODE BEGIN ERROR_APPLI_ENUM */

/* USER CODE END ERROR_APPLI_ENUM */
  ERR_THREAD_CHECK_WIRELESS
  } ErrAppliIdEnum_t;


#endif /* APP_THREAD_H */
