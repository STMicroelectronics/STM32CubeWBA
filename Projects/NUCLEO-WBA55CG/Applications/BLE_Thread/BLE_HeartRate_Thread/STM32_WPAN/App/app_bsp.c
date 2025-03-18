/**
  ******************************************************************************
  * @file    app_bsp.c
  * @author  MCD Application Team
  * @brief   Application to manage BSP.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifdef CFG_BSP_ON_DISCOVERY
  JOYPin_TypeDef      joyPin;
#endif /* CFG_BSP_ON_DISCOVERY */
  UTIL_TIMER_Object_t longTimerId;
  uint8_t             longPressed;
  uint32_t            waitingTime;
} ButtonDesc_t;
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* Private defines -----------------------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
#define BUTTON_LONG_PRESS_SAMPLE_MS           (50u)     /* Sample button level rate in milli seconds. */
#define BUTTON_LONG_PRESS_THRESHOLD_MS        (500u)    /* Long pression time threshold in milli seconds. */
#ifdef CFG_BSP_ON_CEB
#define BUTTON_NB_MAX                         (B2 + 1u)
#else /* CFG_BSP_ON_CEB */
#define BUTTON_NB_MAX                         (B3 + 1u)
#endif /* CFG_BSP_ON_CEB */

#ifndef CFG_BSP_ON_SEQUENCER
/* Push Button B1 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B1             RTOS_STACK_SIZE_NORMAL
#define TASK_PRIO_BUTTON_B1                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B1                 TASK_PREEMP_BUTTON_Bx

/* Push Button B2 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B2             RTOS_STACK_SIZE_NORMAL
#define TASK_PRIO_BUTTON_B2                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B2                 TASK_PREEMP_BUTTON_Bx

/* Push Button B3 Task related defines */
#define TASK_STACK_SIZE_BUTTON_B3             RTOS_STACK_SIZE_NORMAL
#define TASK_PRIO_BUTTON_B3                   TASK_PRIO_BUTTON_Bx
#define TASK_PREEMP_BUTTON_B3                 TASK_PREEMP_BUTTON_Bx
#endif /* CFG_BSP_ON_SEQUENCER */
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* Private macros ------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
#ifdef CFG_BSP_ON_FREERTOS
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

/* FreeRtos PushButton B1 stacks attributes */
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
TX_SEMAPHORE          ButtonB1Semaphore, ButtonB2Semaphore, ButtonB3Semaphore;
TX_THREAD             ButtonB1Thread, ButtonB2Thread, ButtonB3Thread;
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_FREERTOS
osSemaphoreId_t       ButtonB1Semaphore, ButtonB2Semaphore, ButtonB3Semaphore;
osThreadId_t          ButtonB1Thread, ButtonB2Thread, ButtonB3Thread;
#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_DISCOVERY
static ButtonDesc_t   buttonDesc[BUTTON_NB_MAX] = { { B1, JOY_SEL, { 0 }, 0, 0 }, { B2, JOY_LEFT, { 0 }, 0, 0  }, { B3, JOY_RIGHT, { 0 }, 0, 0 } } ;
static JOYPin_TypeDef previousJoyPin = JOY_NONE;
static uint8_t        alwaysPressJoyPin = false;
#else /* CFG_BSP_ON_DISCOVERY */
#ifdef CFG_BSP_ON_CEB
static ButtonDesc_t   buttonDesc[BUTTON_NB_MAX] = { { B2, { 0 } , 0, 0 } };
#else /* CFG_BSP_ON_CEB */
static ButtonDesc_t   buttonDesc[BUTTON_NB_MAX] = { { B1, { 0 }, 0, 0 } , { B2, { 0 } , 0, 0 }, { B3, { 0 }, 0, 0 } };
#endif /* CFG_BSP_ON_CEB */
#endif /* CFG_BSP_ON_DISCOVERY */
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

/* Global variables ----------------------------------------------------------*/

/* Private functions prototypes-----------------------------------------------*/
#if (CFG_BUTTON_SUPPORTED == 1)
static void Button_TriggerActions         ( void * arg );
#endif /* (CFG_BUTTON_SUPPORTED == 1) */

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

#ifdef CFG_BSP_ON_DISCOVERY
#if (CFG_LCD_SUPPORTED == 1)
  APP_BSP_DisplayInit();
#endif /* (CFG_LCD_SUPPORTED == 1) */
#endif /* CFG_BSP_ON_DISCOVERY */

