/**
 * @file zigbee.bdb.h
 * @heading BDB Layer
 * @brief BDB header file
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 */

#ifndef ZIGBEE_BDB_H
#define ZIGBEE_BDB_H

/* Values of the bdbCommissioningStatus attribute.
 * These are not interchangeable with Zigbee Status Codes. */
/** bdbCommissioningStatus */
enum ZbBdbCommissioningStatusT {
    ZB_BDB_COMMISS_STATUS_SUCCESS = 0x00,
    /**< SUCCESS - The commissioning sub-procedure was successful */
    ZB_BDB_COMMISS_STATUS_IN_PROGRESS = 0x01,
    /**< IN_PROGRESS - One of the commissioning sub-procedures has started but is not yet complete */
    ZB_BDB_COMMISS_STATUS_NOT_AA_CAPABLE = 0x02,
    /**< NOT_AA_CAPABLE - The initiator is not address assignment capable during touchlink */
    ZB_BDB_COMMISS_STATUS_NO_NETWORK = 0x03,
    /**< NO_NETWORK - A network has not been found during network steering or touchlink*/
    ZB_BDB_COMMISS_STATUS_TARGET_FAILURE = 0x04,
    /**< TARGET_FAILURE - A node has not joined a network when requested during touchlink */
    ZB_BDB_COMMISS_STATUS_FORMATION_FAILURE = 0x05,
    /**< FORMATION_FAILURE - A network could not be formed during network formation */
    ZB_BDB_COMMISS_STATUS_NO_IDENTIFY_QUERY_RESPONSE = 0x06,
    /**< NO_IDENTIFY_QUERY_RESPONSE - No response to an identify query command has been
     * received during finding & binding */
    ZB_BDB_COMMISS_STATUS_BINDING_TABLE_FULL = 0x07,
    /**< BINDING_TABLE_FULL - A binding table entry could not be created due to insufficient
     * space in the binding table during finding & binding */
    ZB_BDB_COMMISS_STATUS_NO_SCAN_RESPONSE = 0x08,
    /**< NO_SCAN_RESPONSE - No response to a scan request inter-PAN command has been received
     * during touchlink */
    ZB_BDB_COMMISS_STATUS_NOT_PERMITTED = 0x09,
    /**< NOT_PERMITTED - A touchlink (steal) attempt was made when a node is already connected
     * to a centralized security network or when end node attempts to form network */
    ZB_BDB_COMMISS_STATUS_TCLK_EX_FAILURE = 0x0a,
    /**< TCLK_EX_FAILURE - The Trust Center link key exchange procedure has failed attempting
     * to join a centralized security network */
    ZB_BDB_COMMISS_STATUS_NOT_ON_A_NETWORK = 0x0b,
    /**< NOT_ON_A_NETWORK - A commissioning procedure was forbidden since the node was not
     * currently on a network. */
    ZB_BDB_COMMISS_STATUS_ON_A_NETWORK = 0x0c
        /**< ON_A_NETWORK - A commissioning procedure was forbidden since the node was
         * currently on a network. */
};

#define BDB_DEFAULT_TC_NODE_JOIN_TIMEOUT        15U /* seconds */

/* Min valid BDB commissioning group id. */
#define BDB_COMMISSION_EP_GROUP_ID_MIN          0x0001U

/* Default BDB commissioning endpoint group id to be used while adding an APS endpoint.
 * This disables group bindings with F&B. To enable group bindings during F&B process
 * set bdbCommissioningGroupID in the range >= BDB_COMMISSION_EP_GROUP_ID_MIN and
 * < BDB_COMMISSION_EP_GROUP_ID_DEFAULT */
#define BDB_COMMISSION_EP_GROUP_ID_DEFAULT      0xffffU

/* Bits for ZB_BDB_CommissioningMode */
#define BDB_COMMISSION_MODE_MASK                0x0FU
#define BDB_COMMISSION_MODE_TOUCHLINK           0x01U
#define BDB_COMMISSION_MODE_NET_STEER           0x02U
#define BDB_COMMISSION_MODE_NET_FORM            0x04U /* Whether to form a network. Configured by ZbStartup (e.g. ZbStartTypeForm) */
#define BDB_COMMISSION_MODE_FIND_BIND           0x08U

