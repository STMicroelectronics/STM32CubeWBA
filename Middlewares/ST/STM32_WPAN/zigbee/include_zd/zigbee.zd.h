/* Copyright [2019 - 2024] Exegin Technologies Limited. All rights reserved. */

#ifndef ZIGBEE_ZD_H
#define ZIGBEE_ZD_H

#include "zigbee.h"
#include "zcl/general/zcl.identify.h"

#include "sha.h"

/*-----------------------------------------------------------------------------
 * Zigbee Direct and Characteristic Attribute Sizes
 *-----------------------------------------------------------------------------
 */
#define ZD_SEC_KEYSIZE                      16U
#define ZD_AES_BLOCK_SIZE                   16U /* same as AES_BLOCK_SIZE */

#define ZDD_BLE_MSG_SIZE_MAX                160U /* safe value */

/* Characteristic data sizes. All made a bit larger than max expected payload. */
#define ZDD_CHAR_SZ_FORM                    140U /* Max length can be 128 bytes, if all TLVs present, with security. */
#define ZDD_CHAR_SZ_JOIN                    (ZDD_CHAR_SZ_FORM) /* Same format as Form. Max length can be 120 bytes. */
#define ZDD_CHAR_SZ_PJOIN                   12U /* max size = FC (4) + payload (1) + MIC (4) */
#define ZDD_CHAR_SZ_LEAVE                   12U /* max size = FC (4) + payload (2) + MIC (4) */
#define ZDD_CHAR_SZ_STATUS                  80U /* max size = FC (4) + payload (61) + MIC (4) */

#define ZDD_CHAR_SZ_MANAGE_JOINERS          60U /* max size = FC (4) + payload (31) + MIC (4) */
#define ZDD_CHAR_SZ_IDENTIFY                12U /* max size = FC (4) + payload (2) + MIC (4) */
#define ZDD_CHAR_SZ_FINDBIND                12U /* max size = FC (4) + payload (2) + MIC (4) */

#define ZDD_CHAR_SZ_SECURITY                60U /* max size = FC (4) + payload (40) + MIC (4) */

#define ZDD_CHAR_SZ_TUNNEL                  ZDD_BLE_MSG_SIZE_MAX /* max size = FC (4) + payload (127) + MIC (4) */

/*-----------------------------------------------------------------------------
 * Characteristic UUIDs
 *-----------------------------------------------------------------------------
 */
#define ZD_SEC_SERVICE_UUID_LEN       16U
#define ZD_SEC_CHAR_UUID_LEN          16U
#define ZD_SEC_TUNNEL_UUID_LEN        16U
#define ZD_SEC_AUTH_DATA_LENGTH       34U

extern const uint8_t commissioning_service_UUID[ZD_SEC_SERVICE_UUID_LEN];
extern const uint8_t commissioning_form_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_join_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_pjoin_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_leave_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_status_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_manage_joiners_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_identify_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t commissioning_findbind_UUID[ZD_SEC_CHAR_UUID_LEN];
extern const uint8_t tunnelling_UUID[ZD_SEC_TUNNEL_UUID_LEN];
extern const uint8_t npdu_UUID[ZD_SEC_TUNNEL_UUID_LEN];

/*-----------------------------------------------------------------------------
 * Enums
 *-----------------------------------------------------------------------------
 */
enum zd_sec_msg_type {
    ZD_SEC_MSG_TYPE_1 = 1U,
    /**< SE Message 1 is sent from ZVD to ZDD and is used to initiate the SE protocol and send PSK Type
     * (Pre-shared Secrets Bitmask) and ephemeral public key Qi. */
    ZD_SEC_MSG_TYPE_2 = 2U,
    /**< SE Message 2 is sent from ZDD to ZVD and is used to send ephemeral public key Qr and the NWK
     * key sequence number.  */
    ZD_SEC_MSG_TYPE_3 = 3U,
    /**< SE Message 3 is sent from ZVD to ZDD and is used by ZDD to confirm the same session key was
     * derived on both sides. The message payload containing MacTag as defined below.  */
    ZD_SEC_MSG_TYPE_4 = 4U
        /**< SE Message 4 is sent from ZDD to ZVD and is used by ZVD to confirm the same session key was
         * derived on both sides. The message payload containing the MacTag as defined below. */
};

