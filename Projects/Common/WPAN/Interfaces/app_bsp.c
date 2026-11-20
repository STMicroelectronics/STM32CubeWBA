/**
  ******************************************************************************
  * @file    app_bsp.c
  * @author  MCD Application Team
  * @brief   Application to manage BSP.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "app_common.h"
#include "log_module.h"
#include "app_conf.h"
#include "app_bsp.h"
#include "main.h"

#include "stm32_rtos.h"
#include "stm32_timer.h"

/* Private includes -----------------------------------------------------------*/
#include "serial_cmd_interpreter.h"

/* Private typedef -----------------------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
typedef struct
{
  Button_TypeDef      button;
  UTIL_TIMER_Object_t longTimerId;
  uint8_t             longPressed;
  uint32_t            waitingTime;
} ButtonDesc_t;

typedef enum
{
  BUTTON_ACTION_1 = 0,
  BUTTON_ACTION_2,
  BUTTON_ACTION_3
} ButtonActionId_t;

typedef struct
{
  Button_TypeDef      button;
  ButtonMode_TypeDef  mode;
  ButtonActionId_t    actionId;
  const char *        cmd;
  const char *        cmdAlt;
  const char *        longCmd;
  const char *        longCmdAlt;
} ButtonHwConfig_t;

typedef struct
{
  ButtonDesc_t        buttonDesc[BUTTONn];
  ButtonHwConfig_t    buttons[BUTTONn];
} ButtonBoardContext_t;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* Private defines -----------------------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_SAMPLE_MS           (50u)     /* Sample button level rate in milli seconds. */
#define BUTTON_LONG_PRESS_THRESHOLD_MS        (500u)    /* Long pression time threshold in milli seconds. */
#define BUTTON_NB_MAX                         ((uint32_t)BUTTONn)

/* Button board configuration */
#if defined(CFG_BSP_ON_CEB) && defined(STM32WBA5Mxx)

#define APP_BSP_BUTTON_WAKEUP_BUTTON2         B2
#define APP_BSP_BUTTON_DESC_INIT              \
  {                                           \
    { B2, { 0 }, 0u, 0u }                     \
  }
#define APP_BSP_BUTTON_HWCFG_INIT             \
  {                                           \
    { B2, BUTTON_MODE_GPIO, BUTTON_ACTION_2,  \
      "B2", "SW2", "B2L", "SW2L" }            \
  }

#define APP_BSP_BUTTON_ENABLE_WAKEUP()        \
  do                                          \
  {                                           \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_HIGH_1);  /* PC13 */ \
  } while (0)

#define APP_BSP_BUTTON_ENABLE_RETENTION()     \
  do                                          \
  {                                           \
    SET_BIT(PWR->IORETENRC, B2_PIN);          /* Retention for B2 */ \
  } while (0)

#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_ACTION2_SEQ_TASK       CFG_TASK_BSP_BUTTON_B2
#endif /* CFG_BSP_ON_SEQUENCER */

#elif defined(CFG_BSP_ON_CEB) && defined(STM32WBA6Mxx)

#define APP_BSP_BUTTON_WAKEUP_BUTTON2         B1
#define APP_BSP_BUTTON_DESC_INIT              \
  {                                           \
    { B1, { 0 }, 0u, 0u }                     \
  }
#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_HWCFG_INIT             \
  {                                           \
    { B1, BUTTON_MODE_GPIO, BUTTON_ACTION_1,  \
      "B1", "SW1", "B1L", "SW1L" }            \
  }
#else
#define APP_BSP_BUTTON_HWCFG_INIT             \
  {                                           \
    { B1, BUTTON_MODE_GPIO, BUTTON_ACTION_2,  \
      "B1", "SW1", "B1L", "SW1L" }            \
  }
#endif

#define APP_BSP_BUTTON_ENABLE_WAKEUP()        \
  do                                          \
  {                                           \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_HIGH_1);  /* PC13 */ \
  } while (0)

#define APP_BSP_BUTTON_ENABLE_RETENTION()     \
  do                                          \
  {                                           \
    SET_BIT(PWR->IORETENRC, B1_PIN);          /* Retention for B1 */ \
  } while (0)

#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_ACTION1_SEQ_TASK       CFG_TASK_BSP_BUTTON_B1
#endif /* CFG_BSP_ON_SEQUENCER */

#elif defined(CFG_BSP_ON_NUCLEO) && defined(STM32WBA6xxx)

#define APP_BSP_BUTTON_WAKEUP_BUTTON2         B1
#define APP_BSP_BUTTON_DESC_INIT              \
  {                                           \
    { B1, { 0 }, 0u, 0u },                    \
    { B2, { 0 }, 0u, 0u },                    \
    { B3, { 0 }, 0u, 0u }                     \
  }
#define APP_BSP_BUTTON_HWCFG_INIT               \
  {                                             \
    { B1, BUTTON_MODE_GPIO, BUTTON_ACTION_1,    \
      "B1", "SW1", "B1L", "SW1L" },             \
    { B2, BUTTON_MODE_EXTI, BUTTON_ACTION_2,    \
      "B2", "SW2", "B2L", "SW2L" },             \
    { B3, BUTTON_MODE_EXTI, BUTTON_ACTION_3,    \
      "B3", "SW3", "B3L", "SW3L" }              \
  }

/* WBA Nucleo boards push buttons are configured to pwr wakeup pins
   except for WBA65 B2 and B3 configured to GPIO EXTIs. */
#define APP_BSP_BUTTON_ENABLE_WAKEUP()        \
  do                                          \
  {                                           \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW_1);    /* B1 --> PC13 */ \
  } while (0)

#define APP_BSP_BUTTON_ENABLE_RETENTION()     \
  do                                          \
  {                                           \
    SET_BIT(PWR->IORETENRC, B1_PIN);          /* Retention for B1 */ \
  } while (0)

#define APP_BSP_BUTTON_EXTI4_BUTTON           B3
#define APP_BSP_BUTTON_EXTI5_BUTTON           B2

#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_ACTION1_SEQ_TASK       CFG_TASK_BSP_BUTTON_B1
#define APP_BSP_BUTTON_ACTION2_SEQ_TASK       CFG_TASK_BSP_BUTTON_B2
#define APP_BSP_BUTTON_ACTION3_SEQ_TASK       CFG_TASK_BSP_BUTTON_B3
#endif /* CFG_BSP_ON_SEQUENCER */

#elif defined(CFG_BSP_ON_NUCLEO) && defined(STM32WBA2xxx)

#define APP_BSP_BUTTON_WAKEUP_BUTTON2         B2
#define APP_BSP_BUTTON_DESC_INIT              \
  {                                           \
    { B1, { 0 }, 0u, 0u },                    \
    { B2, { 0 }, 0u, 0u },                    \
    { B3, { 0 }, 0u, 0u }                     \
  }
#define APP_BSP_BUTTON_HWCFG_INIT               \
  {                                             \
    { B1, BUTTON_MODE_GPIO, BUTTON_ACTION_1,    \
      "B1", "SW1", "B1L", "SW1L" },             \
    { B2, BUTTON_MODE_GPIO, BUTTON_ACTION_2,    \
      "B2", "SW2", "B2L", "SW2L" },             \
    { B3, BUTTON_MODE_GPIO, BUTTON_ACTION_3,    \
      "B3", "SW3", "B3L", "SW3L" }              \
  }

#define APP_BSP_BUTTON_ENABLE_WAKEUP()        \
  do                                          \
  {                                           \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW_1);    /* B2 --> PC13 */ \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN3_LOW_1);    /* B1 --> PA1  */ \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN4_LOW_0);    /* B3 --> PA2  */ \
  } while (0)

#define APP_BSP_BUTTON_ENABLE_RETENTION()     \
  do                                          \
  {                                           \
    SET_BIT(PWR->IORETENRA, ( B1_PIN | B3_PIN ) );     /* Retention for B1 and B3 */ \
    SET_BIT(PWR->IORETENRC, B2_PIN );                  /* Retention for B2 */ \
  } while (0)

#define APP_BSP_BUTTON_WKUP3_BUTTON           B1
#define APP_BSP_BUTTON_WKUP4_BUTTON           B3

