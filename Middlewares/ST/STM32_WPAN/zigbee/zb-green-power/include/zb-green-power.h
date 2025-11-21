/* Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved. */

#ifndef ZB_GREEN_POWER_H
#define ZB_GREEN_POWER_H

#include <stdint.h>
#include "../aes/zb.aes.h"
#include "ieee802154_api.h"

/*lint -save -e621 "Identifier clash within 31 chars. For readability,
 * we need to exceed the 31 character limit before uniqueness, and
 * extend the limit to 63 charcters.
 * [MISRA Rule 5.1 (REQUIRED), MISRA Rule 5.2 (REQUIRED),
 * MISRA Rule 5.4 (REQUIRED), MISRA Rule 5.5 (REQUIRED)]" */

#define ZGP_CHANNEL_MIN                 11U
#define ZGP_CHANNEL_MAX                 26U

/*-----------------------------------------------------------------------------
 * Common Security Defines and Helpers
 *-----------------------------------------------------------------------------
 */
#define ZGP_KEYSIZE                      16U
#define ZGP_NONCE_LENGTH                 13U
#define ZGP_MIC_LEN                      4U /* ZB_SEC_MIC_LENGTH(ZB_SEC_LEVEL_MIC32) */

extern const uint8_t gpd_key_default_tclk[ZGP_KEYSIZE];

/**
 * Create the AES nonce, defined by the Zigbee Spec.
 * See GP Spec section A.1.5.3.2 Constructing AES Nonce
 * @param gpdId
 * @param counter
 * @param fctrl
 * @param out
 * @return None
 */
void gp_generate_nonce(uint64_t gpdId, uint32_t counter, uint16_t fctrl, void *out);

/**
 *
 * @param appId
 * @param gpdId
 * @param sec_key Security key to use for decryption
 * @param mic
 * @param data The key to decrypt
 * @return
 */
int gp_decrypt_key(uint8_t appId, uint64_t gpdId, const void *sec_key, const void *mic, uint8_t *data);

/*-----------------------------------------------------------------------------
 * Common Command Defines
 *-----------------------------------------------------------------------------
 */
#define ZGP_APPID_SRCID                             (uint8_t)0x00U /* 0b000 */
#define ZGP_APPID_LPED                              (uint8_t)0x01U /* 0b001 */
#define ZGP_APPID_ENDPOINT                          (uint8_t)0x02U /* 0b010 */
/* Used to indicate invalid or unused entries. */
#define ZGP_APPID_INVALID                           (uint8_t)0xffU

/* Device IDs */
#define ZGP_DEVICE_PROXY                            (uint16_t)0x0060U
#define ZGP_DEVICE_PROXY_BASIC                      (uint16_t)0x0061U
#define ZGP_DEVICE_TARGET_PLUS                      (uint16_t)0x0062U
#define ZGP_DEVICE_TARGET                           (uint16_t)0x0063U
#define ZGP_DEVICE_COMMISSIONING_TOOL               (uint16_t)0x0064U
#define ZGP_DEVICE_COMBO                            (uint16_t)0x0065U
#define ZGP_DEVICE_COMBO_BASIC                      (uint16_t)0x0066U

/* Device Version */
#define ZGP_DEVICE_VER                              (uint8_t)0x00U

/* Data Indication Frame Types */
#define ZGP_TYPE_DATA                               (uint8_t)0x00U
#define ZGP_TYPE_MAINTENANCE                        (uint8_t)0x01U

