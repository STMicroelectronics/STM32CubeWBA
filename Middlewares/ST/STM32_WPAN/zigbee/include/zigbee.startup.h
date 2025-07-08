/**
 * @file zigbee.startup.h
 * @heading Startup
 * @brief Zigbee startup header file
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 */

#ifndef ZIGBEE_STARTUP_H
# define ZIGBEE_STARTUP_H

#include "zcl/zcl.touchlink.h"

/* For FlexeLint */
#ifndef CONFIG_ZB_REV
#define CONFIG_ZB_REV 23
#endif

/* Just in case we're not building the R23+ stack. */
struct ZbApsRelayInfoT;

/** Startup control-codes as per the ZCL Spec, Commissioning Cluster's StartupControl
 * attribute in the "Startup Parameters Attribute Set" */
enum ZbStartType {
    ZbStartTypePreconfigured = 0x00, /**< (0x00) Preconfigured. No explicit form, join or rejoin to be performed. */
    ZbStartTypeForm = 0x01, /**< (0x01) Form network */
    ZbStartTypeRejoin = 0x02, /**< (0x02) Rejoin network */
    ZbStartTypeJoin = 0x03, /**< (0x03) Join network */
    /* Exegin add-ons (Internal use only) */
    ZbStartTypeTouchlink = 0xfd, /**< [Internal stack use only] Touchlink */
    ZbStartTypeFindBind = 0xfe, /**< [Internal stack use only] Finding & Binding */
    ZbStartTypeNone = 0xff /**< [Internal stack use only] None */
};

struct ZbStartupKeepAliveT {
    bool enabled;
    /**< If true, Keep-Alive is enabled within the stack. Default is false. */

    uint8_t endpoint;
    /**< Endpoint to assign Keep-Alive cluster. Default is ZB_ENDPOINT_CBKE_DEFAULT (240). */

    uint16_t profileId;
    /**< Profile Id to assign to the endpoint created for the Keep-Alive cluster.
     * Default is ZCL_PROFILE_SMART_ENERGY (0x0109) */

    uint16_t deviceId;
    /**< Device Id to assign to the endpoint created for the Keep-Alive cluster.
     * Default is ZCL_DEVICE_METER */

    /* Keep Alive Server or Client */
    bool server_enable;
    /**< This flag determines whether to allocate the Trust Center Keep Alive Server (true)
     * or Client (false). The Trust Center should set this flag to true, and all other
     * joiners should set this flag to false. */

    /* Keep Alive Server attributes */
    uint8_t server_base;
    /**< Trust Center Keep Alive Server 'Base' attribute value in minutes.
     * If zero, let the stack choose a default value. */
    uint16_t server_jitter;
    /**< Trust Center Keep Alive Server 'Jitter' attribute value in seconds.
     * If zero, let the stack choose a default value. */

    bool (*tcso_callback)(enum ZbTcsoStatusT status, void *arg);
    /**< Application callback that is called to notify of any Trust Center Swap Out (TCSO)
     * events initiated by the Keep Alive Client cluster. If the status is set to
     * ZB_TCSO_STATUS_DISCOVERY_UNDERWAY, the application can return false to this callback
     * to halt the TCSO process from continuing. This allows the application to dictate when
     * to start TCSO; for example, during the next wake-cycle if the device is sleepy.
     * The Keep Alive Client is also halted in this case, but will restart after the application
     * calls ZbStartupTcsoStart to perform TCSO, or it calls ZbZclKeepAliveClientStart to restart
     * the Keep Alive mechanism. If the application returns true to this callback, then the
     * TCSO process will proceed normally. */
};

/** CBKE configuration parameters for ZbStartup. This configuration is only
 * applicable if the 'suite_mask' is non-zero. */
struct ZbStartupCbkeT {
    bool bdbRequireCBKESuccess;
    /**< If true, it indicates that successful completion of CBKE is mandatory as part of
     * network join and on CBKE failure joiner shall terminate the join procedure. If false,
     * the joiner is allowed to use other link key establishment mechanisms in case of
     * CBKE failure. This flag comes into effect only if CBKE is configured on joiner as
     * part of startup cbke config and by default it is set to true. Application can overwrite
     * the default as part of startup cbke configuration. This flag was introduced with BDB 3.1. */

    uint8_t endpoint;
    /**< Endpoint to assign ZCL Key Exchange cluster. Default is ZB_ENDPOINT_CBKE_DEFAULT (240) */