#if (CFG_BUTTON_SUPPORTED == 1)
  APP_BSP_ButtonInit();
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
}

/**
 * @brief   Re-Initialisation of all used BSP after a StandBy.
 */
void APP_BSP_StandbyExit( void )
{
#if (CFG_LED_SUPPORTED == 1)
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE);
#ifdef CFG_BSP_ON_NUCLEO
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
#endif /* CFG_BSP_ON_NUCLEO */
#endif /* (CFG_LED_SUPPORTED == 1) */

#if (CFG_BUTTON_SUPPORTED == 1)
#ifdef CFG_BSP_ON_DISCOVERY
  /* Joystick HW Initialization */
  BSP_JOY_Init( JOY1, JOY_MODE_EXTI, JOY_ALL );
#endif /* CFG_BSP_ON_DISCOVERY */

#ifdef APPLICATION_ON_CEB
  /* Button HW Initialization */
  BSP_PB_Init( B2, BUTTON_MODE_EXTI );
#endif /* APPLICATION_ON_CEB */

#ifdef APPLICATION_ON_NUCLEO
  /* Buttons HW Initialization */
  BSP_PB_Init( B1, BUTTON_MODE_EXTI );
  BSP_PB_Init( B2, BUTTON_MODE_EXTI );
  BSP_PB_Init( B3, BUTTON_MODE_EXTI );
#endif /* APPLICATION_ON_NUCLEO */
#endif /* (CFG_BUTTON_SUPPORTED == 1) */
}

#if ( CFG_BUTTON_SUPPORTED == 1 )

/**
 * @brief   Indicate if the selected button was pressedn during a 'long time' or not.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 * @return  '1' if pressed during a 'long time', else '0'.
 */
uint8_t APP_BSP_ButtonIsLongPressed( uint16_t btnIdx )
{
  uint8_t pressStatus = 0;
#ifndef CFG_BSP_ON_DISCOVERY

  if ( btnIdx < BUTTON_NB_MAX )
  {
    pressStatus = buttonDesc[btnIdx].longPressed;
    buttonDesc[btnIdx].longPressed = 0;
  }
#endif /* CFG_BSP_ON_DISCOVERY */

  return pressStatus;
}

/**
 * @brief   'Manually' set the selected button as pressed during a 'long time'.
 *
 * @param   btnIdx    Button to test, listed in enum Button_TypeDef
 */
void APP_BSP_SetButtonIsLongPressed( uint16_t btnIdx )
{
  buttonDesc[btnIdx].longPressed = 1;
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

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/

#if ( CFG_LED_SUPPORTED == 1 )

void APP_BSP_LedInit( void )
{
  /* Leds Initialization */
  BSP_LED_Init(LED_BLUE);
#ifdef CFG_BSP_ON_NUCLEO
  BSP_LED_Init(LED_GREEN);
  BSP_LED_Init(LED_RED);
#endif /* CFG_BSP_ON_NUCLEO */
}

#endif /* (CFG_LED_SUPPORTED == 1) */
#ifdef CFG_BSP_ON_DISCOVERY
#if (CFG_LCD_SUPPORTED == 1)

void APP_BSP_DisplayInit( void )
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
    UTIL_LCD_SetFont( &Font16 );
    UTIL_LCD_SetBackColor( LCD_COLOR_BLACK );
    UTIL_LCD_SetTextColor( LCD_COLOR_WHITE );
  }
}

#endif /* (CFG_LCD_SUPPORTED == 1) */
#endif /* CFG_BSP_ON_DISCOVERY */
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
    osThreadYield();
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
    osThreadYield();
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
    osThreadYield();
  }
}

