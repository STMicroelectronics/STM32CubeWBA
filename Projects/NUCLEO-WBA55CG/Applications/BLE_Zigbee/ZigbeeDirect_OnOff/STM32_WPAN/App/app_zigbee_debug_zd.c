/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_zigbee_debug_zd.c
  * @author  MCD Application Team
  * @brief   Zigbee Direct  Application
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
/* USER CODE END Header */
#include <stdio.h>
#include "app_zigbee.h"
#include "app_zigbee_debug_zd.h"
#include "app_zigbee_endpoint.h"
#include "log_module_conf.h"
#include "log_module.h"
#include "stm32_rtos.h"
#include "stm32_adv_trace.h"
#include "serial_cmd_interpreter.h"

const char *const CLI_FMTS = "%40s: %s";
const char *const CLI_FMTD = "%40s: %d";
const char *const CLI_FMTPERC = "%40s: %d%%";
const char *const CLI_FMTL = "%40s: %lu";
const char *const CLI_FMTX = "%40s: 0x%08x";
const char *const CLI_FMTLLX = "%40s: " LOGFMTx64;
const char *const CLI_FMTDX = "%40s: 0x%04x";
const char *const CLI_FMTBX = "%40s: 0x%02x";

#define CLI_OUTPUT_LINE_MAX_LEN             256U
#define LOG_MSG_SZ_MAX                      256U

static char log_msg_str[LOG_MSG_SZ_MAX];
static char cli_temp[CLI_OUTPUT_LINE_MAX_LEN];
static const char *cli_prompt_str = "zdd > ";

static void cli_port_print_msg(const char *pMessage);
static void cli_port_print_prompt(bool bSendCr);
static void APP_ZIGBEE_App_Task(void);
static void APP_ZIGBEE_ActivateStatusDebug(void);

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
    struct zbcli_buf_t *bufp = &stZigbeeAppInfo.buf_ring[stZigbeeAppInfo.buf_wr_idx];

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
                stZigbeeAppInfo.buf_wr_idx = (stZigbeeAppInfo.buf_wr_idx + 1U) % CLI_CMD_QUEUE_SZ;
                bufp = &stZigbeeAppInfo.buf_ring[stZigbeeAppInfo.buf_wr_idx];
                APP_ZIGBEE_ActivateStatusDebug();
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



/**
 * @brief  Application task used to get access to some paramters
 * @param  None
 * @retval None
 */
static void APP_ZIGBEE_App_Task(void)
{
    struct zbcli_buf_t *bufp = &stZigbeeAppInfo.buf_ring[stZigbeeAppInfo.buf_rd_idx];


    /* Check for user command */
    if (bufp->status == BUF_STATUS_READY) {
        const char *cmd_str = (const char *)bufp->buf;

        if (strcmp(cmd_str, "help") == 0) {
            LOG_INFO_APP("Available commands total:");
            LOG_INFO_APP("   adv      ; re-enable BLE advertisements");
            LOG_INFO_APP("   help     ; print this help");
            LOG_INFO_APP("   pjoin    ; enable permit-join via ZDO broadcast");
            LOG_INFO_APP("   status   ; print status");
            LOG_INFO_APP("   swreset  ; board reset");
        }
        else if (strcmp(cmd_str, "adv") == 0) {
            LOG_INFO_APP("Enabling BLE advertisements");
            zb_zdd_update_advert(stZigbeeAppInfo.pstZigbee, true);
        }
        else if (strcmp(cmd_str, "pjoin") == 0) {
            struct ZbZdoPermitJoinReqT req;
            enum ZbStatusCodeT status;

            memset(&req, 0, sizeof(req));
            req.destAddr = ZB_NWK_ADDR_BCAST_ROUTERS;
            req.duration = 180;

            LOG_INFO_APP("Enabling Permit-Join for 180 seconds via ZDO broadcast");
            status = ZbZdoPermitJoinReq(stZigbeeAppInfo.pstZigbee, &req, NULL, NULL);
            if (status != ZB_STATUS_SUCCESS) {
                LOG_INFO_APP("Error, Permit-Join request failed (status = 0x%02x)", status);
            }
        }
        else if (strcmp(cmd_str, "status") == 0) {
            cli_status_zdo();
            cli_print_aps_channel_mask();
            cli_status_nwk(); 
            cli_status_nnt();
            cli_security_dump();
            zdd_sec_print_session(stZigbeeAppInfo.pstZigbee);

            /* OnOff Cluster State */
            LOG_INFO_APP("");
            LOG_INFO_APP("On/Off state = %s", stZigbeeAppInfo.onoff_state ? "on" : "off");
        }
        else if (strcmp(cmd_str, "swreset") == 0) {
            HAL_NVIC_SystemReset();
        }
        else {
            LOG_INFO_APP("Error, unknown command: %s", cmd_str);
        }

        /* We're done with this buffer. Clear it and move to the next one. */
        bufp->len = 0;
        bufp->status = BUF_STATUS_EMPTY;
        stZigbeeAppInfo.buf_rd_idx = (stZigbeeAppInfo.buf_rd_idx + 1U) % CLI_CMD_QUEUE_SZ;

        /* Print a new prompt */
        cli_port_print_prompt(false);
    }
}