/** Enumeration for selected key negotiation methods. (ZD_TLV_KEY_NEGO_REQ_SEL_KEY_NEGO_METHOD) */
enum zd_sel_key_nego_method {
    ZD_SEL_KEY_NEGO_METH_RESERVED = 0U, /**< Reserved */
    ZD_SEL_KEY_NEGO_METH_C25519_AES_MMO_128 = 1U, /**< Curve25519 with Hash AES-MMO-128 */
    ZD_SEL_KEY_NEGO_METH_C25519_SHA_256 = 2U, /**< Curve25519 with Hash SHA-256 */
    ZD_SEL_KEY_NEGO_METH_P256_SHA_256 = 3U, /**< P256 with Hash SHA-256 */
    /* 4 - 255 reserved */
};

/** Enumeration for selected PSK (Pre-shared) secret. (ZD_TLV_KEY_NEGO_REQ_SEL_KEY_NEGO_METHOD) */
enum zd_sel_psk_type {
    ZD_SEL_PSK_SYMMETRIC_AUTH_TOKEN = 0x00U, /**< Symmetric authentication token relevant for Zigbee Direct??? */
    ZD_SEL_PSK_IC_DERIVED_LINK_KEY = 0x01U, /**< Pre-configured link-key derived from installation code */
    ZD_SEL_PSK_VARIABLE_LEN_PASSCODE_KEY = 0x02U, /**< Link-key derived from variable-length pass code */
    ZD_SEL_PSK_BASIC_ACCESS_KEY = 0x03U, /**< Basic Access Key */
    ZD_SEL_PSK_ADMIN_ACCESS_KEY = 0x04U, /**< Administrative Access Key */
    /* 05 - 254 reserved */
    ZD_SEL_PSK_ANON_WELL_KNOWN_SECRET = 0xffU, /**< Well-known secret - 'ZigBeeAlliance18' */
};

enum zd_commiss_service_type {
    ZD_COMMISS_SERVICE_TYPE_FORM, /* commissioning_form_UUID */
    ZD_COMMISS_SERVICE_TYPE_JOIN, /* commissioning_join_UUID */
    ZD_COMMISS_SERVICE_TYPE_PJOIN, /* commissioning_pjoin_UUID */
    ZD_COMMISS_SERVICE_TYPE_LEAVE, /* commissioning_leave_UUID */
    ZD_COMMISS_SERVICE_TYPE_STATUS, /* commissioning_status_UUID */
    ZD_COMMISS_SERVICE_TYPE_MANAGE_JOINERS, /* commissioning_manage_joiners_UUID */
    ZD_COMMISS_SERVICE_TYPE_IDENTIFY, /* commissioning_identify_UUID */
    ZD_COMMISS_SERVICE_TYPE_FINDBIND /* commissioning_findbind_UUID */
};

/* Nwk status map */
enum zd_nwk_status_map_t {
    ZD_NWK_STATUS_NOT_COMMISSIONED = 0x00U,
    ZD_NWK_STATUS_COMMISSIONING_IN_PROGRESS = 0x01U,
    ZD_NWK_STATUS_COMMISSIONED = 0x02U
};

/* ZD_TLV_COMM_NWK_STATUS_MAP */
#define ZD_NWK_STATUS_MAP_JOIN_STATUS_NONE          0x00U /* Not commissioned */
#define ZD_NWK_STATUS_MAP_JOIN_STATUS_INPROG        0x01U /* Commissioning in progress */
#define ZD_NWK_STATUS_MAP_JOIN_STATUS_DONE          0x02U /* Commissioned */
#define ZD_NWK_STATUS_MAP_JOIN_STATUS_MASK          0x03U
#define ZD_NWK_STATUS_MAP_NWK_OPEN                  0x08U
#define ZD_NWK_STATUS_MAP_NWK_CENTRAL               0x10U

/* Nwk type */
#define ZD_NWK_STATUS_NWK_DISTRIBUTED               0x00U
#define ZD_NWK_STATUS_NWK_CENTRALIZED               0x10U