/* values for ZB_BDB_TCLinkKeyExchangeMethod / bdbTCLinkKeyExchangeMethod */
/** bdbTCLinkKeyExchangeMethod */
enum ZbBdbLinkKeyExchMethodT {
    BDB_LINKKEY_EXCHANGE_METHOD_APS = 0x00, /**< APS Request Key */
    BDB_LINKKEY_EXCHANGE_METHOD_CBKE = 0x01 /**< Certificate Based Key Exchange (CBKE) */
};

/* Touchlink Default Primary Channel Set (2.4 GHz, Page 0, based on WPAN_CHANNELMASK_2400MHZ)
 * Channels: 11, 15, 20, 25 */
#define BDBC_TL_PRIMARY_CHANNEL_SET             0x02108800U
/* Secondary = 0x05ef7000.
 * Note that scanning the full primary and secondary channels sets may
 * not get done before the Touchlink process times-out. This is an
 * inherent problem with the Touchlink spec. By default, the stack
 * uses a secondary mask equal to zero. */
#define BDBC_TL_SECONDARY_CHANNEL_SET           (WPAN_CHANNELMASK_2400MHZ ^ BDBC_TL_PRIMARY_CHANNEL_SET)

/* value for bdbRssiMin - used by touchLink ONLY */
#define BDB_TL_RSSI_MIN                         (-40)

/* Arbritrary value. Default value from spec is 8 seconds, but later found that 8 seconds is too short when secondary
 * channel scanning is required. */
#define BDB_TL_INTERPAN_TRANS_LIFETIME          12U

/* Table 13-23 Key Encryption Algorithm
 * NOTE: This is what keyMask<->keyIndex translates to:
 * KeyMask   Key Description       Algorithm      Supported
 *   0       development key       13.3.4.10.4    NO - use 4 or 15 instead
 *   4       production key        13.3.4.10.5    YES (same as certification)
 *  15       certification key     13.3.4.10.5    YES
 */

/** ZbBdbTouchlinkKeyIndex */
enum ZbBdbTouchlinkKeyIndexT {
    TOUCHLINK_KEY_INDEX_DEVELOPMENT = 0, /**< Development key */
    TOUCHLINK_KEY_INDEX_PRODUCTION = 4, /**< Production key */
    TOUCHLINK_KEY_INDEX_CERTIFICATION = 15 /**< Certification key */
};

/*** Touchlink Steal Flags (ZB_BDB_TLStealFlags) ***/
/* Target is allowed to process a Touchlink Network Start Request */
#define TOUCHLINK_STEAL_START               0x01U
/* Target is allowed to process a Touchlink Join Request */
#define TOUCHLINK_STEAL_JOIN                0x02U
/* Target is allowed to process a Touchlink Network Start Request if already joined to network. */
#define TOUCHLINK_STEAL_ONNET_START         0x04U
/* Target is allowed to process a Touchlink Network Join Request if already joined to network. */
#define TOUCHLINK_STEAL_ONNET_JOIN          0x08U
/* Target is allowed to process a Touchlink Factory New (Reset) Request */
#define TOUCHLINK_STEAL_RESET               0x10U
/* Target is allowed to process a Touchlink Factory New (Reset) Request if currently acting as Coordinator. */
#define TOUCHLINK_STEAL_COORD_RESET         0x20U
/* Default is all flags enabled */
#define TOUCHLINK_STEAL_DEFAULT             0x3fU

/*** ZB_BDB_Flags ***/
/* A Zigbee device is supposed to only select potential parents with a cost of 3 or lower.
 * This flag disables that check and allows potential parents with higher cost. */
#define ZB_BDB_FLAG_IGNORE_COST_DURING_JOIN             0x00000001U

/* By default, a Zigbee device is supposed to send a ZDO Permit-Join broadcast to the
 * network after it joins, to keep the network open for other devices to join.
 * This flag disables that behaviour. */
#define ZB_BDB_FLAG_DISABLE_ZDO_PJOIN_AFTER_JOIN        0x00000002U

/* By default, the stack will perform a network rejoin after starting from persistence,
 * if it's an end-device. This flag disables that. */
#define ZB_BDB_FLAG_DISABLE_REJOIN_AFTER_PERSIST        0x00000004U

/* By default, a Zigbee device is supposed to send a ZDO Device-Annce broadcast to the
 * network after it joins and whenever it changes short addresses.
 * This flag disables that behaviour. */
