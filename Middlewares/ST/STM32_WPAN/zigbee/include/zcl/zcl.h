/**
 * @file zcl.h
 * @heading Zigbee ZCL Core
 * @brief Zigbee header file.
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 */

#ifndef ZCL_H
# define ZCL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h> /* memset, memcpy, etc */
#include "zigbee.h"
#include "zcl/zcl.enum.h"

/*---------------------------------------------------------------
 * ZCL Debug Logging
 *---------------------------------------------------------------
 */
#ifdef CONFIG_ZB_LOG_NONE
#define ZCL_LOG_PRINTF(zb, mask, hdr, ...)      (void)zb
#define ZCL_LOG_ASSERT(zb, cond)                (void)zb

#else
#if ((ZB_LOG_MASK_ZCL & CONFIG_ZB_LOG_ALLOWED_MASK) != 0U)
/*lint -e{762} "Redundantly declared symbol [LINT]" */
/*lint -e{9004} "previously declared [MISRA Rule 8.5 (REQUIRED)]" */
extern void ZbLogPrintf(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, ...);
#endif

/* If not defined, then disable debugging (minimal Flash size) */
#ifndef CONFIG_ZB_LOG_ALLOWED_MASK
# define CONFIG_ZB_LOG_ALLOWED_MASK         0U
#endif
#ifndef CONFIG_ZCL_LOG_ALLOWED_MASK
# define CONFIG_ZCL_LOG_ALLOWED_MASK         0U
#endif

/* Used with ZCL_LOG_PRINTF for fine grain control in ZCL code */
#define ZCL_LOG_MASK_INFO                   0x01 /* General debug and error information */
#define ZCL_LOG_MASK_DEBUG                  0x02 /* General debug (more verbose than INFO) */
#define ZCL_LOG_MASK_ATTR                   0x04 /* ZCL attributes */
#define ZCL_LOG_MASK_REPORTS                0x08 /* ZCL reporting */
#define ZCL_LOG_MASK_SLEEPY                 0x10

/*lint -emacro(506,ZCL_LOG_PRINTF) "Constant value boolean [MISRA Rule 2.1 (REQUIRED)]" */
/*lint -emacro(774,ZCL_LOG_PRINTF) [ LINT Boolean within 'if' always evaluates to False ] */
/*lint -emacro(831,ZCL_LOG_PRINTF) "right arg to | is certain to be 0 [LINT]" */

#if ((ZB_LOG_MASK_ZCL & CONFIG_ZB_LOG_ALLOWED_MASK) != 0U)
bool zcl_log_allowed(struct ZigBeeT *zb, uint32_t mask);
void zcl_log_mask_set(struct ZigBeeT *zb, uint32_t mask);
void zcl_log_mask_clear(struct ZigBeeT *zb, uint32_t mask);

#define ZCL_LOG_PRINTF(zb, mask, hdr, ...) \
    do { \
        if (((mask) & CONFIG_ZCL_LOG_ALLOWED_MASK) == 0U) { \
            break; \
        } \
        if (zcl_log_allowed(zb, mask)) { \
            ZbLogPrintf(zb, ZB_LOG_MASK_ZCL, hdr, __VA_ARGS__); \
        } \
    } while (false)

#define ZCL_LOG_ASSERT(zb, cond) \
    do { \
        if (!(cond)) { \
            ZbLogPrintf(zb, ZB_LOG_MASK_ERROR, __func__, "Error, assert at line %d", __LINE__); \
        } \
    } while (false)

#else
#define ZCL_LOG_PRINTF(zb, mask, hdr, ...)      (void)zb
#define ZCL_LOG_ASSERT(zb, cond)                (void)zb
#endif
#endif

struct ZbZclAddrInfoT {
    struct ZbApsAddrT addr;
    uint8_t seqnum;
    uint16_t tx_options;
};

/*---------------------------------------------------------------
 * Cluster Commands
 *---------------------------------------------------------------
 */

/* Group ID limit for Scene and Group Cluster Commands */
#define ZCL_GROUPS_ID_MIN                   0x0001U
#define ZCL_GROUPS_ID_MAX                   0xfff7U

struct ZbZclCommandReqT {
    /* From APSDE-DATA.request */
    struct ZbApsAddrT dst;
    uint16_t profileId;
    enum ZbZclClusterIdT clusterId;
    uint16_t srcEndpt;
    uint16_t txOptions; /* e.g. ZB_APSDE_DATAREQ_TXOPTIONS_ACK */
    bool discoverRoute;
    uint8_t radius;
    /* ZCL command */
    struct ZbZclHeaderT hdr;
    /* if (txOptions & ZB_APSDE_DATAREQ_TXOPTIONS_VECTOR), then payload is
     * a pointer to list of struct ZbApsBufT, and length is the number of struct ZbApsBufT
     * items in the list. */
    void *payload;
    unsigned int length;
    /* Timeout in milliseconds to wait for response. If zero, then a
     * suitable default timeout will be used. */
    unsigned int timeout;
};

/* ZCL Command Response
 *
 * Special case if src.mode == ZB_APSDE_ADDRMODE_NOTPRESENT (0), then
 *      - response generated internally.
 *      - src.endpoint = local endpoint that generated request
 *      - hdr.seqNum = request sequence number
 *      - hdr.cmdId = request command id
 */
struct ZbZclCommandRspT {
    enum ZbStatusCodeT aps_status; /**< APS-DATA.confirm status */
    enum ZclStatusCodeT status;
    /**< Status of the ZCL response. If Default Response, it is the status code found
     * within the response. If a cluster-specific response, it is set to ZB_STATUS_SUCCESS,
     * and the application must parse the payload, if any, for any embedded status. */
    struct ZbApsAddrT src;
    uint16_t profileId;
    enum ZbZclClusterIdT clusterId;
    uint8_t linkQuality;
    struct ZbZclHeaderT hdr;
    uint8_t *payload;
    uint16_t length;
};

#define ZCL_REQ_CMD_RSP_ID_IGNORE           0xffU

/**
 * Send a ZCL command. The callback function is called when the
 * associated ZCL response is received, or there's an error.
 * @param zb Zigbee stack instance
 * @param zclreq ZCL Command Request info
 * @param rspCmd The Command Id of the response to wait for, which is added to the APS filter.
 * This may be set to ZCL_REQ_CMD_RSP_ID_IGNORE, in which case only the matching ZCL sequence
 * number, direction bit, etc, are used. The ZCL Default Response is always added to the APS filter.
 * @param callback Callback function to receive ZCL Response
 * @param arg Callback argument
 * @return ZCL Status Code. ZCL_STATUS_SUCCESS if command was sent and caller can wait for
 * the callback to be called. Otherwise, there was a problem sending the command and the
 * callback is never called in this case.
 */
enum ZclStatusCodeT ZbZclCommandReqWithRspFilter(struct ZigBeeT *zb, struct ZbZclCommandReqT *zclReq,
    uint8_t rspCmd, void (*callback)(struct ZbZclCommandRspT *rsp, void *arg), void *arg);

