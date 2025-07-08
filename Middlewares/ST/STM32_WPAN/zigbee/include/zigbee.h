/**
 * @file zigbee.h
 * @heading Zigbee Utilities
 * @brief Zigbee header file.
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 *
 * This file groups global/external definitions from all the layer specific header files
 * e.g, aps, nwk, zdo etc... into a single place, so that one can just include zigbee.h for
 * all the global definitions eliminating the header file inclusion clutter from source files.
 */

#ifndef ZIGBEE_H
# define ZIGBEE_H

#if defined(__GNUC__)
# define ZB_WARN_UNUSED __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && (_MSC_VER >= 1800)
# define ZB_WARN_UNUSED
#else
# define ZB_WARN_UNUSED
#endif

#ifdef __CDT_PARSER__
/* Eclipse indexer only code */
#include "zb_make_config.h"
#endif

/* For FlexeLint */
#ifndef CONFIG_ZB_REV
#define CONFIG_ZB_REV                   23
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> /* for NULL */
#include "pletoh.h" /* Little endian conversion */
#include "llist.h" /* Link list */
#include "ieee802154_enums.h"

#include "zigbee.log.h"

/* Zigbee instance as an opaque data structure. The internal parameters of this structure
 * are only accessible within the Zigbee stack. */
struct ZigBeeT;

/* MAC instance as an opaque data structure. The internal parameters of this structure
 * are only accessible within the Zigbee stack and MAC layers. */
struct WpanPublicT;

/* ZigBee Protocol Versions as related to the spec version. */
#if 0 /* deprecated */
#define ZB_PROTOCOL_VERSION_2004        0x0001U
#endif
#define ZB_PROTOCOL_VERSION_2007        0x0002U

/* Channel Mask / Page Helpers */
#define ZB_CHANNELMASK_GETPAGE(x)       (uint8_t)(((x) >> WPAN_PAGE_CHANNELS_MAX) & 0x1FU)
#define ZB_CHANNELMASK(mask, page)      ((mask) | ((((uint32_t)page) & 0x1FU) << WPAN_PAGE_CHANNELS_MAX))

#define ZB_PANID_MIN                    0x0000U
/* A value of 0xffff means the device is not associated. */
#define ZB_PANID_MAX                    0xfffeU

#define ZB_EPID_MIN                     0x0000000000000001ULL
#define ZB_EPID_MAX                     0xfffffffffffffffeULL

/* The ZigBee distributed trust center address. */
#define ZB_DISTRIBUTED_TC_ADDR          0xffffffffffffffffULL

/* A subset of WPAN_CHANNELMASK_2400MHZ (HA and SE preferred channels) */
#define ZB_CHANNELMASK_2400MHZ_HA       0x0318C800U /* Channels 11, 14, 15, 19, 20, 24, 25 */

/* Convert UNIX time epoch (Jan 1st 1970) to Zigbee time epoch (Jan 1st 2000). */
#define ZB_UNIX_EPOCH_DIFF_SECS         946684800U

/* Define a constant maximum allocation size for both zigbee.mem.heap.c and
 * zigbee.mem.pool.c so both have the same behaviour. The memory pool has a max
 * allocation size of 2048 minus some overhead (should be >= 2016 bytes).
 * NOTE: The max allocation is not the total size of the heap, only the
 * largest chunk of data we can allocate from it at a time.
 * If we define both implementation limits to be 2000, we should be safe. */
#define ZB_HEAP_MAX_ALLOC                   2000U

/* ZigBee Status Codes */
enum ZbStatusCodeT {
    /* General Status Codes */
    ZB_STATUS_SUCCESS = 0x00,
    ZB_STATUS_ALLOC_FAIL = 0x70, /* Memory allocation failure. */

    /* ZDP Status Codes */
    ZB_ZDP_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_ZDP_STATUS_INV_REQTYPE = 0x80,
    ZB_ZDP_STATUS_DEVNOTFOUND = 0x81,
    ZB_ZDP_STATUS_INVALID_EP = 0x82,
    ZB_ZDP_STATUS_NOT_ACTIVE = 0x83,
    ZB_ZDP_STATUS_NOT_SUPPORTED = 0x84,
    ZB_ZDP_STATUS_TIMEOUT = 0x85,
    ZB_ZDP_STATUS_NO_MATCH = 0x86,
    ZB_ZDP_STATUS_NO_ENTRY = 0x88,
    ZB_ZDP_STATUS_NO_DESCRIPTOR = 0x89,
    ZB_ZDP_STATUS_INSUFFICIENT_SPACE = 0x8a,
    ZB_ZDP_STATUS_NOT_PERMITTED = 0x8b,
    ZB_ZDP_STATUS_TABLE_FULL = 0x8c,
    ZB_ZDP_STATUS_NOT_AUTHORIZED = 0x8d,
    ZB_ZDP_STATUS_DEVICE_BINDING_TABLE_FULL = 0x8e,
    ZB_ZDP_STATUS_INVALID_INDEX = 0x8f,
    ZB_ZDP_STATUS_FRAME_TOO_LARGE = 0x90,

