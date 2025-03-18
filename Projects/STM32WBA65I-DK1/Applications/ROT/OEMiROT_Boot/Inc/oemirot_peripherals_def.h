/*
 * Copyright (c) 2018, Arm Limited. All rights reserved.
 * Copyright (c) 2024 STMicroelectronics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __OEMIROT_PERIPHERALS_DEF_H__
#define __OEMIROT_PERIPHERALS_DEF_H__

struct oemirot_spm_partition_platform_data_t;

extern struct oemirot_spm_partition_platform_data_t oemirot_peripheral_std_uart;

#define OEMIROT_PERIPHERAL_STD_UART     (&oemirot_peripheral_std_uart)

#define TAMP_IRQ TAMP_IRQn
#define GTZC_IRQ GTZC_IRQn

#endif /* __OEMIROT_PERIPHERALS_DEF_H__ */