    uint16_t profileId;
    /**< Profile Id to assign ZCL Key Exchange cluster. Default is ZCL_PROFILE_SMART_ENERGY (0x0109) */

    uint16_t deviceId;
    /**< Device Id to assign to the endpoint created for the ZCL Key Exchange cluster.
     * Default is ZCL_DEVICE_METER */

    uint16_t suite_mask;
    /**< The Key Exchange suite bitmask. E.g. ZCL_KEY_SUITE_CBKE2_ECMQV for CBKE version 2 (cbke_v2). */

    struct ZbZclCbkeInfoT cbke_v1;
    /**< CBKE version 1 certificate and security keys configuration.
     * Only applicable if ZCL_KEY_SUITE_CBKE_ECMQV is set in suite_mask. */

    struct ZbZclCbke2InfoT cbke_v2;
    /**< CBKE version 2 certificate and security keys configuration.
     * Only applicable if ZCL_KEY_SUITE_CBKE2_ECMQV is set in suite_mask. */

    uint8_t term_wait_time;
    /**< This is the time in seconds to include in the Wait Time field in the Terminate Key
     * Establishment command. The Wait Time is the time for the CBKE initiator to wait before
     * trying again. The default value is 0xfe (254 seconds). */
};

/**
 * The set of configuration parameters used to form or join a network using ZbStartup.
 * Should be first initialized using ZbStartupConfigGetProDefaults or
 * ZbStartupConfigGetProSeDefaults.
 */
struct ZbStartupT {
    void *cb_arg; /**< Application callback argument for any callbacks defined in this struct. */

    uint16_t shortAddress;
    /**< Network short address. Only applicable if startupControl is
     * ZbStartTypePreconfigured or ZbStartTypeRejoin */

    uint16_t panId;
    /**< Network PAN Id. Only applicable if startupControl is
     * ZbStartTypePreconfigured or ZbStartTypeRejoin */

    uint16_t networkManagerAddress;
    /**< Network Manager Address. Only applicable if startupControl is
     * ZbStartTypePreconfigured or ZbStartTypeRejoin */

    uint64_t extendedPanId;
    /**< Extended PAN Id.
     *
     * If startupControl is ZbStartTypeForm and extendedPanId is zero, then the device's
     * extended address (EUI) will be used.
     *
     * If startupControl is ZbStartTypeRejoin a non-zero extendedPanId must be provided.
     *
     * If startupControl is ZbStartTypeJoin, the device will only attempt to join a
     * network matching the extendedPanId. If extendedPanId is zero, then the device will
     * attempt to join any available network, sorted by LQI.
     *
     * If startupControl is ZbStartTypePreconfigured a non-zero extendedPanId must be provided.
     */

    struct ZbChannelListT channelList;
    /**< Specify the channel mask(s) to use. If no channel masks are specified,
     * ZB_BDB_PrimaryChannelSet and ZB_BDB_SecondaryChannelSet will be used instead. */

    uint32_t bdbPrimaryChannelSet;
    /**< Configures the BDB IB ZB_BDB_PrimaryChannelSet, which is a single 32-bit BDB Primary
     * Channel mask. The 'channelList' parameter should be cleared if you want to use the BDB
     * Primary and Secondary channel masks. */

    uint32_t bdbSecondaryChannelSet;
    /**< Configures the BDB IB ZB_BDB_SecondaryChannelSet, which is a single 32-bit BDB Secondary
     * Channel mask. The 'channelList' parameter should be cleared if you want to use the BDB
     * Primary and Secondary channel masks. */

    uint8_t stackProfile;
    /**< Network Stack Profile. If not ZB_NWK_STACK_PROFILE_PRO, application must
     * configure the following NIB parameters before calling ZbStartup:
     *      nwkMaxChildren, nwkReportConstantCost, nwkLinkStatusPeriod, nwkTransactionPersistenceTime,
     *      nwkPassiveAckTimeout, nwkMaxBroadcastRetries, nwkSecureAllFrames, nwkSecurityLevel
     */

    uint8_t bdbCommissioningMode;
    /**< BDB Commissioning Mode mask. Set to 0 by default.
     * If BDB_COMMISSION_MODE_TOUCHLINK is set, then Touchlink will be used.
     * If BDB_COMMISSION_MODE_FIND_BIND is set, then Finding & Binding will be used after joining.
     */