/* Frame control field  */
#define ZGP_FCTRL_TYPE                              ((uint16_t)0x03U << 0U)
#define ZGP_FCTRL_TYPE_DATA                         ((uint16_t)0x00U << 0U)
#define ZGP_FCTRL_TYPE_MAINTENANCE                  ((uint16_t)0x01U << 0U)
#define ZGP_FCTRL_VERSION                           ((uint16_t)0x0fU << 2U)
#define ZGP_FCTRL_VERSION_GREENPOWER                ((uint16_t)0x03U << 2U)
#define ZGP_FCTRL_COMMISSIONING                     ((uint16_t)0x01U << 6U)
#define ZGP_FCTRL_EXTENSION                         ((uint16_t)0x01U << 7U)
/* Extended frame control field */
#define ZGP_FCTRL_APPID_OFFSET                      8U
#define ZGP_FCTRL_APPID_MASK                        ((uint16_t)0x07U << ZGP_FCTRL_APPID_OFFSET)
#define ZGP_FCTRL_SEC_LEVEL_OFFSET                  11U
#define ZGP_FCTRL_SEC_LEVEL_MASK                    ((uint16_t)0x03U << ZGP_FCTRL_SEC_LEVEL_OFFSET)
#define ZGP_FCTRL_SEC_KEY                           ((uint16_t)0x01U << 13U)
#define ZGP_FCTRL_RX_AFTER_TX                       ((uint16_t)0x01U << 14U)
#define ZGP_FCTRL_DIRECTION                         ((uint16_t)0x01U << 15U)

/* A.3.3.2.2.2.6: Security related options */
#define ZGP_SECURITY_OPT_LEVEL                      (0x03U << 0U)
#define ZGP_SECURITY_OPT_KEYTYPE                    (0x07U << 2U)

/* GP-DATA.request Tx Options */
#define ZGP_TXOPTIONS_QUEUE                         (uint8_t)0x01U
#define ZGP_TXOPTIONS_CSMA                          (uint8_t)0x02U
#define ZGP_TXOPTIONS_ACK                           (uint8_t)0x04U
#define ZGP_TXOPTIONS_TYPE                          (uint8_t)0x18U
#define ZGP_TXOPTIONS_DATA                          (uint8_t)(ZGP_TYPE_DATA << 3U)
#define ZGP_TXOPTIONS_MAINTENANCE                   (uint8_t)(ZGP_TYPE_MAINTENANCE << 3U)
#define ZGP_TXOPTIONS_ENDPOINT                      (uint8_t)0x20U

/* ZGP SecurityLevels (Table 11 – Values of gpSecurityLevel) */
enum zgp_security_level_t {
    ZGP_SECURITY_NONE = 0x00, /* 0b00 */
    ZGP_SECURITY_AUTH = 0x02, /* 0b10 */
    ZGP_SECURITY_ENCRYPT = 0x03 /* 0b11 */
};

/* Key Types (Table 53 – Values of gpSecurityKeyType) */
enum zgp_security_keytype_t {
    ZGP_KEYTYPE_NONE = 0x00, /* 0b000 */
    ZGP_KEYTYPE_NWKKEY = 0x01, /* 0b001 - ZigBee Network Key */
    ZGP_KEYTYPE_GPD_GROUP = 0x02, /* 0b010 - GPD Group Key */
    ZGP_KEYTYPE_NWK_DERIVED = 0x03, /* 0b011 - NWK-Key derived GPD Group Key: Kgp = HMAC(Knwk, "ZGP") */
    ZGP_KEYTYPE_PRECONFIG = 0x04, /* 0b100 - Pre-configured GPD security key. */
    ZGP_KEYTYPE_INDIVIDUAL = 0x07 /* 0b111 - Derived individual GPD key: Kgp = HMAC(K, ID) */
};

/*
 * Green Power Command Ids (Tables 54, 55 and 56 of Green Power Basic Spec)
 */