    /* APS Status Codes. */
    ZB_APS_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_APS_STATUS_ASDU_TOO_LONG = 0xa0,
    ZB_APS_STATUS_DEFRAG_DEFERRED = 0xa1,
    ZB_APS_STATUS_DEFRAG_UNSUPPORTED = 0xa2,
    ZB_APS_STATUS_ILLEGAL_REQUEST = 0xa3,
    ZB_APS_STATUS_INVALID_BINDING = 0xa4,
    ZB_APS_STATUS_INVALID_GROUP = 0xa5,
    ZB_APS_STATUS_INVALID_PARAMETER = 0xa6,
    ZB_APS_STATUS_NO_ACK = 0xa7,
    ZB_APS_STATUS_NO_BOUND_DEVICE = 0xa8,
    ZB_APS_STATUS_NO_SHORT_ADDRESS = 0xa9,
    ZB_APS_STATUS_NOT_SUPPORTED = 0xaa,
    ZB_APS_STATUS_SECURED_LINK_KEY = 0xab,
    ZB_APS_STATUS_SECURED_NWK_KEY = 0xac,
    ZB_APS_STATUS_SECURITY_FAIL = 0xad,
    ZB_APS_STATUS_TABLE_FULL = 0xae,
    ZB_APS_STATUS_UNSECURED = 0xaf,
    ZB_APS_STATUS_UNSUPPORTED_ATTRIBUTE = 0xb0,
    ZB_APS_STATUS_PEER_CANNOT_FRAGMENT = 0xb1,
    ZB_APS_STATUS_UNKNOWN_FRAGMENT_SUPPORT = 0xb2,
    /* These values used in case of internal errors. */
    ZB_APS_STATUS_INVALID_INDEX = 0xbd,

    /* NWK Status Codes. */
    ZB_NWK_STATUS_SUCCESS = ZB_STATUS_SUCCESS,
    ZB_NWK_STATUS_INVALID_PARAMETER = 0xc1,
    ZB_NWK_STATUS_INVALID_REQUEST = 0xc2,
    ZB_NWK_STATUS_NOT_PERMITTED = 0xc3,
    ZB_NWK_STATUS_STARTUP_FAILURE = 0xc4,
    ZB_NWK_STATUS_ALREADY_PRESENT = 0xc5,
    ZB_NWK_STATUS_SYNC_FAILURE = 0xc6,
    ZB_NWK_STATUS_TABLE_FULL = 0xc7,
    ZB_NWK_STATUS_UNKNOWN_DEVICE = 0xc8,
    ZB_NWK_STATUS_UNSUPPORTED_ATTRIBUTE = 0xc9,
    ZB_NWK_STATUS_NO_NETWORKS = 0xca,
    ZB_NWK_STATUS_LEAVE_UNCONFIRMED = 0xcb,
    ZB_NWK_STATUS_MAX_FRM_CNTR = 0xcc,
    ZB_NWK_STATUS_NO_KEY = 0xcd,
    ZB_NWK_STATUS_BAD_CCM_OUTPUT = 0xce,
    ZB_NWK_STATUS_NO_ROUTING_CAPACITY = 0xcf,
    ZB_NWK_STATUS_ROUTE_DISCOVERY_FAILED = 0xd0,
    ZB_NWK_STATUS_ROUTE_ERROR = 0xd1,
    ZB_NWK_STATUS_BT_TABLE_FULL = 0xd2,
    ZB_NWK_STATUS_FRAME_NOT_BUFFERED = 0xd3,
    ZB_NWK_STATUS_INVALID_INTERFACE = 0xd5,
    ZB_NWK_STATUS_MISSING_TLV = 0xd6,
    ZB_NWK_STATUS_INVALID_TLV = 0xd7,
    /* Exegin Custom Status Values. */
    ZB_NWK_STATUS_INVALID_INDEX = 0xd8,
    ZB_NWK_STATUS_INTERNAL_ERR = 0xd9,