    enum ZbStartType startupControl;
    /**< Startup Type. Not applicable if BDB_COMMISSION_MODE_TOUCHLINK commissioning mode is set.
     *
     * If startup type is ZbStartTypeJoin, the ZB_APS_IB_ID_SCAN_COUNT attribute is used to
     * control the number of scans to perform while searching for a network to join. Similarily,
     * the ZB_BDB_ScanDuration attribute is used to configure the MAC scan duration to use for
     * scans during network joining and forming.
     */

    struct {
        uint8_t level; /**< Security Level. Default is 0x05. */
        bool useInsecureRejoin; /**< Configures ZB_APS_IB_ID_USE_INSECURE_JOIN. */
        uint64_t trustCenterAddress;
        /**< Configures ZB_APS_IB_ID_TRUST_CENTER_ADDRESS. If forming the network and assuming the role of
         * Trust Center, the device's extended address will be used instead, unless this parameter has
         * been explicitly set to ZB_DISTRIBUTED_TC_ADDR. */

        uint8_t preconfiguredLinkKey[ZB_SEC_KEYSIZE]; /**< Preconfigured Link Key */
        uint8_t distributedGlobalKey[ZB_SEC_KEYSIZE]; /**< Preconfigured Distributed Global Link Key */

        uint8_t networkKey[ZB_SEC_KEYSIZE];
        /**< Configures the Network Key with key type set to ZB_SEC_KEYTYPE_STANDARD_NWK.
         * Only applicable if startupControl is ZbStartTypePreconfigured. */

        uint8_t networkKeySeqNum;
        /**< Configures the Network Key Sequence Number for the Network Key.
         * Also sets ZB_NWK_NIB_ID_ActiveKeySeqNumber to this value.
         * Only applicable if startupControl is ZbStartTypePreconfigured.
         */

        enum ZbSecKeyTypeT networkKeyType; /**< Deprecated and not used. */

        struct ZbStartupCbkeT cbke; /**< CBKE certificate configuration */

        struct ZbStartupKeepAliveT keepalive; /**< Keep-Alive configuration */

        uint8_t zdLinkKeyFlags;
        /**< APS Link key flags provided as part of commissioning join to ZDD
         * e.g, ZD_TLV_LINK_KEY_FLAG_UNIQUE.
         * NOTE: Only applicable with CONFIG_ZB_ZIGBEE_DIRECT (EXEGIN - ZVD only?) */

        uint8_t passcode[ZB_SEC_KEYSIZE];
        /**< Configures a 16-byte pre-shared secret, derived out of a short passcode. Short passcodes are
         * introduced with R23 and are short human readable codes, that the user can key-in to a device.
         * These codes shall be decoded following the mechanism outlined in the document,
         * '20-66834-001-zigbee-r23-short-device-setup-codes' to generate a 16-byte pre-shared secret.
         * When provided this code shall be used as PSK for authenticated DLK.
         *
         * NOTE: Use this parameter to provide a unique short passcode for the joiner device.
         * And in case of TC, this will be used as a common preconfigured passcode for all the
         * joiner devices, where a joiner specific passcode is not available. i.e, similar
         * to apsPreconfiguredLinkKey. */
    } security;

    uint8_t nwkUpdateId;
    /**< ZB_NWK_NIB_ID_UpdateId, only used in case of ZDD (Zigbee Direct Device)
     * Commissioning form/OOB join performed by ZVD (Zigbee Virtual Device).
     * NOTE: Only applicable with CONFIG_ZB_ZDD_SUPPORT. */

    uint8_t capability;
    /**< Device capability mask. Default value includes:
     * MCP_ASSOC_CAP_PWR_SRC (mains power),
     * MCP_ASSOC_CAP_RXONIDLE (radio receiver is on, not sleepy),
     * MCP_ASSOC_CAP_ALLOC_ADDR (parent allocates network address),
     * MCP_ASSOC_CAP_DEV_TYPE (full function device) */

    uint8_t endDeviceTimeout;
    /**< End-device timeout is only used by end-devices. It configures the
     * time used to periodically update the Parent device so this
     * device is not removed from the Parent's NWK Neighbor Table.
     * Configures ZB_NWK_NIB_ID_EndDeviceTimeoutDefault.
     *
     * Timeout = (60 * 2^n) seconds for n > 0. If n = 0, timeout = 10 seconds.
     * Setting to ZB_NWK_CONST_ENDDEV_TIMEOUT_DISABLED (0xff) disables end-device timeout.*/

    uint16_t fastPollPeriod; /**< Configures ZB_NWK_NIB_ID_FastPollPeriod. */