#define ZGP_COMMAND_IDENTIFY                        (uint8_t)0x00U
#define ZGP_COMMAND_RECALL_SCENE(_x_)               (uint8_t)(0x10U + (_x_))
#define ZGP_COMMAND_STORE_SCENE(_x_)                (uint8_t)(0x18U + (_x_))
#define ZGP_COMMAND_OFF                             (uint8_t)0x20U
#define ZGP_COMMAND_ON                              (uint8_t)0x21U
#define ZGP_COMMAND_TOGGLE                          (uint8_t)0x22U
#define ZGP_COMMAND_RELEASE                         (uint8_t)0x23U
#define ZGP_COMMAND_MOVE_UP                         (uint8_t)0x30U
#define ZGP_COMMAND_MOVE_DOWN                       (uint8_t)0x31U
#define ZGP_COMMAND_STEP_UP                         (uint8_t)0x32U
#define ZGP_COMMAND_STEP_DOWN                       (uint8_t)0x33U
#define ZGP_COMMAND_LEVEL_STOP                      (uint8_t)0x34U
#define ZGP_COMMAND_MOVE_UP_ONOFF                   (uint8_t)0x35U
#define ZGP_COMMAND_MOVE_DOWN_ONOFF                 (uint8_t)0x36U
#define ZGP_COMMAND_STEP_UP_ONOFF                   (uint8_t)0x37U
#define ZGP_COMMAND_STEP_DOWN_ONOFF                 (uint8_t)0x38U
#define ZGP_COMMAND_MOVE_HUE_STOP                   (uint8_t)0x40U
#define ZGP_COMMAND_MOVE_HUE_UP                     (uint8_t)0x41U
#define ZGP_COMMAND_MOVE_HUE_DOWN                   (uint8_t)0x42U
#define ZGP_COMMAND_STEP_HUE_UP                     (uint8_t)0x43U
#define ZGP_COMMAND_STEP_HUE_DOWN                   (uint8_t)0x44U
#define ZGP_COMMAND_MOVE_SATURATION_STOP            (uint8_t)0x45U
#define ZGP_COMMAND_MOVE_SATURATION_UP              (uint8_t)0x46U
#define ZGP_COMMAND_MOVE_SATURATION_DOWN            (uint8_t)0x47U
#define ZGP_COMMAND_STEP_SATURATION_UP              (uint8_t)0x48U
#define ZGP_COMMAND_STEP_SATURATION_DOWN            (uint8_t)0x49U
#define ZGP_COMMAND_MOVE_COLOR                      (uint8_t)0x4aU
#define ZGP_COMMAND_STEP_COLOR                      (uint8_t)0x4bU
#define ZGP_COMMAND_LOCK_DOOR                       (uint8_t)0x50U
#define ZGP_COMMAND_UNLOCK_DOOR                     (uint8_t)0x51U
#define ZGP_COMMAND_PRESS_1_OF_1                    (uint8_t)0x60U
#define ZGP_COMMAND_RELEASE_1_OF_1                  (uint8_t)0x61U
#define ZGP_COMMAND_PRESS_1_OF_2                    (uint8_t)0x62U
#define ZGP_COMMAND_RELEASE_1_OF_2                  (uint8_t)0x63U
#define ZGP_COMMAND_PRESS_2_OF_2                    (uint8_t)0x64U
#define ZGP_COMMAND_RELEASE_2_OF_2                  (uint8_t)0x65U
#define ZGP_COMMAND_SHORT_PRESS_1_OF_1              (uint8_t)0x66U
#define ZGP_COMMAND_SHORT_PRESS_1_OF_2              (uint8_t)0x67U
#define ZGP_COMMAND_SHORT_PRESS_2_OF_2              (uint8_t)0x68U
#define ZGP_COMMAND_ATTRIBUTE_REPORTING             (uint8_t)0xa0U
#define ZGP_COMMAND_MFR_REPORTING                   (uint8_t)0xa1U
#define ZGP_COMMAND_MULTI_REPORTING                 (uint8_t)0xa2U
#define ZGP_COMMAND_MFR_MULTI_REPORTING             (uint8_t)0xa3U
#define ZGP_COMMAND_REQUEST_ATTRIBUTES              (uint8_t)0xa4U
#define ZGP_COMMAND_READ_ATTRIBUTES_RESP            (uint8_t)0xa5U
#define ZGP_COMMAND_ZCL_TUNNELING                   (uint8_t)0xa6U
#define ZGP_COMMAND_GPD_SENSOR                      (uint8_t)0xafU
#define ZGP_COMMAND_MFR_MIN                         (uint8_t)0xb0U
#define ZGP_COMMAND_MFR_MAX                         (uint8_t)0xbfU
#define ZGP_COMMAND_COMMISSIONING                   (uint8_t)0xe0U
#define ZGP_COMMAND_DECOMMISSIONING                 (uint8_t)0xe1U
#define ZGP_COMMAND_SUCCESS                         (uint8_t)0xe2U
#define ZGP_COMMAND_CHANNEL_REQUEST                 (uint8_t)0xe3U
#define ZGP_COMMAND_e4                              (uint8_t)0xe4U
#define ZGP_COMMAND_e5                              (uint8_t)0xe5U
#define ZGP_COMMAND_e6                              (uint8_t)0xe6U
#define ZGP_COMMAND_e7                              (uint8_t)0xe7U
#define ZGP_COMMAND_e8                              (uint8_t)0xe8U
#define ZGP_COMMAND_e9                              (uint8_t)0xe9U
#define ZGP_COMMAND_ea                              (uint8_t)0xeaU
#define ZGP_COMMAND_eb                              (uint8_t)0xebU
#define ZGP_COMMAND_ec                              (uint8_t)0xecU
#define ZGP_COMMAND_ed                              (uint8_t)0xedU
#define ZGP_COMMAND_ee                              (uint8_t)0xeeU
#define ZGP_COMMAND_ef                              (uint8_t)0xefU
#define ZGP_COMMAND_RESERVED_MIN                    (uint8_t)0xf0U
#define ZGP_COMMAND_MASK_b0                         (uint8_t)0xb0U
#define ZGP_COMMAND_MASK_e0                         (uint8_t)0xe0U
#define ZGP_COMMAND_COMMISSIONING_REPLY             (uint8_t)0xf0U
#define ZGP_COMMAND_WRITE_ATTRIBUTES                (uint8_t)0xf1U
#define ZGP_COMMAND_READ_ATTRIBUTES                 (uint8_t)0xf2U
#define ZGP_COMMAND_CHANNEL_CONFIGURATION           (uint8_t)0xf3U
#define ZGP_COMMAND_ZCL_TUNNELING2                  (uint8_t)0xf6U