    /* WPAN Status Codes (copied from mcp_enums.h to here for MISRA) */
    ZB_WPAN_STATUS_SUCCESS = ZB_STATUS_SUCCESS, /* 0x00 */
    ZB_WPAN_STATUS_COUNTER_ERROR = mcp_STATUS_COUNTER_ERROR, /* 0xdb */
    ZB_WPAN_STATUS_IMPROPER_KEY_TYPE = mcp_STATUS_IMPROPER_KEY_TYPE, /* 0xdc */
    ZB_WPAN_STATUS_IMPROPER_SECURITY_LEVEL = mcp_STATUS_IMPROPER_SECURITY_LEVEL, /* 0xdd */
    ZB_WPAN_STATUS_UNSUPPORTED_LEGACY = mcp_STATUS_UNSUPPORTED_LEGACY, /* 0xde */
    ZB_WPAN_STATUS_UNSUPPORTED_SECURITY = mcp_STATUS_UNSUPPORTED_SECURITY, /* 0xdf */
    ZB_WPAN_STATUS_BEACON_LOSS = mcp_STATUS_BEACON_LOSS, /* 0xe0 */
    ZB_WPAN_STATUS_CHANNEL_ACCESS_FAILURE = mcp_STATUS_CHANNEL_ACCESS_FAILURE, /* 0xe1 */
    ZB_WPAN_STATUS_DENIED = mcp_STATUS_DENIED, /* 0xe2 */
    ZB_WPAN_STATUS_DISABLE_TRX_FAILURE = mcp_STATUS_DISABLE_TRX_FAILURE, /* 0xe3 */
    ZB_WPAN_STATUS_SECURITY_ERROR = mcp_STATUS_SECURITY_ERROR, /* 0xe4 */
    ZB_WPAN_STATUS_FRAME_TOO_LONG = mcp_STATUS_FRAME_TOO_LONG, /* 0xe5 */
    ZB_WPAN_STATUS_INVALID_GTS = mcp_STATUS_INVALID_GTS, /* 0xe6 */
    ZB_WPAN_STATUS_INVALID_HANDLE = mcp_STATUS_INVALID_HANDLE, /* 0xe7 */
    ZB_WPAN_STATUS_INVALID_PARAMETER = mcp_STATUS_INVALID_PARAMETER, /* 0xe8 */
    ZB_WPAN_STATUS_NO_ACK = mcp_STATUS_NO_ACK, /* 0xe9 */
    ZB_WPAN_STATUS_NO_BEACON = mcp_STATUS_NO_BEACON, /* 0xea */
    ZB_WPAN_STATUS_NO_DATA = mcp_STATUS_NO_DATA, /* 0xeb */
    ZB_WPAN_STATUS_NO_SHORT_ADDRESS = mcp_STATUS_NO_SHORT_ADDRESS, /* 0xec */
    ZB_WPAN_STATUS_OUT_OF_CAP = mcp_STATUS_OUT_OF_CAP, /* 0xed */
    ZB_WPAN_STATUS_PAN_ID_CONFLICT = mcp_STATUS_PAN_ID_CONFLICT, /* 0xee */
    ZB_WPAN_STATUS_REALIGNMENT = mcp_STATUS_REALIGNMENT, /* 0xef */
    ZB_WPAN_STATUS_TRANSACTION_EXPIRED = mcp_STATUS_TRANSACTION_EXPIRED, /* 0xf0 */
    ZB_WPAN_STATUS_TRANSACTION_OVERFLOW = mcp_STATUS_TRANSACTION_OVERFLOW, /* 0xf1 */
    ZB_WPAN_STATUS_TX_ACTIVE = mcp_STATUS_TX_ACTIVE, /* 0xf2 */
    ZB_WPAN_STATUS_UNAVAILABLE_KEY = mcp_STATUS_UNAVAILABLE_KEY, /* 0xf3 */
    ZB_WPAN_STATUS_UNSUPPORTED_ATTRIBUTE = mcp_STATUS_UNSUPPORTED_ATTRIBUTE, /* 0xf4 */
    ZB_WPAN_STATUS_INVALID_ADDRESS = mcp_STATUS_INVALID_ADDRESS, /* 0xf5 */
    ZB_WPAN_STATUS_ON_TIME_TOO_LONG = mcp_STATUS_ON_TIME_TOO_LONG, /* 0xf6 */
    ZB_WPAN_STATUS_PAST_TIME = mcp_STATUS_PAST_TIME, /* 0xf7 */
    ZB_WPAN_STATUS_TRACKING_OFF = mcp_STATUS_TRACKING_OFF, /* 0xf8 */
    ZB_WPAN_STATUS_INVALID_INDEX = mcp_STATUS_INVALID_INDEX, /* 0xf9 */
    ZB_WPAN_STATUS_LIMIT_REACHED = mcp_STATUS_LIMIT_REACHED, /* 0xfa */
    ZB_WPAN_STATUS_READ_ONLY = mcp_STATUS_READ_ONLY, /* 0xfb */
    ZB_WPAN_STATUS_SCAN_IN_PROGRESS = mcp_STATUS_SCAN_IN_PROGRESS, /* 0xfc */
    ZB_WPAN_STATUS_SUPERFRAME_OVERLAP = mcp_STATUS_SUPERFRAME_OVERLAP, /* 0xfd */
    ZB_WPAN_STATUS_DRIVER_ERROR = mcp_STATUS_DRIVER_ERROR, /* 0xfe */
    ZB_WPAN_STATUS_DEVICE_ERROR = mcp_STATUS_DEVICE_ERROR /* 0xff */
};