    struct {
        uint8_t tl_endpoint; /**< Endpoint for the Touchlink Cluster (e.g. ) */
        uint8_t bind_endpoint; /**< Endpoint to use when binding clusters from Initiator to Target. */
        uint16_t deviceId; /**< e.g. ZCL_DEVICE_ONOFF_SWITCH */
        uint8_t zb_info; /**< e.g. ZCL_TL_ZBINFO_TYPE_ROUTER */
        uint8_t flags; /**< e.g. ZCL_TL_FLAGS_IS_TARGET */
        const void *persist_buf;
        /**< Pointer to persistence data. Only applicable if ZCL_TL_ZBINFO_USE_PERSIST
         * flag is set in zb_info. */
        unsigned int persist_len; /**< Length of persistence data. */
        struct ZbTouchlinkCallbacks callbacks;
        /**< Callback functions for Touchlink Controller Device Utility commands. */
    } touchlink; /**< Touchlink configuration. Only applicable if BDB_COMMISSION_MODE_TOUCHLINK. */

    struct {
        uint8_t server_ep;
        /**< Endpoint to assign to Sub-GHz server cluster to perform duty cycle checks. */
        uint16_t device_id;
        /**< DeviceId to assign to the endpoint on which Sub-GHz server cluster is created. */
    } subghz; /**< Sub-GHz cluster configuration, only applicable on a coordiantor. */

    /*
     * R23 additions
     */
    uint8_t supp_key_nego_methods;
    /**< A bitmask of the supported key negotiation methods e.g, ZB_DLK_STATIC_KEY_REQUEST. */
    uint8_t supp_pre_shared_secrets;
    /**< A bitmask of the supported pre-shared secrets e.g, ZB_DLK_SUPP_PSK_SYMMETRIC_AUTH_TOKEN. */

    void (*device_interview_cb)(struct ZbApsRelayInfoT *relay_info, uint64_t joiner_eui, void *arg);
    /**< Device Interview callback. This is called when the device is acting as
     * Trust Center in order to start the Device Interview process with the
     * Joiner. As packets are sent as Downstream Relays to the Joiner, the
     * security timer is refreshed by the amount given by apsDeviceInterviewTimeoutPeriod.
     * Once the TC application is done with the interview process, it must call
     * ZbStartupDeviceInterviewComplete. Refer to that functions description for more
     * information.
     */
};

/**
 * Executes the startup procedure as described in section 2.5.5.5.6.2 of [R1].
 *
 * The startup code also handles callbacks from the stack to maintain the network
 * and security processes. For example, these include the handling of
 * APSME-UPDATE-DEVICE.indication messages when acting as a Coordinator Trust Center,
 * to handle the authentication procedure described in section 4.6.3.2 of Zigbee R22.
 *
 * The message callback  handlers can be overridden by the application by creating
 * message filters using ZbMsgFilterRegister.
 *
 * This function and the entire startup code can be bypassed by the application, and
 * the Network Layer APIs used directly, if so desired.
 *
 * @param zb Zigbee stack instance
 * @param configPtr Pointer to ZbStartupT configuration structure.
 * @param callback Application callback that is called with the final result
 * of the startup procedure.
 * @param arg Callback argument
 * @return Zigbee Status Code whether the startup procedure has been started.
 * If ZB_STATUS_SUCCESS, then the callback will be called with the final result.
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbStartup(struct ZigBeeT *zb, struct ZbStartupT *configPtr,
    void (*callback)(enum ZbStatusCodeT status, void *cb_arg), void *arg);

/**
 * If Touchlink Target was started with ZbStartup, this API can be used to stop it.
 * @param zb Zigbee stack instance
 * @return Zigbee Status Code whether Touchlink was successfully stopped or not.
 */
enum ZbStatusCodeT ZbStartupTouchlinkTargetStop(struct ZigBeeT *zb);