/* For backward compatibility with previous API that did not filter explicitly for the
 * ZCL Response Command Id.
 *
 * Old prototype (for searching):
 * enum ZclStatusCodeT ZbZclCommandReq(struct ZigBeeT *zb, struct ZbZclCommandReqT *zclReq,
        void (*callback)(struct ZbZclCommandRspT *rsp, void *arg), void *arg); */
#define ZbZclCommandReq(zb, zclReq, callback, arg) \
    ZbZclCommandReqWithRspFilter(zb, zclReq, ZCL_REQ_CMD_RSP_ID_IGNORE, callback, arg)

/*---------------------------------------------------------------
 * Attribute Reporting Intervals
 *---------------------------------------------------------------
 */
/*** Reporting Intervals ***/
/* If max interval == 0xffff, then reporting is disabled */
#define ZCL_ATTR_REPORT_MAX_INTVL_DISABLE   0xffffU
#define ZCL_ATTR_REPORT_MIN_INTVL_DISABLE   0x0000U /* ignored, refer to ZCL_ATTR_REPORT_MAX_INTVL_DISABLE */

/* If max interval == 0x0000, then reporting is enabled, but only when attribute changes, no timer. */
#define ZCL_ATTR_REPORT_MAX_INTVL_CHANGE    0x0000U

/* BDB Spec Section 6.7: Maximum reporting interval is either 0x0000 or in the range 0x003d to 0xfffe */
#define ZCL_ATTR_REPORT_MAX_INTVL_MINIMUM   0x003dU /* 61 seconds */
#define ZCL_ATTR_REPORT_MAX_INTVL_MAXIMUM   0xfffeU /* 18 hours, 12 minutes, 14 seconds */

/* If max interval == 0x0000 and min interval == 0xffff, then reset reporting back to default. */
#define ZCL_ATTR_REPORT_MAX_INTVL_DEFAULT   0x0000U
#define ZCL_ATTR_REPORT_MIN_INTVL_DEFAULT   0xffffU

/* Default reporting intervals that satisfy the BDB requirements. From BDB 3.0.1:
 *
 * Section 6.7 Default reporting configuration
 * A default report configuration (with a maximum reporting interval either of 0x0000 or in the
 * range 0x003d to 0xfffe) SHALL exist for every implemented attribute that is specified as mandatory
 * and reportable in either the ZCL [R2] or the corresponding device description in the ZDL [R8].
 * The default reporting configuration is such that if a binding is created on the node to a given
 * cluster the node SHALL send reports to that binding without any additional reporting configuration
 * needing to be set. The default reporting configuration for an attribute MAY be overwritten at any
 * time. In this case, the updated reporting configuration SHALL be used.
 *
 * NOTE: There is no consensus on what these default intervals should be, and the application
 * is responsible for fine-tuning the reporting configuration for its attributes. Let's set
 * something to give a reasonable "out of the box" user experience.
 */
#define BDB_REPORT_MIN_INTVL_DEFAULT        60U /* 1 minute. */
#define BDB_REPORT_MAX_INTVL_DEFAULT        300U /* 5 minutes. Must be greater than 60 seconds (if not zero) */

/*---------------------------------------------------------------
 * Cluster Attributes
 *---------------------------------------------------------------
 */

#define ZCL_ATTRIBUTE_BUFFER_SIZE_MAX       256U

struct ZbZclAttrCbInfoT;

/**
 * The structure used to initialize a ZCL attribute when calling ZbZclAttrAppendList.
 */
struct ZbZclAttrT {
    uint16_t attributeId; /**< The attribute ID */

    enum ZclDataTypeT dataType;
    /**< The attribute data type (ZCL_DATATYPE_UNKNOWN if last entry) */

    ZclAttrFlagT flags; /**< Attribute flags (e.g. writeable, reportable, etc) */

    unsigned int customValSz;
    /**< A custom size is required if the data type is one of:
     *    ZCL_DATATYPE_STRING_OCTET,
     *    ZCL_DATATYPE_STRING_CHARACTER,
     *    ZCL_DATATYPE_STRING_LONG_OCTET,
     *    ZCL_DATATYPE_STRING_LONG_CHARACTER,
     *    ZCL_DATATYPE_ARRAY,
     *    ZCL_DATATYPE_STRUCT,
     *    ZCL_DATATYPE_SET,
     *    ZCL_DATATYPE_BAG
     *
     * A custom size is also required if both customRead and customWrite callbacks
     * are provided and ZCL_ATTR_FLAG_PERSISTABLE is set.
     *
     * The maximum size should not exceed ZB_APS_CONST_MAX_FRAG_SIZE. */

    enum ZclStatusCodeT (*callback)(struct ZbZclClusterT *cluster, struct ZbZclAttrCbInfoT *info);
    /**< If flags ZCL_ATTR_FLAG_CB_READ or ZCL_ATTR_FLAG_CB_WRITE are set, then
     * this callback is called for attribute read or write commands. */

    struct {
        double min;
        double max;
    } range;
    /**< Optional integer attribute value range. If both are set to zero, range checking is disabled. */

    struct {
        uint16_t interval_min; /**< Default minimum reporting interval in seconds. */
        uint16_t interval_max; /**< Default maximum reporting interval in seconds. */
    } reporting;
    /**< Default attribute minimum and maximum reporting intervals.
     *
     * See ZCL 7 sections 2.5.7.1.5 and 2.5.7.1.6 regarding the minimum and maximum
     * reporting interval values and what they mean.
     *
     * From BDB section 6.7 (Default reporting configuration), the maximum reporting
     * interval is either 0x0000 (on value change only) or between 0x003d and 0xfffe.
     */
};

#define ZCL_ATTR_LIST_LEN(_list_)        (sizeof(_list_) / sizeof(struct ZbZclAttrT))

/* Appends attributes to the cluster.
 * Important note: attrList memory is attached to the cluster, so
 * it cannot be freed until the cluster is freed. */
enum ZclStatusCodeT ZbZclAttrAppendList(struct ZbZclClusterT *cluster,
    const struct ZbZclAttrT *attrList, unsigned int num_attrs);

/** Attribute Callback Types */
enum ZbZclAttrCbTypeT {
    ZCL_ATTR_CB_TYPE_READ,
    /**< Read attribute callback. Use to have the application handle
     * the ZCL Read Requests, rather than the cluster. */
    ZCL_ATTR_CB_TYPE_WRITE
    /**< Write attribute callback. Used to have the application handle
     * the ZCL Write Requests, rather than the cluster. If not used, the
     * cluster will handle the incoming requests. */
};

/** Attribute Callback Information */
struct ZbZclAttrCbInfoT {
    const struct ZbZclAttrT *info;
    /**< Pointer to the original info struct used to create the attribute */

    enum ZbZclAttrCbTypeT type; /**< Callback type (read, write, etc) */

    uint8_t *zcl_data;
    /**< Incoming or outgoing ZCL attribute payload data.
     *
     * If callback type is ZCL_ATTR_CB_TYPE_WRITE, this is the source of the new value,
     * to be validated and written back to attr_data if applicable.
     *
     * If callback type is ZCL_ATTR_CB_TYPE_READ, this is the destination of the value
     * to return in the ZCL Read Response. */