enum zdd_status_domain_t {
    ZDD_STATUS_DOMAIN_GENERAL = 0x00U,
    ZDD_STATUS_DOMAIN_FORM_NWK = 0x01U,
    ZDD_STATUS_DOMAIN_JOIN_NWK = 0x02U,
    ZDD_STATUS_DOMAIN_PJOIN = 0x03U,
    ZDD_STATUS_DOMAIN_LEAVE = 0x04U,
    ZDD_STATUS_DOMAIN_MANAGE_JOINERS = 0x05U,
    ZDD_STATUS_DOMAIN_IDENTIFY = 0x06U,
    ZDD_STATUS_DOMAIN_FINDBIND = 0x07U,
    ZDD_STATUS_DOMAIN_INTERNAL_ZD_CLUSTER = 0xfeU,
    /**< Internal use only! used in case of updates from Zigbee Direct
     * configuration cluster. No need to send the commissioning status
     * notification in this case. */
    ZDD_STATUS_DOMAIN_INTERNAL_UPDATE = 0xffU
        /**< Internal use only! Same as ZDD_STATUS_DOMAIN_GENERAL, but don't
         * send if in the process of forming or joining. */
};

/*---------------------------------------------------------------
 * ECDHE Defines
 *---------------------------------------------------------------
 */

/*
 * Lists the supported curves to perform ECDHE key derivation.
 */
enum zd_ecdhe_curve_t {
    ZD_ECDHE_CURVE_C25519 = 0U, /**< ECDHE curve C25519. */
    ZD_ECDHE_CURVE_P256 = 1U, /**< ECDHE curve P256. */
    ZD_ECDHE_CURVE_INVALID = 2U /**< Invalid curve marker. */
};

/* Common macro's for C25519 & P256. */
#define ZD_ECDHE_PRIVATE_KEY_SIZE               32U
#define ZD_ECDHE_SECRET_POINT_SIZE              32U

/* C25519 specific macro's. */
#define ZD_ECDHE_C25519_BASE_POINT_SIZE         32U
#define ZD_ECDHE_C25519_PUB_POINT_SIZE          32U
#define ZD_ECDHE_C25519_SESSION_ID_SIZE         80U

/* P256 specific macro's. */
#define ZD_ECDHE_P256_PUB_POINT_SIZE            64U
#define ZD_ECDHE_P256_PUB_POINT_X_SIZE          32U
#define ZD_ECDHE_P256_PUB_POINT_Y_SIZE          32U
#define ZD_ECDHE_P256_SESSION_ID_SIZE           144U

/* NPDU TLV Flags field defines */
#define ZD_NPDU_FLAGS_SECURITY_ENABLE       0x01U

#define ZD_NPDU_LENGTH_MAX                  0x62U

/*---------------------------------------------------------------
 * Common Security Defines
 *---------------------------------------------------------------
 */
/* The length of the CCM* nonce. */
#define ZD_SEC_NONCE_LENGTH                 13U

#define ZD_SEC_LEVEL_NONE                   0x00U
#define ZD_SEC_LEVEL_MIC32                  0x01U
#define ZD_SEC_LEVEL_MIC64                  0x02U
#define ZD_SEC_LEVEL_MIC128                 0x03U
#define ZD_SEC_LEVEL_ENC                    0x04U
#define ZD_SEC_LEVEL_ENC_MIC32              (uint8_t)(ZD_SEC_LEVEL_ENC | ZD_SEC_LEVEL_MIC32)
#define ZD_SEC_LEVEL_ENC_MIC64              (uint8_t)(ZD_SEC_LEVEL_ENC | ZD_SEC_LEVEL_MIC64)
#define ZD_SEC_LEVEL_ENC_MIC128             (uint8_t)(ZD_SEC_LEVEL_ENC | ZD_SEC_LEVEL_MIC128)

#define ZD_SEC_MIC_LEN                      0x04U

/* SecureSessionTimeout. */
#define ZD_SEC_SESSION_TIMEOUT_MS           15000U /* 15 seconds */

/* Zigbee Direct PSK size. */
#define ZD_SEC_PSK_SIZE                     16U

/*-----------------------------------------------------------------------------
 * Default Security Keys
 *-----------------------------------------------------------------------------
 */
