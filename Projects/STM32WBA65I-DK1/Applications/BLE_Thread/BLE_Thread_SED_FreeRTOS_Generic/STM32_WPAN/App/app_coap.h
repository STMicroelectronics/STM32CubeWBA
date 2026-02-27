/* app_coap.h
 ******************************************************************************
 * CoAP application interface
 ******************************************************************************
 */

#ifndef APP_COAP_H
#define APP_COAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "coap.h"
#include "instance.h"
#include "thread.h"
#include "cmsis_os2.h"
#include <stdbool.h>
#include <stdint.h>

/* Public defines ---------------------------------------------------------- */
#define COAP_PAYLOAD_LENGTH      (2U)
#define COAP_SEND_TIMEOUT        (2000U)   /**< 2s */

/* Types ------------------------------------------------------------------- */


/* Public API -------------------------------------------------------------- */

void APP_COAP_Init(otInstance *aInstance);
void APP_COAP_OnThreadRoleChanged(otDeviceRole aRole);
void APP_COAP_StartPeriodic(void);
void APP_COAP_StopPeriodic(void);
bool APP_COAP_IsPeriodicRunning(void);
uint8_t APP_COAP_GetLastReceivedCommand(void);


#ifdef __cplusplus
}
#endif

#endif /* APP_COAP_H */