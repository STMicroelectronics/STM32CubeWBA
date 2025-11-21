/* Copyright [2025] Exegin Technologies Limited. All rights reserved. */

#include "zb-green-power.h"

/*-----------------------------------------------------------------------------
 * GPD Initialization and Configuration
 *-----------------------------------------------------------------------------
 */

/** GPD Initialization configuration, which is passed to gpd_init(). */
struct gpd_init_config_t {
    struct WpanPublicT *dev; /**< MAC interface to use */
    enum gpd_device_id_t device_id; /**< The GPD device type */

    uint8_t app_info_options;
    /**< AppInfo options to include in the Commissioning Request (e.g. GPD_COMMISS_APPINFO_MANUF_ID) */

    uint16_t manuf_id; /**< The GPD Manufacturer Id */
    uint8_t model_id; /**< The GPD Manufacturer Model Id */
    uint64_t eui64; /**< EUI64 address to assign to the local device */
    uint8_t app_id; /**< Application Id. Can be one of ZGP_APPID_SRCID or ZGP_APPID_ENDPOINT */
    uint32_t src_id; /**< Manufacturer specific Source Id (if using ZGP_APPID_SRCID) */
    uint8_t endpoint; /**< Endpoint Id to assign to the GPD */
    bool bidirectional; /**< Does the GPD support bi-directional (i.e. can receive packets). */
    uint32_t channel_mask; /**< Channel Mask to operate on */
};

/** GPD Security configuration */
struct gpd_security_config_t {
    enum zgp_security_level_t level;
    enum zgp_security_keytype_t keyType;
    uint8_t key[ZGP_KEYSIZE];
    uint32_t outFrameCounter; /**< Outgoing FC */
};

/**
 * Initializes the Green Power Device
 * @param config Green Power Device configuration
 * @param security Security configuration. See 'struct gpd_security_config_t'
 * @return Pointer to Green Power Device instance, or NULL if there was a problem.
 */
struct gpd_t * gpd_init(struct gpd_init_config_t *config, struct gpd_security_config_t *security);

/**
 * Re-configures the security for the Green Power Device
 * @param gpd Green Power Device instance
 * @param config Security configuration. See 'struct gpd_security_config_t'
 * @return None
 */
void gpd_security_config(struct gpd_t *gpd, struct gpd_security_config_t *config);

/**
 * Configures the RF channel for the Green Power Device to begin operating on.
 * It must be a channel found in the 'channel_mask' assigned to gpd_init().
 * @param gpd Green Power Device instance
 * @param channel Channel to set (11 - 26)
 * @return True if the channel could be set, false otherwise.
 */
bool gpd_set_channel(struct gpd_t *gpd, uint8_t channel);

/*-----------------------------------------------------------------------------
 * GPD Commands
 *-----------------------------------------------------------------------------
 */

/** GPD Channel Request Command information */
struct gpd_channel_cmd_t {
    bool switch_channel;
    /**< After sending the Channel Request command on the current channel,
     * change the channel to the next channel in the mask. */
};

/**
 * Sends a GPD Channel Request command (ZGP_COMMAND_CHANNEL_REQUEST).
 * The GPD must be bi-directional. If 'switch_channel' is true in 'req',
 * then after sending this command, the channel will change to the next
 * channel in the mask. This allows the application to call this function
 * again, to send another Channel Request, but also to potentially receive
 * a Channel Configuration command queued up on the Proxy. Subseqeuent
 * Channel Requests need to be sent within the 5 second timeout window.
 * @param gpd Green Power Device instance
 * @param req Request information
 * @return True if the command was sent, false otherwise.
 */
bool gpd_cmd_channel_req(struct gpd_t *gpd, struct gpd_channel_cmd_t *req);

/** GPD Commissioning Command information */
struct gpd_commissioning_cmd_t {
    bool switch_channel;
    /**< Change the current channel to the next channel in the mask,
     * before sending the command. */

    bool wait_for_response;
    /**< Whether to wait for the Commissioning Reply or not. The first
     * time this is called, this should be set to false, since the
     * reply is only expected on the second attempt. */
};

/**
 * Sends a GPD Commissioning command (ZGP_COMMAND_COMMISSIONING).
 * If the GPD is unidirectional-only, and the Combo or Sink device indicates
 * commissioning was successful, the application should stop calling this
 * function, which will leave the current channel as the operational channel.
 * @param gpd Green Power Device instance
 * @param req Request information
 * @return True if the command was sent, false otherwise.
 */
bool gpd_cmd_commissioning_req(struct gpd_t *gpd, struct gpd_commissioning_cmd_t *req);

struct gpd_decommissioning_cmd_t {
    /* currently empty */
};

/**
 * Sends a GPD Decommissioning command (ZGP_COMMAND_DECOMMISSIONING).
 * Upon success, the application should call gpd_security_config() to revert the
 * security back to factory defaults.
 *
 * @param gpd Green Power Device instance
 * @param req Request information
 * @return True if the command was sent, false otherwise.
 */
bool gpd_cmd_decommissioning_req(struct gpd_t *gpd, struct gpd_decommissioning_cmd_t *req);

/* EXEGIN - comment */
struct gpd_success_cmd_t {
    /* currently empty */
};

/* EXEGIN - comment */
bool gpd_cmd_success_req(struct gpd_t *gpd, struct gpd_success_cmd_t *req);

/**
 * Sends a GPD ZCL Command
 * @param gpd Green Power Device instance
 * @param cmd_id Command Id. E.g. ZGP_COMMAND_TOGGLE
 * @return True if the command was sent, false otherwise
 */
bool gpd_send_zcl_cmd(struct gpd_t *gpd, uint8_t cmd_id);

/** GPD Tunnel Command information */
struct gpd_tunnel_t {
    uint8_t options;
    uint16_t manufacturer_id;
    uint16_t cluster_id;
    uint8_t zcl_cmd_id;
    uint8_t zcl_payload_len;
    uint8_t zcl_payload[GPD_TUNNEL_ZCL_PAYLOAD_MAX_SIZE];
};

/**
 * Sends a GPD ZCL Tunnel Command
 * @param gpd Green Power Device instance
 * @param req Request information
 * @return True if the command was sent, false otherwise
 */
bool gpd_send_zcl_tunnel_cmd(struct gpd_t *gpd, struct gpd_tunnel_t *req);
