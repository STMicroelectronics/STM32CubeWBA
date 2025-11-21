/**
 * @file zigbee.zd.h
 * @heading Zigbee Direct ZDD
 * @brief Zigbee Direct ZDD header file
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 */

#ifndef ZIGBEE_ZD_H
#define ZIGBEE_ZD_H

#include "zigbee.h"
#include "zcl/general/zcl.identify.h"

#define ZD_SEC_KEYSIZE                      16U
#define ZD_AES_BLOCK_SIZE                   16U /* same as AES_BLOCK_SIZE */

/*---------------------------------------------------------
 * Common Security
 *---------------------------------------------------------
 */
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

/*---------------------------------------------------------
 * Default Security Keys
 *---------------------------------------------------------
 */
/* "ZigBeeAlliance18" */
extern const uint8_t zdd_default_anon_key[ZD_SEC_KEYSIZE];
/* For testing only, some default keys */
extern const uint8_t zdd_test_default_basic_key[ZD_SEC_KEYSIZE];
extern const uint8_t zdd_test_default_admin_key[ZD_SEC_KEYSIZE];

/*---------------------------------------------------------
 * ZDD APIs
 *---------------------------------------------------------
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
 * Initializes the Zigbee Direct ZDD. This will also immediately enable BLE advertising.
 * @param zb Zigbee stack instance
 * @param config ZDD Configuration (see documentation for 'struct zb_zdd_config_t')
 * @return True if successfully initialized, false otherwise.
 */
bool zb_zdd_init(struct ZigBeeT *zb, struct zb_zdd_config_t *config);

void zb_zdd_destroy(struct ZigBeeT *zb);

/**
 * Update the advertising data to reflect the current Zigbee stack state,
 * and enable advertisements if there has been a change or 'forced' is set to true.
 * The stack will automatically  update and enable advertisements when the data changes,
 * including enabling Zigbee permit-join.
 * However, note that advertisements will automatically stop after a few minutes.
 * @param zb Zigbee stack instance
 * @param forced (Re)enable advertisements, even if the advertising data hasn't changed.
 * @return None
 */
void zb_zdd_update_advert(struct ZigBeeT *zb, bool forced);

/**
 * Add a ZDD Security Key
 * @param zb Zigbee stack instance
 * @param type Type of key to add (e.g. ZD_SEL_PSK_BASIC_ACCESS_KEY)
 * @param key The key of length ZD_SEC_KEYSIZE (16) bytes.
 * @param seqno The key's sequence number
 * @return true if the key was successfully added, false otherwise.
 */
bool zb_zdd_sec_key_add(struct ZigBeeT *zb, enum zd_sel_psk_type type, const uint8_t *key, uint8_t seqno);

/* Write to a ZDD Server Attribute */
/**
 * Write to an attribute on the local ZDD Server cluster.
 * @param zb Zigbee stack instance
 * @param attrId The attribute Id to write (e.g. ZCL_ZDD_ATTR_ANON_JOIN_TIMEOUT)
 * @param value The new value of the attribute
 * @return true if the key was successfully added, false otherwise.
 */
enum ZclStatusCodeT zb_zdd_server_write_attr(struct ZigBeeT *zb, uint16_t attrId, long long value);

/* NOTE: For debugging only */
void zdd_sec_print_session(struct ZigBeeT *zb);
void zdd_port_status_info(struct ZigBeeT *zb);

#endif
