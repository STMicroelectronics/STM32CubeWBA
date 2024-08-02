/**
******************************************************************************
* @file    app_menu.c
* @author  MCD Application Team
* @brief   Application interface for menu
******************************************************************************
* @attention
*
* Copyright (c) 2020-2021 STMicroelectronics.
* All rights reserved.
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "app_menu.h"
#include "app_conf.h"
#include <stdio.h>
#include "stm32_lcd.h"
#include "stm32wba55g_discovery_lcd.h"
#include "stm32wba55g_discovery_bus.h"
#include "stm32_seq.h"

/* External variables ------------------------------------------------------- */

/* Private defines ---------------------------------------------------------- */

/* Private variables -------------------------------------------------------- */
Menu_Page_t Menu_PagePool[MENU_MAX_PAGE];
tListNode Menu_PagePoolList;
tListNode Menu_PageActiveList;

Menu_Page_t *pCurrentPage;

extern uint8_t arrow_return_byteicon[];

/* Private functions prototypes-----------------------------------------------*/
static int32_t LCD_DrawBitmapArray(uint8_t xpos, uint8_t ypos, uint8_t xlen, uint8_t ylen, uint8_t *data);
static uint8_t Menu_ExecuteAction(Menu_Action_t Action);

/* Exported Functions Definition -------------------------------------------- */

/**
 * @brief Initialize the Menu module
 */
void Menu_Init(void)
{
  uint8_t i;

  LST_init_head(&Menu_PagePoolList);
  LST_init_head(&Menu_PageActiveList);

  for (i = 0; i < MENU_MAX_PAGE; i++)
  {
    Menu_PagePool[i].NumEntry = 0;
    Menu_PagePool[i].SelectedEntry = 0;
    LST_insert_tail(&Menu_PagePoolList, &Menu_PagePool[i].Node);
  }
}

/**
 * @brief Remove a Menu Page
 * @param MenuType: The type of the menu page
 * @retval A pointer to the menu page created, 0 if the pool list is empty
 */
Menu_Page_t* Menu_CreatePage(Menu_Type_t MenuType)
{
  Menu_Page_t *p_menu_page;
  if (LST_is_empty(&Menu_PagePoolList) == FALSE)
  {
    LST_remove_head(&Menu_PagePoolList, (tListNode **)&p_menu_page);
    p_menu_page->MenuType = MenuType;
    LST_insert_tail(&Menu_PageActiveList, (tListNode *)p_menu_page);

    if (MenuType == MENU_TYPE_LIST)
    {
      p_menu_page->SelectedEntry = 0;
      p_menu_page->CurrentStartId = 0;
    }

    return p_menu_page;
  }
  else
  {
    return 0;
  }
}

/**
 * @brief Remove a Menu Page
 * @param pMenuPage: A pointer to the menu page
 */
void Menu_RemovePage(Menu_Page_t* pMenuPage)
{
  if (LST_is_empty(&Menu_PageActiveList) == FALSE)
  {
    LST_remove_node((tListNode *) pMenuPage);
    LST_insert_tail(&Menu_PagePoolList, (tListNode *)pMenuPage);
  }
}

/**
 * @brief Add a list entry to a List menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pText: The text of the entry
 * @param Action: The Action to assign to the list entry
 */
void Menu_AddListEntry(Menu_Page_t* pMenuPage, char *pText, Menu_Action_t Action)
{
  snprintf((char *)&pMenuPage->ListEntry[pMenuPage->NumEntry].Text, MENU_LIST_ENTRY_MAX_TEXT_LEN, "%s",pText);
  pMenuPage->ListEntry[pMenuPage->NumEntry].Action = Action;
  pMenuPage->NumEntry++;
}

/**
 * @brief Clear all entries in a List menu page
 * @param pMenuPage: A pointer to the menu page
 */
void Menu_ClearList(Menu_Page_t* pMenuPage)
{
  pMenuPage->NumEntry = 0;
  pMenuPage->SelectedEntry = 0;
}

/**
 * @brief Set the content of a Control Menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pText: A pointer to a content text structure to display
 * @param pIcon: A pointer to an icon structure to discplay
 */
void Menu_SetControlContent(Menu_Page_t* pMenuPage, Menu_Content_Text_t *pText, Menu_Icon_t* pIcon)
{
  pMenuPage->pIcon = pIcon;
  pMenuPage->pText = pText;
}

/**
 * @brief Assign an action related to a direction on a menu page
 * @param pMenuPage: A pointer to the menu page
 * @param Direction: The direction to assign the action to
 * @param Action: The Action to assign
 */