/**
 * Manually start Finding & Binding. F&B is also started automatically
 * after joining the network.
 * @param zb Zigbee stack instance
 * @param callback Application callback to call after Finding & Binding complete
 * @param arg Application callback argument
 * @return Zigbee Status Code
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbStartupFindBindStart(struct ZigBeeT *zb,
    void (*callback)(enum ZbStatusCodeT status, void *arg), void *arg);

/**
 * Same as ZbStartupFindBindStart, but only for a single endpoint.
 * @param zb Zigbee stack instance
 * @param endpoint Endpoint to perform Finding & Binding from.
 * @param callback Application callback to call after Finding & Binding complete
 * @param arg Application callback argument
 * @return Zigbee Status Code
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbStartupFindBindStartEndpoint(struct ZigBeeT *zb, uint8_t endpoint,
    void (*callback)(enum ZbStatusCodeT status, void *arg), void *arg);

/**
 * ZbStartupRejoin is a wrapper for ZbNlmeJoinReq(ZB_NWK_REJOIN_TYPE_NWKREJOIN).
 * Use ZbStartupRejoin instead, because the internal startup handler will restart
 * any timers we need to maintain our parent.
 *
 * Must already be connected to a network. If not on a network and want to
 * rejoin as way to connect, use ZbStartup with ZbStartTypeRejoin.
 * @param zb Zigbee stack instance
 * @param callback Application callback to call after rejoin is complete,
 * indicating success or failure.
 * @param cbarg Application callback argument
 * @return Zigbee Status Code
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbStartupRejoin(struct ZigBeeT *zb,
    void (*callback)(struct ZbNlmeJoinConfT *conf, void *arg), void *cbarg);

/**
 * Trust Center Rejoin - unsecured rejoin (already joined to network, but
 * missed a NWK key update).
 * @param zb Zigbee stack instance
 * @param callback Application callback to call after Trust Center rejoin is complete.
 * If this function returns an error, then no callback will be generated.
 * @param cbarg Application callback argument
 * @return Zigbee Status Code
 */
enum ZbStatusCodeT ZB_WARN_UNUSED ZbTrustCenterRejoin(struct ZigBeeT *zb,
    void (*callback)(enum ZbStatusCodeT status, void *arg), void *cbarg);

/**
 * Try starting stack using persistence data provided.
 * NOTE: In case of a Zigbee Virtual Device (ZVD) this API only takes care of restoring the
 * stack from persistence data and does not perform secure rejoin. So the ZVD application
 * shall reestablish the secure session over BLE using basic or admin key and then perform
 * the secure rejoin using ZbStartupRejoin() API.
 * @param zb Zigbee stack instance
 * @param pdata Pointer to persistent data
 * @param plen Length of persistent data
 * @param config Optional startup configuration pointer. Currently, ZSDK stack only needs
 * the CBKE & KA configuration from the startup config structure. However, this can be
 * extended to use any other info.
 * @param callback Application callback to call after persistence is completed.
 * It is necessary for the application to specify this callback so it knows exactly
 * when it is safe to start transmitting packets to the Zigbee network.
 * If this function returns an error, then no callback will be generated.
 * @param arg Application callback argument
 * @return Zigbee Status Code
 */
enum ZbStatusCodeT ZbStartupPersist(struct ZigBeeT *zb, const void *pdata, unsigned int plen,
    struct ZbStartupT *config, void (*callback)(enum ZbStatusCodeT status, void *arg), void *arg);

/**
 * Get the default configuration for a PRO network.
 * @param configPtr Startup configuration to initialize
 * @return None
 */
void ZbStartupConfigGetProDefaults(struct ZbStartupT *configPtr);

/**
 * Same as ZbStartupConfigGetProDefaults, but clears the preconfigured
 * global link keys.
 * @param configPtr Startup configuration to initialize
 * @return None
 */
void ZbStartupConfigGetProSeDefaults(struct ZbStartupT *configPtr);

/**
 * The Stack with the help of the ZCL Keep Alive cluster will automatically detect
 * the loss of the Trust Center and start the TCSO process. Before the TCSO process
 * starts and the 'tcso_callback' callback is provided in the startup configuration,
 * it is called to check if the application wants to halt the TCSO process from starting
 * or let it continue. Refer to the description for 'tcso_callback' in the startup
 * configuration for more info.
 *
 * The application can call ZbStartupTcsoStart at any time if it thinks it has lost
 * communication with the Trust Center and to manually begin the TCSO process.
 * @param zb Zigbee stack instance
 * @param zvd_parent_info Parent information structure in case of ZVD. This parameter
 * shall be set to NULL if not acting as ZVD.
 * @param callback Application callback that is called after TCSO is complete.
 * @param arg Application callback argument
 * @return True if TCSO was started and to wait for callback to indicate
 * completion. False if TCSO wasn't started.
 */
bool ZB_WARN_UNUSED ZbStartupTcsoStart(struct ZigBeeT *zb,
    void (*callback)(enum ZbTcsoStatusT status, void *arg), void *arg);

/**
 * Abort a TCSO, if it's running. This function should not be necessary,
 * but a Sleepy Device application may need it if it wants to go to sleep
 * before ZbStartupTcsoStart has finished.
 * @param zb Zigbee stack instance
 * @return True if TCSO was aborted, false otherwise (i.e. wasn't running).
 */
