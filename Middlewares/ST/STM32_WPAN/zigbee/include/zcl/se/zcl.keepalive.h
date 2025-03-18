/**
 * @file zcl.keepalive.h
 * @copyright Copyright [2009 - 2024] Exegin Technologies Limited. All rights reserved.
 * @heading Keep Alive
 * @brief ZCL Keep Alive cluster header
 *
 * The Keep Alive clusters are typically used in Smart Energy applications.
 *
 * The Keep Alive server and client clusters are allocated by the stack if the application
 * configures the Key Exchange information in the ZbStartup configuration (struct ZbStartupCbkeT).
 * The Keep Alive server is allocated if the tc_keepalive_server_enable flag is set to true,
 * otherwise the Keep Alive client is allocated. Typically, the Keep Alive server is allocated
 * on the Trust Center, and the Keep Alive client is allocated on devices joining the SE network.
 *
 * If the Keep Alive client determines there's a problem communicating with the Trust Center,
 * it will call the application callback 'tcso_callback' configured in the ZbStartup configuration.
 * At which point, the stack will perform the necessary Trust Center Swap Out (TCSO) routines
 * to attempt to find a newly swapped-out Trust Center, or if the current Trust Center has moved
 * to a different channel or other configuration.
 */

/*--------------------------------------------------------------------------
 *  DESCRIPTION
 *      Interface definition for the ZCL Keep Alive cluster.
 *--------------------------------------------------------------------------
 */
#ifndef ZCL_KEEPALIVE_H
# define ZCL_KEEPALIVE_H

#include "zcl/zcl.h"

/** Keep Alive Server Attribute IDs */
enum ZbZclKeepAliveSvrAttrT {
    ZCL_KEEPALIVE_SVR_ATTR_BASE = 0x0000,
    /**< TC Keep-Alive Base - minutes (valid range is from 0x01 to 0xff,
     * but not enforced by cluster, for testing) */

    ZCL_KEEPALIVE_SVR_ATTR_JITTER = 0x0001
        /**< TC Keep-Alive Jitter - seconds (valid range is from 0x0000 to 0x0200) */
};

#define ZCL_KEEPALIVE_BASE_MIN                  0x01U
#define ZCL_KEEPALIVE_BASE_MAX                  0xffU

#define ZCL_KEEPALIVE_JITTER_MIN                0x0000U
#define ZCL_KEEPALIVE_JITTER_MAX                0x0200U

/* Keep Alive Server Defaults */
#define ZCL_KEEPALIVE_SERVER_BASE_DEFAULT       10U /* minutes */
#define ZCL_KEEPALIVE_SERVER_JITTER_DEFAULT     300U /* seconds */

/* Keep Alive Client Defaults */
/* The client will poll the server at a faster rate until it is able to read
 * the real attribute values from the Keep Alive Server. */
#define ZCL_KEEPALIVE_CLIENT_BASE_DEFAULT       0U /* minutes */
#define ZCL_KEEPALIVE_CLIENT_JITTER_DEFAULT     30U /* seconds */

/**
 * Write a Keep Alive Server attribute
 * @note Use this API only if Keep-Alive is enabled and Keep-Alive server cluster allocated using the
 * startup configuration.
 * @param zb Zigbee instance
 * @param attrId The attribute Id to write
 * @param value Attribute data to be writen.
 * @return ZCL Status Code
 */
enum ZclStatusCodeT ZbZclKeepAliveServerWriteDirect(struct ZigBeeT *zb, uint16_t attrId, uint16_t value);

/**
 * Start the Keep Alive process. This is ignored if the cluster has not been allocated.
 * The Keep Alive cluster is allocated based on the ZbStartupT configuration given to ZbStartup.
 * @param zb Zigbee stack instance
 * @return Void
 */
void ZbZclKeepAliveClientStart(struct ZigBeeT *zb);

/**
 * Stop the Keep Alive process and abort the TCSO if running. This is ignored if the cluster
 * has not been allocated. The Keep Alive cluster is allocated based on the ZbStartupT
 * configuration given to ZbStartup.
 * @param zb Zigbee stack instance
 * @return Void
 */
void ZbZclKeepAliveClientStop(struct ZigBeeT *zb);

#endif