/* Identify command payload (Table 54 of Green Power Basic Spec) */
#define ZGP_COMMAND_IDENTIFY_DURATION               60U

/* Green Power Commands Received */
/* EXEGIN - rename ? */
#define ZGP_GP_PAIRING                              0x01U
#define ZGP_GP_PROXY_COMMISSIONING_MODE             0x02U
#define ZGP_GP_RESPONSE                             0x06U
#define ZGP_GP_SINK_TABLE_REQUEST                   0x0aU
#define ZGP_GP_PROXY_TABLE_REQUEST                  0x0bU

/* Green Power Commands Sent */
/* EXEGIN - rename ? */
#define ZGP_GP_NOTIFICATION                         0x00U
#define ZGP_GP_COMMISSIONING_NOTIFICATION           0x04U
#define ZGP_GP_SINK_TABLE_RESPONSE                  0x0aU
#define ZGP_GP_PROXY_TABLE_RESPONSE                 0x0bU

/* Response Command Options Bit Masks*/
#define ZGP_RESPONSE_APPLICATIONID                  0x0007U
#define ZGP_RESPONSE_APPLICATIONID_SRCID            0x0000U
#define ZGP_RESPONSE_APPLICATIONID_IEEE             0x0002U
#define ZGP_RESPONSE_TRANSMIT_ON_ENDPOINT_MATCH     0x0008U
#define ZGP_RESPONSE_RESERVED                       0x00F0U