/* Trust Center Swap Out status codes */
enum ZbTcsoStatusT {
    ZB_TCSO_STATUS_SUCCESS = 0x00, /* Successfully performed TCSO with new TC */
    ZB_TCSO_STATUS_DISCOVERY_UNDERWAY = 0x01, /* ZCL Keepalive has initiated TCSO, because of three consecutive failures. */
    ZB_TCSO_STATUS_REJOIN_PREV = 0x02, /* Found previous TC and rejoined to it. */
    ZB_TCSO_STATUS_NOT_FOUND = 0x03, /* Didn't find new or old TC, or process was aborted, resumed previous operation. */
    ZB_TCSO_STATUS_FATAL = 0x04 /* TCSO failed and unable to restore previous operation. */
};

/* Zigbee defines of the scan type. These map directly to the MCP defines,
 * without having to include the entire MCP headers. */
#define ZB_SCAN_TYPE_ENERGY             0U /* MCP_SCAN_ENERGY (0) */
#define ZB_SCAN_TYPE_ACTIVE             1U /* MCP_SCAN_ACTIVE (1) */
#define ZB_SCAN_TYPE_PASSIVE            2U /* MCP_SCAN_PASSIVE (2) */
#define ZB_SCAN_TYPE_ORPHAN             3U /* MCP_SCAN_ORPHAN (3) */
#define ZB_SCAN_TYPE_ENHANCED           4U /* MCP_SCAN_ENHANCED (4) */

/*---------------------------------------------------------------
 * Exegin Manufacturing ID
 *---------------------------------------------------------------
 */
/* The Exegin manufacturer ID. */
#define ZB_MFG_CODE_EXEGIN              0x10D7U
#define ZB_MFG_CODE_WILDCARD            0xFFFFU

/*---------------------------------------------------------------
 * Channel List Structure
 *---------------------------------------------------------------
 */
/* One 2.4 GHz mask + one North American mask + five EU masks + four GB-868 masks. */
#define ZB_CHANNEL_LIST_NUM_MAX         11U

/**
 * A single Channel List Entry. Includes the Channel Page and Channel Mask.
 * It is used within 'struct ZbChannelListT' when describing a list of channels masks,
 * or on its own in some cases.
 */
struct ZbChannelListEntryT {
    uint8_t page; /**< The 802.15.4 Channel Page (e.g. 2.4 GHz O-QPSK = Page 0) */
    uint32_t channelMask;
    /**< The 32-bit Channel Mask. The 5 MSB Channel Page bits are not used and should
     * be set to 0. The 27 LSB define the channel(s) supported or to be used during
     * commissioning. */
};

/**
 * The Channel List structure. Includes a list of 'struct ZbChannelListEntryT'
 * entries; one for each Channel Page supported or to be used during commissioning.
 */
struct ZbChannelListT {
    uint8_t count; /**< Number of channel masks in 'list'. Maximum is ZB_CHANNEL_LIST_NUM_MAX. */
    struct ZbChannelListEntryT list[ZB_CHANNEL_LIST_NUM_MAX];
    /**< The list of Channel Pages and Masks. */
};

/*---------------------------------------------------------------
 * JoinerTLVs (R23+)
 *---------------------------------------------------------------
 */
/* Maximum size of joiner TLV information that can be sent, so that
 * APSME-UPDATE-DEVICE.request command need not be fragmented. */
#define ZB_JOINER_TLVS_MAX_SIZE                 79U

/**
 * TLV structure the application can use to include TLVs in certains packet payloads
 * during the commissioning process.
 */
struct ZbJoinerTLVsT {
    uint8_t length; /**< Length of the TLV buffer 'buf'. Maximum length is ZB_JOINER_TLVS_MAX_SIZE. */
    uint8_t buf[ZB_JOINER_TLVS_MAX_SIZE];
    /**< Buffer containing the TLV data.
     * The data is in the format as would appear over-the-air (i.e. binary blob format) */
};

/*---------------------------------------------------------------
 * Stack Initialization
 *---------------------------------------------------------------
 */

/* Anonymous stack structures. */
struct ZigBeeT;
struct ZbMsgFilterT;

/* Zigbee uptime structure **/
typedef unsigned long ZbUptimeT;