/* "ZigBeeAlliance18" */
extern const uint8_t zdd_default_anon_key[ZD_SEC_KEYSIZE];
/* For testing only, some default keys */
extern const uint8_t zdd_test_default_basic_key[ZD_SEC_KEYSIZE];
extern const uint8_t zdd_test_default_admin_key[ZD_SEC_KEYSIZE];

/*---------------------------------------------------------------
 * Session
 *---------------------------------------------------------------
 */
enum zd_sec_state_t {
    ZD_SEC_STATE_NONE = 0x00U,
    /**< Secure session not active or complete */
    ZD_SEC_STATE_ACTIVE_MSG_3 = 0x01U,
    /**< Secure session active - waiting for ZD_SEC_MSG_TYPE_3 */
    ZD_SEC_STATE_DONE_ANON_NONE = 0x02U,
    /**< Secure session done - used anonymous method. ZVD only has unsecured NPDU access. */
    ZD_SEC_STATE_LIMITED_AUTH = 0x03U,
    /**< Secure session done - used basic key derived from old network key. ZVD can tx/rx
     * unsecured NPDU frames (only network commands for trust center rejoin) */
    ZD_SEC_STATE_EPHEM_UNAUTH_ZVD = 0x04U,
    /**< Secure session done - used anonymous method. ZVD can tx/rx secured NPDU frames
     * (only APS secured commands) */
    ZD_SEC_STATE_EPHEM_AUTH_ZVD = 0x05U,
    /**< Secure session done - used anonymous method. ZVD can tx/rx secured NPDU frames
     * (only APS messages and no network commands, can be unsecured) */
    ZD_SEC_STATE_DONE_BASIC = 0x06U,
    /**< Secure session done - used Basic Key. ZVD can only tx/rx NPDU frames (can be secured) */
    ZD_SEC_STATE_DONE_ADMIN = 0x07U,
    /**< Secure session done - used Admin Key. ZVD has full access */
    ZD_SEC_STATE_DONE_ANON_FULL = 0x08U
        /**< Secure session done - used anonymous method, but ZVD has full access (Provisioning Session) */
};

struct zd_sec_session_t {
    struct ZigBeeT *zb; /* may be NULL, depending on platform */
    bool is_zdd;
    bool on_net;
    bool net_open;
    bool anon_join_timer_active;
    enum zd_sec_state_t state;
    ZbUptimeT sec_timeout;
    uint64_t zdd_eui64;
    uint8_t zdd_pub_point[ZD_ECDHE_P256_PUB_POINT_SIZE]; /* Max length to hold either C25519 & P256 pub point. */
    uint64_t zvd_eui64;
    uint8_t zvd_pub_point[ZD_ECDHE_P256_PUB_POINT_SIZE]; /* Max length to hold either C25519 & P256 pub point. */
    uint8_t psk[ZD_SEC_PSK_SIZE];

    enum zd_sel_key_nego_method sel_key_nego_method;
    enum zd_sel_psk_type sel_key_type;
    enum zd_sec_state_t sel_state; /* ZDD use only */
    bool sel_is_aes_mmo_128;
    enum zd_ecdhe_curve_t curve;

    /* Frame counters */
    uint32_t outgoing_fc;
    uint32_t incoming_fc;

    /* Address of ZVD which provisioned the ZDD. */
    uint64_t prov_zvd_eui64;

    /* Active network key sequence number in case of provisioned ZDD. This will be included in SE_MSG_2. */
    uint8_t nwk_key_seq_num;

    /* temporary buffers, generated keys */
    uint8_t notify_buf[ZDD_CHAR_SZ_SECURITY]; /* security message notification data */
    uint8_t zdd_mac_tag[USHAMaxHashSize]; /* Max length to hold either C25519 & P256 MAC TAG. */
    uint8_t shared_sec[ZD_ECDHE_SECRET_POINT_SIZE];
    uint8_t session_id[ZD_ECDHE_P256_SESSION_ID_SIZE]; /* Max length to hold either C25519 & P256 session Id. */
    uint8_t private_key[ZD_ECDHE_PRIVATE_KEY_SIZE];
    uint8_t generator[ZD_ECDHE_C25519_BASE_POINT_SIZE];
    uint8_t session_key[ZD_SEC_KEYSIZE];
};