/* Notification Bit Masks */
#define ZGP_NOTIF_APPID_MASK                        0x0007U
#define ZGP_NOTIF_APPID_SRCID                       0x0000U
#define ZGP_NOTIF_APPID_IEEE                        0x0002U
#define ZGP_NOTIF_ALSO_UNICAST                      0x0008U
#define ZGP_NOTIF_ALSO_DERIVED_GROUP                0x0010U
#define ZGP_NOTIF_ALSO_COMMISSIONED_GROUP           0x0020U
#define ZGP_NOTIF_SEC_LEVEL_MASK                    0x00C0U
#define ZGP_NOTIF_SEC_LEVEL_NONE                    0x0000U
#define ZGP_NOTIF_SEC_LEVEL_AUTH                    0x0040U
#define ZGP_NOTIF_SEC_LEVEL_ENCRYPT                 0x00C0U
#define ZGP_NOTIF_KEY_TYPE_MASK                     0x0700U
#define ZGP_NOTIF_KEY_TYPE_NONE                     0x0000U
#define ZGP_NOTIF_KEY_TYPE_ZB_NWK                   0x0100U
#define ZGP_NOTIF_KEY_TYPE_GPD_GROUP                0x0200U
#define ZGP_NOTIF_KEY_TYPE_DERIVED_GPD_GROUP        0x0300U
#define ZGP_NOTIF_KEY_TYPE_OOB_GPD_KEY              0x0400U
#define ZGP_NOTIF_KEY_TYPE_DERIVED_GPD_KEY          0x0700U
#define ZGP_NOTIF_RXAFTERTX                         0x0800U
#define ZGP_NOTIF_GP_TX_QUEUE_FULL                  0x1000U
#define ZGP_NOTIF_BIDI_CAPABILITY                   0x2000U
#define ZGP_NOTIF_PROXY_INFO_PRESENT                0x4000U

/* Commissioning Notification Bit Masks */
#define ZGP_COMM_NOTIF_APPID_MASK                   0x0007U
#define ZGP_COMM_NOTIF_APPID_SRCID                  0x0000U
#define ZGP_COMM_NOTIF_APPID_IEEE                   0x0002U
#define ZGP_COMM_NOTIF_RXAFTERTX                    0x0008U
#define ZGP_COMM_NOTIF_SEC_LEVEL_MASK               0x0030U
#define ZGP_COMM_NOTIF_SEC_LEVEL_NONE               0x0000U
#define ZGP_COMM_NOTIF_SEC_LEVEL_AUTH               0x0020U
#define ZGP_COMM_NOTIF_SEC_LEVEL_ENCRYPT            0x0030U
#define ZGP_COMM_NOTIF_KEY_TYPE_MASK                0x01C0U
#define ZGP_COMM_NOTIF_KEY_TYPE_NONE                0x0000U
#define ZGP_COMM_NOTIF_KEY_TYPE_ZB_NWK              0x0040U
#define ZGP_COMM_NOTIF_KEY_TYPE_GPD_GROUP           0x0080U
#define ZGP_COMM_NOTIF_KEY_TYPE_DERIVED_GPD_GROUP   0x00C0U
#define ZGP_COMM_NOTIF_KEY_TYPE_OOB_GPD_KEY         0x0100U
#define ZGP_COMM_NOTIF_KEY_TYPE_DERIVED_GPD_KEY     0x01C0U
#define ZGP_COMM_NOTIF_SECURITY_PROCESSING_FAILED   0x0200U
#define ZGP_COMM_NOTIF_BIDIRECTIONAL_CAPABILITY     0x0400U
#define ZGP_COMM_NOTIF_PROXY_INFO_PRESENT           0x0800U

