/**
  ******************************************************************************
  * File Name          : app_menu.h
  * Description        : Header for Menu management.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_MENU_H
#define APP_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"  


/* Exported types ------------------------------------------------------------*/
#define APPD_MENU_DISPLAY_LINE_MAX              32      /* Number maximum of Characters can be displayed in a Menu line */
#define APPD_MENU_DISPLAY_MENU_MAX              15      /* Number maximum of Characters for a Menu display */
#define APPD_MENU_MENU_LINES_MAX                20      /* Number maximum of a Menu lines */
  
#define APP_MENU_EXIT       "Exit"


typedef enum
{
  MENU_BUTTON_NONE,
  MENU_BUTTON_ENTER,          // To enter on Menu and select line and/ option.
  MENU_BUTTON_ARROW_HIGH,
  MENU_BUTTON_ARROW_LOW,
  MENU_BUTTON_ARROW_RIGHT,
  MENU_BUTTON_ARROW_LEFT,
  MENU_BUTTON_STOP            // To exit from Menu.
} AppMenuButtonsT;


typedef struct
{
  char  szSentence[APPD_MENU_DISPLAY_LINE_MAX];
} AppMenuSentenceT;

  
/* Structure that describe Menu */
struct MenuButtonsT 
{
  char                    szMenuDisplay[APPD_MENU_DISPLAY_MENU_MAX];    /* First Level Menu Display */
  char                    szSubMenu[APPD_MENU_DISPLAY_MENU_MAX];        /* Option Displayed after selection on Menu. */
  const AppMenuSentenceT  *pszOptionList;                               /* List of Display for selected Option. NULL if it's numbers */
  int16_t                 *piOptionValue;                               /* Pointer on Value du display/change. */
  int16_t                 iOptionMin;                                   /* Index/Number Minimum for this option */
  int16_t                 iOptionMax;                                   /* Index/Number Maximum for this option */
  int16_t                 iOptionIncrement;                             /* Number Increment for this option (if not list) */
  int16_t                 iOptionDisplayDiv;                            /* Number Display divider for this option (if not list) */
};


/* Public functions -----------------------------------------------------------*/

void    APP_MENU_Init                ( struct MenuButtonsT * pMenu );
uint8_t APP_MENU_IsMenuDisplayed     ( void ) ;

void    APP_MENU_ButtonArrowHigh     ( void );
void    APP_MENU_ButtonArrowLow      ( void );
uint8_t APP_MENU_ButtonEnter         ( uint16_t * iIndexMenuChange );
void    APP_MENU_ButtonStop          ( void );


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_MENU_H */