#define ZB_BDB_FLAG_DISABLE_DEVICE_ANNCE                0x00000008U

/* Controls whether we allow the local device acting as a Router to perform a
 * Network Rejoin after receiving a Network Leave with the Rejoin flag set.
 * It normally doesn't make sense for this type of behaviour with a Router, only
 * an End-Device. */
#define ZB_BDB_FLAG_ALLOW_ROUTER_LEAVE_REJOIN           0x00000010U

/* Deprecated flag - 0x00000020U */

/* Disable the reception and processing of APS InterPAN messages */
#define ZB_BDB_FLAG_APS_INTERPAN_DISABLE                0x00000040U

/* If set and (ZB_BDB_JoinScanType == ZB_SCAN_TYPE_ENHANCED), don't try to do a standard
 * beacon request when trying to scan for a network to join. If cleared and
 * (ZB_BDB_JoinScanType == ZB_SCAN_TYPE_ENHANCED), the startup joining process
 * will fallback to attempt ZB_SCAN_TYPE_ACTIVE if no networks are found. */
#define ZB_BDB_FLAG_JOIN_SCAN_FALLBACK_DISABLE          0x00000080U

/* If set, disable the ZDO from periodically querying the NNT for TX failures and
 * starting an ED scan if it encounters a high failure rate. If the Coordinator
 * does not support or provide frequency agility to the network, this is a waste of
 * time. The ED scan also causes the device to go offline for a period of time,
 * which can cause further network problems. */
#define ZB_BDB_FLAG_ZDO_NWK_UPDATE_SCAN_DISABLE         0x00000100U

/*< If set, DLK is not allowed by the responder device and it terminates the DLK
 * request from the initiator with NOT_AUTHORIZED error response. */
#define ZB_BDB_FLAG_DLK_NOT_ALLOWED                     0x00000200U

/* If set, concurrent CBKE sessions with different intitiators are disabled.
 * This is required test functionality for SE 1.4 test case 15.22 */
#define ZB_BDB_FLAG_CBKE_SINGLE_SESSION                 0x00000400U

/* Sets mcp_macMaxFrameRetries for testing */
#define ZB_BDB_FLAG_MAC_FRM_RETRIES_ZERO                0x00000800U

/* If set, forcibly set Disable Default Response bit to 0 in the ZCL header.
 * This is used in SE test cases ie. 11.48-11.49 to ensure that, upon receiving a
 * Read/Write Attributes Request, a device will omitsending a Default Response
 * as per the second criteria of 07-5123-08 Zigbee Cluster Library section
 * "2.5.12.2 When Generated...
 * "No other command is sent in response to the received command,
 * using the same Transaction sequence number as the received command." */
#define ZB_BDB_FLAG_ZCL_CLEAR_DEFAULT_RESPONSE_BIT      0x00001000U
#define ZB_BDB_FLAG_ZCL_SET_DEFAULT_RESPONSE_BIT        0x00002000U

/* By default, the stack will attempt a network rejoin on the BDB Primary & Secondary
 * Channel sets, provided network rejoin on the current operating channel and the
 * APS Channel mask were unsuccessful and there are valid BDB Channel masks present.
 * The said default behaviour can be disabled using below BDB flag. */
#define ZB_BDB_FLAG_DISABLE_REJOIN_ON_BDB_CHANNEL_MASK  0x00004000U

