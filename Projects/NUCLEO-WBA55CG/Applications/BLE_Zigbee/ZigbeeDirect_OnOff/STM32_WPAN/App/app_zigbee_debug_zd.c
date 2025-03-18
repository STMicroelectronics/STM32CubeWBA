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

const char *const CLI_FMTS = "%40s: %s";
const char *const CLI_FMTD = "%40s: %d";
const char *const CLI_FMTPERC = "%40s: %d%%";
const char *const CLI_FMTL = "%40s: %lu";
const char *const CLI_FMTX = "%40s: 0x%08x";
const char *const CLI_FMTLLX = "%40s: " LOGFMTx64;
const char *const CLI_FMTDX = "%40s: 0x%04x";
const char *const CLI_FMTBX = "%40s: 0x%02x";

#define CLI_OUTPUT_LINE_MAX_LEN             256U

static char cli_temp[CLI_OUTPUT_LINE_MAX_LEN];

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
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_CapabilityInformation, &capability, sizeof(uint8_t));
    (void)ZbApsGet(app_info.zb, ZB_APS_IB_ID_DESIGNATED_COORD, &coordinator, sizeof(uint8_t));

    tcAddr = 0;
    (void)ZbApsGet(app_info.zb, ZB_APS_IB_ID_TRUST_CENTER_ADDRESS, &tcAddr, sizeof(uint64_t));

    /* Print the ZDO Status Title bar. */
    APP_LOG_PRINTF("-- ZigBee ZDO Status ----------------------------------------------------------");

    /* Print the Device type. */
    if ((tcAddr != ZB_DISTRIBUTED_TC_ADDR) && coordinator) {
        APP_LOG_PRINTF(CLI_FMTS, "Device Type", "Coordinator");
    }
    else if ((capability & MCP_ASSOC_CAP_DEV_TYPE) != 0U) {
        APP_LOG_PRINTF(CLI_FMTS, "Device Type", "Router");
    }
    else if ((capability & MCP_ASSOC_CAP_RXONIDLE) != 0U) {
        APP_LOG_PRINTF(CLI_FMTS, "Device Type", "End Device");
    }
    else {
        APP_LOG_PRINTF(CLI_FMTS, "Device Type", "Sleepy End Device");
    }

    /* Network (Trust Center) Security Model */
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_SecurityLevel, &nwkSecurityLevel, sizeof(uint8_t));
    if ((nwkSecurityLevel == ZB_SEC_LEVEL_NONE) || (tcAddr == 0)) {
        APP_LOG_PRINTF(CLI_FMTS, "Trust Center Type", "None");
    }
    else if (tcAddr == ZB_DISTRIBUTED_TC_ADDR) {
        APP_LOG_PRINTF(CLI_FMTS, "Trust Center Type", "Distributed");
    }
    else {
        APP_LOG_PRINTF(CLI_FMTS, "Trust Center Type", "Centralized");
        /* Print the TC address */
        snprintf(cli_temp, CLI_OUTPUT_LINE_MAX_LEN, LOGFMTx64, LOGVALx64(tcAddr));
        APP_LOG_PRINTF(CLI_FMTS, "Trust Center Address", cli_temp);
    }

    heap_sz = ZbHeapAvailable(app_info.zb);
    APP_LOG_PRINTF(CLI_FMTD, "Zigbee Heap Avail", heap_sz);

    APP_LOG_PRINTF("");
}

void cli_print_aps_channel_mask(void)
{
    enum ZbStatusCodeT status;
    struct ZbChannelListT channel_list;
    uint32_t mask;
    unsigned int i;
    char name[32];
    char temp[128];

    status = ZbApsGet(app_info.zb, ZB_APS_IB_ID_CHANNEL_MASK, &channel_list, sizeof(struct ZbChannelListT));
    if (status != ZB_STATUS_SUCCESS) {
        APP_LOG_PRINTF("Error, ZbApsGet failed (status = 0x%02x)", status);
        return;
    }
    if (channel_list.count == 0U) {
        APP_LOG_PRINTF(CLI_FMTS, "ChannelMask", "<empty>");
        return;
    }
    for (i = 0U; i < channel_list.count; i++) {
        mask = ZB_CHANNELMASK(channel_list.list[i].channelMask, channel_list.list[i].page);

        sprintf(name, "%s[%d]", "ChannelMask", i);
        sprintf(temp, "0x%08" PRIx32 " (page = %d)", mask, channel_list.list[i].page);
        APP_LOG_PRINTF(CLI_FMTS, name, temp);
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
    uint64_t extAddr = ZbExtendedAddress(app_info.zb);
    uint16_t nwkNetworkAddress = ZB_NWK_ADDR_UNDEFINED;
    uint16_t nwkPanId = ZB_NWK_ADDR_UNDEFINED;
    /* uint8_t nwkStackProfile = ZB_NWK_STACK_PROFILE_NWKSPFC; */
    uint8_t nwkUpdateId;
    struct ZbChannelListT channelList;
    uint64_t epid;
    uint8_t pjoin_count;
    unsigned int i;

    epid = 0;
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

    /* Read the values from the NIB. */
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_NetworkAddress, &nwkNetworkAddress, sizeof(uint16_t));
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_PanId, &nwkPanId, sizeof(uint16_t));
    /* (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_StackProfile, &nwkStackProfile, sizeof(uint8_t)); */
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_UpdateId, &nwkUpdateId, sizeof(uint8_t));

    /* Print the network status. */
    APP_LOG_PRINTF("-- ZigBee Network Status ------------------------------------------------------");

    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_ActiveChannelList, &channelList, sizeof(struct ZbChannelListT));
    for (i = 0; i < channelList.count; i++) {
        uint8_t num_channels;
        uint16_t channel;

        snprintf(cli_temp, CLI_OUTPUT_LINE_MAX_LEN, "Channel (Page %2d)", channelList.list[i].page);
        num_channels = cli_get_num_channels_from_mask(channelList.list[i].channelMask, &channel);
        if (num_channels == 1) {
            APP_LOG_PRINTF(CLI_FMTD, cli_temp, channel);
            continue;
        }
        /* Shouldn't get here */
        APP_LOG_PRINTF(CLI_FMTX, cli_temp, channelList.list[i].channelMask);
    }

    /* APP_LOG_PRINTF(CLI_FMTS, "Logical Channels", ZbAppCliGetChannelsStr(app_info.zb, channelsStr)); */
    APP_LOG_PRINTF(CLI_FMTLLX, "Extended PAN Id", LOGVALx64(epid));
    APP_LOG_PRINTF(CLI_FMTLLX, "Extended Address", LOGVALx64(extAddr));
    APP_LOG_PRINTF(CLI_FMTDX, "Short Address", nwkNetworkAddress);
    APP_LOG_PRINTF(CLI_FMTDX, "PAN Id", nwkPanId);
    APP_LOG_PRINTF(CLI_FMTBX, "Update Id", nwkUpdateId);
    APP_LOG_PRINTF(CLI_FMTBX, "Protocol Version", ZB_NWK_CONST_PROTOCOL_VERSION);
    /* APP_LOG_PRINTF(CLI_FMTS, "Stack Profile", ZbNwkStackProfileIdToStr(nwkStackProfile)); */

    /* Current Permit-Join status */
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_PermitJoinCounter, &pjoin_count, sizeof(pjoin_count));
    APP_LOG_PRINTF(CLI_FMTBX, "Permit Join Value", pjoin_count);
    APP_LOG_PRINTF("");
}

