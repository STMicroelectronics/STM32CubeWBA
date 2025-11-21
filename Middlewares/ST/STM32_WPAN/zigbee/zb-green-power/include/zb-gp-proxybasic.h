/* Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved. */

#ifndef ZGP_PROXYBASIC_H
# define ZGP_PROXYBASIC_H

#include "zigbee.h"
#include "zb-green-power.h"
#include "zcl/zcl.h"

/*lint -save -e621 "Identifier clash within 31 chars. For readability,
 * we need to exceed the 31 character limit before uniqueness, and
 * extend the limit to 63 charcters.
 * [MISRA Rule 5.1 (REQUIRED), MISRA Rule 5.2 (REQUIRED),
 * MISRA Rule 5.4 (REQUIRED), MISRA Rule 5.5 (REQUIRED)]" */

/* Proxy Basic Client attribute identifiers */
enum ZbZclGpProxyCliAttrT {
    ZCL_GP_PROXY_CLI_ATTR_GPP_MAX_PROXY_TABLE_ENTRIES = 0x0010,
    ZCL_GP_PROXY_CLI_ATTR_PROXY_TABLE = 0x0011,
    ZCL_GP_PROXY_CLI_ATTR_GPP_NOTIFICATION_RETRY_NUMBER = 0x0012,
    ZCL_GP_PROXY_CLI_ATTR_GPP_NOTIFICATION_RETRY_TIMER = 0x0013,
    ZCL_GP_PROXY_CLI_ATTR_GPP_MAX_SEARCH_COUNTER = 0x0014,
    ZCL_GP_PROXY_CLI_ATTR_GPP_BLOCKED_GPDID = 0x0015,
    ZCL_GP_PROXY_CLI_ATTR_GPP_FUNCTIONALITY = 0x0016,
    ZCL_GP_PROXY_CLI_ATTR_GPP_ACTIVE_FUNCTIONALITY = 0x0017,
    ZCL_GP_PROXY_CLI_ATTR_GPP_SHARED_SECURITY_KEY_TYPE = 0x0020,
    ZCL_GP_PROXY_CLI_ATTR_GPP_SHARED_SECURITY_KEY = 0x0021,
    ZCL_GP_PROXY_CLI_ATTR_GPP_LINK_KEY = 0x0022
};

/* Green Power Status Codes */
enum ZgpStatusCodeT {
    ZGP_STATUS_SUCCESS = 0x00,
    ZGP_STATUS_NO_SECURITY = 0x01,
    ZGP_STATUS_COUNTER_FAILURE = 0x02,
    ZGP_STATUS_AUTH_FAILURE = 0x03,
    ZGP_STATUS_UNPROCESSED = 0x04,
    /* Internal security status codes. */
    ZGP_STATUS_TX_THEN_DROP = 0x05,
    ZGP_STATUS_DROP_FRAME = 0x06,
    /* ZGP packet queue status codes. */
    ZGP_STATUS_TX_QUEUE_FULL = 0x07,
    ZGP_STATUS_ENTRY_REPLACED = 0x08,
    ZGP_STATUS_ENTRY_ADDED = 0x09,
    ZGP_STATUS_ENTRY_EXPIRED = 0x0a,
    ZGP_STATUS_ENTRY_REMOVED = 0x0b,
    ZGP_STATUS_INVALID_PARAMETER = 0x0c
};

/* Same format as ZbNldeDataConfT */
struct ZbZgpDataConfT {
    uint32_t handle;
    enum ZgpStatusCodeT status;
};

struct ZbZgpDataReqT {
    uint8_t txOptions;
    uint8_t applicationId;
    uint32_t srcId;
    uint64_t dstAddr;
    uint8_t endpoint;
    uint8_t commandId;
    const void *asdu;
    uint8_t asduLength;
    uint32_t handle;
    uint16_t lifetime;
};

struct ZbZclClusterT * ZbZgpProxyBasicClientAlloc(struct ZigBeeT *zb, uint8_t endpoint);

void ZbZgpProxyGetLinkKey(struct ZigBeeT *zb, uint8_t key[ZB_SEC_KEYSIZE]);

/* Add items to the gpTxQueue */
void ZbZgpProxyGpdQueueReq(struct ZigBeeT *zb, struct ZbZgpDataReqT *req,
    void (*callback)(struct ZbZgpDataConfT *, void *), void *arg);

/* Remove items from the gpTxQueue */
void ZbZgpProxyDataPurge(struct ZigBeeT *zb, uint8_t appId, uint64_t gpdId, uint8_t endpoint);

enum ZgpStatusCodeT ZbZgpProxyAddKey(struct ZigBeeT *zb, uint8_t applicationId, uint64_t gpdId, uint8_t endpoint,
    uint8_t level, uint8_t keytype, const void *key);

/*lint -restore */

#endif
