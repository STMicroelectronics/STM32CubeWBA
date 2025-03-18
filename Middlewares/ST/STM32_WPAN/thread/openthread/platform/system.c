/*
 *  Copyright (c) 2017, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 * @brief
 *   This file includes the platform-specific initializers.
 *
 */
#include <stdio.h>
#include "stm32wbaxx_hal.h"
#include "stm32wbaxx_hal_cortex.h"
#include "stm32wbaxx_ll_system.h"
#include "platform_wba.h"

#include "openthread/platform/radio.h"
#include "platform.h"

#include "event_manager.h"
#include "thread.h"
#include "system.h"   

#define CSMA_MAX_BE      6
#define CSMA_MIN_BE      3
#define CSMA_MAX_BACKOFF 4

bool g_PseudoReset = FALSE;

static void systemRxDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError);
static uint8_t IsSystemInitCalled = 0 ;

static struct mac_cbk_dispatch_tbl ot_cbk_dispatch_tbl = {
		.mac_ed_scan_done = otPlatRadioEnergyScanDone,
		.mac_tx_done = otPlatRadioTxDone,
		.mac_rx_done = systemRxDone,
		.mac_tx_strtd = otPlatRadioTxStarted,
		.mac_frm_updtd = NULL
		};


static void systemRxDone(otInstance *aInstance, otRadioFrame *aFrame, otError aError)
{
  /*
   * Current version of Thread stack does not handle very well NULL packets (i.e. hardfault).
   * If OT_ERROR_ABORT, it can be considered it as a non-valid RX (when continuous
   * reception is enabled, and aborted in order to perform a TX, a RX event is
   * generated).
   */
  if (aError == OT_ERROR_ABORT) {
    /* Do not call upper layer, OT stack doesn't handle RX error */
  }
  else{
    otPlatRadioReceiveDone(aInstance, aFrame, aError);
  }
}

void otSysInit(int argc, char *argv[])
{
        (void)argc;
        (void)argv;
	LL_LOCK();

	if (FALSE == g_PseudoReset) {
		
		if (IsSystemInitCalled == 0)
		{
                  radio_init();
                  set_max_csma_be(CSMA_MAX_BE);
                  set_min_csma_be(CSMA_MIN_BE);
                  set_max_csma_backoff(CSMA_MAX_BACKOFF);
                  otPlatRadioSetPromiscuous(NULL, (bool) 0);
                  IsSystemInitCalled++;
		}
	}
	arcAlarmInit();
	g_PseudoReset = FALSE;
	LL_UNLOCK();
}

void otDispatch_tbl_init(otInstance *sInstance)
{
	radio_call_back_funcs_init(&ot_cbk_dispatch_tbl);
}

void otSysRequestPseudoReset(void)
{
    /* Perform an NVIC Reset in order to reinitialize the device */
    HAL_NVIC_SystemReset();
}

bool otSysPseudoResetWasRequested(void)
{
    return g_PseudoReset;
}

void otSysProcessDrivers(otInstance *aInstance)
{
#if (OT_CLI_USE == 1)
    arcUartProcess();
#endif // OT_CLI_USE
    emngr_handle_all_events();
    arcAlarmProcess(aInstance);
}


/**
 * __2snprintf symbol missing in aeabi OT libs, add it on platform 
 *
 */
int __2snprintf(char * s, size_t n, const char * format, ...)
{
  va_list args;
  va_start (args, format);
  return vsnprintf(s, n, format, args);
}