static void APP_ZIGBEE_ActivateStatusDebug(void)
{
    UTIL_SEQ_SetTask(1u << CFG_TASK_ZIGBEE_APP1, CFG_SEQ_PRIO_1);
}

void APP_ZIGBEE_InitStatusDebug(void)
{
  UTIL_SEQ_RegTask(1u << CFG_TASK_ZIGBEE_APP1, 0x00, APP_ZIGBEE_App_Task);
}
/*-----------------------------------------------------------------------------
 * Logging
 *-----------------------------------------------------------------------------
 */

void cli_log_zigbee_callback(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argp)
{
    (void)zb;
    (void)mask;
    int len = 0;

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
    cli_log_zigbee_callback(stZigbeeAppInfo.pstZigbee, 0U, hdr, fmt, argptr);
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




/* For printing keys, etc */
static unsigned int
app_hex_bin_to_str(const uint8_t *in_data, unsigned int in_len, char *out_str, unsigned int max_len,
    const char delimiter, unsigned int interval)
{
    unsigned int i, j;
    const char *hex = "0123456789abcdef";

    if (max_len == 0U) {
        return 0;
    }
    j = 0;
    for (i = 0; i < in_len; i++) {
        if ((interval != 0U) && (i != 0U) && ((i % interval) == 0U)) {
            if ((j + 2U) > max_len) {
                out_str[0] = (char)0;
                return 0;
            }
            out_str[j++] = delimiter;
        }
        if ((j + 3U) > max_len) {
            out_str[0] = (char)0;
            return 0;
        }
        out_str[j++] = hex[(in_data[i] >> 4) & 0x0fU];
        out_str[j++] = hex[in_data[i] & 0x0fU];
    }
    out_str[j] = (char)0;
    return j;
}

const char *
ZbNwkNeighborRelationshipToStr(enum ZbNwkNeighborRelT relationship)
{
    const char *str;

    switch (relationship) {
        case ZB_NWK_NEIGHBOR_REL_PARENT:
            str = "Parent";
            break;

        case ZB_NWK_NEIGHBOR_REL_CHILD:
            str = "Child";
            break;

        case ZB_NWK_NEIGHBOR_REL_SIBLING:
            str = "Sibling";
            break;

        case ZB_NWK_NEIGHBOR_REL_NONE:
            str = "None";
            break;

        case ZB_NWK_NEIGHBOR_REL_PREV_CHILD:
            str = "Prev Child";
            break;

        case ZB_NWK_NEIGHBOR_REL_UNAUTH_CHILD:
            str = "Unauth Child";
            break;

        case ZB_NWK_NEIGHBOR_REL_UNAUTH_CHILD_WITH_RELAY:
            str = "Unauth Child with APS Relay allowed";
            break;

        case ZB_NWK_NEIGHBOR_REL_PEND_ASSOCIATE:
        case ZB_NWK_NEIGHBOR_REL_PEND_ORPHAN:
            str = "Pend Child";
            break;

        default:
            str = "Reserved";
            break;
    }
    return (str);
}

const char *
ZbNwkNeighborDeviceTypeToStr(enum ZbNwkNeighborTypeT deviceType)
{
    const char *str;
    switch (deviceType) {
        case ZB_NWK_NEIGHBOR_TYPE_COORD:
            str = "Crd";
            break;

        case ZB_NWK_NEIGHBOR_TYPE_ROUTER:
            str = "Rtr";
            break;

        case ZB_NWK_NEIGHBOR_TYPE_END_DEV:
            str = "End";
            break;

        case ZB_NWK_NEIGHBOR_TYPE_UNKNOWN:
        default:
            str = "Unk";
            break;
    }
    return (str);
}

void cli_status_zdo(void)
{
    uint8_t capability = 0;
    uint8_t coordinator = 0;
    uint8_t nwkSecurityLevel = ZB_SEC_LEVEL_NONE;
    uint64_t tcAddr;
    unsigned long heap_sz;

    /* Get nwkCapabilityInfo and apsDesignatedCoordinator. */
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_CapabilityInformation, &capability, sizeof(uint8_t));
    (void)ZbApsGet(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_DESIGNATED_COORD, &coordinator, sizeof(uint8_t));

    tcAddr = 0;
    (void)ZbApsGet(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_TRUST_CENTER_ADDRESS, &tcAddr, sizeof(uint64_t));

    /* Print the ZDO Status Title bar. */
    LOG_INFO_APP("-- ZigBee ZDO Status ----------------------------------------------------------");

    /* Print the Device type. */
    if ((tcAddr != ZB_DISTRIBUTED_TC_ADDR) && coordinator) {
        LOG_INFO_APP(CLI_FMTS, "Device Type", "Coordinator");
    }
    else if ((capability & MCP_ASSOC_CAP_DEV_TYPE) != 0U) {
        LOG_INFO_APP(CLI_FMTS, "Device Type", "Router");
    }
    else if ((capability & MCP_ASSOC_CAP_RXONIDLE) != 0U) {
        LOG_INFO_APP(CLI_FMTS, "Device Type", "End Device");
    }
    else {
        LOG_INFO_APP(CLI_FMTS, "Device Type", "Sleepy End Device");
    }

    /* Network (Trust Center) Security Model */
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_SecurityLevel, &nwkSecurityLevel, sizeof(uint8_t));
    if ((nwkSecurityLevel == ZB_SEC_LEVEL_NONE) || (tcAddr == 0)) {
        LOG_INFO_APP(CLI_FMTS, "Trust Center Type", "None");
    }
    else if (tcAddr == ZB_DISTRIBUTED_TC_ADDR) {
        LOG_INFO_APP(CLI_FMTS, "Trust Center Type", "Distributed");
    }
    else {
        LOG_INFO_APP(CLI_FMTS, "Trust Center Type", "Centralized");
        /* Print the TC address */
        snprintf(cli_temp, CLI_OUTPUT_LINE_MAX_LEN, LOGFMTx64, LOGVALx64(tcAddr));
        LOG_INFO_APP(CLI_FMTS, "Trust Center Address", cli_temp);
    }

    heap_sz = ZbHeapAvailable(stZigbeeAppInfo.pstZigbee);
    LOG_INFO_APP(CLI_FMTD, "Zigbee Heap Avail", heap_sz);

    LOG_INFO_APP("");
}

