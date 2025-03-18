/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee.c
  * @author  MCD Application Team
  * @brief   Zigbee Direct  Application
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
/* USER CODE END Header */

#include <assert.h>
#include <stdarg.h>

#include "app_common.h"
#include "app_entry.h"
#include "stm32_rtos.h"
#include "app_zigbee.h"
#include "app_zigbee_endpoint.h"
#include "app_conf.h"
#include "stm32_adv_trace.h"
#include "main.h"

#include "zigbee.h"
#include "zigbee.zd.h"
#include "zcl/zcl.h"

/* NOTE: most of these are dummy values */
static const struct ZbZclBasicServerDefaults basic_server_defaults = {
    0x01, /* ZCL_BASIC_ATTR_APP_VERSION */
    0x02, /* ZCL_BASIC_ATTR_STACK_VERSION */
    0x03, /* ZCL_BASIC_ATTR_HARDWARE_VERSION */
    {7, 'S', 'T', 'M', 'I', 'C', 'R','O'}, /* ZCL_BASIC_ATTR_MFR_NAME */
    {0}, /* ZCL_BASIC_ATTR_MODEL_NAME */
    {0}, /* ZCL_BASIC_ATTR_DATE_CODE */
    0x01, /* ZCL_BASIC_ATTR_POWER_SOURCE (ZCL_BASIC_POWER_SINGLE_PHASE) */
    {0}, /* ZCL_BASIC_ATTR_SW_BUILD_ID */
};



#ifdef COND_USE_PERSISTENCE
static void APP_ZIGBEE_App_Persist_Cb(struct ZigBeeT *zb, void *cbarg);
static bool persist_save(struct ZigBeeT *zb);
static const void * persist_load(unsigned int *bufLen);
#endif

extern void cli_log_zigbee_callback(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argp);

void APP_ZIGBEE_StackLayersInit(void)
{
    struct ZbInitTblSizesT tableSizes;
    struct ZbInitSetLoggingT logging;
#ifdef COND_USE_PERSISTENCE
    unsigned int persist_len;
    const uint8_t *persist_data;
#endif

    /* Initialize the Zigbee stack infrastructure */
    APP_ZIGBEE_Init();

    /* -- Initialize UART -- */
    UTIL_ADV_TRACE_StartRxProcess(APP_ZIGBEE_App_UartRxCallback);

    /*-----------------------------------------------------
     * Initialize Zigbee Application
     *-----------------------------------------------------
     */
    memset(&logging, 0, sizeof(logging));
    logging.mask = 0;
    logging.mask |= ZB_LOG_MASK_FATAL;
    logging.mask |= ZB_LOG_MASK_ERROR;
    logging.mask |= ZB_LOG_MASK_INFO;
    logging.mask |= ZB_LOG_MASK_DEBUG;
    logging.mask |= ZB_LOG_MASK_NWK_SECURITY;
    logging.mask |= ZB_LOG_MASK_ZCL;
    logging.func = cli_log_zigbee_callback;

    memset(&tableSizes, 0, sizeof(tableSizes));
    tableSizes.heapPtr = NULL;
    tableSizes.heapSz = 0U;

    app_info.zb = ZbInit(0U, &tableSizes, &logging);
    if (app_info.zb == NULL) {
        APP_LOG_PRINTF("Error, ZbInit failed");
        return;
    }

    app_info.eui64 = ZbExtendedAddress(app_info.zb);
    if (app_info.eui64 == 0U) {
        APP_LOG_PRINTF("Error, ZbExtendedAddress returned 0");
        return;
    }

#ifdef COND_USE_PERSISTENCE
    if (!ZbPersistNotifyRegister(app_info.zb, APP_ZIGBEE_App_Persist_Cb, NULL)) {
        APP_LOG_PRINTF("Error, ZbPersistNotifyRegister failed");
        return;
    }
#endif

    /* Configure Basic Server defaults */
    ZbZclBasicServerConfigDefaults(app_info.zb, &basic_server_defaults);
}


#ifdef COND_USE_PERSISTENCE
static void APP_ZIGBEE_App_Persist_Cb(struct ZigBeeT *zb, void *cbarg)
{
    app_info.savePersistence = true;
    /* EXEGIN - kick task? */
}

#endif