/**
 * Zigbee timeout remaining function.
 * @param now Current time
 * @param expire_time time of timeout expiry
 * @return Returns 0 if now >= expire_time,
 * difference in milliseconds between now and timeout if now < expire_time.
 */
unsigned int ZbTimeoutRemaining(ZbUptimeT now, ZbUptimeT expire_time);

/** A pointer to this struct type is passed to ZbInit to define the various
 * ZigBee tables used in the stack. If the pointer to ZbInit is NULL, the
 * default sizes are used. */
struct ZbInitTblSizesT {
    void *heapPtr;
    /**< Pointer to memory to use for stack's run-time heap. Size equals heapSz bytes.
     * If heapSz is zero, set this to NULL and the buffer will be allocated using ZbMalloc().
     * If the stack is built with CONFIG_ZB_ALLOC_STATIC, then heapPtr and heapSz must be non-zero
     * and provided by the application.
     */
    unsigned int heapSz;
    /**< Size of memory buffer provided by heapPtr.
     * If heapSz is zero, then a default value of 32 kB for FFD or 16 kB for RFD is chosen,
     * and memory will be allocated through ZbMalloc().
     * An FFD vs RFD type of stack is controlled by the CONFIG_ZB_ENDNODE when the stack is built. */

    /* NWK Table Sizes */
    unsigned int nwkNeighborTblSz; /**< Default: FFD:64, RFD:16 */
    unsigned int nwkRouteTblSz; /**< Default: FFD:32, RFD:0 */
    unsigned int nwkAddrMapTblSz; /**< Default: FFD:32, RFD:2 */
    unsigned int nwkBttSz; /**< Default: FFD:32, RFD:9 */
    unsigned int nwkRReqSz; /**< Default: FFD:16, RFD:0 */

    /* APS Table Sizes */
    unsigned int apsBindingTblSz; /**< Default: FFD:64, RFD:16 */
    unsigned int apsGroupTblSz; /**< Default: FFD:16, RFD:4 */
    unsigned int apsPeerLinkKeyTblSz; /**< Default: FFD:32, RFD:2 */
};

/** Same parameters as ZbSetLogging takes. Allows debug log output
 * as stack is being initialized. */
struct ZbInitSetLoggingT {
    uint32_t mask; /**< e.g. ZB_LOG_MASK_LEVEL_2 */
    void (*func)(struct ZigBeeT *zb, uint32_t mask, const char *hdr,
        const char *fmt, va_list argptr);
    /**< Callback function to print log messages. */
};

/**
 * This is called to create a new Zigbee stack instance. One of the parameters provided
 * is an optional pointer to a struct that defines the sizes of some of the tables
 * within the stack (e.g. NNT, Routing Table, Security Keys Table). This lets the
 * application developer tailor the memory usage depending on the type of device
 * being developed.
 * @param extAddr EUI to be assigned to this Zigbee stack instance.
 * @param tblSizes Optional pointer to a 'struct ZbInitTblSizesT' data structure defining
 * some of the table sizes to be allocated.
 * @param setLogging Optional pointer to a 'struct ZbInitSetLoggingT' data structure
 * defining the log mask and callback to call to print log messages.
 * @return Pointer to Zigbee stack instance, or NULL if there was a problem
 * (e.g. not enough memory)
 */
struct ZigBeeT * ZbInit(uint64_t extAddr, struct ZbInitTblSizesT *tblSizes, struct ZbInitSetLoggingT *setLogging);

/**
 * Deallocates a Zigbee stack instance.
 * @param zb Pointer to Zigbee stack instance
 */
void ZbDestroy(struct ZigBeeT *zb);

/**
 * Deallocates a Zigbee stack instance and save callback to application structure
 * @param zb Pointer to Zigbee stack instance
 * @param callback Callback function
 * @param arg Callback argument
 * @return Returns ZB_STATUS_SUCCESS on success, other status code on failure
 */
enum ZbStatusCodeT ZbDestroyWithCb(struct ZigBeeT *zb, void (*callback)(void *arg), void *arg);

/**
 * Called periodically to run the stack.
 * @param zb Pointer to Zigbee stack instance
 */
void ZbTimerWork(struct ZigBeeT *zb);

/**
 * Returns the length of time (in milliseconds) until the next scheduled timer will elapse,
 * or UINT_MAX if there are no scheduled timers.
 * @param zb Pointer to Zigbee stack instance
 * @return Returns Length of time on success, 0 on failure
 */
unsigned int ZbCheckTime(struct ZigBeeT *zb);

/* Configure a callback to wakeup the application if there's a new stack event to
 *      process. Not all stack ports require this. */

/**
 * Configure a callback to activate the application if there is a stack event to process.
 * Not required by all stack ports.
 * @param zb Pointer to Zigbee stack instance
 * @param wakeup_cb Wakeup callback function
 */