    unsigned int zcl_len; /**< Maximum length of 'zcl_data' */

    ZclWriteModeT write_mode;
    /**< Write mode (e.g. normal, test, force, persist).
     * Only applicable if type == ZCL_ATTR_CB_TYPE_WRITE. */

    void *attr_data;
    /**< Pointer to local attribute storage.
     * Only applicable if type == ZCL_ATTR_CB_TYPE_WRITE.
     *
     * If NULL, then application is maintaining attribute value
     * (i.e. both ZCL_ATTR_FLAG_CB_READ and ZCL_ATTR_FLAG_CB_WRITE were set).
     *
     * If not NULL, current attribute value can be read from here, and
     * updated value from WRITE command to be written here.
     */

    const struct ZbApsAddrT *src;
    /**< Source of the command, if not locally generated. May be NULL. */

    enum ZbStatusCodeT security_status;
    /**< security_status - The security level used to decrypt the incoming frame.
     * Only valid if 'src' address parameter is not NULL. If present, it will be
     * one of:
     * ZB_APS_STATUS_UNSECURED (no decryption necessary),
     * ZB_APS_STATUS_SECURED_NWK_KEY (decrypted with the Network Key),
     * ZB_APS_STATUS_SECURED_LINK_KEY, (decrypted with a Link Key) */

    void *app_cb_arg; /**< Application's defined callback argument */
};

/* Returns the length of an attribute, solely based on type. Or 0, if length is unknown. */
unsigned int ZbZclAttrTypeLength(enum ZclDataTypeT type);

/* Returns the attribute length, or -1 on error. */
int ZbZclAttrDefaultValue(enum ZclDataTypeT type, uint8_t *buf, unsigned int max_len);

/*---------------------------------------------------------------
 * Alarm Cluster Helpers
 *---------------------------------------------------------------
 */
/**
 * ZbZclClusterReverseBind is used by the Alarms Server so it can receive loopback
 * messages sent in the direction of the client (i.e. reverse).
 * @param cluster Cluster instance
 * @return Filter pointer (handle)
 */
struct ZbApsFilterT * ZbZclClusterReverseBind(struct ZbZclClusterT *cluster);

/**
 * Remove the reverse (loopback) filter and free it.
 * @param cluster Cluster instance
 * @param filter Filter to remove and free
 * @return None
 */
void ZbZclClusterReverseUnbind(struct ZbZclClusterT *cluster, struct ZbApsFilterT *filter);

/**
 * Send an alarm from a cluster as if it originated from the alarms cluster
 * on the same endpoint. Typically, src_endpoint = cluster->endpoint.
 * If the cluster appears on multiple endpoints (e.g. Basic Cluster), the src_endpoint
 * is required to know which one to actually send the Alarm Command from.
 * The alarm message is sent via APS binding(s).
 * @param cluster Cluster instance
 * @param src_endpoint
 * @param alarm_code
 * @return None
 */
void ZbZclClusterSendAlarm(struct ZbZclClusterT *cluster, uint8_t src_endpoint, uint8_t alarm_code);

/**
 * Register a callback handler for the given cluster to receive Alarm "Reset Alarm"
 * and "Reset all alarms" commands.
 * @param cluster Cluster instance
 * @param callback Application callback to handle reset alarm(s) commands.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclClusterRegisterAlarmResetHandler(struct ZbZclClusterT *cluster,
    enum ZclStatusCodeT (*callback)(struct ZbZclClusterT *cluster, uint8_t alarm_code,
        uint16_t cluster_id, struct ZbApsdeDataIndT *data_ind, struct ZbZclHeaderT *hdr));
void ZbZclClusterRemoveAlarmResetHandler(struct ZbZclClusterT *cluster);

/*---------------------------------------------------------------
 * Cluster Base Type
 *---------------------------------------------------------------
 */
/* This structure describes a ZCL cluster */
struct ZbZclClusterT {
    struct LinkListT link; /**< Optional linking for application use. */
    struct ZigBeeT *zb; /**< ZigBee stack reference. */

    /*-------------------------------------------
     * Cluster Basics
     *-------------------------------------------
     */
    enum ZbZclClusterIdT clusterId;
    uint8_t endpoint;
    uint16_t mfrCode;
    /* minSecurity is one of: ZB_APS_STATUS_UNSECURED, ZB_APS_STATUS_SECURED_NWK_KEY or ZB_APS_STATUS_SECURED_LINK_KEY */
    enum ZbStatusCodeT minSecurity;
    uint16_t profileId;
    uint16_t txOptions; /* e.g. ZB_APSDE_DATAREQ_TXOPTIONS_ACK */
    bool discoverRoute;
    uint8_t radius;
    uint16_t maxAsduLength; /* with fragmentation, if enabled in txOptions. */
    /* Direction of incoming commands. I.e. the type of cluster this is
     * (client or server). */
    enum ZbZclDirectionT direction;

    /*-------------------------------------------
     * Filters, Lists, Timers
     *-------------------------------------------
     */
    struct ZbApsFilterT *dataind_filter; /**< Filter to receive packets destined for this cluster */
    struct LinkListT attributeList; /**< List of 'struct ZbZclAttrListEntryT' entries */
    struct {
        struct ZbMsgFilterT *reset_filter; /**< Filter to receive message for resetting reports back to defaults */
        struct LinkListT list; /* List of "struct ZbZclReportT" reports */
        struct ZbTimerT *timer; /* Report timer */
        bool kicked;

        /* ZbZclClusterReportsSend */
        bool send_all;
        ZbUptimeT last_send_all;

        /**< Set to true if application called ZbZclClusterReportsSend. Otherwise, only send reports based
         * on the attribute's reporting interval and change threshold.  */
        void (*callback)(struct ZbZclClusterT *cluster, unsigned int next_timeout, void *arg);
        /**< Callback saved from ZbZclClusterReportsSend, which is called once the
         * reports have been sent. */
        void *arg;
    } reports;
    struct {
        struct ZbApsFilterT *reset_filter; /**< Filter to receive message for resetting alarms back to defaults */
        enum ZclStatusCodeT (*reset_callback)(struct ZbZclClusterT *cluster, uint8_t alarm_code,
            uint16_t cluster_id, struct ZbApsdeDataIndT *data_ind, struct ZbZclHeaderT *hdr);
    } alarm;

    struct ZbTimerT *persist_timer;
    bool persist_immed;

    /*-------------------------------------------
     * Callbacks
     *-------------------------------------------
     */
    /* The application argument passed through the various callbacks
     * to the application (e.g. custom attribute read/write). */
    void *app_cb_arg;

    /* Callback to handle a Profile-Wide (ZCL_FRAMETYPE_PROFILE) command.
     * Return value is a ZCL_STATUS_ code. If not ZCL_STATUS_SUCCESS, a default
     * response with the status code is sent to the originator of the command. */
    void (*profile_cb)(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdr,
        struct ZbApsdeDataIndT *dataInd);

