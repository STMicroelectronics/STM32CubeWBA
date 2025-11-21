/**
 * @file gpd.h
 * @heading Green Power Device
 * @brief Green Power Device header file
 * @author Exegin Technologies Limited
 * @copyright Copyright [2025] Exegin Technologies Limited. All rights reserved.
 *
 * Green Power Device
 */

#ifndef FILE_GPD_H
# define FILE_GPD_H

#include <stdint.h>
#include <stdbool.h>

#include "zb-gp-device.h"

/*---------------------------------------------------------------
 * Porting
 *---------------------------------------------------------------
 */
void * gpd_port_mem_alloc(unsigned int sz);
void gpd_port_mem_free(void *ptr);

bool gpd_port_timer_init(struct gpd_t *gpd);
bool gpd_port_timer_start(unsigned int ms);
void gpd_port_timer_stop(void);
void gpd_port_timer_wait(void);

void gpd_port_dataconf_init(void);
void gpd_port_dataconf_done(void);
void gpd_port_dataconf_wait(void);

struct gpd_persist_info_t {
    uint8_t channel;
    /* uint32_t inFrameCounter; EXEGIN - needed? */
    struct gpd_security_config_t security;
};

/* EXEGIN - option just to store info->security.outFrameCounter ? */
void gpd_port_persist_write(struct gpd_persist_info_t *info);
bool gpd_port_persist_read(struct gpd_persist_info_t *info);

#endif
