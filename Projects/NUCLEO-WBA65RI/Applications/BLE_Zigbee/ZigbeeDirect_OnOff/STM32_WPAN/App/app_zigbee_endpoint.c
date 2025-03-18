/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee_endpoint.c
  * @author  MCD Application Team
  * @brief   Zigbee Direct  Application
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "app_zigbee_debug_zd.h"
#include "app_conf.h"
#include "stm32_adv_trace.h"
#include "stm32wbaxx_nucleo.h" /* LEDs */
#include "main.h"

#include "zigbee.h"
#include "zigbee.zd.h"
#include "zcl/zcl.h"
#include "zcl/general/zcl.onoff.h"
#include "zcl/general/zcl.identify.h"


/* #define COND_USE_PERSISTENCE */

#define APP_ENDPOINT                        1U

#define APP_ONOFF_LED                       LED_RED
#define APP_RADIO_LED                       LED_GREEN
#if (CFG_USB_INTERFACE_ENABLE != 0)
# error "CFG_USB_INTERFACE_ENABLE is not supported by this application"
#endif


struct app_info_t app_info;

static const char *cli_prompt_str = "zdd > ";

#ifdef COND_USE_PERSISTENCE
static void APP_ZIGBEE_App_Persist_Cb(struct ZigBeeT *zb, void *cbarg);
#endif

void cli_log_zigbee_callback(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argp);
static void cli_port_print_msg(const char *pMessage);
static void cli_port_print_prompt(bool bSendCr);
static void APP_ZIGBEE_App_Task(void);
static void APP_ZIGBEE_App_Kick(void);



#ifdef COND_USE_PERSISTENCE
static bool persist_save(struct ZigBeeT *zb);
static const void * persist_load(unsigned int *bufLen);
#endif

static enum ZclStatusCodeT app_onoff_cb_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT app_onoff_cb_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static enum ZclStatusCodeT app_onoff_cb_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg);
static void app_identify_cb_identify(struct ZbZclClusterT *cluster, enum ZbZclIdentifyServerStateT state, void *arg);


void APP_ZIGBEE_ApplicationInit(void)
{

#ifdef COND_USE_PERSISTENCE
    unsigned int persist_len;
    const uint8_t *persist_data;
#endif
    struct ZbApsmeAddEndpointReqT add_ep_req;
    struct ZbApsmeAddEndpointConfT add_ep_conf;
    uint16_t inputClusterList[1] = {
        ZCL_CLUSTER_ONOFF
    };
    struct ZbZclOnOffServerCallbacksT onoff_callbacks;
    struct zb_zdd_config_t zdd_config;

    /* -- Register Task -- */
    UTIL_SEQ_RegTask(1u << CFG_TASK_ZIGBEE_APP1, 0x00, APP_ZIGBEE_App_Task);


    APP_ZIGBEE_StackLayersInit();

    /*-----------------------------------------------------
     * Endpoint and OnOff Cluster
     *-----------------------------------------------------
     */
    memset(&add_ep_req, 0, sizeof(add_ep_req));
    add_ep_req.endpoint = APP_ENDPOINT;
    add_ep_req.profileId = ZCL_PROFILE_HOME_AUTOMATION;
    add_ep_req.deviceId = ZCL_DEVICE_ONOFF_OUTPUT;
    add_ep_req.version = 1;
    add_ep_req.inputClusterCount = 1;
    add_ep_req.inputClusterList = inputClusterList;
    add_ep_req.outputClusterCount = 0;
    add_ep_req.outputClusterList = NULL;
    add_ep_req.bdbCommissioningGroupID = DEFAULT_EP_BDB_COMMISSION_GRP_ID;
    ZbZclAddEndpoint(app_info.zb, &add_ep_req, &add_ep_conf);

    memset(&onoff_callbacks, 0, sizeof(onoff_callbacks));
    onoff_callbacks.off = app_onoff_cb_off;
    onoff_callbacks.on = app_onoff_cb_on;
    onoff_callbacks.toggle = app_onoff_cb_toggle;
    app_info.onoff_server = ZbZclOnOffServerAlloc(app_info.zb, APP_ENDPOINT, &onoff_callbacks, NULL);
    if (app_info.onoff_server == NULL) {
        APP_LOG_PRINTF("Error, ZbZclOnOffServerAlloc failed");
        return;
    }

#ifdef COND_USE_PERSISTENCE
    /*-----------------------------------------------------
     * Persistence
     *-----------------------------------------------------
     */
    APP_LOG_PRINTF("Reading persistence data from Flash");
    persist_len = 0;
    persist_data = persist_load(&persist_len);
    APP_LOG_PRINTF("Persistence data length = %d", persist_len);
    /* EXEGIN - call ZbStartupPersist */
    (void)persist_len;
    (void)persist_data;
#endif

    /*-----------------------------------------------------
     * Zigbee Direct ZDD
     *-----------------------------------------------------
     */
    APP_LOG_PRINTF("Enabling ZDD");
    memset(&zdd_config, 0, sizeof(zdd_config));
    zdd_config.open_tunnel = true;
    zdd_config.zdd_server_endpt = APP_ENDPOINT;
    zdd_config.identify_callbacks.identify = app_identify_cb_identify;
    zdd_config.identify_callbacks.trigger_effect = NULL;
    if (!zb_zdd_init(app_info.zb, &zdd_config)) {
        APP_LOG_PRINTF("Error, zb_zdd_init failed");
        return;
    }

    /* Make sure the OnOff LED is off */
    /* APP_LED_OFF(APP_ONOFF_LED); */
    app_onoff_cb_off(app_info.onoff_server, NULL, NULL);

    APP_LOG_PRINTF("Initialization of ZDD Application complete.\n");

    /* Print the zbcli prompt */
    cli_port_print_prompt(false);
}