void ZbWakeupCallbackConfig(struct ZigBeeT *zb, void (*wakeup_cb)(void));

/**
 * Called to get the file descriptor to be used to wake-up the stack thread calling
 * ZbTimerWork if a stack event needs to be processed. This is only required in
 * multi-threaded environments. Without this event, it is possible for a user thread to
 * initiate a stack function which does not activate the MAC layer, which in turn would
 * wake up the stack thread.
 * @param zb Pointer to Zigbee stack instance
 */
int ZbPortStackEventFd(struct ZigBeeT *zb);

/**
 * Change extended address of the stack instance
 * @param zb Pointer to Zigbee stack instance
 * @param extAddr New extended address
 */
void ZbChangeExtAddr(struct ZigBeeT *zb, uint64_t extAddr);

struct ZbNlmeLeaveConfT;

/**
 * Send a leave request to the stack
 * @param zb Pointer to Zigbee stack instance
 * @param callback Callback function
 * @param cbarg Callback argument
 * @return Returns ZB_STATUS_SUCCESS on success, other status code on failure
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbLeaveReq(struct ZigBeeT *zb,
    void (*callback)(struct ZbNlmeLeaveConfT *conf, void *arg), void *cbarg);

/**
 * Helper function to perform an APS and NWK reset
 * @param zb Pointer to Zigbee stack instance
 */
void ZbReset(struct ZigBeeT *zb);

/**
 * Attaches an IEEE 802.15.4 device driver to the ZigBee stack. Uses the link pointers
 * within the device structure for linking.
 * @param zb Pointer to Zigbee stack instance
 * @param dev Pointer to wpan device
 * @return Returns true on success, false otherwise
 */
bool ZbIfAttach(struct ZigBeeT *zb, struct WpanPublicT *dev);

/**
 * Detaches an IEEE 802.15.4 device driver from the ZigBee stack.
 * @param zb Pointer to Zigbee stack instance
 * @param dev Pointer to wpan device
 */
void ZbIfDetach(struct ZigBeeT *zb, struct WpanPublicT *dev);

/*---------------------------------------------------------------
 * ZCL Basic Server API
 *---------------------------------------------------------------
 */
/* Basic Cluster maximum string lengths. */
#define ZCL_BASIC_MANUFACTURER_NAME_LENGTH       32U
#define ZCL_BASIC_MODEL_IDENTIFIER_LENGTH        32U
#define ZCL_BASIC_DATE_CODE_LENGTH               16U
#define ZCL_BASIC_LOCATION_DESC_LENGTH           16U
#define ZCL_BASIC_SW_BUILD_ID_LENGTH             16U

/**
 * This data structure is used to configure the default attribute values
 * for the Basic Server. All values are in ZCL data format (i.e. strings
 * are prefixed with the length byte)
 */
struct ZbZclBasicServerDefaults {
    uint8_t app_version; /**< ZCL_BASIC_ATTR_APP_VERSION */
    uint8_t stack_version; /**< ZCL_BASIC_ATTR_STACK_VERSION */
    uint8_t hw_version; /**< ZCL_BASIC_ATTR_HARDWARE_VERSION */
    uint8_t mfr_name[ZCL_BASIC_MANUFACTURER_NAME_LENGTH + 1U]; /**< ZCL_BASIC_ATTR_MFR_NAME (First byte length) */
    uint8_t model_name[ZCL_BASIC_MODEL_IDENTIFIER_LENGTH + 1U]; /**< ZCL_BASIC_ATTR_MODEL_NAME (First byte length) */
    uint8_t date_code[ZCL_BASIC_DATE_CODE_LENGTH + 1U]; /**< ZCL_BASIC_ATTR_DATE_CODE (First byte length) */
    uint8_t power_source; /**< ZCL_BASIC_ATTR_POWER_SOURCE (e.g. ZCL_BASIC_POWER_UNKNOWN) */
    uint8_t sw_build_id[ZCL_BASIC_SW_BUILD_ID_LENGTH + 1U]; /**< ZCL_BASIC_ATTR_SW_BUILD_ID (First byte length) */
};

/**
 * Configure the default ZCL Basic Server attribute values. The Basic Server
 * is integral to the stack in order for the attribute values to be made "global"
 * and shared between all Basic Server instances on all endpoints.
 * This should be called after calling ZbInit and before creating any ZCL endpoints.
 * @param zb Zigbee instance
 * @param defaults Pointer to the default configuration data structure
 */
void ZbZclBasicServerConfigDefaults(struct ZigBeeT *zb, const struct ZbZclBasicServerDefaults *defaults);