#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_ACTION1_SEQ_TASK       CFG_TASK_BSP_BUTTON_B1
#define APP_BSP_BUTTON_ACTION2_SEQ_TASK       CFG_TASK_BSP_BUTTON_B2
#define APP_BSP_BUTTON_ACTION3_SEQ_TASK       CFG_TASK_BSP_BUTTON_B3
#endif /* CFG_BSP_ON_SEQUENCER */

#elif defined(CFG_BSP_ON_NUCLEO) && defined(STM32WBA5xxx)

#define APP_BSP_BUTTON_WAKEUP_BUTTON2         B1
#define APP_BSP_BUTTON_DESC_INIT              \
  {                                           \
    { B1, { 0 }, 0u, 0u },                    \
    { B2, { 0 }, 0u, 0u },                    \
    { B3, { 0 }, 0u, 0u }                     \
  }
#define APP_BSP_BUTTON_HWCFG_INIT               \
  {                                             \
    { B1, BUTTON_MODE_GPIO, BUTTON_ACTION_1,    \
      "B1", "SW1", "B1L", "SW1L" },             \
    { B2, BUTTON_MODE_GPIO, BUTTON_ACTION_2,    \
      "B2", "SW2", "B2L", "SW2L" },             \
    { B3, BUTTON_MODE_GPIO, BUTTON_ACTION_3,    \
      "B3", "SW3", "B3L", "SW3L" }              \
  }

#define APP_BSP_BUTTON_ENABLE_WAKEUP()        \
  do                                          \
  {                                           \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW_1);    /* B1 --> PC13 */ \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN3_LOW_2);    /* B2 --> PB6  */ \
    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN5_LOW_2);    /* B3 --> PB7  */ \
  } while (0)

#define APP_BSP_BUTTON_ENABLE_RETENTION()     \
  do                                          \
  {                                           \
    SET_BIT(PWR->IORETENRC, B1_PIN );                 /* Retention for B1 */ \
    SET_BIT(PWR->IORETENRB, ( B2_PIN | B3_PIN ) );    /* Retention for B2 & B3 */ \
  } while (0)

#define APP_BSP_BUTTON_WKUP3_BUTTON           B2
#define APP_BSP_BUTTON_WKUP5_BUTTON           B3

#ifdef CFG_BSP_ON_SEQUENCER
#define APP_BSP_BUTTON_ACTION1_SEQ_TASK       CFG_TASK_BSP_BUTTON_B1
#define APP_BSP_BUTTON_ACTION2_SEQ_TASK       CFG_TASK_BSP_BUTTON_B2
#define APP_BSP_BUTTON_ACTION3_SEQ_TASK       CFG_TASK_BSP_BUTTON_B3
#endif /* CFG_BSP_ON_SEQUENCER */

#else
#error "Unsupported button board configuration"
#endif

#define APP_BSP_BUTTON_WKUP2_BUTTON           APP_BSP_BUTTON_WAKEUP_BUTTON2
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

#if (CFG_LCD_SUPPORTED == 1) && defined(CFG_BSP_ON_DISCOVERY)
#if defined(STM32WBA6xxx)
#define APP_BSP_LCD_ENABLE_RST_RETENTION()    HAL_PWREx_EnableStandbyIORetention(PWR_GPIO_E, LCD_RST_PIN)
#elif defined(STM32WBA5xxx)
#define APP_BSP_LCD_ENABLE_RST_RETENTION()    HAL_PWREx_EnableStandbyIORetention(PWR_GPIO_A, LCD_RST_PIN)
#else
#error "Unsupported LCD board configuration"
#endif
#endif /* (CFG_LCD_SUPPORTED == 1) && defined(CFG_BSP_ON_DISCOVERY) */

#ifndef CFG_BSP_ON_SEQUENCER
#if (CFG_BUTTON_SUPPORTED == 1)
/* Push Button B1 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B1             TASK_STACK_SIZE_BUTTON_Bx
#define TASK_PRIO_BUTTON_B1                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B1                 TASK_PREEMP_BUTTON_Bx

/* Push Button B2 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B2             TASK_STACK_SIZE_BUTTON_Bx
#define TASK_PRIO_BUTTON_B2                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B2                 TASK_PREEMP_BUTTON_Bx

/* Push Button B3 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B3             TASK_STACK_SIZE_BUTTON_Bx
#define TASK_PRIO_BUTTON_B3                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B3                 TASK_PREEMP_BUTTON_Bx

#endif /* (CFG_BUTTON_SUPPORTED == 1) */
#if (CFG_JOYSTICK_SUPPORTED == 1)
/* Joystick Sample Management Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_SAMPLE       TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_SAMPLE             TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_SAMPLE           TASK_PREEMP_JOYSTICK_x

/* Push Joystick Up Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_UP           TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_UP                 TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_UP               TASK_PREEMP_JOYSTICK_x

/* Push Joystick Right Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_RIGHT        TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_RIGHT              TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_RIGHT            TASK_PREEMP_JOYSTICK_x

/* Push Joystick Down Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_DOWN         TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_DOWN               TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_DOWN             TASK_PREEMP_JOYSTICK_x

/* Push Joystick Left Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_LEFT         TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_LEFT               TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_LEFT             TASK_PREEMP_JOYSTICK_x

/* Push Joystick Select Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_SELECT       TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_SELECT             TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_SELECT           TASK_PREEMP_JOYSTICK_x

/* Push Joystick None Task related defines */
#define TASK_STACK_SIZE_JOYSTICK_NONE         TASK_STACK_SIZE_JOYSTICK_x
#define TASK_PRIO_JOYSTICK_NONE               TASK_PRIO_JOYSTICK_x
#define TASK_PREEMP_JOYSTICK_NONE             TASK_PREEMP_JOYSTICK_x

#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
#endif /* CFG_BSP_ON_SEQUENCER */

#if (CFG_JOYSTICK_SUPPORTED == 1)
#ifdef CFG_BSP_ON_SEQUENCER
#define JOYSTICK_PRESS_SAMPLE_MS              (100u)    /* Sample Joystick level rate in milli seconds. */
#define JOYSTICK_LONG_PRESS_THRESHOLD_MS      (500u)    /* Long pression time threshold in milliseconds. */
#else /* CFG_BSP_ON_SEQUENCER */
#define JOYSTICK_PRESS_SAMPLE_MS              (200u)    /* Sample Joystick level rate in milli seconds. */
#define JOYSTICK_LONG_PRESS_THRESHOLD_MS      (800u)    /* Long pression time threshold in milliseconds. */
#endif /* CFG_BSP_ON_SEQUENCER */

#ifndef CFG_JOYSTICK_USE_TYPE
#define CFG_JOYSTICK_USE_TYPE                 JOYSTICK_USE_AS_BUTTON  /* By Default Joystick used same as Button */
#endif /* CFG_JOYSTICK_USE_TYPE */
#ifndef CFG_JOYSTICK_MODE
#define CFG_JOYSTICK_MODE                     JOY_MODE_EXTI  /* Default Joystick Mode */
#endif /* CFG_JOYSTICK_MODE */

/* Joystick is supported only on Discovery DK boards. */
#if defined(CFG_BSP_ON_DISCOVERY) && defined(STM32WBA6xxx)
#define APP_BSP_JOYSTICK_WAKEUP_FLAG          PWR_WAKEUP_PIN5
#define APP_BSP_JOYSTICK_WAKEUP_CALLBACK      HAL_PWR_WKUP5_Callback
#define APP_BSP_JOYSTICK_ENABLE_WAKEUP()      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN5_LOW_1)  /* JOY-PA3 */
#define APP_BSP_JOYSTICK_STATE_LIST_INIT      { JOY_RIGHT, JOY_UP, JOY_DOWN, JOY_LEFT, JOY_SEL }
#elif defined(CFG_BSP_ON_DISCOVERY) && defined(STM32WBA5xxx)
#define APP_BSP_JOYSTICK_WAKEUP_FLAG          PWR_WAKEUP_PIN3
#define APP_BSP_JOYSTICK_WAKEUP_CALLBACK      HAL_PWR_WKUP3_Callback
#define APP_BSP_JOYSTICK_ENABLE_WAKEUP()      HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN3_LOW_1)  /* JOY-PA1 */
#define APP_BSP_JOYSTICK_STATE_LIST_INIT      { JOY_DOWN, JOY_RIGHT, JOY_LEFT, JOY_UP, JOY_SEL }
#else
#error "Unsupported joystick board configuration"
#endif
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