/**
 * @brief   Reception of UART data until a CR occurs.
 *
 * @param   pData   Received character(s).
 * @param   iSize   Number of received characters
 * @param   cError  USART error transmission during reception.
 */
void APP_ZIGBEE_App_UartRxCallback(uint8_t *pData, uint16_t iSize, uint8_t cError)
{
    uint8_t cData;
    uint16_t iIndex = 0x00;
    struct zbcli_buf_t *bufp = &app_info.buf_ring[app_info.buf_wr_idx];

    /* -- Copy received Data until UserBuffer Overflow of 'CR' Reception -- */
    do {
        cData = pData[iIndex++];

        if (bufp->status == BUF_STATUS_READY) {
            /* All buffers full */
            break;
        }

        if (bufp->status == BUF_STATUS_OVERFLOW) {
            if (cData == (uint8_t)'\r') {
                bufp->status = BUF_STATUS_EMPTY;
            }
            break;
        }

        if (cData == (uint8_t)'\r') {
            if (bufp->len > 0) {
                /* Flag this buffer as ready and move to the next buffer in the ring. */
                bufp->buf[bufp->len] = 0x00;
                bufp->status = BUF_STATUS_READY;
                app_info.buf_wr_idx = (app_info.buf_wr_idx + 1U) % CLI_CMD_QUEUE_SZ;
                bufp = &app_info.buf_ring[app_info.buf_wr_idx];
                APP_ZIGBEE_App_Kick();
            }
        }
        else if (cData == (uint8_t)'\n') {
            /* Skip newline characters */
        }
        else if (cData == (uint8_t)'\b') {
            /* Handle backspaces */
            if (bufp->len > 0) {
                bufp->len--;
            }
        }
        /* EXEGIN - handle up-arrow for history?  1b '.', 5b '[', 41 'A' */
        else {
            bufp->buf[bufp->len++] = cData;
            if (bufp->len >= CLI_MAX_LINE_LEN) {
                /* Overflow. Erase this command and wait for terminating end-of-line. */
                bufp->status = BUF_STATUS_OVERFLOW;
                bufp->len = 0;
            }
        }
    } while (iIndex < iSize);
}


