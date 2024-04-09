/**
  ******************************************************************************
  * @file    app_menu.h
  * @author  MCD Application Team
  * @brief   Header for Menu file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef APP_MENU_H
#define APP_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tmap_app.h"
#include "stm_list.h"


/* Defines ------------------------------------------------------------------ */
#define MENU_ENTRY_LINE_HEIGHT  16
#define MENU_ENTRY_TEXT_X_OFFSET 2
#define MENU_LIST_ENTRY_MAX_TEXT_LEN 20
#define MENU_CONTROL_MAX_LINE_LEN 20
#define MENU_CONTROL_MAX_LINE_NUMBER 4

#define MENU_MAX_PAGE 11
#define MENU_NUM_LIST_DISPLAY_ENTRY 4
#define MENU_NUM_LIST_ENTRY 20

/* Exported Types ------------------------------------------------------------ */

typedef enum
{
  MENU_TYPE_LIST,
  MENU_TYPE_CONTROL,
  MENU_TYPE_LOGO
} Menu_Type_t;

typedef uint8_t Menu_Action_Type_t;
#define MENU_ACTION_MENU_PAGE     0x01 /* Set a selected Menu Page as active */
#define MENU_ACTION_CALLBACK      0x02 /* Call a callback function */
#define MENU_ACTION_LIST_CALLBACK 0x04 /* Call a callback function with current list ID (MENU_TYPE_LIST only)*/

typedef enum
{
  MENU_DIRECTION_LEFT,
  MENU_DIRECTION_RIGHT,
  MENU_DIRECTION_UP,
  MENU_DIRECTION_DOWN
} Menu_Action_Direction_t;

typedef enum
{
  MENU_ICON_TYPE_IMAGE,
  MENU_ICON_TYPE_CHAR
} Menu_Icon_Type_t;

typedef struct
{
  uint8_t NumLines;
  char Lines[MENU_CONTROL_MAX_LINE_NUMBER][MENU_CONTROL_MAX_LINE_LEN];
} Menu_Content_Text_t;

typedef struct
{
  Menu_Icon_Type_t IconType;

  /* MENU_ICON_TYPE_IMAGE parameters */
  uint8_t *pImage;
  uint8_t ImageWidth;
  uint8_t ImageHeight;

  /* MENU_ICON_TYPE_CHAR parameters */
  char Character;
} Menu_Icon_t;

typedef struct
{
  Menu_Action_Type_t ActionType;
  Menu_Icon_t *pIcon;

  /* MENU_ACTION_CALLBACK parameters */
  void (*Callback)(void);

  /* MENU_ACTION_MENU_PAGE parameters */
  struct Menu_Page_t *pPage;

  /* MENU_ACTION_LIST_CALLBACK parameters */
  void (*ListCallback)(uint8_t);
} Menu_Action_t;

typedef struct
{
  char Text[MENU_LIST_ENTRY_MAX_TEXT_LEN];
  Menu_Action_t Action;
} Menu_List_Entry_t;

typedef struct
{
  tListNode                     Node;
  Menu_Type_t                   MenuType;
  struct Menu_Page_t            *pReturnPage;

  /* List Type */
  uint8_t                       NumEntry;
  Menu_List_Entry_t             ListEntry[MENU_NUM_LIST_ENTRY];
  uint8_t                       SelectedEntry;
  uint8_t                       CurrentStartId;

  /* Control Type */
  Menu_Content_Text_t           *pText;
  Menu_Icon_t                   *pIcon;
  Menu_Action_t                 ActionRight;
  Menu_Action_t                 ActionLeft;
  Menu_Action_t                 ActionUp;
  Menu_Action_t                 ActionDown;
} Menu_Page_t;

/* Shared variables --------------------------------------------------------- */

/* Exported Prototypes -------------------------------------------------------*/
/**
 * @brief Initialize the Menu module
 */
void Menu_Init(void);

/**
 * @brief Remove a Menu Page
 * @param MenuType: The type of the menu page
 * @retval A pointer to the menu page created, 0 if the pool list is empty
 */
Menu_Page_t* Menu_CreatePage(Menu_Type_t MenuType);

/**
 * @brief Remove a Menu Page
 * @param pMenuPage: A pointer to the menu page
 */
void Menu_RemovePage(Menu_Page_t* pMenuPage);

/**
 * @brief Add a list entry to a List menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pText: The text of the entry
 * @param Action: The Action to assign to the list entry
 */
void Menu_AddListEntry(Menu_Page_t* pMenuPage, char *pText, Menu_Action_t Action);

/**
 * @brief Clear all entries in a List menu page
 * @param pMenuPage: A pointer to the menu page
 */
void Menu_ClearList(Menu_Page_t* pMenuPage);

/**
 * @brief Set the content of a Control Menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pText: A pointer to a content text structure to display
 * @param pIcon: A pointer to an icon structure to discplay
 */
void Menu_SetControlContent(Menu_Page_t* pMenuPage, Menu_Content_Text_t *pText, Menu_Icon_t* pIcon);

/**
 * @brief Assign an action related to a direction on a menu page
 * @param pMenuPage: A pointer to the menu page
 * @param Direction: The direction to assign the action to
 * @param Action: The Action to assign
 */
void Menu_SetControlAction(Menu_Page_t* pMenuPage, Menu_Action_Direction_t Direction, Menu_Action_t Action);

/**
 * @brief Set the logo of a Logo Menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pIcon: A pointer to an icon structure to discplay
 */
void Menu_SetLogo(Menu_Page_t* pMenuPage, Menu_Icon_t* pIcon);

/**
 * @brief Set new page as active page
 * @param pMenuPage: A pointer to the menu page to set
 */
void Menu_SetActivePage(Menu_Page_t* pMenuPage);

/**
 * @brief Navigate in the menu, direction Left
 */
void Menu_Left(void);

/**
 * @brief Navigate in the menu, direction Right
 */
void Menu_Right(void);

/**
 * @brief Navigate in the menu, direction Up
 */
void Menu_Up(void);

/**
 * @brief Navigate in the menu, direction Down
 */
void Menu_Down(void);

/**
 * @brief Request execution of the Print Task
 */
void Menu_Print(void);

/**
 * @brief Print the current menu on the screen
 */
void Menu_Print_Task(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* APP_MENU_H */