/* Private macros ------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)

static ButtonBoardContext_t s_buttonBoard =
{
  .buttonDesc    = APP_BSP_BUTTON_DESC_INIT,
  .buttons       = APP_BSP_BUTTON_HWCFG_INIT
};

#endif /* (CFG_BUTTON_SUPPORTED == 1) */

#if (CFG_JOYSTICK_SUPPORTED == 1)
  /* The ADC value gives the pad pressed. It depends on the DK board
  WBA55-DK1   WBA65-DK
  JOY_NONE    JOY_NONE  -> around 3,3V  -> ADC value around 4095.
  JOY_DOWN    JOY_RIGHT -> around 2.6V  -> ADC value around 3288.
  JOY_RIGHT   JOY_UP    -> around 2.0V  -> ADC value around 2494.
  JOY_LEFT    JOY_DOWN  -> around 1.3V  -> ADC value around 1638.
  JOY_UP      JOY_LEFT  -> around 0.6V  -> ADC value around 831.
  JOY_SEL     JOY_SEL   -> around 0V    -> ADC value around 0. */
static const JOYPin_TypeDef eJoystickStateList[] = APP_BSP_JOYSTICK_STATE_LIST_INIT;
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

#ifdef CFG_BSP_ON_FREERTOS
#if (CFG_JOYSTICK_SUPPORTED == 1)
/* FreeRtos Joystick Sample stack attributes */
const osThreadAttr_t JoystickSampleThreadAttributes =
{
  .name         = "Joystick Sample Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_SAMPLE,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_SAMPLE
};

/* FreeRtos Joystick Up stack attributes */
const osThreadAttr_t JoystickUpThreadAttributes =
{
  .name         = "Joystick Up Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_UP,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_UP
};

/* FreeRtos Joystick Right stack attributes */
const osThreadAttr_t JoystickRightThreadAttributes =
{
  .name         = "Joystick Right Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_RIGHT,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_RIGHT
};

/* FreeRtos Joystick Down stack attributes */
const osThreadAttr_t JoystickDownThreadAttributes =
{
  .name         = "Joystick Down Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_DOWN,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_DOWN
};

/* FreeRtos Joystick Left stack attributes */
const osThreadAttr_t JoystickLeftThreadAttributes =
{
  .name         = "Joystick Left Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_LEFT,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_LEFT
};

/* FreeRtos Joystick Select stack attributes */
const osThreadAttr_t JoystickSelectThreadAttributes =
{
  .name         = "Joystick Select Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_SELECT,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_SELECT
};

/* FreeRtos Joystick None stack attributes */
const osThreadAttr_t JoystickNoneThreadAttributes =
{
  .name         = "Joystick None Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_JOYSTICK_NONE,
  .stack_size   = TASK_STACK_SIZE_JOYSTICK_NONE
};

#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */
#if (CFG_BUTTON_SUPPORTED == 1)

/* FreeRtos PushButton B1 stacks attributes */
const osThreadAttr_t ButtonB1ThreadAttributes =
{
  .name         = "PushButton B1 Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_BUTTON_B1,
  .stack_size   = TASK_STACK_SIZE_BUTTON_B1
};

/* FreeRtos PushButton B2 stacks attributes */
const osThreadAttr_t ButtonB2ThreadAttributes =
{
  .name         = "PushButton B2 Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_BUTTON_B2,
  .stack_size   = TASK_STACK_SIZE_BUTTON_B2
};

/* FreeRtos PushButton B3 stacks attributes */
const osThreadAttr_t ButtonB3ThreadAttributes =
{
  .name         = "PushButton B3 Thread",
  .attr_bits    = TASK_DEFAULT_ATTR_BITS,
  .cb_mem       = TASK_DEFAULT_CB_MEM,
  .cb_size      = TASK_DEFAULT_CB_SIZE,
  .stack_mem    = TASK_DEFAULT_STACK_MEM,
  .priority     = TASK_PRIO_BUTTON_B3,
  .stack_size   = TASK_STACK_SIZE_BUTTON_B3
};
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
#endif /* CFG_BSP_ON_FREERTOS */

/* Private variables ---------------------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
/* Button management */
#ifdef CFG_BSP_ON_THREADX
TX_SEMAPHORE                ButtonB1Semaphore, ButtonB2Semaphore, ButtonB3Semaphore;
static TX_THREAD            ButtonB1Thread, ButtonB2Thread, ButtonB3Thread;
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_FREERTOS
osSemaphoreId_t             ButtonB1Semaphore, ButtonB2Semaphore, ButtonB3Semaphore;
static osThreadId_t         ButtonB1Thread, ButtonB2Thread, ButtonB3Thread;
#endif /* CFG_BSP_ON_FREERTOS */
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

#if (CFG_JOYSTICK_SUPPORTED == 1)
/* Joystick management */
static UTIL_TIMER_Object_t  stJoystickTimer;
static JOYPin_TypeDef       eJoystickPreviousState = JOY_NONE;
static uint8_t              cJoystickLongPressed, cJoystickInitialPress;
#ifdef CFG_BSP_ON_THREADX
TX_SEMAPHORE                JoystickUpSemaphore, JoystickRightSemaphore, JoystickDownSemaphore, JoystickLeftSemaphore, JoystickSelectSemaphore, JoystickNoneSemaphore;
static TX_SEMAPHORE         JoystickSampleSemaphore;
static TX_THREAD            JoystickSampleThread, JoystickUpThread, JoystickRightThread, JoystickDownThread, JoystickLeftThread, JoystickSelectThread, JoystickNoneThread;
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_FREERTOS
osSemaphoreId_t             JoystickUpSemaphore, JoystickRightSemaphore, JoystickDownSemaphore, JoystickLeftSemaphore, JoystickSelectSemaphore, JoystickNoneSemaphore;
static osSemaphoreId_t      JoystickSampleSemaphore;
static osThreadId_t         JoystickSampleThread, JoystickUpThread, JoystickRightThread, JoystickDownThread, JoystickLeftThread, JoystickSelectThread, JoystickNoneThread;
#endif /* CFG_BSP_ON_FREERTOS */
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

/* Global variables ----------------------------------------------------------*/

/* Private functions prototypes-----------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_TriggerActions         ( void * arg );
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
#if (CFG_JOYSTICK_SUPPORTED == 1)
static void APP_BSP_JoystickTimerCallback ( void *arg );
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

/* External variables --------------------------------------------------------*/

/* Functions Definition ------------------------------------------------------*/
/**
 * @brief   Initialisation of all used BSP and their Task if needed.
 */
void APP_BSP_Init( void )
{
#if (CFG_LED_SUPPORTED == 1)
  APP_BSP_LedInit();
#endif /* (CFG_LED_SUPPORTED == 1) */

#if (CFG_LCD_SUPPORTED == 1)
  APP_BSP_LcdInit();
#endif /* (CFG_LCD_SUPPORTED == 1) */

#if (CFG_JOYSTICK_SUPPORTED == 1)
  APP_BSP_JoystickInit();
#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

#if (CFG_BUTTON_SUPPORTED == 1)
  APP_BSP_ButtonInit();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
}

/**
 * @brief   Verify if Wakeup is not done by a button (or Joystick)
 */