/* Write to the local attributes (e.g. ZCL_BASIC_ATTR_MFR_NAME) */
/**
 * Write to a Basic Server attribute
 * @param zb Zigbee instance
 * @param attrId The attribute Id to write
 * @param buf Pointer to the attribute data, in the ZCL defined format.
 * @param len Maximum length of the attribute data. May exceed the length of the particular attribute.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclBasicWriteDirect(struct ZigBeeT *zb, uint16_t attrId, const void *buf, unsigned int max_len);

/**
 * Read a Basic Server attribute
 * @param zb Zigbee instance
 * @param attrId The attribute Id to read
 * @param buf Pointer to the attribute data, in the ZCL defined format.
 * @param len Maximum length of the attribute data. May exceed the length of the particular attribute.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclBasicReadDirect(struct ZigBeeT *zb, uint16_t attrId, void *buf, unsigned int max_len);

/**
 * Post an alarm code to the Basic Cluster.
 * @param zb Pointer to Zigbee stack instance
 * @param endpoint Endpoint of basic cluster
 * @param alarm_code Alarm code
 * @return Returns true on success, false otherwise
 */
bool ZbZclBasicPostAlarm(struct ZigBeeT *zb, uint8_t endpoint, uint8_t alarm_code);

/*---------------------------------------------------------------
 * ZigBee Timer
 *---------------------------------------------------------------
 */
/** Anonymous timer structure. */
struct ZbTimerT;

/**
 * Creates a ZigBee timer structure.
 * @param zb Pointer to Zigbee stack instance
 * @param callback Callback function
 * @param arg Callback argument
 * @return Zigbee timer structure
 */
struct ZbTimerT * ZbTimerAlloc(struct ZigBeeT *zb, void (*callback)(struct ZigBeeT *zb, void *cb_arg), void *arg);

/**
 * Changes callback function for the specified timer, without the need to free and allocate a new timer.
 * @param timer Pointer to Zigbee timer structure
 * @param callback Callback function
 * @param arg Callback argument
 */
void ZbTimerChangeCallback(struct ZbTimerT *timer, void (*callback)(struct ZigBeeT *zb, void *cb_arg), void *arg);

/**
 * Stop a timer before it expires.
 * @param timer Pointer to Zigbee timer structure
 */
void ZbTimerStop(struct ZbTimerT *timer);

/**
 * Free a timer.
 * @param timer Pointer to Zigbee timer structure
 */
void ZbTimerFree(struct ZbTimerT *timer);

/**
 * Reset and schedule a ZigBee timer.
 * @param timer Pointer to Zigbee timer structure
 * @param timeout Delay in starting timer
 */
void ZbTimerReset(struct ZbTimerT *timer, unsigned int timeout);

/**
 * Checks if the specified timer is active.
 * @param timer Callback argument
 * @return True if the specified timer is active
 */
bool ZbTimerRunning(struct ZbTimerT *timer);

/**
 * Returns time remaining in ms for a timer.
 * @param timer Pointer to Zigbee timer structure
 * @return Time remaining in ms as an unsigned int
 */
unsigned int ZbTimerRemaining(struct ZbTimerT *timer);

/*---------------------------------------------------------
 * Persistence
 *---------------------------------------------------------
 */
/**
 * Configure the persistence callback that tells the application when important
 * stack parameters have changed and should be saved.
 * @param zb Pointer to Zigbee stack instance
 * @param callback Callback function
 * @param cbarg Callback argument
 * @return True if callback was registered, false otherwise.
 */
bool ZbPersistNotifyRegister(struct ZigBeeT *zb, void (*callback)(struct ZigBeeT *zb, void *cbarg), void *cbarg);

/**
 * Get the stack persistence data and write it to the buffer provided by buf.
 * If buf is NULL and maxlen is zero, this function determines the buffer size
 * required to save the persistence data.
 * Use ZbStartupPersist to restore the persistence data to the stack and restart
 * the zigbee stack.
 * @param zb Pointer to Zigbee stack instance
 * @param buf Pointer to memory to write persistence data
 * @param maxlen Max length of persistence data
 * @return Returns length of persistence data
 */
unsigned int ZbPersistGet(struct ZigBeeT *zb, uint8_t *buf, unsigned int maxlen);

/**
 * Same as ZbPersistGet, but saves additional more dynamic data such as routing tables.
 * Use the same ZbStartupPersist to restore the persistence data to the stack and
 * to restart the zigbee stack.
 * @param zb Pointer to Zigbee stack instance
 * @param buf Pointer to memory to write persistence and additional data
 * @param maxlen Max length of persistence and additional data
 * @return Returns length of persistence and additional data
 */
unsigned int ZbStateGet(struct ZigBeeT *zb, uint8_t *buf, unsigned int maxlen);