void cli_print_aps_channel_mask(void)
{
    enum ZbStatusCodeT status;
    struct ZbChannelListT channel_list;
    uint32_t mask;
    unsigned int i;
    char name[32];
    char temp[128];

    status = ZbApsGet(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_CHANNEL_MASK, &channel_list, sizeof(struct ZbChannelListT));
    if (status != ZB_STATUS_SUCCESS) {
        LOG_INFO_APP("Error, ZbApsGet failed (status = 0x%02x)", status);
        return;
    }
    if (channel_list.count == 0U) {
        LOG_INFO_APP(CLI_FMTS, "ChannelMask", "<empty>");
        return;
    }
    for (i = 0U; i < channel_list.count; i++) {
        mask = ZB_CHANNELMASK(channel_list.list[i].channelMask, channel_list.list[i].page);

        sprintf(name, "%s[%d]", "ChannelMask", i);
        sprintf(temp, "0x%08" PRIx32 " (page = %d)", mask, channel_list.list[i].page);
        LOG_INFO_APP(CLI_FMTS, name, temp);
    }
}

static uint8_t
cli_get_num_channels_from_mask(uint32_t mask, uint16_t *first_channel)
{
    uint8_t i, num_channels = 0;

    if (first_channel != NULL) {
        *first_channel = WPAN_CHANNELS_MAX;
    }
    for (i = 0; i < WPAN_PAGE_CHANNELS_MAX; i++) {
        if (((1 << i) & mask)) {
            num_channels++;
            if (first_channel && (*first_channel == WPAN_CHANNELS_MAX)) {
                *first_channel = i;
            }
        }
    }
    return num_channels;
}