#ifdef COND_USE_PERSISTENCE
static void
APP_ZIGBEE_App_Persist_Cb(struct ZigBeeT *zb, void *cbarg)
{
    app_info.savePersistence = true;
    /* EXEGIN - kick task? */
}

#endif

static void APP_ZIGBEE_App_Task(void)
{
    struct zbcli_buf_t *bufp = &app_info.buf_ring[app_info.buf_rd_idx];

#ifdef COND_USE_PERSISTENCE
    /* Check if we need to save persistence */
    if (app_info.savePersistence) {
        app_info.savePersistence = false;
        if (!persist_save(app_info.zb)) {
            /* EXEGIN handle error? */
        }
    }
#endif

    /* Check for user command */
    if (bufp->status == BUF_STATUS_READY) {
        const char *cmd_str = (const char *)bufp->buf;

        if (strcmp(cmd_str, "help") == 0) {
            APP_LOG_PRINTF("Available commands total:");
            APP_LOG_PRINTF("   adv      ; re-enable BLE advertisements");
            APP_LOG_PRINTF("   help     ; print this help");
            APP_LOG_PRINTF("   on       ; Turn OnOff cluster LED ON");
            APP_LOG_PRINTF("   off      ; Turn OnOff cluster LED OFF");
            APP_LOG_PRINTF("   pjoin    ; enable permit-join via ZDO broadcast");
            APP_LOG_PRINTF("   status   ; print status");
            APP_LOG_PRINTF("   swreset  ; board reset");
            APP_LOG_PRINTF("   whoAmI   ; simpleDesReq");
            APP_LOG_PRINTF("   match    ; match");
        }
        else if (strcmp(cmd_str, "adv") == 0) {
            APP_LOG_PRINTF("Enabling BLE advertisements");
            zb_zdd_update_advert(app_info.zb, true);
        }
        else if (strcmp(cmd_str, "on") == 0) {
            app_onoff_cb_on(app_info.onoff_server, NULL, NULL);
        }
        else if (strcmp(cmd_str, "off") == 0) {
            app_onoff_cb_off(app_info.onoff_server, NULL, NULL);
        }
        else if (strcmp(cmd_str, "pjoin") == 0) {
            struct ZbZdoPermitJoinReqT req;
            enum ZbStatusCodeT status;

            memset(&req, 0, sizeof(req));
            req.destAddr = ZB_NWK_ADDR_BCAST_ROUTERS;
            req.duration = 180;

            APP_LOG_PRINTF("Enabling Permit-Join for 180 seconds via ZDO broadcast");
            status = ZbZdoPermitJoinReq(app_info.zb, &req, NULL, NULL);
            if (status != ZB_STATUS_SUCCESS) {
                APP_LOG_PRINTF("Error, Permit-Join request failed (status = 0x%02x)", status);
            }
        }
        else if (strcmp(cmd_str, "status") == 0) {
            cli_status_zdo();
            cli_print_aps_channel_mask();
            cli_status_nwk();
            cli_status_nnt();
            cli_security_dump();
            zdd_sec_print_session(app_info.zb);

            /* OnOff Cluster State */
            APP_LOG_PRINTF("");
            APP_LOG_PRINTF("On/Off state = %s", app_info.onoff_state ? "on" : "off");
        }
        else if (strcmp(cmd_str, "swreset") == 0) {
            HAL_NVIC_SystemReset();
        }
        else {
            APP_LOG_PRINTF("Error, unknown command: %s", cmd_str);
        }

        /* We're done with this buffer. Clear it and move to the next one. */
        bufp->len = 0;
        bufp->status = BUF_STATUS_EMPTY;
        app_info.buf_rd_idx = (app_info.buf_rd_idx + 1U) % CLI_CMD_QUEUE_SZ;

        /* Print a new prompt */
        cli_port_print_prompt(false);
    }
}
/*-----------------------------------------------------------------------------
 * OnOff Server Callbacks
 *-----------------------------------------------------------------------------
 */