void Menu_SetControlAction(Menu_Page_t* pMenuPage, Menu_Action_Direction_t Direction, Menu_Action_t Action)
{
  switch (Direction)
  {
    case MENU_DIRECTION_LEFT:
    {
      pMenuPage->ActionLeft = Action;
      break;
    }
    case MENU_DIRECTION_RIGHT:
    {
      pMenuPage->ActionRight = Action;
      break;
    }
    case MENU_DIRECTION_DOWN:
    {
      pMenuPage->ActionDown = Action;
      break;
    }
    case MENU_DIRECTION_UP:
    {
      pMenuPage->ActionUp = Action;
      break;
    }
  }
}

/**
 * @brief Get current active page
 * @return pMenuPage: A pointer to the current menu page
 */
Menu_Page_t* Menu_GetActivePage(void)
{
  return pCurrentPage;
}

/**
 * @brief Set the logo of a Logo Menu page
 * @param pMenuPage: A pointer to the menu page
 * @param pIcon: A pointer to an icon structure to discplay
 */
void Menu_SetLogo(Menu_Page_t* pMenuPage, Menu_Icon_t* pIcon)
{
  pMenuPage->pIcon = pIcon;
}

/**
 * @brief Set new page as active page
 * @param pMenuPage: A pointer to the menu page to set
 */
void Menu_SetActivePage(Menu_Page_t* pMenuPage)
{
  pCurrentPage = pMenuPage;
  Menu_Print();
}

/**
 * @brief Navigate in the menu, direction Left
 */
void Menu_Left(void)
{
  uint8_t ret;
  ret = Menu_ExecuteAction(pCurrentPage->ActionLeft);
  if (ret == 0 && pCurrentPage->pReturnPage != 0)
  {
    /* Return to previous menu page */
    pCurrentPage = (Menu_Page_t *) pCurrentPage->pReturnPage;
    Menu_Print();
  }
}

/**
 * @brief Navigate in the menu, direction Right
 */
void Menu_Right(void)
{
  switch (pCurrentPage->MenuType)
  {
    case MENU_TYPE_LIST:
    {
      Menu_ExecuteAction(pCurrentPage->ListEntry[pCurrentPage->SelectedEntry].Action);
      break;
    }
    case MENU_TYPE_CONTROL:
    {
      Menu_ExecuteAction(pCurrentPage->ActionRight);
      break;
    }
    default:
    break;
  }
}

/**
 * @brief Navigate in the menu, direction Up
 */
void Menu_Up(void)
{
  switch (pCurrentPage->MenuType)
  {
    case MENU_TYPE_LIST:
    {
      if (pCurrentPage->SelectedEntry == 0)
      {
        pCurrentPage->SelectedEntry = pCurrentPage->NumEntry - 1;
      }
      else
      {
        pCurrentPage->SelectedEntry = (pCurrentPage->SelectedEntry - 1) % pCurrentPage->NumEntry;
      }
      Menu_Print();
      break;
    }
    case MENU_TYPE_CONTROL:
    {
      Menu_ExecuteAction(pCurrentPage->ActionUp);
      break;
    }
    default:
    break;
  }
}

/**
 * @brief Navigate in the menu, direction Down
 */
void Menu_Down(void)
{
  switch (pCurrentPage->MenuType)
  {
    case MENU_TYPE_LIST:
    {
      pCurrentPage->SelectedEntry = (pCurrentPage->SelectedEntry + 1) % pCurrentPage->NumEntry;
      Menu_Print();
      break;
    }
    case MENU_TYPE_CONTROL:
    {
      Menu_ExecuteAction(pCurrentPage->ActionDown);
      break;
    }
    default:
    break;
  }
}

/**
 * @brief Request execution of the Print Task
 */
void Menu_Print(void)
{
#if (CFG_LCD_SUPPORTED == 1)
  UTIL_SEQ_SetTask( 1U << CFG_TASK_MENU_PRINT_ID, CFG_SEQ_PRIO_0);
#endif /* CFG_LCD_SUPPORTED */
}

/**
 * @brief Print the current menu on the screen
 */
