/**
 * @file zigbee.log.h
 * @heading Zigbee Logging Utilities
 * @brief Zigbee header file.
 * @author Exegin Technologies
 * @copyright Copyright [2009 - 2025] Exegin Technologies Limited. All rights reserved.
 *
 * This file groups global/external definitions from all the layer specific header files
 * e.g, aps, nwk, zdo etc... into a single place, so that one can just include zigbee.h for
 * all the global definitions eliminating the header file inclusion clutter from source files.
 */

#ifndef ZIGBEE_LOG_H
# define ZIGBEE_LOG_H

#include <stdint.h>

/* stdarg is required for the logging (ZbSetLogging) */
/*lint -save -e829 "'stdarg.h' usage should be deprecated [MISRA Rule 17.1 (REQUIRED)]" */
/*lint -save -e451 "header file included withoutstandard guard [Dir 4.10 (REQUIRED)]" */
#include <stdarg.h>
/*lint -restore */
/*lint -restore */

struct ZigBeeT;

/* Debugging log mask. */
#define ZB_LOG_MASK_FATAL               0x00000001U /* Unrecoverable errors. */
#define ZB_LOG_MASK_ERROR               0x00000002U /* Recoverable internal errors. */
#define ZB_LOG_MASK_INFO                0x00000004U /* Basic debugging info. Less verbose than ZB_LOG_MASK_DEBUG. */
#define ZB_LOG_MASK_DEBUG               0x00000008U /* General debug info */
/* NWK */
#define ZB_LOG_MASK_NWK_ROUTING         0x00000010U /* Network routing details. */
#define ZB_LOG_MASK_NWK_LQI             0x00000020U /* Network link status and lqi updates. */
#define ZB_LOG_MASK_NWK_SECURITY        0x00000040U /* Network security. */
#define ZB_LOG_MASK_NWK_ADDR_MAP        0x00000080U /* Network address map changes */
#define ZB_LOG_MASK_NWK_PARENT_SEL      0x00000100U /* Parent selection during join/rejoin */
#define ZB_LOG_MASK_NWK_SEND            0x00000200U /* Network packet debugging */
#define ZB_LOG_MASK_NWK_FILTER          0x00000400U /* NWK/MAC Filtering debug */
/* APS */
#define ZB_LOG_MASK_APS_SEND            0x00001000U /* APS packet transmission */
#define ZB_LOG_MASK_APS_FRAG            0x00002000U /* APS fragmentation debugging */
/* ZDO */
#define ZB_LOG_MASK_ZDO_ANNCE           0x00010000U /* Print on reception of ZDO Device_Annce */
/* Misc */
#define ZB_LOG_MASK_PERSIST             0x00100000U /* Persistence */
#define ZB_LOG_MASK_GREENPOWER          0x00200000U /* Green Power */
/* 0x00400000U Reserved (was ZB_LOG_MASK_HEAP) */
#define ZB_LOG_MASK_TIMER               0x00800000U /* ZbTimer */
#define ZB_LOG_MASK_SLEEPY              0x01000000U /* Sleepy (e.g. Polling) */
#define ZB_LOG_MASK_MAC_RSSI            0x02000000U /* Print debug message per MCPS-DATA.indication showing RSSI */
#define ZB_LOG_MASK_ZB_DIRECT           0x04000000U /* Zigbee Direct debugging */
/* ZCL / Application */
#define ZB_LOG_MASK_ZCL                 0x10000000U /* ZCL */

/**
 * Specifies the level of logging to use, and a callback that outputs the log information.
 * @param zb Pointer to Zigbee stack instance
 * @param mask Logging mask
 * @param func Callback function that outputs the log information
 */
void ZbSetLogging(struct ZigBeeT *zb, uint32_t mask,
    void (*func)(struct ZigBeeT *zb, uint32_t mask, const char *hdr, const char *fmt, va_list argptr));

/*
 * The following replaces PRIx64, which is not always supported by a
 * 32-bit newlib implementation
 */
#ifdef PRIx64
# undef PRIx64
#endif

#define LOGFMTx64                  "0x%08x%08x"
#define LOGVALx64(val)             (uint32_t)(val >> 32U), (uint32_t)(val)

/* E.g. PRIu64 */
#ifndef LOGFMTu64
/* Check for 32-bit ARM 7 architecture (e.g. arm-linux-gnueabihf-) */
#if defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__)    \
    || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
/* ARM7 */
# define LOGFMTu64                  "%llu"
#else
# define LOGFMTu64                  "%lu"
#endif
#endif

#endif