void cli_status_nnt(void)
{
    struct ZbNwkNeighborT neighbor;
    uint64_t epid;
    unsigned int tempLen, i;
    unsigned int nnt_index = 0;

    epid = 0;
    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_ExtendedPanId, &epid, sizeof(uint64_t));

    APP_LOG_PRINTF("-- ZigBee Network Neighbor Table ----------------------------------------------");
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

    APP_LOG_PRINTF(cli_temp);

    for (i = 0;; i++) {
        if (epid == 0) {
            /* The NNT doesn't make much sense if we haven't joined/formed a network.
             * Skip it and use the discovery table instead. */
            break;
        }
        if (ZbNwkGetIndex(app_info.zb, ZB_NWK_NIB_ID_NeighborTable, &neighbor,
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

        last_rx = nwk_nnt_entry_get_last_rx_time(app_info.zb, neighbor.nwkAddr);
        elapsed = ZbTimeoutRemaining(last_rx, ZbZclUptime(app_info.zb));
        tempLen += snprintf(&cli_temp[tempLen], CLI_OUTPUT_LINE_MAX_LEN - tempLen,
                "  %6d", elapsed / 1000U);
#endif

        APP_LOG_PRINTF(cli_temp);
    }

    APP_LOG_PRINTF("");
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

    APP_LOG_PRINTF("-- ZigBee Security Info and Keys ----------------------");

    (void)ZbNwkGet(app_info.zb, ZB_NWK_NIB_ID_ActiveKeySeqNumber, &keySeqNumber, sizeof(uint8_t));
    ZbNwkGetSecMaterial(app_info.zb, keySeqNumber, &secMaterial);
    app_hex_bin_to_str(secMaterial.key, sizeof(secMaterial.key), key_str, sizeof(key_str), ':', 2);
    APP_LOG_PRINTF("%24s: %s", "Network Key", key_str);
    APP_LOG_PRINTF("%24s: %d", "Network Seqno", keySeqNumber);

    (void)ZbApsGet(app_info.zb, ZB_APS_IB_ID_PRECONFIGURED_LINK_KEY, preconf, ZB_SEC_KEYSIZE);
    app_hex_bin_to_str(preconf, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
    APP_LOG_PRINTF("%24s: %s", "APS Preconf Key", key_str);

    (void)ZbApsGet(app_info.zb, ZB_APS_IB_ID_DISTRIBUTED_GLOBAL_KEY, distrib, ZB_SEC_KEYSIZE);
    app_hex_bin_to_str(distrib, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
    APP_LOG_PRINTF("%24s: %s", "APS Distrib Key", key_str);

    for (i = 0;; i++) {
        if (ZbApsGetIndex(app_info.zb, ZB_APS_IB_ID_DEVICE_KEY_PAIR_SET, &keypair, sizeof(struct ZbApsmeKeyPairT), i)) {
            break;
        }
        if (keypair.deviceAddress == 0) {
            continue;
        }
        sprintf(addr_str, LOGFMTx64 " Key", LOGVALx64(keypair.deviceAddress));

        /* Print keyAttribute? ZB_APSME_KEY_ATTR_UNVERIFIED,
         * ZB_APSME_KEY_ATTR_VERIFIED, ZB_APSME_KEY_ATTR_CBKE */
        app_hex_bin_to_str(keypair.linkKey, ZB_SEC_KEYSIZE, key_str, sizeof(key_str), ':', 2);
        APP_LOG_PRINTF("%24s: %s", addr_str, key_str);
    }

    APP_LOG_PRINTF("");
}