void APP_BSP_PostIdle( void )
{
#if ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U))
#if (CFG_BUTTON_SUPPORTED == 1)
  /* Treatment of WakeUp Button */
  if ( ( PWR->WUSR & PWR_WAKEUP_PIN2 ) != 0 )
  {
    PWR->WUSCR = PWR_WAKEUP_PIN2;
    BSP_PB_Callback(APP_BSP_BUTTON_WKUP2_BUTTON);
  }
#ifdef APP_BSP_BUTTON_WKUP3_BUTTON
  if ( ( PWR->WUSR & PWR_WAKEUP_PIN3 ) != 0 )
  {
    PWR->WUSCR = PWR_WAKEUP_PIN3;
    BSP_PB_Callback(APP_BSP_BUTTON_WKUP3_BUTTON);
  }
#endif
#ifdef APP_BSP_BUTTON_WKUP4_BUTTON
  if ( ( PWR->WUSR & PWR_WAKEUP_PIN4 ) != 0 )
  {
    PWR->WUSCR = PWR_WAKEUP_PIN4;
    BSP_PB_Callback(APP_BSP_BUTTON_WKUP4_BUTTON);
  }
#endif
#ifdef APP_BSP_BUTTON_WKUP5_BUTTON
  if ( ( PWR->WUSR & PWR_WAKEUP_PIN5 ) != 0 )
  {
    PWR->WUSCR = PWR_WAKEUP_PIN5;
    BSP_PB_Callback(APP_BSP_BUTTON_WKUP5_BUTTON);
  }
#endif
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
#if (CFG_JOYSTICK_SUPPORTED == 1)
  /* Treatment of WakeUp Joystick */
  if ( ( PWR->WUSR & APP_BSP_JOYSTICK_WAKEUP_FLAG ) != 0 )
  {
    PWR->WUSCR = APP_BSP_JOYSTICK_WAKEUP_FLAG;
    APP_BSP_JoystickTimerCallback(NULL);
  }
#endif /* CFG_JOYSTICK_SUPPORTED */
#endif /* ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U)) */
}

/**
 * @brief   Re-Initialisation of all used BSP after a StandBy.
 */
void APP_BSP_StandbyExit( void )
{
#if (CFG_LED_SUPPORTED == 1)
  /* Leds Initialization */
  for (uint32_t i = 0; i < LEDn; i++)
  {
    BSP_LED_Init((Led_TypeDef)i);
  }
#endif /* CFG_LED_SUPPORTED */

#if (CFG_LCD_SUPPORTED == 1)
  HAL_GPIO_WritePin(LCD_RST_GPIO_PORT, LCD_RST_PIN, GPIO_PIN_SET);

  /* GPIO are reinitialized after wakeup from stdby */
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin     = LCD_RST_PIN;
  GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull    = GPIO_NOPULL;
  GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_RST_GPIO_PORT, &GPIO_InitStruct);

  APP_BSP_LCD_ENABLE_RST_RETENTION();

  GPIO_InitStruct.Pin     = LCD_DC_PIN;
  HAL_GPIO_Init(LCD_DC_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin     = LCD_CS_PIN;
  HAL_GPIO_Init(LCD_CS_GPIO_PORT, &GPIO_InitStruct);
#endif /* (CFG_LCD_SUPPORTED == 1) */

#if (CFG_BUTTON_SUPPORTED == 1)
  /* Button HW Initialization */
  for (uint32_t i = 0; i < BUTTON_NB_MAX; i++)
  {
    BSP_PB_Init( s_buttonBoard.buttons[i].button,
                 s_buttonBoard.buttons[i].mode );
  }

  HAL_NVIC_SetPriority(WKUP_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(WKUP_IRQn);
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
}

#if ( CFG_BUTTON_SUPPORTED == 1 )

/**
 * @brief   Indicate if the selected button was pressed during a 'long time' or not.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APP_BSP_ButtonIsLongPressed( uint16_t btnIdx )
{
  uint8_t pressStatus = 0;

  if ( btnIdx < BUTTON_NB_MAX )
  {
    pressStatus = s_buttonBoard.buttonDesc[btnIdx].longPressed;
    s_buttonBoard.buttonDesc[btnIdx].longPressed = 0;
  }

  return pressStatus;
}

/**
 * @brief   'Manually' set the selected button as pressed during a 'long time'.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 */
void APP_BSP_SetButtonIsLongPressed( uint16_t btnIdx )
{
  if ( btnIdx < BUTTON_NB_MAX )
  {
    s_buttonBoard.buttonDesc[btnIdx].longPressed = 1;
  }
}

/**
 * @brief  Action of button 1 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_Button1Action( void )
{
}

/**
 * @brief  Action of button 2 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_Button2Action( void )
{
}

/**
 * @brief  Action of button 3 when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_Button3Action( void )
{
}

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */
#if ( CFG_JOYSTICK_SUPPORTED == 1 )

/**
 * @brief   In UseCase '1' & '3', indicate if the current Joystick was pressed during a 'long time' or not.
 *
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APP_BSP_JoystickIsLongPressed( void )
{
  return cJoystickLongPressed;
}

/**
 * @brief   In UseCase '1' & '3', indicate if the current Joystick was pressed during a 'Short time' or not.
 *
 * @return  '1' if pressed during a 'Short time', else '0'.
 */
uint8_t APP_BSP_JoystickIsShortReleased( void )
{
  if ( cJoystickLongPressed == 0u )
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

/**
 * @brief   In UseCase '1' & '3', indicate if the current Joystick was just pressed or not.
 *
 * @return  '1' if just pressed, else '0'.
 */
uint8_t APP_BSP_JoystickIsInitialPress( void )
{
  return cJoystickInitialPress;
}

/**
 * @brief  Action of Joystick UP when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickUpAction( void )
{
}

/**
 * @brief  Action of Joystick RIGHT when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickRightAction( void )
{
}

/**
 * @brief  Action of Joystick DOWN when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickDownAction( void )
{
}

/**
 * @brief  Action of Joystick LEFT when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickLeftAction( void )
{
}

/**
 * @brief  Action of Joystick SELECT when pressed, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickSelectAction( void )
{
}

/**
 * @brief  Action of Joystick when Joystick return to NONE after an Action, to be implemented by user.
 * @param  None
 * @retval None
 */
__WEAK void APP_BSP_JoystickNoneAction( void )
{
}

#endif /* ( CFG_JOYSTICK_SUPPORTED == 1 ) */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

#if ( CFG_LED_SUPPORTED == 1 )

/**
 * @brief  Initialisation of all LED on used boards.
 */
void APP_BSP_LedInit( void )
{
  for (uint32_t i = 0; i < LEDn; i++)
  {
    BSP_LED_Init((Led_TypeDef)i);
  }
}

#endif /* (CFG_LED_SUPPORTED == 1) */

#if (CFG_LCD_SUPPORTED == 1)

/**
 * @brief  Initialisation of the LCD screen on used Discovery board.
 */
void APP_BSP_LcdInit( void )
{
  int32_t   iStatus;

  /* LCD Initialisation */
  iStatus = BSP_LCD_Init( LCD1, LCD_ORIENTATION_LANDSCAPE );
  if ( iStatus == BSP_ERROR_NONE )
  {
    iStatus = BSP_LCD_DisplayOn( LCD1 );
  }

  if ( iStatus == BSP_ERROR_NONE )
  {
    /* LCD Management Initialisation */
    UTIL_LCD_SetFuncDriver( &LCD_Driver );

    /* Clear the Background Layer */
    UTIL_LCD_Clear( LCD_COLOR_BLACK );

    /* Select font and Color */
    UTIL_LCD_SetFont( &Font12 );
    UTIL_LCD_SetBackColor( LCD_COLOR_BLACK );
    UTIL_LCD_SetTextColor( LCD_COLOR_WHITE );
  }
}

#endif /* (CFG_LCD_SUPPORTED == 1) */
#if (CFG_JOYSTICK_SUPPORTED == 1)

/**
 * @brief  Launch the Task selected by the joystick.
 *
 * @param  joystickState  position of joystick.
 * @retval None
 */
static void Joystick_LaunchActionTask( JOYPin_TypeDef joystickState )
{
  switch ( joystickState )
  {
#ifdef CFG_BSP_ON_FREERTOS
    case JOY_UP:
        osSemaphoreRelease( JoystickUpSemaphore );
        break;

    case JOY_RIGHT:
        osSemaphoreRelease( JoystickRightSemaphore );
        break;

    case JOY_DOWN:
        osSemaphoreRelease( JoystickDownSemaphore );
        break;

    case JOY_LEFT:
        osSemaphoreRelease( JoystickLeftSemaphore );
        break;

    case JOY_SEL:
        osSemaphoreRelease( JoystickSelectSemaphore );
        break;

    case JOY_NONE:
        osSemaphoreRelease( JoystickNoneSemaphore );
        break;
#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_THREADX
    case JOY_UP:
        tx_semaphore_put( &JoystickUpSemaphore );
        break;

    case JOY_RIGHT:
        tx_semaphore_put( &JoystickRightSemaphore );
        break;

    case JOY_DOWN:
        tx_semaphore_put( &JoystickDownSemaphore );
        break;

    case JOY_LEFT:
        tx_semaphore_put( &JoystickLeftSemaphore );
        break;

    case JOY_SEL:
        tx_semaphore_put( &JoystickSelectSemaphore );
        break;

    case JOY_NONE:
        tx_semaphore_put( &JoystickNoneSemaphore );
        break;
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_SEQUENCER
    case JOY_UP:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_UP, CFG_SEQ_PRIO_0 );
        break;

    case JOY_RIGHT:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_RIGHT, CFG_SEQ_PRIO_0 );
        break;

    case JOY_DOWN:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_DOWN, CFG_SEQ_PRIO_0 );
        break;

    case JOY_LEFT:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_LEFT, CFG_SEQ_PRIO_0 );
        break;

    case JOY_SEL:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_SELECT, CFG_SEQ_PRIO_0 );
        break;

    case JOY_NONE:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_NONE, CFG_SEQ_PRIO_0 );
        break;
