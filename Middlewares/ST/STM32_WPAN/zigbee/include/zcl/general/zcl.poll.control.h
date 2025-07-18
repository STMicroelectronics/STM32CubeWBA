/**
 * @file zcl.poll.control.h
 * @heading Poll Control
 * @brief ZCL Poll Control cluster header
 * @copyright Copyright [2019 - 2025] Exegin Technologies Limited. All rights reserved.
 *
 * ZCL 8 section 3.16
 *
 * A Sleepy End Device (SED) may instantiate the Poll Control server to allow
 * external devices to control its polling behaviour, including a short interval of
 * rapid polling in order to communicate with the SED in a timely fashion. SEDs are
 * usually in their sleep state where they are not able to receive packets, and
 * devices may not know when those SEDs are awake and polling for data.
 * The Poll Control cluster is used to solve this dilemma.
 *
 * Devices with Poll Control clients can use Finding & Binding to create the bindings
 * necessary to communicate with the Poll Control servers. The Poll Control server will
 * perform periodic check-ins with any bound Poll Control clients, where the check-in
 * response may ask the SED to start fast polling to allow the server device to
 * exchange messages with it.
 */

#ifndef ZCL_POLL_CONTROL_H
# define ZCL_POLL_CONTROL_H

/* @PICS.ZCL.Poll.Control
 * POLL.S | Server | True
 * POLL.C | Client | True
 *
 * Server Attributes
 * POLL.S.A0000 | Check-inInterval | True
 * POLL.S.A0001 | LongPollInterval | True
 * POLL.S.A0002 | ShortPollInterval | True
 * POLL.S.A0003 | FastPollTimeout | True
 * POLL.S.A0004 | Check-inIntervalMin | True | Optional
 * POLL.S.A0005 | LongPollIntervalMin | True | Optional
 * POLL.S.A0006 | FastPollTimeoutMax | True | Optional
 * POLL.S.AFFFD | ClusterRevision | True
 * POLL.S.AFFFE | AttributeReportingStatus | True
 *
 * Commands Received
 * POLL.S.C00.Rsp | Check-in Response | True
 * POLL.S.C01.Rsp | Fast Poll Stop | True
 * POLL.S.C02.Rsp | Set Long Poll Interval | True
 * POLL.S.C03.Rsp | Set Short Poll Interval | True
 *
 * Commands Generated
 * POLL.S.C00.Tx | Check-in | True
 *
 * Client Attributes
 * POLL.C.AFFFD | ClusterRevision | True
 * POLL.C.AFFFE | AttributeReportingStatus | True
 *
 * Commands Received
 * POLL.C.C00.Rsp | Check-in | True
 *
 * Commands Generated
 * POLL.C.C00.Tx | Check-in Response | True
 * POLL.C.C01.Tx | Fast Poll Stop | True
 * POLL.C.C02.Tx | Set Long Poll Interval | True
 * POLL.C.C03.Tx | Set Short Poll Interval | True
 */

#include "zcl/zcl.h"

/** Poll Control Server Attribute IDs */
enum ZbZclPollControlSvrAttrT {
    ZCL_POLL_CHECK_IN_INTERVAL = 0x0000,
    /**< Check-inInterval. Range is (0, 0x6E0000), where 0 means disabled.
     * 0x6E0000 = 7208960 qs = ~500 hours.
     * Other than 0, the Min value is also governed by ZCL_POLL_CHECK_IN_INTERVAL_MIN. */

    ZCL_POLL_LONG_POLL_INTERVAL = 0x0001,
    /**< LongPollInterval. Range is (0x04, 0x6E0000) = (1 sec, ~500 hours).
     * Min value is also governed by the value of ZCL_POLL_LONG_POLL_INTERVAL_MIN. */

    ZCL_POLL_SHORT_POLL_INTERVAL = 0x0002,
    /**< ShortPollInterval. Range is (1, 4) = (250 ms, 1 sec) */

    ZCL_POLL_FAST_POLL_TIMEOUT = 0x0003,
    /**< FastPollTimeout. Range is (0x0001, 0xffff) = (250 ms, 4.55 hours).
     * Max value is also governed by the value of ZCL_POLL_FAST_POLL_TIMEOUT_MAX. */

    ZCL_POLL_CHECK_IN_INTERVAL_MIN = 0x0004,
    /**< Check-inIntervalMin (Optional). Range is (0, 0x6E0000) */