    /* Callback to handle a Cluster-Specific (ZCL_FRAMETYPE_CLUSTER) command.
     * Return value is a ZCL_STATUS_ code. If not ZCL_STATUS_SUCCESS, a default
     * response with the status code is sent to the originator of the command. */
    enum ZclStatusCodeT (*command)(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdr,
        struct ZbApsdeDataIndT *dataInd);

    /* Callback to receive ZCL Report Commands. Configured by calling ZbZclClusterReportCallbackAttach.
     * Attribute reporting can be configured on the remote device by calling ZbZclAttrReportConfigReq.
     * When the remote device sends reports, those reports can be received through this callback. */
    void (*report)(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdr, struct ZbApsdeDataIndT *dataInd,
        uint16_t attributeId, enum ZclDataTypeT dataType, const uint8_t *in_payload, uint16_t in_len, bool *discard);

    /* Callback invoked on sending the ZCL Report commands. Configured by calling ZbZclClusterReportConfirmCallbackAttach.
     * Called only if periodic timers are enabled and reporting is managed by stack. This is primarily used by simple
     * meterering cluster server to check if it expects Mirror Report attribute response from client cluster and if yes,
     * to enable polling to receive the same. */
    void (*report_confirm)(struct ZbZclClusterT *cluster);

    /* ZCL Internal Callback for the Scenes Cluster to Get a cluster's attribute
     * extension set.
     * extBuf has the format: [cluster (2 octets) | length (1 octet) | attribute data (N octets)]
     *
     * Returns length written to extBuf.  */
    uint8_t (*get_scene_data)(struct ZbZclClusterT *cluster, uint8_t *extBuf, uint8_t extMaxLen);

    /* ZCL Internal Callback for the Scenes Cluster to Set a cluster's attribute
     * extension set.#define ZCL_BUILD_ADD_UINT8(_payload_, _max_len_, _index_, _arg_)  \
        do {                                                             \
            if (((_index_) + sizeof(uint8_t)) > (_max_len_)) {           \
                return -1;                                               \
            }                                                            \
            if (_payload_) {                                             \
                (_payload_)[_index_] = _arg_;                            \
            }                                                            \
            _index_ += 1U;                                  \
     * extData has the format: [attribute data (N octets)]
     * extLen is the length of extData
     * transition_tenths ; transition time in tenths of a second.
     *
     * Returns ZCL status code (enum ZclStatusCodeT) */
    enum ZclStatusCodeT (*set_scene_data)(struct ZbZclClusterT *cluster, uint8_t *extData, uint8_t extLen, uint16_t transition_tenths);

    /* cleanup() is called to free any cluster-specific data.
     * ZbZclClusterFree will already free any attributes and APS filter allocated by
     * ZbZclClusterAlloc and ZbZclAttrAppendList. */
    void (*cleanup)(struct ZbZclClusterT *cluster);
};

/*---------------------------------------------------------------
 * Cluster Initialization
 *---------------------------------------------------------------
 */
/* Helper to create a ZCL endpoint. Calls ZbApsmeAddEndpoint to create the endpoint. Sets up a
 * filter to catch any ZCL commands for a cluster that doesn't exist on the endpoint, and
 * returns ZCL_STATUS_UNSUPP_CLUSTER. Sets up filter bindings for the Basic Server
 * cluster (global stack clusters) and register's their cluster IDs with the endpoint
 * (e.g. for ZDO Match_Desc_req)  */
void ZbZclAddEndpoint(struct ZigBeeT *zb, struct ZbApsmeAddEndpointReqT *req, struct ZbApsmeAddEndpointConfT *conf);
/* ZbZclAddEndpointNoBasic is the same as ZbZclAddEndpoint, except it does not allocate
 * a Basic Server cluster for the endpoint. */
void ZbZclAddEndpointNoBasic(struct ZigBeeT *zb, struct ZbApsmeAddEndpointReqT *req, struct ZbApsmeAddEndpointConfT *conf);

/* Helper to remove a ZCL endpoint created by ZbZclAddEndpoint.
 * Calls ZbApsmeRemoveEndpoint and removes the ZCL filters created by ZbZclAddEndpoint. */
void ZbZclRemoveEndpoint(struct ZigBeeT *zb, struct ZbApsmeRemoveEndpointReqT *req, struct ZbApsmeRemoveEndpointConfT *conf);

/* Allocate and initialize a new cluster */
void * ZbZclClusterAlloc(struct ZigBeeT *zb, unsigned int alloc_sz, enum ZbZclClusterIdT cluster_id,
    uint8_t endpoint, enum ZbZclDirectionT direction);
/* Attach the cluster to the stack (after callbacks have been configured) */
enum ZclStatusCodeT ZbZclClusterAttach(struct ZbZclClusterT *cluster);
/* Free and detach the cluster */
void ZbZclClusterFree(struct ZbZclClusterT *cluster);

/* Register this cluster's ID on the endpoint if not already listed. */
bool ZbZclClusterEndpointRegister(struct ZbZclClusterT *cluster);
bool ZbZclClusterEndpointRemove(struct ZbZclClusterT *cluster);

/*---------------------------------------------------------------
 * Building and Parsing Frames
 *---------------------------------------------------------------
 */
/** Parses a ZCL Header from a received frame.
 * Returns Length of the ZCL header, or negative value (-1) on error. */
int ZbZclParseHeader(struct ZbZclHeaderT *zclHdr, const uint8_t *buf, unsigned int len);

/** Builds and appends a ZCL Header to the start of a buffer.
 * Returns length of data written, or negative value (-1) on error. */
int ZbZclPrependHeader(struct ZbZclHeaderT *zclHdr, uint8_t *data, unsigned int len);

/** Builds and appends a ZCL Header to the end of a buffer.
 * Returns Length of data written, or negative value (-1) on error. */
int ZbZclAppendHeader(struct ZbZclHeaderT *zclHdr, uint8_t *data, unsigned int max_len);

/*---------------------------------------------------------------
 * Attribute Helpers
 *---------------------------------------------------------------
 */
bool ZbZclAttrIsInteger(enum ZclDataTypeT dataType);
bool ZbZclAttrIsFloat(enum ZclDataTypeT dataType);
bool ZbZclAttrIsAnalog(enum ZclDataTypeT dataType);

/* Based on attribute data type and attribute data (if necessary), determine the
 * length of the attribute. */
int ZbZclAttrParseLength(enum ZclDataTypeT type, const uint8_t *ptr,
    unsigned int max_len, uint8_t recurs_depth);

/**
 * Parses a integer number from a buffer starting at the given pointer and returns it as a
 * long long integer. Note that the length of the buffer isn't checked. The caller is
 * assumed to have sanity-checked the buffer length already by a call to ZbZclAttrParseLength
 * or ZbZclAttrTypeLength
 * @param dataType ZCL Attribute data type.
 * @param data ZCL attribute data.
 * @param status ZCL Status Code (OUT)
 * @return The parsed value, if *status == SUCCESS
 */
long long ZbZclParseInteger(enum ZclDataTypeT dataType, const uint8_t *data, enum ZclStatusCodeT *status);