#endif /* CFG_BSP_ON_SEQUENCER */

    default :
        break;
  }
}

/**
  * @brief  Get Joystick state.
  * @param  JOY Joystick.
  *   This parameter can be JOY1
  * @retval BSP error code if value negative or one of following value:
  *     @arg JOY_NONE
  *     @arg JOY_SEL
  *     @arg JOY_DOWN
  *     @arg JOY_LEFT
  *     @arg JOY_RIGHT
  *     @arg JOY_UP
  */
static JOYPin_TypeDef APP_BSP_JoystickGetState( JOY_TypeDef eJoy )
{
  JOYPin_TypeDef  eJoyPin;
  uint16_t        iKeyConvertedValue;

  /* Get the converted value of regular channel */
  iKeyConvertedValue = HAL_ADC_GetValue( &hjoy_adc[eJoy] );

  /* The ADC value gives the pad pressed. Function of DK */
  if ( ( iKeyConvertedValue >= 2800U ) && ( iKeyConvertedValue < 3600U ) )
  {
    eJoyPin = eJoystickStateList[0];
  }
  else if ( ( iKeyConvertedValue >= 2000U ) && ( iKeyConvertedValue < 2800U ) )
  {
    eJoyPin = eJoystickStateList[1];
  }
  else if ( ( iKeyConvertedValue >= 1200U ) && ( iKeyConvertedValue < 2000U ) )
  {
    eJoyPin = eJoystickStateList[2];
  }
  else if ( ( iKeyConvertedValue >= 400U ) && ( iKeyConvertedValue < 1200U ) )
  {
    eJoyPin = eJoystickStateList[3];
  }
  else if ( iKeyConvertedValue < 400U )
  {
    eJoyPin = eJoystickStateList[4];
  }
  else
  {
    eJoyPin = JOY_NONE;
  }

  return eJoyPin;
}

/**
 *
 */
static void APP_BSP_JoystickSampleManage( void )
{
  JOYPin_TypeDef          eJoystickState = JOY_NONE;
#if (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_BUTTON_WITH_TIME) || (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER)
  static bool             bActionDone = false;
  static uint32_t         lStartTime = 0;
#endif /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_BUTTON_WITH_TIME) || (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER) */

  /* Init, Sample & DeInit Joystick */
  BSP_JOY_Init( JOY1, CFG_JOYSTICK_MODE, JOY_ALL );

#if (CFG_JOYSTICK_MODE == JOY_MODE_POLLING)
  /* Start the conversion process */
  HAL_ADC_Start(&hjoy_adc[JOY1]);
#endif /* (CFG_JOYSTICK_MODE == JOY_MODE_POLLING) */

  /* Wait first conversion */
  if ( HAL_ADC_PollForConversion( &hjoy_adc[JOY1], ( JOYSTICK_PRESS_SAMPLE_MS / 2u ) ) == HAL_OK )
  {
    eJoystickState = APP_BSP_JoystickGetState( JOY1 );

    BSP_JOY_DeInit(JOY1, JOY_ALL);

#if (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_BUTTON_WITH_TIME) || (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER)
    /* Process Joystick information. */
    if ( ( eJoystickState != JOY_NONE ) && ( eJoystickState == eJoystickPreviousState ) )
    {
      /* Verify if it's a LongTime */
      if ( bActionDone == false )
      {
        lStartTime += JOYSTICK_PRESS_SAMPLE_MS;
        if ( lStartTime > JOYSTICK_LONG_PRESS_THRESHOLD_MS )
        {
          cJoystickLongPressed = 1;
          bActionDone = true;
          cJoystickInitialPress = 0;
          Joystick_LaunchActionTask( eJoystickState );
        }
#if (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER)
        else
        {
          if ( cJoystickInitialPress == 0 )
          {
            cJoystickInitialPress = 1;
            Joystick_LaunchActionTask( eJoystickState );
          }
        }
#endif /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER) */
      }
    }
    else
    {
      if ( ( eJoystickState == JOY_NONE ) && ( eJoystickState != eJoystickPreviousState ) )
      {
        /* Verify if it's a 'Short Pressed' case */
        if ( ( lStartTime != 0u ) && ( bActionDone == false ) )
        {
          cJoystickInitialPress = 0;
          Joystick_LaunchActionTask( eJoystickPreviousState );
        }
      }
      else
      {
        cJoystickLongPressed = 0;
        cJoystickInitialPress = 0;
        bActionDone = false;
        lStartTime = 0;
      }
    }
#else /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_BUTTON_WITH_TIME) || (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER) */
    /* Process Joystick information. */
#if (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_CHANGE)
    if ( eJoystickState != eJoystickPreviousState )
#else
#if (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_JOYSTICK)
    if ( eJoystickState != JOY_NONE )
#else
    if ( ( eJoystickState != JOY_NONE ) && ( eJoystickState != eJoystickPreviousState ) )
#endif /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_JOYSTICK) */
#endif /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_CHANGE) */
    {
      Joystick_LaunchActionTask( eJoystickState );
    }
#endif /* (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_BUTTON_WITH_TIME) || (CFG_JOYSTICK_USE_TYPE == JOYSTICK_USE_AS_MATTER) */

    /* Set State */
    eJoystickPreviousState = eJoystickState;
  }
  else
  {
    BSP_JOY_DeInit(JOY1, JOY_ALL);
  }

  /* Restart Timer if needed */
#if (((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U)) && (CFG_JOYSTICK_USE_TYPE != JOYSTICK_USE_AS_CHANGE))
  if ( eJoystickState != JOY_NONE )
  {
    UTIL_TIMER_Start( &stJoystickTimer );
  }
#else
  UTIL_TIMER_Start( &stJoystickTimer );
#endif
}

/**
 *
 */
static void APP_BSP_JoystickTimerCallback(void *arg)
{
  UNUSED(arg);

#ifdef CFG_BSP_ON_SEQUENCER
  UTIL_SEQ_SetTask( 1U << CFG_TASK_BSP_JOY_SAMPLE, CFG_SEQ_PRIO_0);
#endif /* CFG_BSP_ON_SEQUENCER */
#ifdef CFG_BSP_ON_FREERTOS
  osSemaphoreRelease( JoystickSampleSemaphore );
#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_THREADX
  tx_semaphore_put( &JoystickSampleSemaphore );
#endif /* CFG_BSP_ON_THREADX */
}

#ifdef CFG_BSP_ON_FREERTOS

/**
 * @brief  Management of the Joystick Sample task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickSampleTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickSampleSemaphore, osWaitForever );
    APP_BSP_JoystickSampleManage();
  }
}

/**
 * @brief  Management of the Joystick Up task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickUpTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickUpSemaphore, osWaitForever );
    APP_BSP_JoystickUpAction();
  }
}

/**
 * @brief  Management of the Joystick Right task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickRightTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickRightSemaphore, osWaitForever );
    APP_BSP_JoystickRightAction();
  }
}

/**
 * @brief  Management of the Joystick Down task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickDownTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickDownSemaphore, osWaitForever );
    APP_BSP_JoystickDownAction();
  }
}

/**
 * @brief  Management of the Joystick Left task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickLeftTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickLeftSemaphore, osWaitForever );
    APP_BSP_JoystickLeftAction();
  }
}

/**
 * @brief  Management of the Joystick Select task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickSelectTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickSelectSemaphore, osWaitForever );
    APP_BSP_JoystickSelectAction();
  }
}

/**
 * @brief  Management of the Joystick None task
 * @param  argument  Not used.
 * @retval None
 */
