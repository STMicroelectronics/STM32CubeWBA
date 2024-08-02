
/**
  ******************************************************************************
  * File Name          : app_menu.c
  * Description        : Menu management to change a parameter on Application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <assert.h>
#include <stdint.h>

#include "app_common.h"
#include "app_conf.h"
#include "log_module.h"
#include "app_entry.h"
#include "app_menu.h"
#include "dbg_trace.h"


/* Private Types -----------------------------------------------*/



/* Private function prototypes -----------------------------------------------*/



/* Private variables -----------------------------------------------*/
static struct MenuButtonsT    *pstMenuButton;
static uint8_t                bMenuDisplayed, bOptionLineDisplayed;
static uint16_t               iMenuIndex, iMenuIndexMax;
static int16_t                iOptionIndex;

static char                   szText[APPD_MENU_DISPLAY_LINE_MAX];


void APP_MENU_Init ( struct MenuButtonsT * pMenu )
{
  uint8_t   bCompare = FALSE;
  uint16_t  iIndex = 0;
  
  pstMenuButton = pMenu;
  bMenuDisplayed = FALSE;
  bOptionLineDisplayed = FALSE;
  iMenuIndex = 0;
  iOptionIndex = 0;
  
  /* Detection of End of Menu */
  do
  {
    if ( strcmp( pstMenuButton[iIndex].szMenuDisplay, APP_MENU_EXIT ) == 0u )
    {
      bCompare = TRUE;
      iMenuIndexMax = iIndex;
    }
    iIndex++;
  }
  while ( ( bCompare == FALSE ) && ( iIndex < APPD_MENU_MENU_LINES_MAX ) );
}


void APP_MENU_DisplayMenu( void )
{
  LOG_INFO_APP( "%s", pstMenuButton[iMenuIndex].szMenuDisplay );
}


uint8_t APP_MENU_IsMenuDisplayed( void )
{
  return( bMenuDisplayed );
}

void APP_MENU_DisplayOption( void )
{
  int16_t   iDiv;
  
  strncpy( szText, pstMenuButton[iMenuIndex].szSubMenu, sizeof( szText ) );
  if ( pstMenuButton[iMenuIndex].pszOptionList != NULL )
  {
    LOG_INFO_APP( "%s%s", szText, pstMenuButton[iMenuIndex].pszOptionList[iOptionIndex].szSentence );
  }
  else
  {
    if ( pstMenuButton[iMenuIndex].iOptionDisplayDiv != 1u )
    { 
      iDiv = pstMenuButton[iMenuIndex].iOptionDisplayDiv;
      LOG_INFO_APP( "%s %d.%02d", szText, ( iOptionIndex / iDiv ), (uint16_t)( iOptionIndex % iDiv ) );
    }
    else
    {
      LOG_INFO_APP( "%s %d", szText, iOptionIndex );
    }
  }
}


void APP_MENU_DisplayExit( uint8_t bExitWithValid )
{
  if ( bExitWithValid == FALSE )
  {
    LOG_INFO_APP( "Exit from Menu without change." );
  }
  else
  {
    LOG_INFO_APP( "Exit from Menu with new value." );
  }
}


void APP_MENU_ButtonArrowHigh( void )
{
  /* Arrow High can be used only if Menu is displayed */
  if ( bMenuDisplayed != FALSE ) 
  { 
    if ( bOptionLineDisplayed != FALSE )
    {
      if ( iOptionIndex < pstMenuButton[iMenuIndex].iOptionMax )
      {
        iOptionIndex += pstMenuButton[iMenuIndex].iOptionIncrement;
        APP_MENU_DisplayOption();
      }
    }
    else
    {
      if ( iMenuIndex < iMenuIndexMax )
      {
        iMenuIndex++;
        APP_MENU_DisplayMenu();
      }
    }
  }
}


void APP_MENU_ButtonArrowLow( void )
{
  /* Arrow Low can be used only if Menu is displayed */
  if ( bMenuDisplayed != FALSE ) 
  { 
    if ( bOptionLineDisplayed != FALSE )
    {
      if ( iOptionIndex > pstMenuButton[iMenuIndex].iOptionMin )
      {
        iOptionIndex -= pstMenuButton[iMenuIndex].iOptionIncrement;
        APP_MENU_DisplayOption();
      }
    }
    else
    {
      if ( iMenuIndex > 0 )
      {
        iMenuIndex--;
        APP_MENU_DisplayMenu();
      }
    }
  }
}


uint8_t APP_MENU_ButtonEnter( uint16_t * iIndexMenuChange )
{
  uint8_t   bReturn = FALSE;
  
  /* First 'Enter' : Display Menu */
  if ( bMenuDisplayed == FALSE ) 
  { 
    bMenuDisplayed = TRUE;
    bOptionLineDisplayed = FALSE;
    iMenuIndex = 0;
    iOptionIndex = 0;
    
    APP_MENU_DisplayMenu();
  }
  else
  {
    /* Second 'Enter' : Select Menu, verify if not 'Exit' else enter SubMenu */
    if ( bOptionLineDisplayed == FALSE )
    {
      if ( iMenuIndex == iMenuIndexMax )
      { 
        bMenuDisplayed = FALSE;
        APP_MENU_DisplayExit( FALSE );
      }
      else
      {
        bOptionLineDisplayed = TRUE;
        iOptionIndex = *pstMenuButton[iMenuIndex].piOptionValue;
        APP_MENU_DisplayOption();
      }
    }
    else
    {
      /* Third 'Enter' : Select Option */
      *pstMenuButton[iMenuIndex].piOptionValue = iOptionIndex;
      *iIndexMenuChange = iMenuIndex;
      bMenuDisplayed = FALSE;
      APP_MENU_DisplayExit( TRUE );
      bReturn = TRUE;
    }
  }
  
  return( bReturn );
}


void APP_MENU_ButtonStop( void )
{
  bMenuDisplayed = FALSE;
  APP_MENU_DisplayExit( FALSE );
}