static void Button_InitTask( void )
{
  /* Register Semaphore to launch the pushbutton B1 Task */
  ButtonB1Semaphore = osSemaphoreNew( 1, 0, NULL );
  if ( ButtonB1Semaphore == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B1 SEMAPHORE CREATION FAILED" );
    while(1);
  }

  /* Create the pushbutton B1 Thread  */
  ButtonB1Thread = osThreadNew( ButtonB1Task, NULL, &ButtonB1ThreadAttributes );
  if ( ButtonB1Thread == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B1 THREAD CREATION FAILED" );
    while(1);
  }

  /* Register Semaphore to launch the pushbutton B2 Task */
  ButtonB2Semaphore = osSemaphoreNew( 1, 0, NULL );
  if ( ButtonB2Semaphore == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B2 SEMAPHORE CREATION FAILED" );
    while(1);
  }

  /* Create the pushbutton B2 Thread  */
  ButtonB2Thread = osThreadNew( ButtonB2Task, NULL, &ButtonB2ThreadAttributes );
  if ( ButtonB2Thread == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B2 THREAD CREATION FAILED" );
    while(1);
  }

  /* Register Semaphore to launch the pushbutton B3 Task */
  ButtonB3Semaphore = osSemaphoreNew( 1, 0, NULL );
  if ( ButtonB3Semaphore == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B3 SEMAPHORE CREATION FAILED" );
    while(1);
  }

  /* Create the pushbutton B3 Thread  */
  ButtonB3Thread = osThreadNew( ButtonB3Task, NULL, &ButtonB3ThreadAttributes );
  if ( ButtonB3Thread == NULL )
  {
    LOG_ERROR_APP( "ERROR FREERTOS : BUTTON B3 THREAD CREATION FAILED" );
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
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
    tx_thread_relinquish();
  }
}

static void Button_InitTask( void )
{
  UINT    ThreadXStatus;
  CHAR    *pStack;

  /* Register Semaphore to launch the pushbutton B1 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonB1Semaphore, "ButtonB1 Semaphore", 0 );

  /* Create the pushbutton Bx Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B1, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &ButtonB1Thread, "ButtonB1 Thread", ButtonB1Task, 0, pStack,
                                      TASK_STACK_SIZE_BUTTON_B1, TASK_PRIO_BUTTON_B1, TASK_PREEMP_BUTTON_B1,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : PUSH BUTTON B1 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }

  /* Register Semaphore to launch the pushbutton B2 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonB2Semaphore, "ButtonB2 Semaphore", 0 );

  /* Create the pushbutton B2 Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B2, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &ButtonB2Thread, "ButtonB2 Thread", ButtonB2Task, 0, pStack,
                                      TASK_STACK_SIZE_BUTTON_B2, TASK_PRIO_BUTTON_B2, TASK_PREEMP_BUTTON_B2,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : PUSH BUTTON B2 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }

  /* Register Semaphore to launch the pushbutton B3 Task */
  ThreadXStatus = tx_semaphore_create( &ButtonB3Semaphore, "ButtonB3 Semaphore", 0 );

  /* Create the pushbutton B3 Thread and this Stack */
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_byte_allocate( pBytePool, (VOID**) &pStack, TASK_STACK_SIZE_BUTTON_B3, TX_NO_WAIT);
  }
  if ( ThreadXStatus == TX_SUCCESS )
  {
    ThreadXStatus = tx_thread_create( &ButtonB3Thread, "ButtonB3 Thread", ButtonB3Task, 0, pStack,
                                      TASK_STACK_SIZE_BUTTON_B3, TASK_PRIO_BUTTON_B3, TASK_PREEMP_BUTTON_B3,
                                      TX_NO_TIME_SLICE, TX_AUTO_START );
  }

  /* Verify if it's OK */
  if ( ThreadXStatus != TX_SUCCESS )
  {
    LOG_ERROR_APP( "ERROR THREADX : PUSH BUTTON B3 THREAD CREATION FAILED (0x%04X)", ThreadXStatus );
    while(1);
  }
}

#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_SEQUENCER

static void Button_InitTask( void )
{
#ifdef CFG_BSP_ON_CEB
  /* Task associated with push button B2 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_B2, UTIL_SEQ_RFU, APP_BSP_Button2Action );
#else /* CFG_BSP_ON_CEB */
  /* Task associated with push button B1 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_B1, UTIL_SEQ_RFU, APP_BSP_Button1Action );

  /* Task associated with push button B2 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_B2, UTIL_SEQ_RFU, APP_BSP_Button2Action );

  /* Task associated with push button B3 */
  UTIL_SEQ_RegTask( 1U << CFG_TASK_BUTTON_B3, UTIL_SEQ_RFU, APP_BSP_Button3Action );
#endif /* CFG_BSP_ON_CEB */
}

#endif /* CFG_BSP_ON_SEQUENCER */