    ZCL_POLL_LONG_POLL_INTERVAL_MIN = 0x0005,
    /**< LongPollIntervalMin (Optional). Range is (0x4, 0x6E0000) */

    ZCL_POLL_FAST_POLL_TIMEOUT_MAX = 0x0006,
    /**< FastPollTimeoutMax (Optional). Range is (0x2, 0xffff) */

    /* discontinuity */

    ZCL_POLL_LONG_POLL_FROM_CLUSTER = 0x7ffe,
    /**< If True, the Poll Server cluster will perform the NLME-SYNC.request (polling)
     * at the interval given by the 'LongPollInterval' (ZCL_POLL_LONG_POLL_INTERVAL).
     * If False, the Poll Server will only expose the 'LongPollInterval' via the
     * attribute to devices that want to read it, but the application will be
     * responsible for any long polling it wants to do.
     * Default is False (disabled), meaning the cluster will not perform an
     * NLME-SYNC.request at the long poll intervals. */

    ZCL_POLL_CHECK_IN_RSP_WAIT_AFTER = 0x7fff,
    /**< If True, then wait for the full check-in response timeout before
     * stopping fast-polling for responses. If False, then as soon as the
     * number of received check-in responses matches the number of bound
     * clients, then stop fast-polling immediately.
     * Note that this is an Exegin custom attribute and not exposed to
     * the network. (Optional) */

};

/* Client Generated Commands */
enum {
    ZCL_POLL_CTRL_CLI_CHECK_IN_RSP = 0x00, /* mandatory */
    ZCL_POLL_CTRL_CLI_FAST_POLL_STOP = 0x01, /* mandatory */
    ZCL_POLL_CTRL_CLI_SET_LONG_POLL_INTERVAL = 0x02,
    ZCL_POLL_CTRL_CLI_SET_SHOR_POLL_INTERVAL = 0x03
};

/* Server Generated Commands */
enum {
    ZCL_POLL_CTRL_SVR_CHECK_IN = 0x00 /* mandatory */
};

/** Check-in Response command structure */
struct zcl_poll_checkin_rsp_t {
    enum ZclStatusCodeT status; /**< Status */
    bool start_fast_poll; /**< Start Fast Polling */
    uint16_t fast_poll_timeout; /**< Fast Poll Timeout */
};

/*---------------------------------------------------------------
 * Server API
 *---------------------------------------------------------------
 */

/** Poll Control Server callbacks configuration */
struct ZbZclPollControlServerCallbackT {
    bool (*checkin_rsp)(struct ZbZclClusterT *clusterPtr,
        struct zcl_poll_checkin_rsp_t *rsp_info, struct ZbZclAddrInfoT *srcInfo, void *arg);
    /**< Callback to application, invoked on receipt of Check-in Response command.
     * If the callback returns true, then the Check-in Response is allowed to start
     * fast polling. The ZCL8 Spec makes it optional whether these commands allow
     * the device to start fast polling, even if "Start Fast Polling" payload
     * parameter is set to true. If the callback return false, then this command
     * shall not have an effect on fast polling. */
};

/**
 * Create a new instance of the Poll Control Server cluster
 * @param zb Zigbee stack instance
 * @param endpoint Endpoint on which to create cluster
 * @param callbacks Structure containing any callback function pointers for this cluster
 * @param arg Pointer to application data that will included in the callback when invoked.
 * @return Cluster pointer, or NULL if there is an error
 */
struct ZbZclClusterT * zcl_poll_server_alloc(struct ZigBeeT *zb, uint8_t endpoint,
    struct ZbZclPollControlServerCallbackT *callbacks, void *arg);

/**
 * Send a Check-in command to any bound Clients.
 * @param cluster Cluster instance from which to send this command
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZclStatusCodeT zcl_poll_server_send_checkin(struct ZbZclClusterT *cluster);

/*---------------------------------------------------------------
 * Client API
 *---------------------------------------------------------------
 */

/** Check-in Info structure */
struct ZbZclPollControlClientCheckinInfo {
    struct ZbApsAddrT dst; /**< Destination Address */
    bool start_fast_poll; /**< Start Fast Poll */
    uint16_t fast_poll_timeout; /**< Fast Poll Timeout */
    bool auto_refresh;
    /**< If true, check-in response is automatically refreshed for the next
     * check-in request. If false, this is a one-time event. */
};