void cli_status_nwk(void)
{
    uint64_t extAddr = ZbExtendedAddress(stZigbeeAppInfo.pstZigbee);
    uint16_t nwkNetworkAddress = ZB_NWK_ADDR_UNDEFINED;
    uint16_t nwkPanId = ZB_NWK_ADDR_UNDEFINED;
    /* uint8_t nwkStackProfile = ZB_NWK_STACK_PROFILE_NWKSPFC; */
    uint8_t nwkUpdateId;
    struct ZbChannelListT channelList;
    uint64_t epid;
    uint8_t pjoin_count;
    unsigned int i;

    epid = 0;
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

    /* Read the values from the NIB. */
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_NetworkAddress, &nwkNetworkAddress, sizeof(uint16_t));
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_PanId, &nwkPanId, sizeof(uint16_t));
    /* (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_StackProfile, &nwkStackProfile, sizeof(uint8_t)); */
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_UpdateId, &nwkUpdateId, sizeof(uint8_t));

    /* Print the network status. */
    LOG_INFO_APP("-- ZigBee Network Status ------------------------------------------------------");

    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_ActiveChannelList, &channelList, sizeof(struct ZbChannelListT));
    for (i = 0; i < channelList.count; i++) {
        uint8_t num_channels;
        uint16_t channel;

        snprintf(cli_temp, CLI_OUTPUT_LINE_MAX_LEN, "Channel (Page %2d)", channelList.list[i].page);
        num_channels = cli_get_num_channels_from_mask(channelList.list[i].channelMask, &channel);
        if (num_channels == 1) {
            LOG_INFO_APP(CLI_FMTD, cli_temp, channel);
            continue;
        }
        /* Shouldn't get here */
        LOG_INFO_APP(CLI_FMTX, cli_temp, channelList.list[i].channelMask);
    }

    /* LOG_INFO_APP(CLI_FMTS, "Logical Channels", ZbAppCliGetChannelsStr(stZigbeeAppInfo.pstZigbee, channelsStr)); */
    LOG_INFO_APP(CLI_FMTLLX, "Extended PAN Id", LOGVALx64(epid));
    LOG_INFO_APP(CLI_FMTLLX, "Extended Address", LOGVALx64(extAddr));
    LOG_INFO_APP(CLI_FMTDX, "Short Address", nwkNetworkAddress);
    LOG_INFO_APP(CLI_FMTDX, "PAN Id", nwkPanId);
    LOG_INFO_APP(CLI_FMTBX, "Update Id", nwkUpdateId);
    LOG_INFO_APP(CLI_FMTBX, "Protocol Version", ZB_NWK_CONST_PROTOCOL_VERSION);
    /* LOG_INFO_APP(CLI_FMTS, "Stack Profile", ZbNwkStackProfileIdToStr(nwkStackProfile)); */

    /* Current Permit-Join status */
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_PermitJoinCounter, &pjoin_count, sizeof(pjoin_count));
    LOG_INFO_APP(CLI_FMTBX, "Permit Join Value", pjoin_count);
    LOG_INFO_APP("");
}

void cli_status_nnt(void)
{
    struct ZbNwkNeighborT neighbor;
    uint64_t epid;
    unsigned int tempLen, i;
    unsigned int nnt_index = 0;

    epid = 0;
    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

    LOG_INFO_APP("-- ZigBee Network Neighbor Table ----------------------------------------------");
    tempLen = 0;
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "    %-23s", "EXTADDR");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %-7s", "NWKADDR");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %-4s", "TYPE");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %-6s", "RXIDLE");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %-12s", "RELATION");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %6s", "TXFAIL");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %4s", "AGE");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %4s", "LQI");
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %4s", "COST");
#ifdef __linux__
    tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen, "  %6s", "LASTRX");