void APP_BSP_ButtonInit( void )
{
  Button_TypeDef  buttonIndex;

#ifdef CFG_BSP_ON_DISCOVERY
  /* Joystick HW Initialization */
  BSP_JOY_Init( JOY1, JOY_MODE_EXTI, JOY_ALL );
#endif /* CFG_BSP_ON_DISCOVERY */
  /* Buttons HW Initialization */
#ifdef CFG_BSP_ON_CEB
  BSP_PB_Init( B2, BUTTON_MODE_EXTI );
#endif /* CFG_BSP_ON_CEB */
#ifdef CFG_BSP_ON_NUCLEO
  BSP_PB_Init( B1, BUTTON_MODE_EXTI );
  BSP_PB_Init( B2, BUTTON_MODE_EXTI );
  BSP_PB_Init( B3, BUTTON_MODE_EXTI );
#endif /* CFG_BSP_ON_NUCLEO */

  /* Button task initialisation */
  Button_InitTask();

  /* Button timers initialisation (one for each button) */
#ifdef CFG_BSP_ON_CEB
  buttonIndex = B2;
  UTIL_TIMER_Create( &buttonDesc[buttonIndex].longTimerId, 0, UTIL_TIMER_PERIODIC, &Button_TriggerActions, &buttonDesc[buttonIndex] );
#else /* CFG_BSP_ON_CEB */
  for ( buttonIndex = B1; buttonIndex < BUTTON_NB_MAX; buttonIndex++ )
  {
    UTIL_TIMER_Create( &buttonDesc[buttonIndex].longTimerId, 0, UTIL_TIMER_PERIODIC, &Button_TriggerActions, &buttonDesc[buttonIndex] );
  }
#endif /* CFG_BSP_ON_CEB */
}

/**
 *
 */
static void Button_TriggerActions( void * arg )
{
  ButtonDesc_t  * p_buttonDesc = arg;
#ifdef CFG_BSP_ON_DISCOVERY

  /* Wait until Joystick is not pressed */
  if ( alwaysPressJoyPin != false )
  {
    alwaysPressJoyPin = false;
    return;
  }

  /* Save 'button' state */
  p_buttonDesc->waitingTime += BUTTON_LONG_PRESS_SAMPLE_MS;
  if ( p_buttonDesc->waitingTime > BUTTON_LONG_PRESS_THRESHOLD_MS )
  {
    APP_BSP_SetButtonIsLongPressed( p_buttonDesc->button );
  }

  /* Reset previous Joystick state */
  previousJoyPin = JOY_NONE;
#else /* CFG_BSP_ON_DISCOVERY */
  int32_t       buttonState;

  buttonState = BSP_PB_GetState( p_buttonDesc->button );

  /* If Button pressed and Threshold time not finish, continue waiting */
  p_buttonDesc->waitingTime += BUTTON_LONG_PRESS_SAMPLE_MS;
  if ( ( buttonState == 1 ) && ( p_buttonDesc->waitingTime < BUTTON_LONG_PRESS_THRESHOLD_MS ) )
  {
    return;
  }

  /* Save button state */
  if ( buttonState != 0u )
  {
    APP_BSP_SetButtonIsLongPressed( p_buttonDesc->button );
  }
#endif /* CFG_BSP_ON_DISCOVERY */

  /* Stop Timer */
  UTIL_TIMER_Stop( &p_buttonDesc->longTimerId );

#ifdef CFG_BSP_ON_CEB
  if ( p_buttonDesc->button == B2 )
  {
#ifdef CFG_BSP_ON_FREERTOS
    osSemaphoreRelease( ButtonB2Semaphore );
#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_THREADX
    tx_semaphore_put( &ButtonB2Semaphore );
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_SEQUENCER
    UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_B2, CFG_SEQ_PRIO_0 );
#endif /* CFG_BSP_ON_SEQUENCER */
  }
#endif /* CFG_BSP_ON_CEB */
#ifdef CFG_BSP_ON_NUCLEO
  switch ( p_buttonDesc->button )
  {
#ifdef CFG_BSP_ON_FREERTOS
    case B1:
        osSemaphoreRelease( ButtonB1Semaphore );
        break;

    case B2:
        osSemaphoreRelease( ButtonB2Semaphore );
        break;

    case B3:
        osSemaphoreRelease( ButtonB3Semaphore );
        break;
#endif /* CFG_BSP_ON_FREERTOS */
#ifdef CFG_BSP_ON_THREADX
    case B1:
        tx_semaphore_put( &ButtonB1Semaphore );
        break;

    case B2:
        tx_semaphore_put( &ButtonB2Semaphore );
        break;

    case B3:
        tx_semaphore_put( &ButtonB3Semaphore );
        break;
#endif /* CFG_BSP_ON_THREADX */
#ifdef CFG_BSP_ON_SEQUENCER
    case B1:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_B1, CFG_SEQ_PRIO_0 );
        break;

    case B2:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_B2, CFG_SEQ_PRIO_0 );
        break;

    case B3:
        UTIL_SEQ_SetTask( 1U << CFG_TASK_BUTTON_B3, CFG_SEQ_PRIO_0 );
        break;
