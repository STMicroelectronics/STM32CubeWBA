/* Copyright [2009 - 2023] Exegin Technologies Limited. All rights reserved. */

#ifndef ZIGBEE_STM32WBA_H
#define ZIGBEE_STM32WBA_H

struct WpanPublicT * zsdk_stmac_open(struct WpanPublicT *device);
uint64_t zsdk_stmac_get_eui64(struct WpanPublicT *device);

void ZbPortHwTimerStop(void);
void ZbPortHwTimerReStart(ZbUptimeT time);

void ZbPortTickCallback(void);

#endif