static void JoystickNoneTask( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( JoystickNoneSemaphore, osWaitForever );
    APP_BSP_JoystickNoneAction();
  }
}

/**
 * @brief  Initialisation of the Joystick Tasks & Semaphores
 */
static void Joystick_InitTask( void )
{
  /* Register Semaphore & Task for the Joystick Sample Management Semaphore */
  JoystickSampleSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickSampleThread = osThreadNew( JoystickSampleTask, NULL, &JoystickSampleThreadAttributes );

  /* Register Semaphore & Task for the Joystick Up Semaphore */
  JoystickUpSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickUpThread = osThreadNew( JoystickUpTask, NULL, &JoystickUpThreadAttributes );

  /* Register Semaphore & Task for the Joystick Right Semaphore */
  JoystickRightSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickRightThread = osThreadNew( JoystickRightTask, NULL, &JoystickRightThreadAttributes );

  /* Register Semaphore & Task for the Joystick Down Semaphore */
  JoystickDownSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickDownThread = osThreadNew( JoystickDownTask, NULL, &JoystickDownThreadAttributes );

  /* Register Semaphore & Task for the Joystick Left Semaphore */
  JoystickLeftSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickLeftThread = osThreadNew( JoystickLeftTask, NULL, &JoystickLeftThreadAttributes );

  /* Register Semaphore & Task for the Joystick Select Semaphore */
  JoystickSelectSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickSelectThread = osThreadNew( JoystickSelectTask, NULL, &JoystickSelectThreadAttributes );

  /* Register Semaphore & Task for the Joystick None Semaphore */
  JoystickNoneSemaphore = osSemaphoreNew( 1, 0, NULL );
  JoystickNoneThread = osThreadNew( JoystickNoneTask, NULL, &JoystickNoneThreadAttributes );

  if ( ( JoystickSampleSemaphore == NULL ) || ( JoystickUpSemaphore == NULL ) || ( JoystickRightSemaphore == NULL ) || ( JoystickDownSemaphore == NULL )
      || ( JoystickLeftSemaphore == NULL ) || ( JoystickSelectSemaphore == NULL ) || ( JoystickNoneSemaphore == NULL ) )
  {
    LOG_ERROR_APP( "FreeRtos : Error during creation of Semaphore for Joystick" );
    while(1);
  }

  if ( ( JoystickSampleThread == NULL ) || ( JoystickUpThread == NULL ) || ( JoystickRightThread == NULL ) || ( JoystickDownThread == NULL )
      || ( JoystickLeftThread == NULL ) || ( JoystickSelectThread == NULL ) || ( JoystickNoneThread == NULL ) )
  {
    LOG_ERROR_APP( "FreeRtos : Error during creation of Task for Joystick" );
    while(1);
  }
}

#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_THREADX

/**
 * @brief  Management of the Joystick Sample task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickSampleTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickSampleSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickSampleManage();
  }
}

/**
 * @brief  Management of the Joystick Up task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickUpTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickUpSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickUpAction();
  }
}

/**
 * @brief  Management of the Joystick Right task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickRightTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickRightSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickRightAction();
  }
}

/**
 * @brief  Management of the Joystick Down task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickDownTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickDownSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickDownAction();
  }
}

/**
 * @brief  Management of the Joystick Left task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickLeftTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickLeftSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickLeftAction();
  }
}

/**
 * @brief  Management of the Joystick Select task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickSelectTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickSelectSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickSelectAction();
  }
}

/**
 * @brief  Management of the Joystick None task
 * @param  lArgument  Not used.
 * @retval None
 */
static void JoystickNoneTask( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &JoystickNoneSemaphore, TX_WAIT_FOREVER );
    APP_BSP_JoystickNoneAction();
  }
}

/**
 * @brief  Initialisation of the Joystick Tasks & Semaphores
 */
static void Joystick_InitTask( void )
{
  UINT    ThreadXStatus;
  CHAR    *pStack;

  /* Register Semaphore to launch the Joystick Sample Task */
  ThreadXStatus = tx_semaphore_create( &JoystickSampleSemaphore, "JoystickSample Semaphore", 0 );
  ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_SAMPLE, TX_NO_WAIT);
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &JoystickSampleThread, "JoystickSample Thread", JoystickSampleTask, 0, pStack,
                                      TASK_STACK_SIZE_JOYSTICK_SAMPLE, TASK_PRIO_JOYSTICK_SAMPLE, TASK_PREEMP_JOYSTICK_SAMPLE,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Register Semaphore to launch the Joystick Up Task */
  ThreadXStatus = tx_semaphore_create( &JoystickUpSemaphore, "JoystickUp Semaphore", 0 );
  ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_UP, TX_NO_WAIT);
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &JoystickUpThread, "JoystickUp Thread", JoystickUpTask, 0, pStack,
                                      TASK_STACK_SIZE_JOYSTICK_UP, TASK_PRIO_JOYSTICK_UP, TASK_PREEMP_JOYSTICK_UP,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Register Semaphore to launch the Joystick Right Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &JoystickRightSemaphore, "JoystickRight Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_RIGHT, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &JoystickRightThread, "JoystickRight Thread", JoystickRightTask, 0, pStack,
                                        TASK_STACK_SIZE_JOYSTICK_RIGHT, TASK_PRIO_JOYSTICK_RIGHT, TASK_PREEMP_JOYSTICK_RIGHT,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Register Semaphore to launch the Joystick Down Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &JoystickDownSemaphore, "JoystickDown Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_DOWN, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &JoystickDownThread, "JoystickDown Thread", JoystickDownTask, 0, pStack,
                                        TASK_STACK_SIZE_JOYSTICK_DOWN, TASK_PRIO_JOYSTICK_DOWN, TASK_PREEMP_JOYSTICK_DOWN,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Register Semaphore to launch the Joystick Left Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &JoystickLeftSemaphore, "JoystickLeft Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_LEFT, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &JoystickLeftThread, "JoystickLeft Thread", JoystickLeftTask, 0, pStack,
                                        TASK_STACK_SIZE_JOYSTICK_LEFT, TASK_PRIO_JOYSTICK_LEFT, TASK_PREEMP_JOYSTICK_LEFT,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Register Semaphore to launch the Joystick Select Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &JoystickSelectSemaphore, "JoystickSelect Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_SELECT, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &JoystickSelectThread, "JoystickSelect Thread", JoystickSelectTask, 0, pStack,
                                        TASK_STACK_SIZE_JOYSTICK_SELECT, TASK_PRIO_JOYSTICK_SELECT, TASK_PREEMP_JOYSTICK_SELECT,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Register Semaphore to launch the Joystick None Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &JoystickNoneSemaphore, "JoystickNone Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_JOYSTICK_NONE, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &JoystickNoneThread, "JoystickNone Thread", JoystickNoneTask, 0, pStack,
                                        TASK_STACK_SIZE_JOYSTICK_NONE, TASK_PRIO_JOYSTICK_NONE, TASK_PREEMP_JOYSTICK_NONE,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ThreadX : Error during the creation of Semaphore & Task for Joystick (0x%04X)", ThreadXStatus );
    while(1);
  }
}

#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_SEQUENCER

/**
 * @brief  Initialisation of the Joystick Tasks & Semaphores
 */
static void Joystick_InitTask( void)
{
  /* Task to sample management */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_SAMPLE, UTIL_SEQ_RFU, APP_BSP_JoystickSampleManage );

  /* Task associated with Joystick */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_UP, UTIL_SEQ_RFU, APP_BSP_JoystickUpAction );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_RIGHT, UTIL_SEQ_RFU, APP_BSP_JoystickRightAction );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_DOWN, UTIL_SEQ_RFU, APP_BSP_JoystickDownAction );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_LEFT, UTIL_SEQ_RFU, APP_BSP_JoystickLeftAction );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_SELECT, UTIL_SEQ_RFU, APP_BSP_JoystickSelectAction );
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BSP_JOY_NONE, UTIL_SEQ_RFU, APP_BSP_JoystickNoneAction );
}