/** BDB IB Attributes */
enum ZbBdbAttrIdT {
    /* Removed -- 0x1000 (ZB_BDB_CommissioningGroupID) */
    ZB_BDB_CommissioningMode = 0x1001,
    /**< bdbCommissioningMode - BDB_COMMISSION_MODE_MASK
     * (type: uint8_t, reset: no, persist: no) */
    /* Removed -- 0x1002 (ZB_BDB_JoiningNodeEui64) */
    /* Removed -- 0x1002 (ZB_BDB_JoiningNodeNewTCLinkKey) */
    ZB_BDB_JoinUsesInstallCodeKey = 0x1004,
    /**< bdbJoinUsesInstallCodeKey - Deprecated in BDB 3.1
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_NodeCommissioningCapability = 0x1005,
    /**< bdbNodeCommissioningCapability
     * Deprecated in BDB 3.1 (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_NodeIsOnANetwork = 0x1006,
    /**< bdbNodeIsOnANetwork - Checks nwkExtendedPanId if non-zero
     * Deprecated in BDB 3.1 (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_NodeJoinLinkKeyType = 0x1007,
    /**< bdbNodeJoinLinkKeyType - BDB_JOINLINK_KEYTYPE_FLAG, Link key with which
     * the node was able to decrypt the network key
     * Deprecated in BDB 3.1 (type: uint8_t, reset: yes, persist: no) */
    ZB_BDB_PrimaryChannelSet = 0x1008,
    /**< bdbPrimaryChannelSet (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_ScanDuration = 0x1009,
    /**< bdbcfScanDuration (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_SecondaryChannelSet = 0x100a,
    /**< bdbSecondaryChannelSet (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_TCLK_ExchangeAttempts = 0x100b,
    /**< bdbTCLinkKeyExchangeAttempts - Deprecated in BDB 3.1.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_TCLK_ExchangeAttemptsMax = 0x100c,
    /**< bdbTCLinkKeyExchangeAttemptsMax - Deprecated in BDB 3.1.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_TCLinkKeyExchangeMethod = 0x100d,
    /**< bdbTCLinkKeyExchangeMethod - Deprecated in BDB 3.1
     * (type: uint8_t / enum ZbBdbLinkKeyExchMethodT, reset: no, persist: no) */
    ZB_BDB_TrustCenterNodeJoinTimeout = 0x100e,
    /**< bdbTrustCenterNodeJoinTimeout - Deprecated in BDB 3.1
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_TrustCenterRequiresKeyExchange = 0x100f,
    /**< bdbTrustCenterRequireKey-Exchange. Modifies ZB_APSME_POLICY_TCLK_UPDATE_REQUIRED bit
     * in ZB_APS_IB_ID_TRUST_CENTER_POLICY - Deprecated in BDB 3.1
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_AcceptNewUnsolicitedTCLinkKey = 0x1010,
    /**< acceptNewUnsolicitedTrustCenterLinkKey (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_AcceptNewUnsolicitedApplicationLinkKey = 0x1011,
    /**< acceptNewUnsolicitedApplicationLinkKey (type: uint8_t, reset: no, persist: no) */

    /* Extra stuff not explicitly covered by the BDB spec. */
    /* discontinuity */
    ZB_BDB_vDoPrimaryScan = 0x1101,
    /**< Boolean whether to use ZB_BDB_PrimaryChannelSet or ZB_BDB_SecondaryChannelSet
     * (type: uint8_t, reset: no, persist: no) */