static enum ZclStatusCodeT app_onoff_cb_off(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
    APP_LOG_PRINTF("");
    APP_LOG_PRINTF("OFF Command (was = %s)", app_info.onoff_state ? "on" : "off");
    APP_LOG_PRINTF("");

    APP_LED_OFF(APP_ONOFF_LED);
    ZbZclAttrIntegerWrite(app_info.onoff_server, ZCL_ONOFF_ATTR_ONOFF, 0);

    if (!app_info.onoff_state) {
        /* Already off */
        return ZCL_STATUS_SUCCESS;
    }
    app_info.onoff_state = false;
    return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT app_onoff_cb_on(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
    APP_LOG_PRINTF("");
    APP_LOG_PRINTF("ON Command (was = %s)", app_info.onoff_state ? "on" : "off");
    APP_LOG_PRINTF("");

    APP_LED_ON(APP_ONOFF_LED);
    ZbZclAttrIntegerWrite(app_info.onoff_server, ZCL_ONOFF_ATTR_ONOFF, 1);

    if (app_info.onoff_state) {
        /* Already on */
        return ZCL_STATUS_SUCCESS;
    }
    app_info.onoff_state = true;
    return ZCL_STATUS_SUCCESS;
}

static enum ZclStatusCodeT app_onoff_cb_toggle(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *srcInfo, void *arg)
{
    APP_LOG_PRINTF("");
    APP_LOG_PRINTF("TOGGLE Command (was = %s)", app_info.onoff_state ? "on" : "off");
    APP_LOG_PRINTF("");

    if (app_info.onoff_state) {
        app_info.onoff_state = false;
        APP_LED_OFF(APP_ONOFF_LED);
        ZbZclAttrIntegerWrite(app_info.onoff_server, ZCL_ONOFF_ATTR_ONOFF, 0);
    }
    else {
        app_info.onoff_state = true;
        APP_LED_ON(APP_ONOFF_LED);
        ZbZclAttrIntegerWrite(app_info.onoff_server, ZCL_ONOFF_ATTR_ONOFF, 1);
    }
    return ZCL_STATUS_SUCCESS;
}

/*-----------------------------------------------------------------------------
 * Identify Server Callbacks
 *-----------------------------------------------------------------------------
 */
static void app_identify_cb_identify(struct ZbZclClusterT *cluster, enum ZbZclIdentifyServerStateT state, void *arg)
{
    APP_LOG_PRINTF("IDENTIFY Command (%s)", (state == ZCL_IDENTIFY_START) ? "start" : "stop");
}

#ifdef COND_USE_PERSISTENCE
/*-----------------------------------------------------------------------------
 * Persistence
 *-----------------------------------------------------------------------------
 */
static bool
persist_save(struct ZigBeeT *zb)
{
    uint32_t len;
    uint8_t *buf_p;

    /* clear the RAM cache before saving */
    APP_DBG("Clear cache");
    APP_ZIGBEE_Persistence_ClearCache();

    /* get length to store */
    len = ZbPersistGet(zb, NULL, 0);

    if (len == 0U) {
        /* if the persistence length was zero then no data available. */
        APP_DBG("APP_ZIGBEE_persist_save: no persistence data to save !");
        return true;
    }
    if (len > (uint32_t)APP_ZIGBEE_Persistence_GetCacheSize()) {
        APP_DBG("APP_ZIGBEE_persist_save: persist size too large for storage (%d)", len);
        return false;
    }

    /* get pointer to RAM persistent data buffer */
    buf_p = APP_ZIGBEE_Persistence_GetCacheBuffer();

    /* populate buffer */
    len = ZbPersistGet(zb, buf_p, len);

    /* store data length in cache */
    APP_ZIGBEE_Persistence_SetLength((uint16_t)(len & 0xFFFF));

    /* and finally save */
    if (APP_ZIGBEE_Persistence_Save() == false) {
        return false;
    }

#ifdef CFG_PERSISTENCE_ENABLED
    APP_DBG("APP_persistence_Save: Persistent data saved in FLASH (%d bytes)", len);
#else
    APP_DBG("APP_persistence_Save: Persistent data saved in RAM (%d bytes)", len);
#endif

    return true;
}

static const void * persist_load(unsigned int *bufLen)
{
#ifdef CFG_PERSISTENCE_ENABLED
    APP_DBG("Retrieving persistent data from FLASH");
#else
    APP_DBG("Retrieving persistent data from RAM cache");
#endif

    /* load persistent data */
    if (APP_ZIGBEE_Persistence_Load() == false) {
        *bufLen = 0;
        return NULL;
    }
    else {
        /* returns pointer to RAM buffer and length of retrieved data */
        *bufLen = APP_ZIGBEE_Persistence_GetLength();
        return APP_ZIGBEE_Persistence_GetCacheBuffer();
    }
}

#endif

static void APP_ZIGBEE_App_Kick(void)
{
    UTIL_SEQ_SetTask(1u << CFG_TASK_ZIGBEE_APP1, CFG_SEQ_PRIO_1);
}

/*-----------------------------------------------------------------------------
 * Logging
 *-----------------------------------------------------------------------------
 */
#define LOG_MSG_SZ_MAX                      256U

static char log_msg_str[LOG_MSG_SZ_MAX];
// TODO 
void cli_log_zigbee_callback(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argp)
{
    (void)zb;
    (void)mask;
    int len = 0;

#if 0 /* needed? */
    if (cli_p->args->log_timestamp) {
        /* Get the relative timestamp for this message */
        unsigned int secs, ms;

        ms = cli_port_uptime_ms();
        secs = ms / 1000;
        ms = ms % 1000;

        /* Format the message */
        len += snprintf(&log_msg_str[len], LOG_MSG_SZ_MAX - (size_t)len, "%03d.%03d ", secs, ms);
    }
#endif

    if (fmt != NULL) {
        if (hdr) {
            len += snprintf(&log_msg_str[len], LOG_MSG_SZ_MAX - (size_t)len, "%30s : ", hdr);
        }
        len += vsnprintf(&log_msg_str[len], LOG_MSG_SZ_MAX - (size_t)len, (char *)fmt, argp);
        if (len >= LOG_MSG_SZ_MAX) {
            len = LOG_MSG_SZ_MAX - 6;
            len += snprintf(&log_msg_str[len], LOG_MSG_SZ_MAX - (size_t)len, ". . .");
        }
    }
    else if (hdr != NULL) {
        len += snprintf(&log_msg_str[len], LOG_MSG_SZ_MAX - (size_t)len, "%s", hdr);
    }

    cli_port_print_msg(log_msg_str);
}

void cli_port_print_fmt(const char *hdr, const char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    cli_log_zigbee_callback(app_info.zb, 0U, hdr, fmt, argptr);
    va_end(argptr);
}

static void cli_port_print_msg(const char *pMessage)
{
    UTIL_ADV_TRACE_Status_t eReturn;

    if (pMessage != NULL) {
        do {
            eReturn = UTIL_ADV_TRACE_Send((uint8_t *)pMessage, strlen(pMessage));
        } while (eReturn != UTIL_ADV_TRACE_OK);
        /* -- Add a CR at the end of message -- */
        UTIL_ADV_TRACE_Send((uint8_t *)"\r\n", 0x02u);
    }
}

void cli_port_print_mac_debug(void *null_p, const char *pMessage)
{
    cli_port_print_msg(pMessage);
}

static void cli_port_print_prompt(bool bSendCr)
{
    if (bSendCr) {
        UTIL_ADV_TRACE_Send((uint8_t *)"\r\n", 0x02u);
    }
    UTIL_ADV_TRACE_Send((uint8_t *)cli_prompt_str, strlen(cli_prompt_str));
}