#endif

    LOG_INFO_APP(cli_temp);

    for (i = 0;; i++) {
        if (epid == 0) {
            /* The NNT doesn't make much sense if we haven't joined/formed a network.
             * Skip it and use the discovery table instead. */
            break;
        }
        if (ZbNwkGetIndex(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_NeighborTable, &neighbor,
                sizeof(neighbor), i) != ZB_NWK_STATUS_SUCCESS) {
            break;
        }
        /* Skip unused entries. */
        if (neighbor.nwkAddr == ZB_NWK_ADDR_UNDEFINED) {
            continue;
        }
        nnt_index++;

        tempLen = 0;
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "%3d ", nnt_index);

        /* EXTADDR */
        if (neighbor.extAddr != 0U) {
            tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                    LOGFMTx64 "     ", LOGVALx64(neighbor.extAddr));
        }
        else {
            tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                    "%-23s", " ");
        }
        /* NWKADDR */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  0x%04x ", neighbor.nwkAddr);
        /* TYPE */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %-4s", ZbNwkNeighborDeviceTypeToStr(neighbor.deviceType));
        /* RXIDLE */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %-6s", ((neighbor.capability & MCP_ASSOC_CAP_RXONIDLE) != 0U) ? "True" : "False");
        /* RELATION */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %-12s", ZbNwkNeighborRelationshipToStr(neighbor.relationship));
        /* TXFAIL */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %6d", neighbor.txFailure);
        /* AGE */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %4d", neighbor.age);
        /* LQI */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %4d", neighbor.lqi);
        /* COST */
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %4d", neighbor.outgoingCost);
#ifdef __linux__
        /* LASTRX */
        ZbUptimeT last_rx;
        unsigned int elapsed;

        last_rx = nwk_nnt_entry_get_last_rx_time(stZigbeeAppInfo.pstZigbee, neighbor.nwkAddr);
        elapsed = ZbTimeoutRemaining(last_rx, ZbZclUptime(stZigbeeAppInfo.pstZigbee));
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %6d", elapsed / 1000U);
#endif

        LOG_INFO_APP(cli_temp);
    }

    LOG_INFO_APP("");
}

void cli_security_dump(void)
{
    uint8_t keySeqNumber;
    char key_str[ZB_SEC_KEYSTR_SIZE];
    struct ZbNwkSecMaterialT secMaterial;
    uint8_t preconf[ZB_SEC_KEYSIZE];
    uint8_t distrib[ZB_SEC_KEYSIZE];
    struct ZbApsmeKeyPairT keypair;
    char addr_str[32];
    unsigned int i;

    LOG_INFO_APP("-- ZigBee Security Info and Keys ----------------------");

    (void)ZbNwkGet(stZigbeeAppInfo.pstZigbee, ZB_NWK_NIB_ID_ActiveKeySeqNumber, &keySeqNumber, sizeof(uint8_t));
    ZbNwkGetSecMaterial(stZigbeeAppInfo.pstZigbee, keySeqNumber, &secMaterial);
    app_hex_bin_to_str(secMaterial.key, sizeof(secMaterial.key), key_str, sizeof(key_str), ':', 2);
    LOG_INFO_APP("%24s: %s", "Network Key", key_str);
    LOG_INFO_APP("%24s: %d", "Network Seqno", keySeqNumber);

    (void)ZbApsGet(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_PRECONFIGURED_LINK_KEY, preconf, ZB_SEC_KEYSIZE);
    app_hex_bin_to_str(preconf, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
    LOG_INFO_APP("%24s: %s", "APS Preconf Key", key_str);

    (void)ZbApsGet(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_DISTRIBUTED_GLOBAL_KEY, distrib, ZB_SEC_KEYSIZE);
    app_hex_bin_to_str(distrib, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
    LOG_INFO_APP("%24s: %s", "APS Distrib Key", key_str);

    for (i = 0;; i++) {
        if (ZbApsGetIndex(stZigbeeAppInfo.pstZigbee, ZB_APS_IB_ID_DEVICE_KEY_PAIR_SET, &keypair, sizeof(struct ZbApsmeKeyPairT), i)) {
            break;
        }
        if (keypair.deviceAddress == 0) {
            continue;
        }
        sprintf(addr_str, LOGFMTx64 " Key", LOGVALx64(keypair.deviceAddress));

        /* Print keyAttribute? ZB_APSME_KEY_ATTR_UNVERIFIED,
         * ZB_APSME_KEY_ATTR_VERIFIED, ZB_APSME_KEY_ATTR_CBKE */
        app_hex_bin_to_str(keypair.linkKey, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
        LOG_INFO_APP("%24s: %s", addr_str, key_str);
    }

    LOG_INFO_APP("");
}