/** Fast Poll Stop command structure */
struct ZbZclPollControlClientStopReq {
    struct ZbApsAddrT dst; /**< Destination Address */
};

/** Set Long Poll Interval command structure */
struct ZbZclPollControlClientSetLongReq {
    struct ZbApsAddrT dst; /**< Destination Address */
    uint32_t interval; /**< NewLongPollInterval */
};

/** Set Short Poll Interval command structure */
struct ZbZclPollControlClientSetShortReq {
    struct ZbApsAddrT dst; /**< Destination Address */
    uint16_t interval; /**< New Short Poll Interval */
};

/** Poll Control Client callbacks configuration */
struct ZbZclPollControlClientCallbackT {
    /* This function is called after the Poll Client sends a Check-In response for
     * a Check-In request from a Poll Server. It will indicate that a Poll Server has
     * just recently send a Check-In request and whether the Server is now polling.
     * The Check-In response information that we want to respond to the Poll Server is
     * configured by calling zcl_poll_client_set_checkin_rsp().
     *
     * Callback limitation note:
     * If timeout returned to the callback is zero, that means that the server is fast polling
     * for the amount of time it's fast poll timeout attribute is set to.
     *
     * We can only assume that the server is fast polling if the default response status is
     * success, since there is no way for the client to know if the server is actually fast
     * polling from the check-in response due to various reasons (i.e. already fast polling,
     * fails to start fast poll after sending the default response, or stopped fast polling).
     *
     * If the polling boolean is false, there is a chance that the server is fast polling
     * still. This could be due to other Poll Clients the server is bound to.
     * */
    enum ZclStatusCodeT (*checkin_rsp_callback)(struct ZbZclClusterT *clusterPtr,
        struct zcl_poll_checkin_rsp_t *rsp_info, struct ZbZclAddrInfoT *srcInfo, void *arg);
    /**< Callback to application, invoked on receipt of Check-in Response command */
};

/**
 * Create a new instance of the Poll Control Client cluster
 * @param zb Zigbee stack instance
 * @param endpoint Endpoint on which to create cluster
 * @param callbacks Structure containing any callback function pointers for this cluster
 * @param arg Pointer to application data that will included in the callback when invoked.
 * @return Cluster pointer, or NULL if there is an error
 */
struct ZbZclClusterT * zcl_poll_client_alloc(struct ZigBeeT *zb, uint8_t endpoint,
    struct ZbZclPollControlClientCallbackT *callbacks, void *arg);

/**
 * Set Check-in Response configuration
 * @param cluster Cluster instance from which to send this command
 * @param info Check-in configuration info
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZclStatusCodeT zcl_poll_client_set_checkin_rsp(struct ZbZclClusterT *cluster,
    struct ZbZclPollControlClientCheckinInfo *info);

/**
 * Send a Fast Poll Stop command
 * @param cluster Cluster instance from which to send this command
 * @param req Fast Poll Stop command request structure
 * @param callback Callback function that will be invoked when the response is received.
 * @param arg Pointer to application data that will included in the callback when invoked.
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZclStatusCodeT zcl_poll_client_stop_fastpoll_req(struct ZbZclClusterT *cluster,
    struct ZbZclPollControlClientStopReq *req,
    void (*callback)(struct ZbZclCommandRspT *rsp, void *arg), void *arg);

/**
 * Send a Set Long Interval command
 * @param cluster Cluster instance from which to send this command
 * @param req Set Long Interval command request structure
 * @param callback Callback function that will be invoked when the response is received.
 * @param arg Pointer to application data that will included in the callback when invoked.
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZclStatusCodeT zcl_poll_client_set_long_intvl_req(struct ZbZclClusterT *cluster,
    struct ZbZclPollControlClientSetLongReq *req,
    void (*callback)(struct ZbZclCommandRspT *rsp, void *arg), void *arg);

/**
 * Send a Set Short Interval command
 * @param cluster Cluster instance from which to send this command
 * @param req Set Short Interval command request structure
 * @param callback Callback function that will be invoked when the response is received.
 * @param arg Pointer to application data that will included in the callback when invoked.
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZclStatusCodeT zcl_poll_client_set_short_intvl_req(struct ZbZclClusterT *cluster,
    struct ZbZclPollControlClientSetShortReq *req,
    void (*callback)(struct ZbZclCommandRspT *rsp, void *arg), void *arg);

#endif /* ZCL_POLL_CONTROL_H */