/* Pairing Command Options Bit Mask */
#define ZGP_PAIR_OPTIONS_APP_ID_MASK                        0x000007U
#define ZGP_PAIR_OPTIONS_APP_ID_SRCID                       0x000000U
#define ZGP_PAIR_OPTIONS_APP_ID_IEEE                        0x000002U
#define ZGP_PAIR_OPTIONS_ADD_SINK                           0x000008U
#define ZGP_PAIR_OPTIONS_REMOVE_GPD                         0x000010U
#define ZGP_PAIR_OPTIONS_COMM_MODE_SHIFT                    5U
#define ZGP_PAIR_OPTIONS_COMM_MODE_MASK                     0x000060U
#define ZGP_PAIR_OPTIONS_COMM_MODE_FULL_UNICAST             0x000000U /* 0 */
#define ZGP_PAIR_OPTIONS_COMM_MODE_GROUPCAST_GROUPID        0x000020U /* 1 */
#define ZGP_PAIR_OPTIONS_COMM_MODE_GROUPCAST_PRECOMMIS      0x000040U /* 2 */
#define ZGP_PAIR_OPTIONS_COMM_MODE_LW_UNICAST               0x000060U /* 3 */
#define ZGP_PAIR_OPTIONS_GPD_FIXED                          0x000080U
#define ZGP_PAIR_OPTIONS_GPD_MAC_SEQ_CAPAB                  0x000100U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_SHIFT                    9U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_MASK                     0x000600U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_NONE                     0x000000U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_DEPREC                   0x000200U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_AUTH                     0x000400U
#define ZGP_PAIR_OPTIONS_SEC_LEVEL_ENCRYPT                  0x000600U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_SHIFT                     11U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_MASK                      0x003800U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_NONE                      0x000000U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_ZB_NWK                    0x000800U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_GPD_GROUP                 0x001000U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_DERIVED_GPD_GROUP         0x001800U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_OOB_GPD_KEY               0x002000U
#define ZGP_PAIR_OPTIONS_KEY_TYPE_DERIVED_GPD_KEY           0x003800U
#define ZGP_PAIR_OPTIONS_GPD_FC_PRESENT                     0x004000U
#define ZGP_PAIR_OPTIONS_GPD_KEY_PRESENT                    0x008000U
#define ZGP_PAIR_OPTIONS_ASSIGNED_ALIAS_PRESENT             0x010000U
#define ZGP_PAIR_OPTIONS_GROUPCAST_RADIUS_PRESENT           0x020000U
#define ZGP_PAIR_OPTIONS_RESERVED                           0xFC0000U

/* Proxy Table Options Bit Masks */
#define ZGP_PT_APPID_MASK                           0x0007U
#define ZGP_PT_APPID_SRCID                          0x0000U
#define ZGP_PT_APPID_IEEE                           0x0002U
#define ZGP_PT_ENTRYACTIVE                          0x0008U
#define ZGP_PT_ENTRYVALID                           0x0010U
#define ZGP_PT_SEQUENCE_NUMBER_CAPABILITIES         0x0020U
#define ZGP_PT_LIGHTWEIGHT_UNICAST_GPS              0x0040U
#define ZGP_PT_DERIVED_GROUP_GPS                    0x0080U
#define ZGP_PT_COMMISSIONED_GROUP_GPS               0x0100U
#define ZGP_PT_FIRSTTOFORWARD                       0x0200U
#define ZGP_PT_INRANGE                              0x0400U
#define ZGP_PT_GPD_FIXED                            0x0800U
#define ZGP_PT_HASALLUNICASTROUTES                  0x1000U
#define ZGP_PT_ASSIGNEDALIAS                        0x2000U
#define ZGP_PT_SECURITYUSE                          0x4000U
#define ZGP_PT_OPTIONS_EXTENSION                    0x8000U

/* Proxy table Options_Extension Bit Mask */
#define ZGP_PT_FULL_UNICAST_GPS                     0x0001U

/* Proxy Table Request Options Bit Mask */
#define ZGP_PT_REQUEST_TYPE_MASK                    0x0018U
#define ZGP_PT_REQUEST_TYPE_GPID                    0x0000U
#define ZGP_PT_REQUEST_TYPE_INDEX                   0x0008U