/*---------------------------------------------------------
 * Shutdown
 *---------------------------------------------------------
 */
/**
 * This API moves the stack to shutdown mode, used in case of a sleepy end device to conserve power.
 * @param zb Pointer to Zigbee stack instance
 */
void ZbShutdown(struct ZigBeeT *zb);

/*---------------------------------------------------------
 * Pause
 *---------------------------------------------------------
 */
/**
 * Pauses the stack by stopping any active timers and tasks. The 802.15.4 MAC
 * interface is also disconnected from the stack. This allows something else to
 * use the MAC/Radio interface while the stack is paused, for example Bluetooth.
 * @param zb Pointer to Zigbee stack instance
 * @param callback Callback gets called once pausing the stack is complete.
 * Callback is only called if this function returns ZB_STATUS_SUCCESS
 * @param cbarg Callback argument
 * @return Returns ZB_STATUS_SUCCESS on success, other status code on failure
 */
enum ZbStatusCodeT ZbStatePause(struct ZigBeeT *zb, void (*callback)(void *arg), void *arg);

/**
 * Resumes stack operation after calling ZbStatePause.
 * @param zb Pointer to Zigbee stack instance
 * @return Returns ZB_STATUS_SUCCESS on success, other status code on failure
 */
enum ZbStatusCodeT ZbStateResume(struct ZigBeeT *zb);

/*---------------------------------------------------------------
 * Misc. Helper Functions
 *---------------------------------------------------------------
 */

/**
 * Returns the Extended address of the Zigbee stack.
 * @param zb Pointer to Zigbee stack instance
 * @return Returns extended address
 */
uint64_t ZbExtendedAddress(struct ZigBeeT *zb);

/**
 * Returns the Short address of the Zigbee stack.
 * @param zb Pointer to Zigbee stack instance
 * @return Returns short address
 */
uint16_t ZbShortAddress(struct ZigBeeT *zb);

/* Returns the number of channels (bits) set in 'mask'.
 * Sets '*first_channel' to the first channel found in the mask. */

/* Returns the amount of memory allocated from the zigbee stack heap. */

/*---------------------------------------------------------------
 * Zigbee Heap
 *---------------------------------------------------------------
 */

/* NOTE: ZbHeapAlloc and ZbHeapFree are for internal use only, and allocate memory
 * from the Zigbee stack memory pool. Some of the ZCL clusters need to make use of
 * these functions, so their prototypes have to be included here.
 */
void * zb_heap_alloc(struct ZigBeeT *zb, unsigned int sz, const char *funcname, unsigned int linenum);
void zb_heap_free(struct ZigBeeT *zb, void *ptr, const char *funcname, unsigned int linenum);

#if defined(CONFIG_ZB_MEMORY_DEBUG) || defined(CONFIG_ZB_MEMPOOL_HISTORY)
# define ZbHeapAlloc(_zb_, _sz_)             zb_heap_alloc(_zb_, _sz_, __func__, __LINE__)
# define ZbHeapFree(_zb_, _ptr_)             zb_heap_free(_zb_, _ptr_, __func__, __LINE__)
#else
/* To reduce binary size */
# define ZbHeapAlloc(_zb_, _sz_)             zb_heap_alloc(_zb_, _sz_, "", 0)
# define ZbHeapFree(_zb_, _ptr_)             zb_heap_free(_zb_, _ptr_, "", 0)
#endif

/**
 * Current size of memory allocated from the zigbee stack heap
 * @param zb Pointer to Zigbee stack instance
 * @return Returns the memory allocated from the zigbee stack heap
 */
unsigned long ZbHeapUsed(struct ZigBeeT *zb);

/**
 * Current size of memory available in the zigbee stack heap
 * @param zb Pointer to Zigbee stack instance
 * @return Returns the amount of memory available in the zigbee stack heap,
 * or 0 if unknown or unbounded
 */
unsigned long ZbHeapAvailable(struct ZigBeeT *zb);

/**
 * Zigbee memory heap high-water mark. Useful during testing to determine maximum
 * amount of memory consumed from total heap size.
 * @param zb Pointer to Zigbee stack instance
 * @return Returns the worst-case heap usage (high-water mark)
 */
unsigned long ZbHeapHighWaterMark(struct ZigBeeT *zb);

/*---------------------------------------------------------------
 * Additional Layer Includes
 *---------------------------------------------------------------
 */
#include "zigbee.filter.h"
#include "zigbee.security.h"
#include "zigbee.hash.h"
#include "zigbee.tlv.h"
#include "zigbee.dlk.h"
#include "zigbee.bdb.h"
#include "zigbee.aps.h"
#include "zigbee.nwk.h"
#include "zigbee.startup.h"
#include "zigbee.zdo.h"
#endif