#endif /* CFG_BSP_ON_SEQUENCER */

/**
 * @brief  Initialisation of the Joystick
 */
void APP_BSP_JoystickInit( void )
{
  /* Joystick task initialisation */
  Joystick_InitTask();
  cJoystickLongPressed = 0;
  cJoystickInitialPress = 0;

#if ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U))
  /* StandBy WakeUp via Joystick */
  APP_BSP_JOYSTICK_ENABLE_WAKEUP();
#endif /* ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U)) */

  /* Create periodic timer for joystick position reading */
  UTIL_TIMER_Create( &stJoystickTimer, JOYSTICK_PRESS_SAMPLE_MS, UTIL_TIMER_ONESHOT, &APP_BSP_JoystickTimerCallback, 0 );
  UTIL_TIMER_Start( &stJoystickTimer );
}

#if ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U))
/**
 * @brief  Joystick wakeup pin event while in Run/Sleep/Stop0/Stop1 (WKUP_IRQn clears the flag before APP_BSP_PostIdle).
 */
void APP_BSP_JOYSTICK_WAKEUP_CALLBACK( void )
{
  APP_BSP_JoystickTimerCallback( NULL );
}
#endif /* ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U)) */

#endif /* (CFG_JOYSTICK_SUPPORTED == 1) */

#if ( CFG_BUTTON_SUPPORTED == 1 )
#ifdef CFG_BSP_ON_FREERTOS

/**
 * @brief  Management of the B1 pushbutton task
 * @param  argument  Not used.
 * @retval None
 */
static void ButtonB1Task( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( ButtonB1Semaphore, osWaitForever );
    APP_BSP_Button1Action();
  }
}

/**
 * @brief  Management of the B2 pushbutton task
 * @param  argument  Not used.
 * @retval None
 */
static void ButtonB2Task( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( ButtonB2Semaphore, osWaitForever );
    APP_BSP_Button2Action();
  }
}

/**
 * @brief  Management of the B3 pushbutton task
 * @param  argument  Not used.
 * @retval None
 */
static void ButtonB3Task( void * argument )
{
  UNUSED( argument );

  for(;;)
  {
    osSemaphoreAcquire( ButtonB3Semaphore, osWaitForever );
    APP_BSP_Button3Action();
  }
}

static void Button_InitTask( void )
{
  /* Register Semaphore & Thread to launch the pushbutton B1 Task */
  ButtonB1Semaphore = osSemaphoreNew( 1, 0, NULL );
  ButtonB1Thread = osThreadNew( ButtonB1Task, NULL, &ButtonB1ThreadAttributes );

  /* Register Semaphore & Thread to launch the pushbutton B2 Task */
  ButtonB2Semaphore = osSemaphoreNew( 1, 0, NULL );
  ButtonB2Thread = osThreadNew( ButtonB2Task, NULL, &ButtonB2ThreadAttributes );

  /* Register Semaphore & Thread to launch the pushbutton B3 Task */
  ButtonB3Semaphore = osSemaphoreNew( 1, 0, NULL );
  ButtonB3Thread = osThreadNew( ButtonB3Task, NULL, &ButtonB3ThreadAttributes );

  if ( ( ButtonB1Semaphore == NULL ) || ( ButtonB2Semaphore == NULL ) || ( ButtonB3Semaphore == NULL ) )
  {
    LOG_ERROR_APP( "FreeRtos : Error during creation of Semaphore for Buttons" );
    while(1);
  }

  if ( ( ButtonB1Thread == NULL ) || ( ButtonB2Thread == NULL ) || ( ButtonB3Thread == NULL ) )
  {
    LOG_ERROR_APP( "FreeRtos : Error during creation of Thread for Buttons" );
    while(1);
  }
}

#endif /* CFG_BSP_ON_FREERTOS */

#ifdef CFG_BSP_ON_THREADX

/**
 * @brief  Management of the B1 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonB1Task( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &ButtonB1Semaphore, TX_WAIT_FOREVER );
    APP_BSP_Button1Action();
  }
}

/**
 * @brief  Management of the B2 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonB2Task( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &ButtonB2Semaphore, TX_WAIT_FOREVER );
    APP_BSP_Button2Action();
  }
}

/**
 * @brief  Management of the B3 pushbutton task
 * @param  lArgument  Not used.
 * @retval None
 */
static void ButtonB3Task( ULONG lArgument )
{
  UNUSED( lArgument );

  for(;;)
  {
    tx_semaphore_get( &ButtonB3Semaphore, TX_WAIT_FOREVER );
    APP_BSP_Button3Action();
  }
}

static void Button_InitTask( void )
{
  UINT    ThreadXStatus;
  CHAR    *pStack;

  /* Register Semaphore to launch the pushbutton B1 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonB1Semaphore, "ButtonB1 Semaphore", 0 );
  ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B1, TX_NO_WAIT);
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &ButtonB1Thread, "ButtonB1 Thread", ButtonB1Task, 0, pStack,
                                      TASK_STACK_SIZE_BUTTON_B1, TASK_PRIO_BUTTON_B1, TASK_PREEMP_BUTTON_B1,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Register Semaphore to launch the pushbutton B2 Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &ButtonB2Semaphore, "ButtonB2 Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B2, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &ButtonB2Thread, "ButtonB2 Thread", ButtonB2Task, 0, pStack,
                                        TASK_STACK_SIZE_BUTTON_B2, TASK_PRIO_BUTTON_B2, TASK_PREEMP_BUTTON_B2,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Register Semaphore to launch the pushbutton B3 Task */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_semaphore_create( &ButtonB3Semaphore, "ButtonB3 Semaphore", 0 );
    ThreadXStatus |= tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B3, TX_NO_WAIT);
    if ( ThreadXStatus == TX_SUCCESS )
    {
      ThreadXStatus = tx_thread_create( &ButtonB3Thread, "ButtonB3 Thread", ButtonB3Task, 0, pStack,
                                        TASK_STACK_SIZE_BUTTON_B3, TASK_PRIO_BUTTON_B3, TASK_PREEMP_BUTTON_B3,
                                        TX_NO_TIME_SLICE, TX_AUTO_START );
    }
  }

  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ThreadX : Error during the creation of Semaphore & Task for Buttons (0x%04X)", ThreadXStatus );
    while(1);
  }
}

#endif /* CFG_BSP_ON_THREADX */

#ifdef CFG_BSP_ON_SEQUENCER

/**
 * @brief  Initialisation of Buttons Tasks & Semaphores
 */
static void Button_InitTask( void )
{
#ifdef APP_BSP_BUTTON_ACTION1_SEQ_TASK
  UTIL_SEQ_RegTask( 1U << APP_BSP_BUTTON_ACTION1_SEQ_TASK, UTIL_SEQ_RFU, APP_BSP_Button1Action );
#endif

#ifdef APP_BSP_BUTTON_ACTION2_SEQ_TASK
  UTIL_SEQ_RegTask( 1U << APP_BSP_BUTTON_ACTION2_SEQ_TASK, UTIL_SEQ_RFU, APP_BSP_Button2Action );
#endif

#ifdef APP_BSP_BUTTON_ACTION3_SEQ_TASK
  UTIL_SEQ_RegTask( 1U << APP_BSP_BUTTON_ACTION3_SEQ_TASK, UTIL_SEQ_RFU, APP_BSP_Button3Action );
#endif
}

#endif /* CFG_BSP_ON_SEQUENCER */

/**
 * @brief  Initialisation of Buttons
 */
