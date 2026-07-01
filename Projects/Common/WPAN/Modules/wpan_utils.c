/**
  ******************************************************************************
  * @file    wpan_utils.c
  * @author  MCD Application Team
  * @brief   Source file for Utilities for Common/WPAN
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "stm32wbaxx_ll_utils.h"
#include "stm32wbaxx_ll_system.h"
#include "wpan_utils.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

/* Private constant ----------------------------------------------------------*/
static const uint16_t   iDeviceIdList[] = { 0xFFFF, 0x492, 0x4B0, 0x4B2 };
static const char *     szDeviceNameList[] = { "Unknown", "STM32WBA5xxx", "STM32WBA6xx", "STM32WBA2xxx" };

static const uint16_t   iDeviceRevIdList[] = { 0xFFFF, 0x1000, 0x2000 };
static const char *     szDeviceRevNameList[] = { "Unknown", "A", "B" };

static const uint16_t   iDevicePackageIdList[] = { 0xFFFF, 
#if defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx) || defined(STM32WBA5Mxx)
                                                 LL_UTILS_PACKAGETYPE_UFQFPN32,         LL_UTILS_PACKAGETYPE_UFQFPN48,
                                                 LL_UTILS_PACKAGETYPE_WLCSP41_SMPS,     LL_UTILS_PACKAGETYPE_UFQFPN48_SMPS,
                                                 LL_UTILS_PACKAGETYPE_UFBGA59,
#elif defined(STM32WBA62xx) || defined(STM32WBA63xx) || defined(STM32WBA64xx) || defined(STM32WBA65xx) || defined (STM32WBA6Mxx)
                                                 LL_UTILS_PACKAGETYPE_UFQFPN48_USB,     LL_UTILS_PACKAGETYPE_WLCSP88_USB,
                                                 LL_UTILS_PACKAGETYPE_UFBGA121_USB,     LL_UTILS_PACKAGETYPE_UFQFPN48_SMPS,
                                                 LL_UTILS_PACKAGETYPE_UFQFPN48_SMPS_USB,LL_UTILS_PACKAGETYPE_VFQFPN68_SMPS_USB,
                                                 LL_UTILS_PACKAGETYPE_WLCSP88_SMPS_USB, LL_UTILS_PACKAGETYPE_UFBGA121_SMPS_USB,
#elif defined(STM32WBA20xx) || defined(STM32WBA23xx) || defined(STM32WBA24xx) || defined(STM32WBA25xx)
                                                 LL_UTILS_PACKAGETYPE_UFQFPN32,         LL_UTILS_PACKAGETYPE_UFQFPN48,
                                                 LL_UTILS_PACKAGETYPE_WLCSP37,          LL_UTILS_PACKAGETYPE_UFQFPN32_USB,
                                                 LL_UTILS_PACKAGETYPE_UFQFPN48_USB,     LL_UTILS_PACKAGETYPE_WLCSP37_USB,
                                                 LL_UTILS_PACKAGETYPE_UFQFPN48_SMPS,    LL_UTILS_PACKAGETYPE_WLCSP37_SMPS_USB,
                                                 LL_UTILS_PACKAGETYPE_UFQFPN48_SMPS_USB,
#endif /* defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx) || defined(STM32WBA5Mxx) */
                                               };
static const char *     szDevicePackageNameList[] = { "Unknown", 
#if defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx) || defined(STM32WBA5Mxx)
                                                     "UFQFPN32", "UFQFPN48", "WLCSP41_SMPS", "UFQFPN48_SMPS", "UFBGA59",
#elif defined(STM32WBA62xx) || defined(STM32WBA63xx) || defined(STM32WBA64xx) || defined(STM32WBA65xx) || defined (STM32WBA6Mxx)
                                                     "UFQFPN48_USB", "WLCSP88_USB", "UFBGA121_USB", "UFQFPN48_SMPS",   
                                                     "UFQFPN48_SMPS_USB", "VFQFPN68_SMPS_USB", "WLCSP88_SMPS_USB", "UFBGA121_SMPS_USB",
#elif defined(STM32WBA20xx) || defined(STM32WBA23xx) || defined(STM32WBA24xx) || defined(STM32WBA25xx)
                                                     "UFQFPN32", "UFQFPN48", "WLCSP37", "UFQFPN32_USB", "UFQFPN48_USB", 
                                                     "WLCSP37_USB", "UFQFPN48_SMPS", "WLCSP37_SMPS_USB", "UFQFPN48_SMPS_USB",
#endif /* defined(STM32WBA52xx) || defined(STM32WBA54xx) || defined(STM32WBA55xx) || defined(STM32WBA5Mxx) */
                                                    };
													
/* Private macros ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/


/* Functions Definition ------------------------------------------------------*/
uint16_t WpanUtils_GetDeviceId( void )
{
  return (uint16_t)( LL_DBGMCU_GetDeviceID() & 0x0FFFu );
}

char * WpanUtils_GetDeviceName( void )
{
  uint16_t  iIndex = 0, iFoundIndex = 0, iMaxIndex, iDeviceId;
  
  iDeviceId = WpanUtils_GetDeviceId();
  iMaxIndex = sizeof(iDeviceIdList) / sizeof(uint16_t);
  
  do
  {
    if ( iDeviceId == iDeviceIdList[iIndex] )
    {
      iFoundIndex = iIndex;
    }
    iIndex++;
  }
  while ( ( iFoundIndex == 0u ) && ( iIndex < iMaxIndex ) );
    
  return (char *)szDeviceNameList[iFoundIndex];
}

uint16_t WpanUtils_GetDeviceRevId( void )
{
  return (uint16_t)LL_DBGMCU_GetRevisionID();
}

char * WpanUtils_GetDeviceRevName( void )
{
  uint16_t  iIndex = 0, iFoundIndex = 0, iMaxIndex, iRevId;
  
  iRevId = WpanUtils_GetDeviceRevId();
  iMaxIndex = sizeof(iDeviceRevIdList) / sizeof(uint16_t);
  
  do
  {
    if ( iRevId == iDeviceRevIdList[iIndex] )
    {
      iFoundIndex = iIndex;
    }
    iIndex++;
  }
  while ( ( iFoundIndex == 0u ) && ( iIndex < iMaxIndex ) );
    
  return (char *)szDeviceRevNameList[iFoundIndex];
}

uint16_t WpanUtils_GetDevicePackageId( void )
{
  return (uint16_t)LL_GetPackageType();
}

char * WpanUtils_GetDevicePackageName( void )
{
  uint16_t  iIndex = 0, iFoundIndex = 0, iMaxIndex, iPackageId;
  
  iPackageId = WpanUtils_GetDevicePackageId();
  iMaxIndex = sizeof(iDevicePackageIdList) / sizeof(uint16_t);
  
  do
  {
    if ( iPackageId == iDevicePackageIdList[iIndex] )
    {
      iFoundIndex = iIndex;
    }
    iIndex++;
  }
  while ( ( iFoundIndex == 0u ) && ( iIndex < iMaxIndex ) );
    
  return (char *)szDevicePackageNameList[iFoundIndex];
}