#endif /* CFG_BSP_ON_SEQUENCER */

    default:
        break;
  }
#endif /* CFG_BSP_ON_NUCLEO */
}

/**
 * @brief  Treat USART commands to simulate button press for instance.
 *
 * @param  pRxBuffer      Pointer on received data from USART.
 * @param  iRxBufferSize  Number of received data.
 * @retval 1 if function can be execute a command else 0.
 */
uint8_t APP_BSP_SerialCmdExecute( uint8_t * pRxBuffer, uint16_t iRxBufferSize )
{
  uint8_t   cReturn = 0;
  uint16_t  iButton = UINT16_MAX;

  /* Parse received frame */
#ifdef CFG_BSP_ON_CEB
  if ( (strcmp( (char const*)pRxBuffer, "B2" ) == 0) ||
       (strcmp( (char const*)pRxBuffer, "SW2" ) == 0) )
  {
    iButton = B2;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B2L" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW2L" ) == 0) )
  {
    APP_BSP_SetButtonIsLongPressed(B2);
    iButton = B2;
  }
#else /* CFG_BSP_ON_CEB */
  if ( (strcmp( (char const*)pRxBuffer, "B1" ) == 0) ||
       (strcmp( (char const*)pRxBuffer, "SW1" ) == 0) )
  {
    iButton = B1;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B1L" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW1L" ) == 0) )
  {
    APP_BSP_SetButtonIsLongPressed(B1);
    iButton = B1;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B2" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW2" ) == 0) )
  {
    iButton = B2;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B2L" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW2L" ) == 0) )
  {
    APP_BSP_SetButtonIsLongPressed(B2);
    iButton = B2;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B3" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW3" ) == 0) )
  {
    iButton = B3;
  }
  else if ( (strcmp( (char const*)pRxBuffer, "B3L" ) == 0) ||
            (strcmp( (char const*)pRxBuffer, "SW3L" ) == 0) )
  {
    APP_BSP_SetButtonIsLongPressed(B3);
    iButton = B3;
  }
#endif /* CFG_BSP_ON_CEB */

  if ( iButton != UINT16_MAX )
  {
    /* Launch Button Command */
    LOG_INFO_APP( "%s pressed by Serial Command.", pRxBuffer );
    BSP_PB_Callback( (Button_TypeDef)iButton );
    cReturn = 1;
  }

  return cReturn;
}

#ifdef CFG_BSP_ON_DISCOVERY
/**
 *
 */
void BSP_JOY_Callback( JOY_TypeDef joyNb, JOYPin_TypeDef joyPin )
{
  uint8_t   button = 0;
  uint8_t   found = false;

  /* If always same 'button', quit immediately */
  if ( previousJoyPin == joyPin )
  {
    alwaysPressJoyPin = true;
    return;
  }

  /* Transform Joystick movement in 'button' */
  do
  {
    if ( ( joyPin & buttonDesc[button].joyPin ) != 0 )
    {
      found = true;
      previousJoyPin = joyPin;
      alwaysPressJoyPin = true;

      buttonDesc[button].waitingTime = 0;
      UTIL_TIMER_StartWithPeriod( &buttonDesc[button].longTimerId, BUTTON_LONG_PRESS_SAMPLE_MS );
    }
    button++;
  }
  while ( ( button < BUTTON_NB_MAX ) && ( found == false ) );
}

#endif /* CFG_BSP_ON_DISCOVERY */

/**
 *
 */
void BSP_PB_Callback( Button_TypeDef button )
{
  buttonDesc[button].waitingTime = 0;
  UTIL_TIMER_StartWithPeriod( &buttonDesc[button].longTimerId, BUTTON_LONG_PRESS_SAMPLE_MS );
}

#endif /* ( CFG_BUTTON_SUPPORTED == 1 )  */