bool ZbStartupTcsoAbort(struct ZigBeeT *zb);

/**
 * The application can call ZbStartupTclkStart if it wants to update the trust center link key
 * post join using APS-Request Key method. This is usually done under following scenarios,
 *
 * 1. In case of a Zigbee Direct (ZDD) updating a provisional link key, after it was
 *    commissioned via out-of-band commissioning mechanism.
 * 2. To periodically generate a new APS Link Key with the Trust Center. Continual cycling
 *    of the TC Link Key is a 'Works With All Hubs (WWAHU)' security recommendation.
 *
 * @param zb Zigbee stack instance
 * @param callback Application callback that is called with the final result
 * of the tclk procedure.
 * @param arg Callback argument
 * @return true if the tclk is completed successfuly, false otherwise.
 */
enum ZbStatusCodeT ZbStartupTclkStart(struct ZigBeeT *zb,
    void (*callback)(enum ZbStatusCodeT status, void *arg), void *arg);

/**
 * The application calls this when it is done with the Device Interview process for a
 * particular Joiner. The Device Interview process starts in the application when the
 * stack calls the device_interview_cb() callback function defined in the 'struct ZbStartupT'
 * startup configuration.
 * @param zb Zigbee stack instance
 * @param joiner_eui64 The EUI64 address of the Joiner being interviewed.
 * @param success If true, then the stack will send the Transport Key with the Network
 * Key to end the Device Interview and Joining process.
 * @return True if the request was processed successfully (i.e. the Joiner is actively
 * being tracked by the stack), or false otherwise.
 */
bool ZbStartupDeviceInterviewComplete(struct ZigBeeT *zb, uint64_t joiner_eui64, bool success);

/* Type declaration, required for struct ZbStartupAppLkT. */
struct ZbZdoSecGetAuthLevelRspT;

/**
 * Application link-key information structure. This structure needs to be populated by
 * the initiator/responder devices at the time of starting the application link-key
 * establishment process. */
struct ZbStartupAppLkT {
    bool is_initiator;
    /**< If true, we take on the initiator role. else, act as responder. */

    uint64_t partner_addr;
    /**< Extended address of partner device. */

    bool (*auth_level_cb)(struct ZbZdoSecGetAuthLevelRspT *rsp, void *arg);
    /**< If not NULL, then this callback will be invoked by stack on receiving the ZDO
     * Security_Get_Authentication_Level_Rsp, so that the application is notified of the
     * authentication level of target. In case, the ZDO Security_Get_Authentication_Level_Rsp
     * returns an error status, then this callback marks the termination of the application
     * link-key establishment process and the aps_link_key_cb callback shall not be invoked by stack.
     *
     * In case ZDO Security_Get_Authentication_Level_Rsp returns with a status of SUCCESS, then this
     * callback, provides application an opportunity to check if it is ok with the authentication level
     * of target device. If yes, it shall return a boolean value of 'true' as status. Whereas, a status
     * value of 'false' is an indication to terminate the application link-key establishment process due
     * to 'insufficient target security'. */

    void (*aps_link_key_cb)(uint64_t partner_addr, enum ZbStatusCodeT status, void *arg);
    /**< If not NULL, this callback will be invoked by stack once the link-key is successfully verified
     * by the responder/partner device or in case of timeout to complete the verification process. */

    void *arg;
    /**< Callback argument pointer to auth_level_cb & aps_link_key_cb. */
};

/**
 * Perform application link-key establishment with a non-TC partner device after having
 * joined the network. The initiator or the responder devices can't be TC.
 *
 * This API supports application link-key establishment as per BDB 3.1 which involves
 * querying the authentication level of the partner device and verification of new
 * link-key.
 *
 * In case of Initiator, this API starts the application link-key establishment process,
 * whereas, in case of responder, this API shall be called by application on receiving
 * the APS-TRANSPORT-KEY.indication from TC with an application link key and if the
 * application intends to run the responder side of the application link-key state-machine
 * as per BDB 3.1 specification.
 *
 * @param zb Zigbee instance
 * @param info Pointer to application link key information structure.
 * @return ZB Status Code. If not ZB_STATUS_SUCCESS, then the callback will not be called.
 */
enum ZbStatusCodeT ZbStartupAppLkStart(struct ZigBeeT *zb, struct ZbStartupAppLkT *info);
#endif