/* Proxy Table Security Options Bit Mask */
#define ZGP_PT_SEC_LEVEL_MASK                       0x0003U
#define ZGP_PT_SEC_LEVEL_NONE                       0x0000U
#define ZGP_PT_SEC_LEVEL_AUTH                       0x0002U
#define ZGP_PT_SEC_LEVEL_ENCRYPT                    0x0003U
#define ZGP_PT_KEY_TYPE_MASK                        0x001CU
#define ZGP_PT_KEY_TYPE_NONE                        0x0000U
#define ZGP_PT_KEY_TYPE_ZB_NWK                      0x0004U
#define ZGP_PT_KEY_TYPE_GPD_GROUP                   0x0008U
#define ZGP_PT_KEY_TYPE_DERIVED_GPD_GROUP           0x000CU
#define ZGP_PT_KEY_TYPE_OOB_GPD_KEY                 0x0010U
#define ZGP_PT_KEY_TYPE_DERIVED_GPD_KEY             0x001CU

/* Proxy Commissioning Mode Options Bit Masks */
#define ZGP_PCOMM_ACTION                                                0x01U
#define ZGP_PCOMM_EXIT_MODE                                             0x0EU
#define ZGP_PCOMM_EXIT_MODE_ON_COMMISSIONINGWINDOW_EXPIRATION           0x02U
#define ZGP_PCOMM_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS                    0x04U
#define ZGP_PCOMM_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT         0x08U
#define ZGP_PCOMM_CHANNEL_PRESENT                                       0x10U
#define ZGP_PCOMM_UNICAST_COMMUNICATION                                 0x20U
#define ZGP_PCOMM_RESERVED                                              0xC0U

/* Overhead is GP stub NWK Header length without tunnel payload */
#define GPD_TUNNEL_ZCL_PAYLOAD_OVERHEAD     22U
#define GPD_TUNNEL_ZCL_PAYLOAD_MAX_SIZE \
    (WPAN_CONST_MAX_PHY_PACKET_SIZE - GPD_TUNNEL_ZCL_PAYLOAD_OVERHEAD)

/*-----------------------------------------------------------------------------
 * Green Power Device
 *-----------------------------------------------------------------------------
 */
/* docs-13-0166-01-batt-master-list-of-green-power-device-definitions-v1.1.xlsx */
enum gpd_device_id_t {
    GPD_DEVICE_ID_SIMPLE_GENERIC_ONE_STATE = 0x00,
    GPD_DEVICE_ID_SIMPLE_GENERIC_TWO_STATE = 0x01,
    GPD_DEVICE_ID_ONOFF_SWITCH = 0x02,
    GPD_DEVICE_ID_LEVEL_CONTROL_SWITCH = 0x03,
    GPD_DEVICE_ID_SIMPLE_SENSOR = 0x04,
    GPD_DEVICE_ID_ADV_GENERIC_ONE_STATE = 0x05,
    GPD_DEVICE_ID_ADV_GENERIC_TWO_STATE = 0x06,
    GPD_DEVICE_ID_GENERIC_EIGHT_CONTACT = 0x07,
    GPD_DEVICE_ID_COLOR_DIMMER = 0x10,
    GPD_DEVICE_ID_LIGHT_SENSOR = 0x11,
    GPD_DEVICE_ID_OCCUPANCY_SENSOR = 0x12,
    GPD_DEVICE_ID_DOOR_LOCK_CONTROLLER = 0x20,
    GPD_DEVICE_ID_TEMP_SENSOR = 0x30,
    GPD_DEVICE_ID_PRESSURE_SENSOR = 0x31,
    GPD_DEVICE_ID_FLOW_SENSOR = 0x32,
    GPD_DEVICE_ID_INDOOR_ENVIRO_SENSOR = 0x33
};