/* Write a float value to the output 'data' pointer in ZCL format based on the attribute data type. */
int ZbZclAppendFloat(double value, enum ZclDataTypeT dataType, uint8_t *data, unsigned int maxlen);
double ZbZclParseFloat(enum ZclDataTypeT dataType, const uint8_t *data, enum ZclStatusCodeT *status);

/*---------------------------------------------------------------
 * Local Attribute Read/Write
 *---------------------------------------------------------------
 */
/**
 * Reads a local cluster's attribute value. Returns the attribute type if provided as
 * a pointer through 'attrType'.
 * @param cluster Cluster instance
 * @param attrId Attribute Id of the attribute to read
 * @param attrType Pointer to the attribute type. If not NULL and this function returns
 * ZCL_STATUS_SUCCESS, the type value of the attribute is returned in this parameter.
 * @param outputBuf Buffer to write attribute value data to.
 * @param max_len Maximum length of 'outputBuf'.
 * @param isReporting If true and this attribute is being read for generating a report,
 * the attribute's flag is checked for ZCL_ATTR_FLAG_REPORTABLE and if not set, a status
 * of ZCL_STATUS_UNREPORTABLE_ATTRIBUTE is returned.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrRead(struct ZbZclClusterT *cluster, uint16_t attrId, enum ZclDataTypeT *attrType,
    void *outputBuf, unsigned int max_len, bool isReporting);

/**
 * Writes a local cluster's attribute value.
 * @param cluster Cluster instance
 * @param src Source address of this command. May be NULL if generated locally.
 * @param attr_id Attribute Id of the attribute to write
 * @param attr_data Attribute data in ZCL format (i.e. same as what is sent over-the-air)
 * @param max_len Maximum length of attribute data (attr_data) buffer.
 * @param mode Write mode
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrWrite(struct ZbZclClusterT *cluster, const struct ZbApsAddrT *src, uint16_t attr_id,
    const uint8_t *attr_data, unsigned int max_len, ZclWriteModeT mode);

/**
 * Helper to ZbZclAttrRead to read an integer attribute value.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param type If pointer provided, returns the attribute type.
 * @param status ZCL Status Code that can be returned to caller if pointer provided.
 * @return Value of attribute
 */
long long ZbZclAttrIntegerRead(struct ZbZclClusterT *cluster, uint16_t attributeId,
    enum ZclDataTypeT *type, enum ZclStatusCodeT *status);

/**
 * Helper to ZbZclAttrWrite to write an integer attribute value.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param value The value to write to the attribute.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrIntegerWrite(struct ZbZclClusterT *cluster, uint16_t attributeId, long long value);

/**
 * Helper to ZbZclAttrWrite to read, increment and write an integer attribute value.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param value The value to increment the attribute by.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrIntegerIncrement(struct ZbZclClusterT *cluster, uint16_t attributeId, long long value);

/**
 * Helper to ZbZclAttrRead to read an EUI-64 attribute value.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param status ZCL Status Code that can be returned to caller if pointer provided.
 * @return The EUI-64 value
 */
uint64_t ZbZclAttrEuiRead(struct ZbZclClusterT *cluster, uint16_t attributeId, enum ZclStatusCodeT *status);

/**
 * Helper to ZbZclAttrWrite to write an EUI-64 attribute value.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param eui The EUI-64 value to write to the attribute.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrEuiWrite(struct ZbZclClusterT *cluster, uint16_t attributeId, uint64_t eui);

/**
 * Helper to ZbZclAttrWrite to write a string attribute value of type
 * ZCL_DATATYPE_STRING_OCTET or ZCL_DATATYPE_STRING_CHARACTER.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param zcl_str ZCL String. First byte is the string length.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrStringWriteShort(struct ZbZclClusterT *cluster, uint16_t attributeId,
    const uint8_t *zcl_str);

/**
 * Helper to ZbZclAttrWrite to write a string attribute value of type
 * ZCL_DATATYPE_STRING_LONG_OCTET or ZCL_DATATYPE_STRING_LONG_CHARACTER.
 * @param cluster Cluster instance
 * @param attributeId The attribute Id
 * @param zcl_str ZCL String. First two bytes is the string length.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrStringWriteLong(struct ZbZclClusterT *cluster, uint16_t attributeId,
    const uint8_t *zcl_str);

/* Zero-length ZCL strings */
extern const uint8_t zcl_attr_str_short_zero[1]; /* i.e. {0x00} */
extern const uint8_t zcl_attr_str_long_zero[2]; /* i.e. {0x00, 0x00} */

/*---------------------------------------------------------------
 * Remote Attribute Read/Write
 *---------------------------------------------------------------
 */
/* The maximum number of attributes we support reading/writing through the API at once.
 * This is not the maximum that we support from requests arriving from the network.
 * Those depend on maximum ASDU limits and other resources. */
#define ZCL_ATTRIBUTE_LIST_MAX_SZ       32U

struct ZbZclReadReqT {
    struct ZbApsAddrT dst;
    unsigned int count;
    uint16_t attr[ZCL_ATTRIBUTE_LIST_MAX_SZ];
};

struct ZbZclReadRspDataT {
    enum ZclStatusCodeT status;
    uint16_t attrId;
    enum ZclDataTypeT type;
    const uint8_t *value; /* Value as-is over-the-air (strings include the length octet(s)) */
    unsigned int length;
};

struct ZbZclReadRspT {
    enum ZbStatusCodeT aps_status; /**< APS-DATA.confirm status */
    enum ZclStatusCodeT status;
    struct ZbApsAddrT src;
    uint16_t profileId;
    uint8_t linkQuality;
    unsigned int count;
    struct ZbZclReadRspDataT attr[ZCL_ATTRIBUTE_LIST_MAX_SZ];
};

enum ZclStatusCodeT ZbZclReadReq(struct ZbZclClusterT *cluster, struct ZbZclReadReqT *req,
    void (*callback)(const struct ZbZclReadRspT *rsp, void *arg), void *arg);

struct ZbZclWriteReqDataT {
    /* EXEGIN: implement a selector for structured version. */
    uint16_t attrId;
    enum ZclDataTypeT type;
    const uint8_t *value; /* in zigbee format (integers are little endian) */
    unsigned int length;
};

struct ZbZclWriteRspDataT {
    enum ZclStatusCodeT status;
    uint16_t attrId;
};

struct ZbZclWriteReqT {
    struct ZbApsAddrT dst;
    unsigned int count;
    struct ZbZclWriteReqDataT attr[ZCL_ATTRIBUTE_LIST_MAX_SZ];
};

struct ZbZclWriteRspT {
    enum ZbStatusCodeT aps_status; /**< APS-DATA.confirm status */
    enum ZclStatusCodeT status;
    struct ZbApsAddrT src;
    uint16_t profileId;
    uint8_t linkQuality;
    unsigned int count;
    struct ZbZclWriteRspDataT attr[ZCL_ATTRIBUTE_LIST_MAX_SZ];
};

enum ZclStatusCodeT ZbZclWriteReq(struct ZbZclClusterT *cluster, struct ZbZclWriteReqT *req,
    void (*callback)(const struct ZbZclWriteRspT *rsp, void *arg), void *arg);