/*-----------------------------------------------------------------------------
 * ZDD Security Information
 *-----------------------------------------------------------------------------
 */
#define ZDD_KEY_TABLE_SZ                            4U

struct zdd_key_entry_t {
    uint8_t key[ZD_SEC_PSK_SIZE];
    enum zd_sel_psk_type type;
    uint8_t seqno; /* valid only in case of basic key. */
    bool valid;
};

struct zdd_sec_info_t {
    struct zdd_key_entry_t keys[ZDD_KEY_TABLE_SZ];
    struct zd_sec_session_t session;
};

/*-----------------------------------------------------------------------------
 * ZDD APIs
 *-----------------------------------------------------------------------------
 */

/** zb_zdd_init configuration */
struct zb_zdd_config_t {
    struct ZbStartupT startup;
    /**< Default ZbStartup configuration to use when ZVD sends the command
     * to Form or Join a Zigbee network. Initialize this struct by calling
     * ZbStartupConfigGetProDefaults, and then add any configuration desired
     * by your application. Note that the ZVD may overwrite some of these
     * values. */

    bool open_tunnel;
    /**< Whether to open a tunneling interface for the ZDD with the stack.
     * If false, then the ZDD BLE interface is only used for commissioning. */

    uint8_t zdd_server_endpt;
    /**< Defines the endpoint to allocate the ZDD Server Cluster endpoint,
     * or set to zero if don't want to allocate the cluster. It will also allocate an Identify Server
     * cluster on this endpoint, which is required for a ZDD. */

    struct zcl_identify_server_callbacks_t identify_callbacks;
    /**< Identify Server Callbacks. */

    void *arg;
    /**< Application defined argument included with any callbacks. */

    const char *adapter_name;
    /**< Used on linux systems to indicate which BlueTooth adapter to use (E.g. hci1).
     * Set to NULL if not applicable. */
    const char *virtual_path;
    /**< Used on linux systems for the virtual interface. Set to NULL if not applicable. */
    const char *virtual_sniffer;
    /**< Used on linux systems for the virtual interface. Set to NULL if not applicable. */
};

/**
 * Initializes the Zigbee Direct ZDD. BLE Advertising starts once called.
 * @param zb
 * @param config ZDD Configuration (see documentation for 'struct zb_zdd_config_t')
 * @return True if successfully initialized, false otherwise.
 */
bool zb_zdd_init(struct ZigBeeT *zb, struct zb_zdd_config_t *config);

void zb_zdd_destroy(struct ZigBeeT *zb);

/**
 * Update advertising data and enable advertisements
 * @param zb
 * @param forced (Re)enable advertisements, even if the advertising data hasn't changed.
 * @return None
 */
void zb_zdd_update_advert(struct ZigBeeT *zb, bool forced);

/* Add a ZDD Security Key */
bool zb_zdd_sec_key_add(struct ZigBeeT *zb, enum zd_sel_psk_type type, const uint8_t *key, uint8_t seqno);

/* Write to a ZDD Server Attribute */
enum ZclStatusCodeT zb_zdd_server_write_attr(struct ZigBeeT *zb, uint16_t attrId, long long value);

/*
 * The following are for Internal Use Only
 */

/* NOTE: linux use only */
bool zdd_port_process_input(struct ZigBeeT *zb);
void zdd_port_kill(struct ZigBeeT *zb);

/* NOTE: Golden Unit use only */
void zdd_gu_set_invalid_fc(struct ZigBeeT *zb, bool use_invalid_fc);

/* NOTE: For debugging only */
void zdd_sec_print_session(struct ZigBeeT *zb);
void zdd_port_status_info(struct ZigBeeT *zb);

/* EXEGIN - Deprecated function. Do not use. */
bool zb_zdd_allocated(struct ZigBeeT *zb);

/*-----------------------------------------------------------------------------
 * Helpers
 *-----------------------------------------------------------------------------
 */
static inline bool
zd_sec_valid_key(const uint8_t *key)
{
    unsigned int i;

    for (i = 0; i < ZD_SEC_KEYSIZE; i++) {
        if (key[i] != 0U) {
            return true;
        }
    }
    return false;
}

#endif