    /* Address assignment */
    ZB_BDB_FreeNetAddrBegin = 0x1102,
    /**< (type: uint16_t, reset: no, persist: no) */
    ZB_BDB_FreeNetAddrCurrent = 0x1103,
    /**< (type: uint16_t, reset: no, persist: no) */
    ZB_BDB_FreeNetAddrEnd = 0x1104,
    /**< (type: uint16_t, reset: no, persist: no) */
    /* discontinuity */
    ZB_BDB_TLRssiMin = 0x1107,
    /**< Minimum RSSI threshold for Touchlink commissioning
     * (type: int8_t, reset: no, persist: no) */
    /* discontinuity */
    ZB_BDB_UpdateDeviceKeyId = 0x1109,
    /**< E.g. ZB_SEC_KEYID_NETWORK (default) or ZB_SEC_KEYID_LINK.
     * (type: uint8_t / enum ZbSecHdrKeyIdT, reset: no, persist: no) */
    ZB_BDB_JoinScanType = 0x110a,
    /**< ZB_SCAN_TYPE_ACTIVE (default) or ZB_SCAN_TYPE_ENHANCED
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_PrevJoinScanType = 0x110b,
    /**< (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_NlmeSyncFailNumBeforeError = 0x110c,
    /**< Number of consecutive NLME-SYNC failures before reporting
     * ZB_NWK_STATUS_CODE_PARENT_LINK_FAILURE (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_ZdoTimeout = 0x110d,
    /**< ZDO response wait timeout in milliseconds - default is 6000 mS.
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_TLStealFlags = 0x110e,
    /**< Touchlink Stealing Flags (TOUCHLINK_STEAL_xxx defines.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_JoinTclkNodeDescReqDelay = 0x110f,
    /**< (type: uint16_t, reset: no, persist: no) */
    ZB_BDB_JoinTclkRequestKeyDelay = 0x1110,
    /**< (type: uint16_t, reset: no, persist: no) */
    /* discontinuity */
    ZB_BDB_TLKey = 0x1112,
    /**< Touchlink preconfigured link key
     * (type: array of uint8_t, reset: no, persist: yes) */
    ZB_BDB_TLKeyIndex = 0x1113,
    /**< Touchlink key encryption algorithm key index
     * (type: uint8_t / enum ZbBdbTouchlinkKeyIndexT, reset: no, persist: yes) */
    /* discontinuity */
    ZB_BDB_ZdoZigbeeProtocolRevision = 0x1115,
    /**< Default = 23 (R23) (type: uint8_t, reset: no, persist: no) */
    /* discontinuity */
    ZB_BDB_PersistTimeoutMs = 0x1117,
    /**< Minimum delay between persistence updates. Default value is 10000 mS (10 sec).
     * This delay helps to group a bunch of persistence notifications due to multiple
     * parameters being modified into a single notification and eventual persistence
     * write to Flash.
     *
     * WARNING: Be careful with how often your application saves persistence data
     * to Flash, if this is applicable to your platform.
     * Excessive Flash writes can lead to wear and eventual failure of the memory.
     * Minimize write operations and use wear-leveling techniques to extend the
     * lifespan of the Flash.
     *
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_JoinAttemptsMax = 0x1118,
    /**< Maximum number attempts to join a network. If an attempt fails,
     * the EPID is added to a blacklist before the next attempt.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_MaxConcurrentJoiners = 0x1119,
    /**< Maximum number of concurrent joiners the coordinator supports.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_PersistImmedTimeoutMs = 0x111a,
    /**< Minimum delay between "immediate" persistence updates. Default value is 500 mS.
     * Used in conjunction with the ZCL attribute flag ZCL_ATTR_FLAG_PERSIST_IMMED.
     * If ZB_BDB_PersistTimeoutMs is less than the value of this parameter, then
     * ZB_BDB_PersistTimeoutMs takes presedence.
     *
     * WARNING: Be careful with how often your application saves persistence data
     * to Flash, if this is applicable to your platform.
     * Excessive Flash writes can lead to wear and eventual failure of the memory.
     * Minimize write operations and use wear-leveling techniques to extend the
     * lifespan of the Flash.
     *
     * (type: uint32_t, reset: no, persist: no) */

    /* discontinuity */
    ZB_BDB_Uptime = 0x111d,
    /**< Returns the current stack uptime in milliseconds (ZbUptime).
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_Flags = 0x111e,
    /**< e.g. ZB_BDB_FLAG_IGNORE_COST_DURING_JOIN
     * (type: uint32_t, reset: no, persist: no) */
    /* discontinuity */
    ZB_BDB_TC_Supports_ZD = 0x1120,
    /**< Boolean value that indicates whether TC supports Zigbee Direct.
     * This value is automatically discovered after joining by Match-Desc.request
     * for ZCL_CLUSTER_ZDD. (type: uint8_t, reset: yes, persist: yes) */
    ZB_BDB_ZVD_SecureSessionRenegotiated = 0x1121,
    /**< Boolean value that indicates if ZVD successfully renegotiated secure
     * session during network join or rejoin.
     * (type: uint8_t, reset: yes, persist: no) */
    ZB_BDB_TLIdentifyTime = 0x1122,
    /**< Touchlink Indetify Time Duration - default is 7 seconds.
     * Setting the IB to zero will disable sending the TL identify request.
     * (type: uint16_t, reset: no, persist: no) */
    ZB_BDB_AppendBeaconAppendix = 0x1123,
    /**< Boolean value that determines if the Beacon appendix will be appended to
     * the beacons sent from an R23 device. By default, this is always true.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_StartupLongDelayMs = 0x1124,
    /**< Configurable delay added to the end of a successful ZbStartup when joining
     * or rejoining a network. This is to give the stack time to send and receive
     * a NWK Link Status command which is necessary for routing. Otherwise, the
     * application may be trying to send packets right away and get failures until the
     * first Link Status commands are exchanged. This delay is not applicable to
     * end-devices or when forming a network. The default value is 17000 mS (17 seconds).
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_ZclBomdReqWaitTimeoutMs = 0x1125,
    /**< Timeout value in milliseconds to wait for a ZCL request sent to a BOMD device,
     * via mirroring. Default is 10 minutes.
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_ZclBomdReqQueueTimeoutMs = 0x1126,
    /**< Timeout value in milliseconds for the ZCL request to be queued for a BOMD device
     * on the Mirror. If this timeout expires before the BOMD polls for this message,
     * a Default Response with status of ZCL_STATUS_TIMEOUT is returned to the requestor.
     * The default timeout is 10 minutes minus 10 seconds (slightly less than
     * ZB_BDB_ZclBomdReqWaitTimeoutMs).
     * (type: uint32_t, reset: no, persist: no) */
    ZB_BDB_ZclSequenceNumber = 0x1127,
    /**< API to get/set the ZCL Sequence number provided by the stack.
     * (type: uint8_t, reset: no, persist: no) */
    ZB_BDB_KE_Status = 0x1128,
    /**< Key Establishment status code from CBKE during ZbStartup.
     * The initial value is set to ZCL_KEY_STATUS_NOT_STARTED (Exegin extension to the status codes)
     * at the beginning of ZbStartup. It will be set to the status code returned by the KE process.
     * It's a uint8_t type, but it's based on the 'enum ZbZclKeyStatusT' values.
     * (type: uint8_t, reset: no, persist: no) */