/*---------------------------------------------------------------
 * Attribute Reporting
 *---------------------------------------------------------------
 */
enum ZbZclReportDirectionT {
    /* For Configure Reporting: receiver configures attribute to send
     * reports via binding table.
     *
     * For Read Reporting: specifies whether values of the attribute are reported */
    ZCL_REPORT_DIRECTION_NORMAL = 0x00,

    /* For Configure Reporting: sender is sending reports to the receiver,
     * based on the state of the sender's bindings. Tells receiver how / when
     * it should expect reports.
     *
     * For Read Reporting: specifies whether reports of the attribute are received. */
    ZCL_REPORT_DIRECTION_REVERSE = 0x01
};

/*---------------------------------------------------------------
 * Attribute Reporting
 *---------------------------------------------------------------
 */
/* Maximum number of Attribute reporting configuration records. */
#define ZCL_ATTR_REPORT_CONFIG_NUM_MAX          6U

/* Attribute reporting configuration record. Max size is 16 */
struct ZbZclAttrReportConfigRecordT {
    enum ZbZclReportDirectionT direction; /* e.g. ZCL_REPORT_DIRECTION_NORMAL. */
    uint16_t attr_id;
    enum ZclDataTypeT attr_type; /* Only if ZCL_REPORT_DIRECTION_NORMAL. */
    uint16_t min; /* Only if ZCL_REPORT_DIRECTION_NORMAL. */
    uint16_t max; /* Only if ZCL_REPORT_DIRECTION_NORMAL. */
    double change; /**< Reportable change. Only if ZCL_REPORT_DIRECTION_NORMAL. */
    uint16_t timeout_period; /* Only if ZCL_REPORT_DIRECTION_REVERSE. */
};

/* Configure Reporting Command payload entry format */
struct ZbZclAttrReportConfigT {
    struct ZbApsAddrT dst;
    uint8_t num_records;
    struct ZbZclAttrReportConfigRecordT record_list[ZCL_ATTR_REPORT_CONFIG_NUM_MAX];
};

/**
 * Send a Configure Reporting Command. Response is returned in the 'callback' if provided by application.
 * @param cluster Cluster instance
 * @param config Attribute reporting configuration destination and records
 * @param callback Callback function to receive Configure Reporting Response Command
 * @param arg Callback argument
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrReportConfigReq(struct ZbZclClusterT *cluster, struct ZbZclAttrReportConfigT *config,
    void (*callback)(struct ZbZclCommandRspT *cmd_rsp, void *arg), void *arg);

/* Attribute reporting configuration record. Max size is 16 */
struct ZbZclAttrReportReadRecordT {
    enum ZbZclReportDirectionT direction; /* e.g. ZCL_REPORT_DIRECTION_NORMAL. */
    uint16_t attr_id;
};

#define ZCL_ATTR_REPORT_READ_NUM_MAX        ZCL_ATTR_REPORT_CONFIG_NUM_MAX

struct ZbZclAttrReportReadT {
    struct ZbApsAddrT dst;
    uint8_t num_records;
    struct ZbZclAttrReportConfigRecordT record_list[ZCL_ATTR_REPORT_READ_NUM_MAX];
};

/**
 * Send a Read Reporting Configuration Command. Response is returned in the 'callback' if provided by application.
 * @param cluster Cluster instance
 * @param read Attribute reporting configuration destination and records to read
 * @param callback Callback function to receive Read Reporting Configuration Response Command
 * @param arg Callback argument
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrReportReadReq(struct ZbZclClusterT *cluster, struct ZbZclAttrReportReadT *read,
    void (*callback)(struct ZbZclCommandRspT *cmd_rsp, void *arg), void *arg);

/**
 * Configure an attribute's default reporting intervals. It will also set the
 * active reporting intervals to these values and reset the reporting timer.
 * The Reportable Change value will also be reset back to defaults (value = 1).
 * @param cluster Cluster instance
 * @param attrId Attribute Id of the attribute to modify
 * @param default_min New default minimum reporting interval in seconds.
 * @param default_max New default maximum reporting interval in seconds.
 * @param default_change (Optional) New default reportable change value.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclAttrReportConfigDefault(struct ZbZclClusterT *cluster,
    uint16_t attrId, uint16_t default_min, uint16_t default_max, double *default_change);

/**
 * If the application disables the automatic attribute reporting mechanism in the stack by
 * setting the NIB 'ZB_NWK_NIB_ID_DisablePeriodicTimers' to 1, then the application will
 * need to manually and periodically send reports by calling this function.
 * @param cluster Cluster instance
 * @param send_all If true, send all reportable attributes no matter if they have changed or
 * reporting interval has timed-out. Normally, when used by a sleepy device application to
 * manually drive reporting, this parameter should be set to false.
 * @param callback Callback that is called once all reports have been sent. The 'next_timeout'
 * parameter is the time until the next scheduled report, in milliseconds.
 * @param arg Callback argument
 * @return ZCL Status Code. If not ZCL_STATUS_SUCCESS, then callback will not be called.
 */
enum ZclStatusCodeT ZbZclClusterReportsSend(struct ZbZclClusterT *cluster, bool send_all,
    void (*callback)(struct ZbZclClusterT *cluster, unsigned int next_timeout, void *arg), void *arg);

/* WARNING: ZbZclAttrReportKick is deprecated and has been replaced by ZbZclClusterReportsSend. */
#define ZbZclAttrReportKick ZbZclClusterReportsSend

/*---------------------------------------------------------------
 * Attribute Discovery
 *---------------------------------------------------------------
 */
struct ZbZclDiscoverAttrReqT {
    struct ZbApsAddrT dst;
    uint16_t startAttr;
    uint8_t max;
};

struct ZbZclDiscoverAttrRspDataT {
    uint16_t attrId;
    enum ZclDataTypeT type;
};

struct ZbZclDiscoverAttrRspT {
    enum ZclStatusCodeT status;
    struct ZbApsAddrT src;
    uint8_t discComplete;
    struct ZbZclDiscoverAttrRspDataT attrInfo[ZCL_ATTRIBUTE_LIST_MAX_SZ];
    uint8_t numAttr;
};

enum ZclStatusCodeT ZbZclDiscoverAttrReq(struct ZbZclClusterT *cluster, struct ZbZclDiscoverAttrReqT *req,
    void (*callback)(const struct ZbZclDiscoverAttrRspT *rsp, void *cb_arg), void *arg);

/*---------------------------------------------------------------
 * Attribute Persistence
 *---------------------------------------------------------------
 */

/**
 * The application must call this function to inform the stack to persist the new attribute data,
 * if the attribute is modified outside of the normal ZCL Write mechanism,
 * It will be included in the data the next time ZbPersistGet is called.
 * @param cluster Cluster instance
 * @param attr_id Attribute causing the persistence update.
 * @return True if persistence will be updated, false otherwise (e.g. attribute
 * does not have ZCL_ATTR_FLAG_PERSISTABLE flag set.
 */
bool ZbZclAttrPersist(struct ZbZclClusterT *cluster, uint16_t attr_id);

/*---------------------------------------------------------------
 * Cluster Request helpers
 *---------------------------------------------------------------
 */

