/*$Id: //dwh/bluetooth/DWC_ble154combo/firmware/rel/1.30a-SOW05PatchV6/firmware/public_inc/rfd_dev_config.h#1 $*/


#ifndef MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_
#define MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_

#include "ll_fw_config.h"


#if (!FFD_DEVICE_CONFIG)  /* RFD Device Configuration */

#define RFD_SUPPORT_ACTIVE_SCAN						1 /* Enable\Disable :RFD supports Active Scanning Enable:1 - Disable:0 */
#define RFD_SUPPORT_ENERGY_DETECT					1 /* Enable\Disable :RFD supports Energy Detection Enable:1 - Disable:0 */
#define RFD_SUPPORT_DATA_PURGE						1 /* Enable\Disable :RFD supports Data Purge Primitive Enable:1 - Disable:0 */
#define RFD_SUPPORT_ASSOCIATION_IND_RSP				1 /* Enable\Disable :RFD supports Association Indication and Response Primitives Enable:1 - Disable:0 */
#define RFD_SUPPORT_ORPHAN_IND_RSP					1 /* Enable\Disable :RFD supports Orphan Indication and Response Primitives Enable:1 - Disable:0 */
#define RFD_SUPPORT_START_PRIM						1 /* Enable\Disable :RFD supports Start Primitive Enable:1 - Disable:0 */
#define RFD_SUPPORT_PROMISCUOUS_MODE				1 /* Enable\Disable :RFD supports Promiscuous Mode Enable:1 - Disable:0 */
#define RFD_SUPPORT_SEND_BEACON						1 /* Enable\Disable :RFD supports Sending Beacons if Coordinator Enable:1 - Disable:0 */
#define RFD_SUPPORT_PANID_CONFLICT_RSLN				1 /* Enable\Disable :RFD supports Pan Id conflict detection and resolution Enable:1 - Disable:0 */

#endif


#endif /* MAC_CONTROLLER_INC_RFD_DEV_CONFIG_H_ */