void Menu_Print_Task(void)
{
  BSP_SPI3_Init();
  BSP_LCD_Clear(0,SSD1315_COLOR_BLACK);

  switch (pCurrentPage->MenuType)
  {
    case MENU_TYPE_LIST:
    {
      uint8_t i;
      if (pCurrentPage->SelectedEntry > pCurrentPage->CurrentStartId + 3)
      {
        pCurrentPage->CurrentStartId = pCurrentPage->SelectedEntry - 3;
      }
      else if (pCurrentPage->SelectedEntry < pCurrentPage->CurrentStartId)
      {
        pCurrentPage->CurrentStartId = pCurrentPage->SelectedEntry;
      }

      for (i = 0; i < 4; i++)
      {
        if (i+pCurrentPage->CurrentStartId < pCurrentPage->NumEntry)
        {
          if (i+pCurrentPage->CurrentStartId == pCurrentPage->SelectedEntry)
          {
            UTIL_LCD_FillRect(0, i*MENU_ENTRY_LINE_HEIGHT, SSD1315_LCD_PIXEL_WIDTH, MENU_ENTRY_LINE_HEIGHT, SSD1315_COLOR_WHITE);
            UTIL_LCD_SetTextColor(SSD1315_COLOR_BLACK);
            UTIL_LCD_SetBackColor(SSD1315_COLOR_WHITE);
            UTIL_LCD_DisplayStringAt(0, i*MENU_ENTRY_LINE_HEIGHT + MENU_ENTRY_TEXT_X_OFFSET, (uint8_t *) ">", RIGHT_MODE);
          }
          else
          {
            UTIL_LCD_SetTextColor(SSD1315_COLOR_WHITE);
            UTIL_LCD_SetBackColor(SSD1315_COLOR_BLACK);
          }
          UTIL_LCD_DisplayStringAt(0, i*MENU_ENTRY_LINE_HEIGHT + MENU_ENTRY_TEXT_X_OFFSET,
                                  (uint8_t *) pCurrentPage->ListEntry[i+pCurrentPage->CurrentStartId].Text, CENTER_MODE);
        }
      }
      break;
    }

    case MENU_TYPE_CONTROL:
    {
      uint8_t num_lines = 0;

      UTIL_LCD_SetTextColor(SSD1315_COLOR_WHITE);
      UTIL_LCD_SetBackColor(SSD1315_COLOR_BLACK);

      if (pCurrentPage->pIcon != 0)
      {
        num_lines++;
      }

      num_lines += pCurrentPage->pText->NumLines;

      if (num_lines == 1)
      {
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 6,
                                 (uint8_t *) pCurrentPage->pText->Lines[0], CENTER_MODE);
      }
      else if (num_lines == 2)
      {
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 14,
                                 (uint8_t *) pCurrentPage->pText->Lines[0], CENTER_MODE);
        if (pCurrentPage->pIcon == 0)
        {
          UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 2,
                                   (uint8_t *) pCurrentPage->pText->Lines[1], CENTER_MODE);
        }
        else
        {
          if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_IMAGE)
          {
            LCD_DrawBitmapArray((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - pCurrentPage->pIcon->ImageWidth/2,
                                (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2),
                                pCurrentPage->pIcon->ImageWidth,
                                pCurrentPage->pIcon->ImageHeight,
                                pCurrentPage->pIcon->pImage);
          }
          else if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_CHAR)
          {
            UTIL_LCD_DisplayChar((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - 3,
                                 (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 2, pCurrentPage->pIcon->Character);
          }
        }
      }
      else if (num_lines == 3)
      {
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 22,
                                 (uint8_t *) pCurrentPage->pText->Lines[0], CENTER_MODE);
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 6,
                                 (uint8_t *) pCurrentPage->pText->Lines[1], CENTER_MODE);
        if (pCurrentPage->pIcon == 0)
        {
          UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 10,
                                   (uint8_t *) pCurrentPage->pText->Lines[2], CENTER_MODE);
        }
        else
        {
          if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_IMAGE)
          {
            LCD_DrawBitmapArray((uint8_t) SSD1315_LCD_PIXEL_WIDTH  / 2 - pCurrentPage->pIcon->ImageWidth/2,
                                (uint8_t) SSD1315_LCD_PIXEL_HEIGHT / 2 + pCurrentPage->pIcon->ImageHeight/2,
                                pCurrentPage->pIcon->ImageWidth,
                                pCurrentPage->pIcon->ImageHeight,
                                pCurrentPage->pIcon->pImage);
          }
          else if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_CHAR)
          {
            UTIL_LCD_DisplayChar((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - 3,
                                 (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 10, pCurrentPage->pIcon->Character);
          }
        }
      }
      else if (num_lines == 4)
      {
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 30,
                                 (uint8_t *) pCurrentPage->pText->Lines[0], CENTER_MODE);
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 14,
                                 (uint8_t *) pCurrentPage->pText->Lines[1], CENTER_MODE);
        UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 2,
                                 (uint8_t *) pCurrentPage->pText->Lines[2], CENTER_MODE);
        if (pCurrentPage->pIcon == 0)
        {
          UTIL_LCD_DisplayStringAt(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 18,
                                   (uint8_t *) pCurrentPage->pText->Lines[3], CENTER_MODE);
        }
        else
        {
          if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_IMAGE)
          {
            LCD_DrawBitmapArray((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - pCurrentPage->pIcon->ImageWidth/2,
                                (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + pCurrentPage->pIcon->ImageHeight,
                                pCurrentPage->pIcon->ImageWidth,
                                pCurrentPage->pIcon->ImageHeight,
                                pCurrentPage->pIcon->pImage);
          }
          else if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_CHAR)
          {
            UTIL_LCD_DisplayChar((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - 3,
                                 (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) + 18, pCurrentPage->pIcon->Character);
          }
        }
      }
      else
      {
        /* Error */
      }

      if (((pCurrentPage->ActionRight.ActionType & MENU_ACTION_CALLBACK) && pCurrentPage->ActionRight.Callback != 0)
          || ((pCurrentPage->ActionRight.ActionType & MENU_ACTION_MENU_PAGE) && pCurrentPage->ActionRight.pPage != 0))
      {
        if (pCurrentPage->ActionRight.pIcon->IconType == MENU_ICON_TYPE_CHAR)
        {
          UTIL_LCD_DisplayChar(SSD1315_LCD_PIXEL_WIDTH - 7, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 6,
                               pCurrentPage->ActionRight.pIcon->Character);
        }
        else if (pCurrentPage->ActionRight.pIcon->IconType == MENU_ICON_TYPE_IMAGE)
        {
          LCD_DrawBitmapArray(SSD1315_LCD_PIXEL_WIDTH - pCurrentPage->ActionRight.pIcon->ImageWidth,
                              (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - pCurrentPage->ActionRight.pIcon->ImageHeight/2,
                              pCurrentPage->ActionRight.pIcon->ImageWidth,
                              pCurrentPage->ActionRight.pIcon->ImageHeight,
                              pCurrentPage->ActionRight.pIcon->pImage);
        }
      }

      if (((pCurrentPage->ActionUp.ActionType & MENU_ACTION_CALLBACK) && pCurrentPage->ActionUp.Callback != 0)
          || ((pCurrentPage->ActionUp.ActionType & MENU_ACTION_MENU_PAGE) && pCurrentPage->ActionUp.pPage != 0))
      {
        if (pCurrentPage->ActionUp.pIcon->IconType == MENU_ICON_TYPE_CHAR)
        {
          UTIL_LCD_DisplayChar((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - 3, 0, pCurrentPage->ActionUp.pIcon->Character);
        }
        else if (pCurrentPage->ActionUp.pIcon->IconType == MENU_ICON_TYPE_IMAGE)
        {
          LCD_DrawBitmapArray((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - pCurrentPage->ActionUp.pIcon->ImageWidth/2,
                              0,
                              pCurrentPage->ActionUp.pIcon->ImageWidth,
                              pCurrentPage->ActionUp.pIcon->ImageHeight,
                              pCurrentPage->ActionUp.pIcon->pImage);
        }
      }

      if (((pCurrentPage->ActionDown.ActionType & MENU_ACTION_CALLBACK) && pCurrentPage->ActionDown.Callback != 0)
          || ((pCurrentPage->ActionDown.ActionType & MENU_ACTION_MENU_PAGE) && pCurrentPage->ActionDown.pPage != 0))
      {
        if (pCurrentPage->ActionDown.pIcon->IconType == MENU_ICON_TYPE_CHAR)
        {
          UTIL_LCD_DisplayChar((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - 3, SSD1315_LCD_PIXEL_HEIGHT - 12,
                               pCurrentPage->ActionDown.pIcon->Character);
        }
        else if (pCurrentPage->ActionDown.pIcon->IconType == MENU_ICON_TYPE_IMAGE)
        {
          LCD_DrawBitmapArray((uint8_t) SSD1315_LCD_PIXEL_WIDTH / 2 - pCurrentPage->ActionDown.pIcon->ImageWidth/2,
                              SSD1315_LCD_PIXEL_HEIGHT - pCurrentPage->ActionDown.pIcon->ImageHeight,
                              pCurrentPage->ActionDown.pIcon->ImageWidth,
                              pCurrentPage->ActionDown.pIcon->ImageHeight,
                              pCurrentPage->ActionDown.pIcon->pImage);
        }
      }

      if (((pCurrentPage->ActionLeft.ActionType & MENU_ACTION_CALLBACK) && pCurrentPage->ActionLeft.Callback != 0)
          || ((pCurrentPage->ActionLeft.ActionType & MENU_ACTION_MENU_PAGE) && pCurrentPage->ActionLeft.pPage != 0))
      {
        if (pCurrentPage->ActionLeft.pIcon->IconType == MENU_ICON_TYPE_CHAR)
        {
          UTIL_LCD_DisplayChar(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 6, pCurrentPage->ActionLeft.pIcon->Character);
        }
        else if (pCurrentPage->ActionLeft.pIcon->IconType == MENU_ICON_TYPE_IMAGE)
        {
          LCD_DrawBitmapArray(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - pCurrentPage->ActionLeft.pIcon->ImageHeight/2,
                              pCurrentPage->ActionLeft.pIcon->ImageWidth, pCurrentPage->ActionLeft.pIcon->ImageHeight,
                              pCurrentPage->ActionLeft.pIcon->pImage);
        }
      }
      else if (pCurrentPage->pReturnPage != 0)
      {
        LCD_DrawBitmapArray(0, (uint8_t) (SSD1315_LCD_PIXEL_HEIGHT / 2) - 8/2,
                    8,
                    8,
                    arrow_return_byteicon);
      }

      break;
    }

    case MENU_TYPE_LOGO:
    {
      if (pCurrentPage->pIcon->IconType == MENU_ICON_TYPE_IMAGE)
      {
        LCD_DrawBitmapArray((uint8_t) ((SSD1315_LCD_PIXEL_WIDTH - pCurrentPage->pIcon->ImageWidth )/ 2),
                            (uint8_t) ((SSD1315_LCD_PIXEL_HEIGHT - pCurrentPage->pIcon->ImageHeight )/ 2),
                            pCurrentPage->pIcon->ImageWidth, pCurrentPage->pIcon->ImageHeight,
                            pCurrentPage->pIcon->pImage);
      }

      break;
    }
  }

  BSP_LCD_Refresh(0);
  BSP_SPI3_DeInit();
}

/* Private Functions Definition --------------------------------------------- */

/**
 * @brief Draw an array of bits at the specified offsets starting from corner top left. Ensure xlen is multiple of 8
 * @param xpos: X coordinate to print at
 * @param ypos: Y coordinate to print at
 * @param xlen: Width of the bitmap array
 * @param ylen: Height of the bitmap array
 * @param data: Pointer to the bitmap array
 * @retval 0 if success, -1 if the coordinates are out of screen
 */
static int32_t LCD_DrawBitmapArray(uint8_t xpos, uint8_t ypos, uint8_t xlen, uint8_t ylen, uint8_t *data){

  int32_t i,j,k;
  uint8_t mask;
  uint8_t* pdata = data;

  if (((xpos+xlen) > 128) || ((ypos+ylen) > 64))
  {
    /*out of screen*/
    return -1;
  }

  for (j=0 ; j < ylen ; j++)
  {
    for (i=0 ; i < xlen/8 ; i++)
    {
      mask = 0x80;
      for (k=0 ; k < 8 ; k++)
      {
        if ( mask & *pdata)
        {
          UTIL_LCD_SetPixel(i*8+k+xpos, j+ypos, SSD1315_COLOR_WHITE);
        }
        else
        {
          UTIL_LCD_SetPixel(i*8+k+xpos, j+ypos, SSD1315_COLOR_BLACK);
        }
        mask = mask >> 1;
      }
      pdata++;
    }
  }

  return 0;
}

/**
 * @brief Execute Selected Action
 * @param Action: Action to execute
 */
static uint8_t Menu_ExecuteAction(Menu_Action_t Action)
{
  uint8_t ret = 0;

  if ((Action.ActionType & MENU_ACTION_CALLBACK) && Action.Callback != 0)
  {
    Action.Callback();
  }

  if ((Action.ActionType & MENU_ACTION_MENU_PAGE) && Action.pPage != 0)
  {
    ((Menu_Page_t *) Action.pPage)->pReturnPage = (struct Menu_Page_t *)pCurrentPage;
    pCurrentPage = (Menu_Page_t *) Action.pPage;
    Menu_Print();
    ret = 1;
  }

  if ((Action.ActionType & MENU_ACTION_LIST_CALLBACK) && Action.ListCallback != 0)
  {
    Action.ListCallback(pCurrentPage->SelectedEntry);
  }
  return ret;
}