/* Timeout for originating device of request */
/* EXEGIN 1 - make configurable? */
#define ZCL_BOMD_REQ_DEFAULT_TIMEOUT_MS         (10U * 60U * 1000U) /* 10 minutes */

/* Timeout for the mirror to keep a command queued for the BOMD.
 * Make this less than ZCL_BOMD_REQ_DEFAULT_TIMEOUT_MS so by default
 * we return a response to the originator before it times out. */
#define ZCL_BOMD_REQ_QUEUE_WATCHDOG_MS          (ZCL_BOMD_REQ_DEFAULT_TIMEOUT_MS - 10000U) /* 10 minutes - 10 seconds */

struct ZbZclClusterCommandReqT {
    /* Destination for command */
    struct ZbApsAddrT dst;
    /* Command ID */
    uint8_t cmdId;
    /* If noDefaultResp is TRUE, disables a Default Response on status = SUCCESS. */
    enum ZbZclNoDefaultResponseT noDefaultResp;
    /* ZCL Request Command Payload */
    uint8_t *payload;
    unsigned int length;
    /* Timeout in milliseconds to wait for response. If zero, then a
     * suitable default timeout will be used. */
    unsigned int timeout;
};

/**
 * ZbZclClusterCommandReq is a helper wrapper to ZbZclCommandReq.
 * Information from the cluster is used when sending the command (source addressing, APS TX Options).
 * @param cluster Cluster instance
 * @param req Request struct pointer
 * @param rspCmd The Command Id of the response to wait for, which is added to the APS filter.
 * This may be set to ZCL_REQ_CMD_RSP_ID_IGNORE, in which case only the matching ZCL sequence
 * number, direction bit, etc, are used. The ZCL Default Response is always added to the APS filter.
 * @param delay Delay in milliseconds. If non-zero, then a copy of the ASDU must be made, and the
 *  message is sent later, after the delay time expires.
 * @param callback Callback function. This may be set to NULL.
 * @param arg Callback argument. Provided to the callback function and used by the application.
 * @return ZCL Status Code. If ZCL_STATUS_SUCCESS, the caller can expect the callback once the
 * command is complete. If something other than ZCL_STATUS_SUCCESS, that indicates an error and
 * the callback is never called in that case.
 */
enum ZclStatusCodeT ZbZclClusterCommandReqWithRspFilter(struct ZbZclClusterT *cluster,
    struct ZbZclClusterCommandReqT *req, uint8_t rspCmd, unsigned int delay,
    void (*callback)(struct ZbZclCommandRspT *zcl_rsp, void *arg), void *arg);

#define ZbZclClusterCommandReq(cluster, req, delay, callback, arg) \
    ZbZclClusterCommandReqWithRspFilter(cluster, req, ZCL_REQ_CMD_RSP_ID_IGNORE, \
    delay, callback, arg)

/*---------------------------------------------------------------
 * Cluster Response helpers
 *---------------------------------------------------------------
 */
/* ZbZclClusterCommandRsp sends a cluster-specific response to the destination provided.
 * There is no checking if the originating request was sent to broadcast, etc. That
 * checking must be done by the caller before this function is called.
 *
 * Returns ZCL_STATUS_SUCCESS if the message was queued to the stack, or an error status
 * otherwise. */
enum ZclStatusCodeT ZbZclClusterCommandRsp(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *dst,
    uint8_t cmdId, struct ZbApsBufT *payloads, uint8_t numPayloads);

enum ZclStatusCodeT ZbZclClusterCommandRspWithCb(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *dst,
    uint8_t cmdId, struct ZbApsBufT *payloads, uint8_t numPayloads,
    void (*callback)(struct ZbApsdeDataConfT *conf, void *arg), void *arg);

/* ZbZclSendClusterStatusResponse is a wrapper to ZbZclClusterCommandRsp. The parameters
 * zclPayload and zclPaylen must be populated and the first byte must represent a status
 * code of some sort with success == 0x00. Most ZCL respones follow this format. If the
 * request was broadcast (address or endpoint) and status != 0x00 (ZCL_STATUS_SUCCESS),
 * then the response will not be sent. This simplifies the cluster application from having
 * to make these checks. */
enum ZclStatusCodeT ZbZclSendClusterStatusResponse(struct ZbZclClusterT *cluster,
    struct ZbApsdeDataIndT *ind, struct ZbZclHeaderT *hdr, uint8_t cmdId,
    uint8_t *zclPayload, uint8_t zclPaylen, bool allow_bcast);

/* Send a Default Response. */
void ZbZclSendDefaultResponse(struct ZbZclClusterT *cluster, struct ZbApsdeDataIndT *ind,
    struct ZbZclHeaderT *hdr, enum ZclStatusCodeT status);

/* Different version of ZbZclSendDefaultResponse, but takes in ZbZclAddrInfoT rather than
 * struct ZbApsdeDataIndT and ZbZclHeaderT */
void ZbZclSendDefaultResponse2(struct ZbZclClusterT *cluster, struct ZbZclAddrInfoT *dst,
    uint8_t cmdId, enum ZclStatusCodeT status);

/*---------------------------------------------------------------
 * Bind a cluster so it can receive messages directly from the APS layer
 *---------------------------------------------------------------
 */
/* Binds the cluster structure to a particular endpoint and optional remote
 * address, and sets up a packet filter to handle the command reception. */
enum ZclStatusCodeT ZbZclClusterBind(struct ZbZclClusterT *cluster, uint8_t endpoint,
    uint16_t profileId, enum ZbZclDirectionT direction);
void ZbZclClusterUnbind(struct ZbZclClusterT *cluster);

/*---------------------------------------------------------------
 * Device Log (whitelist)
 *---------------------------------------------------------------
 */
/* Since the Trust Center address is not known ahead of time, joiners using CBKE will automatically
 * add the Trust Center to the Device Log, so they can perform CBKE if Device Log is enabled.
 * The TC is removed if CBKE fails. */
void ZbZclDeviceLogEnable(struct ZigBeeT *zb, bool enable);
bool ZbZclDeviceLogAdd(struct ZigBeeT *zb, uint64_t ext_addr);
bool ZbZclDeviceLogRemove(struct ZigBeeT *zb, uint64_t ext_addr);
void ZbZclDeviceLogClear(struct ZigBeeT *zb);

/*---------------------------------------------------------------
 * Helper Functions
 *---------------------------------------------------------------
 */
/* Get the current uptime from ZB_APS_IB_ID_CHANNEL_TIMER (ZbUptime) */
ZbUptimeT ZbZclUptime(struct ZigBeeT *zb);

/* Get the next ZCL sequence number to use in a request/notify message. */
uint8_t ZbZclGetNextSeqnum(struct ZigBeeT *zb);

/* Sets the profile Id for the cluster, which can affect the filter rules
 * for receiving packets based on cluster and profile Id. */
void ZbZclClusterSetProfileId(struct ZbZclClusterT *cluster, uint16_t profileId);