    /* Constants which are accessible through a BDB GET IB request. */
    ZB_BDBC_MaxSameNetworkRetryAttempts = 0x1200,
    /**< (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_MinCommissioningTime = 0x1201,
    /**< Seconds (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_RecSameNetworkRetryAttempts = 0x1202,
    /**< (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_TLInterPANTransIdLifetime = 0x1203,
    /**< Seconds (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_TLMinStartupDelayTime = 0x1204,
    /**< Seconds (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_TLRxWindowDuration = 0x1205,
    /**< Seconds (type: uint8_t, reset: no, persist: no) */
    ZB_BDBC_TLScanTimeBaseDuration = 0x1206,
    /**< Milliseconds (type: uint8_t, reset: no, persist: no) */

    ZB_BDB_RequireCBKESuccess = 0x1305
        /**< A boolean flag to indicate how to handle CBKE failure as part of startup join.
         * (type: uint8_t, reset: no, persist: no) */
};

/**
 * Read a BDB IB attribute.
 * @param zb Zigbee stack instance
 * @param attrId Attribute ID
 * @param attrPtr Pointer to the attribute
 * @param attrSz Attribute size
 * @param attrIndex Index attribute
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZbStatusCodeT ZbBdbGetIndex(struct ZigBeeT *zb, enum ZbBdbAttrIdT attrId,
    void *attrPtr, unsigned int attrSz, unsigned int attrIndex);

/**
 * Write a BDB IB attribute.
 * @param zb Zigbee stack instance
 * @param attrId Attribute ID
 * @param attrPtr Pointer to the attribute
 * @param attrSz Attribute size
 * @param attrIndex Index attribute
 * @return ZCL_STATUS_SUCCESS if successful, or other ZclStatusCodeT value on error
 */
enum ZbStatusCodeT ZbBdbSetIndex(struct ZigBeeT *zb, enum ZbBdbAttrIdT attrId,
    const void *attrPtr, unsigned int attrSz, unsigned int attrIndex);

#define ZbBdbGet(_zb_, _id_, _ptr_, _sz_) ZbBdbGetIndex(_zb_, _id_, _ptr_, _sz_, 0)
#define ZbBdbSet(_zb_, _id_, _ptr_, _sz_) ZbBdbSetIndex(_zb_, _id_, _ptr_, _sz_, 0)

/* Configures the endpoint with the given commissioning status. Mostly for internal use only. */
void ZbBdbSetEndpointStatus(struct ZigBeeT *zb, enum ZbBdbCommissioningStatusT status, uint8_t endpoint);

/**
 * Get commissioning status for the given endpoint (same for all endpoints?).
 * If endpoint = ZB_ENDPOINT_BCAST, returns the status for the first endpoint found.
 * @param zb Zigbee instance
 * @param endpoint Endpoint identifier
 * @return Returns ZB_BDB_COMMISS_STATUS_SUCCESS on success, other BDB status code on failure
 */
enum ZbBdbCommissioningStatusT ZbBdbGetEndpointStatus(struct ZigBeeT *zb, uint8_t endpoint);

#endif