void APP_BSP_ButtonInit( void )
{
  uint32_t        buttonIndex;
  Button_TypeDef  button;

  /* Buttons HW Initialization */
  for ( buttonIndex = 0; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  {
    button = s_buttonBoard.buttons[buttonIndex].button;
    BSP_PB_Init( button, s_buttonBoard.buttons[buttonIndex].mode );
  }

  APP_BSP_BUTTON_ENABLE_WAKEUP();

#if ((CFG_LPM_STOP2_SUPPORTED == 1U) || (CFG_LPM_STANDBY_SUPPORTED == 1U))
  APP_BSP_BUTTON_ENABLE_RETENTION();
#endif

  HAL_NVIC_SetPriority(WKUP_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(WKUP_IRQn);

  /* Button task initialisation */
  Button_InitTask();

  /* Button timers initialisation (one for each button) */
  for ( buttonIndex = 0; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  {
    button = s_buttonBoard.buttons[buttonIndex].button;
    UTIL_TIMER_Create( &s_buttonBoard.buttonDesc[button].longTimerId,
                       0,
                       UTIL_TIMER_PERIODIC,
                       &Button_TriggerActions,
                       &s_buttonBoard.buttonDesc[button] );
  }
}

/**
 * @brief  Launch the Task selected by the button.
 *
 * @param  button  ID of pressed button.
 * @retval None
 */
static void Button_LaunchActionTask( Button_TypeDef button )
{
  ButtonActionId_t actionId;

  if ( (uint32_t)button >= BUTTON_NB_MAX )
  {
    return;
  }

  actionId = s_buttonBoard.buttons[button].actionId;

  switch ( actionId )
  {
#ifdef CFG_BSP_ON_FREERTOS
    case BUTTON_ACTION_1:
        osSemaphoreRelease( ButtonB1Semaphore );
        break;

    case BUTTON_ACTION_2:
        osSemaphoreRelease( ButtonB2Semaphore );
        break;

    case BUTTON_ACTION_3:
        osSemaphoreRelease( ButtonB3Semaphore );
        break;
#endif /* CFG_BSP_ON_FREERTOS */

#ifdef CFG_BSP_ON_THREADX
    case BUTTON_ACTION_1:
        tx_semaphore_put( &ButtonB1Semaphore );
        break;

    case BUTTON_ACTION_2:
        tx_semaphore_put( &ButtonB2Semaphore );
        break;

    case BUTTON_ACTION_3:
        tx_semaphore_put( &ButtonB3Semaphore );
        break;
#endif /* CFG_BSP_ON_THREADX */

#ifdef CFG_BSP_ON_SEQUENCER
    case BUTTON_ACTION_1:
#ifdef APP_BSP_BUTTON_ACTION1_SEQ_TASK
        UTIL_SEQ_SetTask( 1U << APP_BSP_BUTTON_ACTION1_SEQ_TASK, CFG_SEQ_PRIO_0 );
#endif
        break;

    case BUTTON_ACTION_2:
#ifdef APP_BSP_BUTTON_ACTION2_SEQ_TASK
        UTIL_SEQ_SetTask( 1U << APP_BSP_BUTTON_ACTION2_SEQ_TASK, CFG_SEQ_PRIO_0 );
#endif
        break;

    case BUTTON_ACTION_3:
#ifdef APP_BSP_BUTTON_ACTION3_SEQ_TASK
        UTIL_SEQ_SetTask( 1U << APP_BSP_BUTTON_ACTION3_SEQ_TASK, CFG_SEQ_PRIO_0 );
#endif
        break;
#endif /* CFG_BSP_ON_SEQUENCER */

    default:
        break;
  }
}

static void Button_TriggerActions( void * arg )
{
  ButtonDesc_t    * p_buttonDesc = (ButtonDesc_t *)arg;
  int32_t         buttonState = 0;

  buttonState = BSP_PB_GetState( p_buttonDesc->button );

  /* If Button pressed and Threshold time not finish, continue waiting */
  p_buttonDesc->waitingTime += BUTTON_LONG_PRESS_SAMPLE_MS;
  if ( ( buttonState != 0u ) && ( p_buttonDesc->waitingTime < BUTTON_LONG_PRESS_THRESHOLD_MS ) )
  {
    return;
  }

  /* Save button state */
  if ( buttonState != 0u )
  {
    APP_BSP_SetButtonIsLongPressed( p_buttonDesc->button );
  }

  /* Stop Timer */
  UTIL_TIMER_Stop( &p_buttonDesc->longTimerId );

  /* Launch Task */
  Button_LaunchActionTask( p_buttonDesc->button );
}

void BSP_PB_Callback( Button_TypeDef button )
{
  s_buttonBoard.buttonDesc[button].waitingTime = 0;
  UTIL_TIMER_StartWithPeriod( &s_buttonBoard.buttonDesc[button].longTimerId, BUTTON_LONG_PRESS_SAMPLE_MS );
}

#ifdef APP_BSP_BUTTON_WKUP2_BUTTON
void HAL_PWR_WKUP2_Callback(void)
{
  BSP_PB_Callback(APP_BSP_BUTTON_WKUP2_BUTTON);
}
#endif

#ifdef APP_BSP_BUTTON_WKUP3_BUTTON
void HAL_PWR_WKUP3_Callback(void)
{
  BSP_PB_Callback(APP_BSP_BUTTON_WKUP3_BUTTON);
}
#endif

#ifdef APP_BSP_BUTTON_WKUP4_BUTTON
void HAL_PWR_WKUP4_Callback(void)
{
  BSP_PB_Callback(APP_BSP_BUTTON_WKUP4_BUTTON);
}
#endif

#ifdef APP_BSP_BUTTON_WKUP5_BUTTON
void HAL_PWR_WKUP5_Callback(void)
{
  BSP_PB_Callback(APP_BSP_BUTTON_WKUP5_BUTTON);
}
#endif

#ifdef APP_BSP_BUTTON_EXTI4_BUTTON
/**
  * @brief This function handles EXTI Line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  BSP_PB_IRQHandler(APP_BSP_BUTTON_EXTI4_BUTTON);
}
#endif

#ifdef APP_BSP_BUTTON_EXTI5_BUTTON
/**
  * @brief This function handles EXTI Line5 interrupt.
  */
void EXTI5_IRQHandler(void)
{
  BSP_PB_IRQHandler(APP_BSP_BUTTON_EXTI5_BUTTON);
}
#endif

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */

/**
 * @brief  Treat USART commands to simulate button press for instance.
 *
 * @param  pRxBuffer      Pointer on received data from USART.
 * @param  iRxBufferSize  Number of received data.
 * @retval 1 if function can be execute a command else 0.
 */
uint8_t APP_BSP_SerialCmdExecute( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  UNUSED(iRxBufferSize);

#if ( CFG_BUTTON_SUPPORTED == 1 )
  for (uint32_t buttonIndex = 0; buttonIndex < BUTTON_NB_MAX; buttonIndex++)
  {
    ButtonHwConfig_t * pButton = &s_buttonBoard.buttons[buttonIndex];

    if ( (strcmp( (char const*)pRxBuffer, pButton->cmd ) == 0) ||
         (strcmp( (char const*)pRxBuffer, pButton->cmdAlt ) == 0) )
    {
      LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
      Button_LaunchActionTask( pButton->button );
      return 1;
    }

    if ( (strcmp( (char const*)pRxBuffer, pButton->longCmd ) == 0) ||
         (strcmp( (char const*)pRxBuffer, pButton->longCmdAlt ) == 0) )
    {
      APP_BSP_SetButtonIsLongPressed( pButton->button );
      LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
      Button_LaunchActionTask( pButton->button );
      return 1;
    }
  }
#endif /* ( CFG_BUTTON_SUPPORTED == 1 ) */

#if ( CFG_JOYSTICK_SUPPORTED == 1 )
  if (strcmp( (char const*)pRxBuffer, "UP" ) == 0)
  {
    LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
    Joystick_LaunchActionTask( JOY_UP );
    return 1;
  }
  else if (strcmp( (char const*)pRxBuffer, "RIGHT" ) == 0)
  {
    LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
    Joystick_LaunchActionTask( JOY_RIGHT );
    return 1;
  }
  else if (strcmp( (char const*)pRxBuffer, "DOWN" ) == 0 )
  {
    LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
    Joystick_LaunchActionTask( JOY_DOWN );
    return 1;
  }
  else if (strcmp( (char const*)pRxBuffer, "LEFT" ) == 0)
  {
    LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
    Joystick_LaunchActionTask( JOY_LEFT );
    return 1;
  }
  else if (strcmp( (char const*)pRxBuffer, "SELECT" ) == 0)
  {
    LOG_INFO_APP( "%s pressed by Serial Command.\n", pRxBuffer );
    Joystick_LaunchActionTask( JOY_SEL );
    return 1;
  }
#endif /* ( CFG_JOYSTICK_SUPPORTED == 1 ) */

  return 0;
}