/* Sets the minimum security level for incoming frames, and also adjusts
 * the tx options to match (assuming symmetrical security).
 * minSecurity can be one of: ZB_APS_STATUS_UNSECURED, ZB_APS_STATUS_SECURED_NWK_KEY or ZB_APS_STATUS_SECURED_LINK_KEY  */
bool ZbZclClusterSetMinSecurity(struct ZbZclClusterT *cluster, enum ZbStatusCodeT minSecurity);

/* Set the maximum ASDU length for the cluster. The ASDU length includes
 * fragmentation, if enabled in the cluster's tx options. */
bool ZbZclClusterSetMaxAsduLength(struct ZbZclClusterT *cluster, uint16_t maxAsduLength);

/* Helper functions to initialize requests based on cluster parameters */
void ZbZclClusterInitCommandReq(struct ZbZclClusterT *cluster, struct ZbZclCommandReqT *cmdReq);
void ZbZclClusterInitApsdeReq(struct ZbZclClusterT *cluster, struct ZbApsdeDataReqT *apsReq, struct ZbApsdeDataIndT *dataInd);

/* Helper to generate proper APS TX Options (e.g. ZB_APSDE_DATAREQ_TXOPTIONS_SECURITY)
 * for a response, based on the given (incoming message) security status code. */
uint16_t ZbZclTxOptsFromSecurityStatus(enum ZbStatusCodeT security_status);

/**
 * Attach a callback function to receive ZCL Attribute Report Commands.
 * @param cluster Cluster instance
 * @param callback Application callback to handle Report commands.
 * @return None
 */
void ZbZclClusterReportCallbackAttach(struct ZbZclClusterT *cluster,
    void (*callback)(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdr,
        struct ZbApsdeDataIndT *dataInd, uint16_t attr_id, enum ZclDataTypeT data_type,
        const uint8_t *in_payload, uint16_t in_len, bool *discard));

/**
 * Attach a confirm callback function to be called on sending ZCL Attribute Report Commands.
 * @param cluster Cluster instance
 * @param callback Application callback to handle Report commands.
 * @return None
 */
void ZbZclClusterReportConfirmCallbackAttach(struct ZbZclClusterT *cluster,
    void (*callback)(struct ZbZclClusterT *cluster));

/**
 * This is a helper function that inspects a ZCL header struct and returns true if
 * it's for a ZCL Default Response command, or false otherwise.
 * @param hdr ZCL Header struct
 * @return True if ZCL header indicates it's for a ZCL Default Response command, or false otherwise.
 */
bool ZbZclIsDefaultRsp(struct ZbZclHeaderT *hdr);

/**
 * This is a helper function to convert an ascii string of hexidecimal characters
 * to an array of binary octets. The input string can have upper or lower case
 * hex characters, and must not contain any delimiters. Since this is a helper
 * function, it may change or be removed in the future.
 * @param str Input ascii string to convert, null terminated.
 * @param out Output buffer to write converted binary data to.
 * @param maxlen Maximum length of output buffer. If output length would be
 * longer than the maximum length, then -1 is returned.
 * @return Number of bytes written to 'out' or -1 on error.
 */
int zcl_hexstr_to_bin(const char *str, uint8_t *out, unsigned int maxlen);

/*---------------------------------------------------------------
 * Deprecated Helper APIs. For backward compatibility only.
 *---------------------------------------------------------------
 */
/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline void
ZbZclClusterSetCallbackArg(struct ZbZclClusterT *cluster, void *app_cb_arg)
{
    cluster->app_cb_arg = app_cb_arg;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline void
ZbZclClusterSetMfrCode(struct ZbZclClusterT *cluster, uint16_t mfrCode)
{
    cluster->mfrCode = mfrCode;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline void
ZbZclClusterSetTxOptions(struct ZbZclClusterT *cluster, uint16_t txOptions)
{
    cluster->txOptions = txOptions;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline void
ZbZclClusterSetDiscoverRoute(struct ZbZclClusterT *cluster, bool discoverRoute)
{
    cluster->discoverRoute = discoverRoute;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline void
ZbZclClusterSetRadius(struct ZbZclClusterT *cluster, uint8_t radius)
{
    cluster->radius = radius;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline uint8_t
ZbZclClusterGetEndpoint(struct ZbZclClusterT *cluster)
{
    return cluster->endpoint;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline enum ZbZclClusterIdT
ZbZclClusterGetClusterId(struct ZbZclClusterT *cluster)
{
    return cluster->clusterId;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline uint16_t
ZbZclClusterGetProfileId(struct ZbZclClusterT *cluster)
{
    return cluster->profileId;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline uint16_t
ZbZclClusterGetTxOptions(struct ZbZclClusterT *cluster)
{
    return cluster->txOptions;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline enum ZbZclDirectionT
ZbZclClusterGetDirection(struct ZbZclClusterT *cluster)
{
    return cluster->direction;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline uint8_t
ZbZclClusterGetRadius(struct ZbZclClusterT *cluster)
{
    return cluster->radius;
}

/* WARNING: This API is deprecated. Just access the cluster parameters directly. */
static inline unsigned int
ZbZclClusterGetMaxAsduLength(struct ZbZclClusterT *cluster)
{
    return cluster->maxAsduLength;
}

/*---------------------------------------------------------------
 * Exegin Internal ZCL Loopback Commands
 *---------------------------------------------------------------
 */
/* Our internal Manufacturer Code (ZCL_FRAMECTRL_MANUFACTURER).
 * This code is never sent over the air. */
enum {
    ZCL_MANUF_CODE_INTERNAL = 0xfffe
};

/* Offsets into ZCL_CMD_MANUF_INTERNAL_SET_SCENE_EXTDATA message */
#define SET_SCENE_EXTDATA_OFFSET_TRANSITION             0U /* 4 octets (tenths of a second) */
#define SET_SCENE_EXTDATA_OFFSET_EXT_LEN                4U /* 1 octet */
#define SET_SCENE_EXTDATA_OFFSET_EXT_FIELD              5U /* variable length */
#define SET_SCENE_EXTDATA_HEADER_LEN                    (4U + 1U)

enum {
    ZCL_PERSIST_SVR_CMD_PUSH
};

/**
 * Helper function to return value from BDB parameter 'ZB_BDB_ZclBomdReqWaitTimeoutMs'
 * @param zb Zigbee stack instance
 * @return Timeout value in milliseconds
 */
uint32_t zcl_get_bomd_req_wait_timeout_ms(struct ZigBeeT *zb);

/**
 * Helper function to return the timeout value, based on the BDB parameter
 * 'ZB_BDB_ZclBomdReqQueueTimeoutMs'
 * @param zb Zigbee stack instance
 * @return ZbZclUptime() plus the ZB_BDB_ZclBomdReqQueueTimeoutMs value.
 */
ZbUptimeT zcl_get_bomd_req_queue_timeout_abs(struct ZigBeeT *zb);

/* Handles Profile-Wide commands (e.g. ZCL Read Request) */
void zcl_handle_profile_command(struct ZbZclClusterT *cluster, struct ZbZclHeaderT *zclHdr,
    struct ZbApsdeDataIndT *ind);

#ifdef __cplusplus
}
#endif

#endif