/* ZGP_COMMAND_COMMISSIONING 'Options' */
#define GPD_COMMISS_OPTIONS_MAC_SEQ                 0x01U /* MACsequenceNumberCapability */
#define GPD_COMMISS_OPTIONS_RX_ON                   0x02U /* RxOnCapability */
#define GPD_COMMISS_OPTIONS_APP_INFO                0x04U /* Application information present */
#define GPD_COMMISS_OPTIONS_PANID_REQ               0x10U /* PANIdrequest */
#define GPD_COMMISS_OPTIONS_GP_SEC_KEY_REQ          0x20U /* GPsecurityKeyRequest */
#define GPD_COMMISS_OPTIONS_FIXED_LOC               0x40U /* FixedLocation */
#define GPD_COMMISS_OPTIONS_EXT_OPTIONS             0x80U /* ExtendedOptionsPresent */

/* ZGP_COMMAND_COMMISSIONING 'Extended Options' */
#define GPD_COMMISS_EXT_OPTS_SEC_LVL_MASK           0x03U /* E.g. ZGP_SECURITY_ENCRYPT */
#define GPD_COMMISS_EXT_OPTS_KEY_TYPE_MASK          0x1CU /* E.g. ZGP_KEYTYPE_PRECONFIG */
#define GPD_COMMISS_EXT_OPTS_KEY_TYPE_SHIFT         2U
#define GPD_COMMISS_EXT_OPTS_GPD_KEY_PRESENT        0x20U
#define GPD_COMMISS_EXT_OPTS_GPD_KEY_ENCRYPT        0x40U
#define GPD_COMMISS_EXT_OPTS_GPD_OUT_PRESENT        0x80U

/* The Application information field of the GPD Commissioning command */
#define GPD_COMMISS_APPINFO_MANUF_ID                0x01U /* ManufacturerID present */
#define GPD_COMMISS_APPINFO_MODEL_ID                0x02U /* ModelID present */
#define GPD_COMMISS_APPINFO_GPD_CMDS                0x04U /* GPD commands present */
#define GPD_COMMISS_APPINFO_CLUSTER_LIST            0x08U /* Cluster list present */
#define GPD_COMMISS_APPINFO_SWITCH_INFO             0x10U /* Switch information present */
#define GPD_COMMISS_APPINFO_GPD_APP_DESC            0x20U /* GPD Application Description command follows */
#define GPD_COMMISS_APPINFO_RESERVED                0xc0U /* Reserved */

/* Options field of Commissioning Reply command */
#define GPD_COMMISS_REPLY_OPTS_PANID_PRESENT                    0x01U /* PANId present */
#define GPD_COMMISS_REPLY_OPTS_SEC_KEY_PRESENT                  0x02U /* Security key present */
#define GPD_COMMISS_REPLY_OPTS_KEY_ENCRYPT                      0x04U /* Security key is encrypted */
#define GPD_COMMISS_REPLY_OPTS_SEC_LVL_MASK                     0x18U /* Security level */
#define GPD_COMMISS_REPLY_OPTS_SEC_LVL_SHIFT                    3U
#define GPD_COMMISS_REPLY_OPTS_KEY_TYPE_MASK                    0xe0U /* Security key type */
#define GPD_COMMISS_REPLY_OPTS_KEY_TYPE_SHIFT                   5U

/* ZB_ZGP_COMMAND_TUNNELING 'Options' */
#define GPD_TUNNEL_OPTIONS_FRAMETYPE_GENERIC        0x00U
#define GPD_TUNNEL_OPTIONS_FRAMETYPE_SPECIFIC       0x01U
#define GPD_TUNNEL_OPTIONS_MANUID_PRESENT           0x04U
#define GPD_TUNNEL_OPTIONS_DIRECTION_TO_SERVER      0x00U
#define GPD_TUNNEL_OPTIONS_DIRECTION_TO_CLIENT      0x08U
#define GPD_TUNNEL_OPTIONS_FRAMETYPE_MASK           (0x3 << 0)
#define GPD_TUNNEL_OPTIONS_MANUID_PRESENT_MASK      (0x1 << 2)
#define GPD_TUNNEL_OPTIONS_DIRECTION_MASK           (0x1 << 3)

/*lint -restore */

#endif
