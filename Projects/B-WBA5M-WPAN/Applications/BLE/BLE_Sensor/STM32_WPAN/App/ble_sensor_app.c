/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    BLE_Sensor_app.c
  * @author  MCD Application Team
  * @brief   BLE_Sensor_app application definition.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_common.h"
#include "log_module.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble_sensor_app.h"
#include "ble_sensor.h"
#include "stm32_rtos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "host_stack_if.h"

#include "b_wba5m_wpan_bus.h"
#include "b_wba5m_wpan_env_sensors.h"
#include "b_wba5m_wpan_motion_sensors.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
 * @brief  HW/Environmental Service/Char Context structure definition
 */
typedef struct
{
  uint8_t  NotificationStatus;
  int32_t PressureValue;
  uint16_t HumidityValue;
  int16_t TemperatureValue[1];
} ENV_Server_App_Context_t;


/**
 * @brief  Motion Service/Char Context structure definition
 */
typedef struct
{
  uint8_t  NotificationStatus;
  MOTION_SENSOR_Axes_t acceleration;
  MOTION_SENSOR_Axes_t angular_velocity;
  uint8_t hasAcc;
  uint8_t hasGyro;
} MOTION_Server_App_Context_t;

/* USER CODE END PTD */

typedef enum
{
  Motion_c_NOTIFICATION_OFF,
  Motion_c_NOTIFICATION_ON,
  Env_c_NOTIFICATION_OFF,
  Env_c_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  BLE_SENSOR_APP_SENDINFORMATION_LAST
} BLE_SENSOR_APP_SendInformation_t;

typedef struct
{
  BLE_SENSOR_APP_SendInformation_t     Motion_c_Notification_Status;
  BLE_SENSOR_APP_SendInformation_t     Env_c_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */
  UTIL_TIMER_Object_t           ENV_TimerMeasurement_Id;
  UTIL_TIMER_Object_t           MOTION_TimerMeasurement_Id;

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} BLE_SENSOR_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TEMPERATURE_BYTES       (2)
#define HUMIDITY_BYTES          (2)
#define VALUE_LEN_ENV           (2+TEMPERATURE_BYTES+HUMIDITY_BYTES)

#define ACC_BYTES               (2)
#define GYRO_BYTES              (2)
#define VALUE_LEN_MOTION        (2+3*ACC_BYTES+3*GYRO_BYTES)
/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static BLE_SENSOR_APP_Context_t BLE_SENSOR_APP_Context;

uint8_t a_BLE_SENSOR_UpdateCharData[247];

/* USER CODE BEGIN PV */
static ENV_Server_App_Context_t ENV_Server_App_Context;
static MOTION_Server_App_Context_t MOTION_Server_App_Context;

static ENV_SENSOR_Capabilities_t env_capabilities = {0};
static MOTION_SENSOR_Capabilities_t motion_capabilities = {0};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void BLE_SENSOR_Motion_c_SendNotification(void);
static void BLE_SENSOR_Env_c_SendNotification(void);

/* USER CODE BEGIN PFP */
void ENV_and_MOTION_Context_Init(void);

static void ENV_GetCaps(void);
static void ENV_Handle_Sensor(void);
static void ENV_APP_Measurements_timCB(void *arg);
void ENV_Update(void);

static void MOTION_GetCaps(void);
static void MOTION_ACC_Handle_Sensor(void);
static void MOTION_GYRO_Handle_Sensor(void);
static void MOTION_APP_Measurements_timCB(void *arg);
void MOTION_Update(void);
/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void BLE_SENSOR_Notification(BLE_SENSOR_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case BLE_SENSOR_MOTION_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_ENABLED_EVT */
      BLE_SENSOR_APP_Context.Motion_c_Notification_Status = Motion_c_NOTIFICATION_ON;
      LOG_INFO_APP("-- BLE_SENSOR MOTION : NOTIFICATION ENABLED\n");
      LOG_INFO_APP(" \n\r");
      UTIL_TIMER_Stop(&(BLE_SENSOR_APP_Context.MOTION_TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod( &(BLE_SENSOR_APP_Context.MOTION_TimerMeasurement_Id), MOTION_UPDATE_PERIOD);
      /* USER CODE END Service1Char1_NOTIFY_ENABLED_EVT */
      break;

    case BLE_SENSOR_MOTION_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char1_NOTIFY_DISABLED_EVT */
      BLE_SENSOR_APP_Context.Motion_c_Notification_Status = Motion_c_NOTIFICATION_OFF;
      LOG_INFO_APP("-- BLE_SENSOR MOTION : NOTIFICATION DISABLED\n");
      LOG_INFO_APP(" \n\r");
      UTIL_TIMER_Stop(&(BLE_SENSOR_APP_Context.MOTION_TimerMeasurement_Id));
      /* USER CODE END Service1Char1_NOTIFY_DISABLED_EVT */
      break;

    case BLE_SENSOR_ENV_C_READ_EVT:
      /* USER CODE BEGIN Service1Char2_READ_EVT */
      LOG_INFO_APP("-- BLE_SENSOR ENV : ENV CHARACTERISTIC READ\n");
      LOG_INFO_APP(" \n\r");
      UTIL_SEQ_SetTask(1<<CFG_TASK_ENV_UPDATE_ID, CFG_SEQ_PRIO_0);
      /* USER CODE END Service1Char2_READ_EVT */
      break;

    case BLE_SENSOR_ENV_C_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      BLE_SENSOR_APP_Context.Env_c_Notification_Status = Env_c_NOTIFICATION_ON;
      LOG_INFO_APP("-- BLE_SENSOR ENV : NOTIFICATION ENABLED\n");
      LOG_INFO_APP(" \n\r");
      UTIL_TIMER_Stop(&(BLE_SENSOR_APP_Context.ENV_TimerMeasurement_Id));
      UTIL_TIMER_StartWithPeriod( &(BLE_SENSOR_APP_Context.ENV_TimerMeasurement_Id), ENV_UPDATE_PERIOD);
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case BLE_SENSOR_ENV_C_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      BLE_SENSOR_APP_Context.Env_c_Notification_Status = Env_c_NOTIFICATION_OFF;
      LOG_INFO_APP("-- BLE_SENSOR ENV : NOTIFICATION DISABLED\n");
      LOG_INFO_APP(" \n\r");
      UTIL_TIMER_Stop(&(BLE_SENSOR_APP_Context.ENV_TimerMeasurement_Id));

      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void BLE_SENSOR_APP_EvtRx(BLE_SENSOR_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case BLE_SENSOR_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case BLE_SENSOR_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void BLE_SENSOR_APP_Init(void)
{
  UNUSED(BLE_SENSOR_APP_Context);
  BLE_SENSOR_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  UTIL_TIMER_Create(&(BLE_SENSOR_APP_Context.ENV_TimerMeasurement_Id),
                  ENV_UPDATE_PERIOD,
                  UTIL_TIMER_PERIODIC,
                  &ENV_APP_Measurements_timCB, 0);
  UTIL_SEQ_RegTask( 1U << CFG_TASK_ENV_UPDATE_ID, UTIL_SEQ_RFU, ENV_Update);

  UTIL_TIMER_Create(&(BLE_SENSOR_APP_Context.MOTION_TimerMeasurement_Id),
                  MOTION_UPDATE_PERIOD,
                  UTIL_TIMER_PERIODIC,
                  &MOTION_APP_Measurements_timCB, 0);
  UTIL_SEQ_RegTask( 1U << CFG_TASK_MOTION_UPDATE_ID, UTIL_SEQ_RFU, MOTION_Update);

    
  /* Init ENV and MOTION context */
  ENV_and_MOTION_Context_Init();    
  
  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void BLE_SENSOR_Motion_c_SendNotification(void) /* Property Notification */
{
  BLE_SENSOR_APP_SendInformation_t notification_on_off = Motion_c_NOTIFICATION_OFF;
  BLE_SENSOR_Data_t ble_sensor_notification_data;

  ble_sensor_notification_data.p_Payload = (uint8_t*)a_BLE_SENSOR_UpdateCharData;
  ble_sensor_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char1_NS_1 */

  /* USER CODE END Service1Char1_NS_1 */

  if (notification_on_off != Motion_c_NOTIFICATION_OFF)
  {
    BLE_SENSOR_UpdateValue(BLE_SENSOR_MOTION_C, &ble_sensor_notification_data);
  }

  /* USER CODE BEGIN Service1Char1_NS_Last */

  /* USER CODE END Service1Char1_NS_Last */

  return;
}

__USED void BLE_SENSOR_Env_c_SendNotification(void) /* Property Notification */
{
  BLE_SENSOR_APP_SendInformation_t notification_on_off = Env_c_NOTIFICATION_OFF;
  BLE_SENSOR_Data_t ble_sensor_notification_data;

  ble_sensor_notification_data.p_Payload = (uint8_t*)a_BLE_SENSOR_UpdateCharData;
  ble_sensor_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1 */

  /* USER CODE END Service1Char2_NS_1 */

  if (notification_on_off != Env_c_NOTIFICATION_OFF)
  {
    BLE_SENSOR_UpdateValue(BLE_SENSOR_ENV_C, &ble_sensor_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last */

  /* USER CODE END Service1Char2_NS_Last */

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS */

void ENV_and_MOTION_Context_Init(void)
{
#if defined(USE_MOTION_SENSOR_ISM330DHCX_0)  
  int32_t ret;
  
  /* ENV_SENSOR_SHT40AD1B_0 Init */ 
  ret = BSP_ENV_SENSOR_Init(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE | ENV_HUMIDITY);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  /* MOTION_SENSOR_ISM330DHCX_0 Init */ 
  ret = BSP_MOTION_SENSOR_Init(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO | MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  ret = BSP_MOTION_SENSOR_Enable(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO | MOTION_GYRO);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  ENV_GetCaps(); 
  MOTION_GetCaps();

#endif /* USE_MOTION_SENSOR_ISM330DHCX_0 */
  return;
}

static void ENV_APP_Measurements_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask(1<<CFG_TASK_ENV_UPDATE_ID, CFG_SEQ_PRIO_0);

  return;
}

static void MOTION_APP_Measurements_timCB(void *arg)
{
  /**
   * The code shall be executed in the background as aci command may be sent
   * The background is the only place where the application can make sure a new aci command
   * is not sent if there is a pending one
   */
  UTIL_SEQ_SetTask(1<<CFG_TASK_MOTION_UPDATE_ID, CFG_SEQ_PRIO_0);

  return;
}

/**
 * @brief  Update the Environmental char value
 * @param  None
 * @retval None
 */
void ENV_Update(void)
{
  uint8_t BuffPos = 2;

  BLE_SENSOR_Data_t ble_env_sensor_notification_data;
  ble_env_sensor_notification_data.Length = VALUE_LEN_ENV;
  ble_env_sensor_notification_data.p_Payload = (uint8_t*)a_BLE_SENSOR_UpdateCharData;
  
  /* Read ENV values */
  ENV_Handle_Sensor();

  /* Timestamp */
  STORE_LE_16(ble_env_sensor_notification_data.p_Payload, (HAL_GetTick()>>3));
  if(env_capabilities.Humidity == SENSOR_FEATURE_SUPPORTED)
  {
    STORE_LE_16(&ble_env_sensor_notification_data.p_Payload[BuffPos], ENV_Server_App_Context.HumidityValue);   
    BuffPos += HUMIDITY_BYTES;
  }

  if(env_capabilities.Temperature == SENSOR_FEATURE_SUPPORTED)
  {
    STORE_LE_16(&ble_env_sensor_notification_data.p_Payload[BuffPos], ENV_Server_App_Context.TemperatureValue[0]);
    BuffPos += TEMPERATURE_BYTES;
  }

  BLE_SENSOR_UpdateValue(BLE_SENSOR_ENV_C, &ble_env_sensor_notification_data);
  return;
}

/**
 * @brief  Parse the values read by Environmental sensors
 * @param  None
 * @retval None
 */
static void ENV_Handle_Sensor(void)
{
  float humidity = 0.0;
  float temperature = 0.0;
  int32_t decPart= 0, intPart=0; 
  
  if(env_capabilities.Humidity == SENSOR_FEATURE_SUPPORTED)
  {    
    BSP_ENV_SENSOR_GetValue(ENV_SENSOR_SHT40AD1B_0, ENV_HUMIDITY, &humidity);
    LOG_INFO_APP("\tHumidity = %f \n", (float_t)humidity);
    F2I_1D(humidity, intPart, decPart);
    ENV_Server_App_Context.HumidityValue = intPart*10+decPart; ;
  }
  if(env_capabilities.Temperature == SENSOR_FEATURE_SUPPORTED)
  {    
    BSP_ENV_SENSOR_GetValue(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE, &temperature);
    LOG_INFO_APP("\tTemperature = %f \n", (float_t)temperature);
    F2I_1D(temperature, intPart, decPart);
    ENV_Server_App_Context.TemperatureValue[0] = intPart*10+decPart; ;
  }
}



/**
 * @brief  Update the Motion char value
 * @param  None
 * @retval None
 */
void MOTION_Update(void)
{
  BLE_SENSOR_Data_t ble_motion_sensor_notification_data;
  ble_motion_sensor_notification_data.Length=VALUE_LEN_MOTION;
  ble_motion_sensor_notification_data.p_Payload = (uint8_t*)a_BLE_SENSOR_UpdateCharData;
  
  /* Read Motion values */
  MOTION_ACC_Handle_Sensor();
  MOTION_GYRO_Handle_Sensor();

  /* Timestamp */
  STORE_LE_16(ble_motion_sensor_notification_data.p_Payload, (HAL_GetTick()>>3));

  if(motion_capabilities.Acc == SENSOR_FEATURE_SUPPORTED)
  {
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+2, MOTION_Server_App_Context.acceleration.x);
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+4, MOTION_Server_App_Context.acceleration.y);
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+6, MOTION_Server_App_Context.acceleration.z);
  }

  if(motion_capabilities.Gyro == SENSOR_FEATURE_SUPPORTED)
  {
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+8, MOTION_Server_App_Context.angular_velocity.x);
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+10, MOTION_Server_App_Context.angular_velocity.y);
    STORE_LE_16(ble_motion_sensor_notification_data.p_Payload+12, MOTION_Server_App_Context.angular_velocity.z);
    
  }

  BLE_SENSOR_UpdateValue(BLE_SENSOR_MOTION_C, &ble_motion_sensor_notification_data);
 
  return;
}


void MOTION_ACC_Handle_Sensor(void)
{
 #if defined(USE_MOTION_SENSOR_ISM330DHCX_0)
  MOTION_SENSOR_Axes_t acceleration;
  int32_t ret;
  
  memset(&acceleration, 0, sizeof(MOTION_SENSOR_Axes_t));

  ret = BSP_MOTION_SENSOR_GetAxes(MOTION_SENSOR_ISM330DHCX_0, MOTION_ACCELERO, &acceleration);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  LOG_INFO_APP( "\tAccel (x = %ld,\t y = %ld,\t z = %ld)\n", acceleration.x, acceleration.y, acceleration.z);

  MOTION_Server_App_Context.acceleration=acceleration;  

#endif /* USE_MOTION_SENSOR_ISM330DHCX_0 */
}

void MOTION_GYRO_Handle_Sensor(void)
{
  #if defined(USE_MOTION_SENSOR_ISM330DHCX_0)
  static MOTION_SENSOR_Axes_t angular_velocity;
  int32_t ret;
  
  
  memset(&angular_velocity, 0, sizeof(MOTION_SENSOR_Axes_t));
  ret = BSP_MOTION_SENSOR_GetAxes(MOTION_SENSOR_ISM330DHCX_0, MOTION_GYRO, &angular_velocity);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  LOG_INFO_APP("\tGyro (x = %ld,\t y = %ld,\t z = %ld)\n", angular_velocity.x, angular_velocity.y, angular_velocity.z);
  
  
  MOTION_Server_App_Context.angular_velocity=angular_velocity;
  
  
#endif /* USE_MOTION_SENSOR_ISM330DHCX_0 */
  
  
}

void ENV_GetCaps(void)
{
  /* Env Sensors */  
#if defined(USE_ENV_SENSOR_SHT40AD1B_0)
  int32_t ret;
  uint8_t id = 0xAB;
  ret = BSP_ENV_SENSOR_ReadID(ENV_SENSOR_SHT40AD1B_0, &id);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* ID on this sensor is emulated by software and set to 0: see SHT40AD1B_ID */
  if (id != 0)
  {
    Error_Handler();
  }
  
  ret = BSP_ENV_SENSOR_Enable(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  ret = BSP_ENV_SENSOR_GetCapabilities(ENV_SENSOR_SHT40AD1B_0, &env_capabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  LOG_INFO_APP("\n Environement Sensor Capabilities: (1 = Supported / 0 = Not Supported) \n\
               LowPower:%d\n\
               temperature:%d\n\
               Pressure:%d\n\
               Humidity:%d\n",
               env_capabilities.LowPower,
               env_capabilities.Temperature,
               env_capabilities.Pressure,
               env_capabilities.Humidity);
  
  /* LowPower is not supported by the sensor */
  if (env_capabilities.LowPower != 0)
  {
    Error_Handler();
  }
  /* Pressure is not supported by the SHT40AD1B sensor */
  if (env_capabilities.Pressure != 0)
  {
    Error_Handler();
  }
  /* Odr is not supported by the SHT40AD1B sensor */
  if ((env_capabilities.TempMaxOdr != 0) || (env_capabilities.PressMaxOdr != 0) || (env_capabilities.HumMaxOdr != 0))
  {
    Error_Handler();
  }
  
  float odrT = 555;
  float odrP = 555;
  float odrH = 555;
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE, &odrT);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* Pressure is not supported on this sensor */
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_PRESSURE, &odrP);
  if (ret != BSP_ERROR_WRONG_PARAM)
  {
    Error_Handler();
  }
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_HUMIDITY, &odrH);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if ((odrT != env_capabilities.TempMaxOdr) || (odrP != 555) || (odrH != env_capabilities.HumMaxOdr))
  {
    Error_Handler();
  }
  
  /* Seting Odr is not supported on this sensor */
  ret = BSP_ENV_SENSOR_SetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE, 10);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  ret = BSP_ENV_SENSOR_SetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_PRESSURE, 10);
  if (ret != BSP_ERROR_WRONG_PARAM)
  {
    Error_Handler();
  }
  ret = BSP_ENV_SENSOR_SetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_HUMIDITY, 10);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  
  /* Check that Odr was not changed as it is not suported */
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_TEMPERATURE, &odrT);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  /* Pressure is not supported on this sensor */
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_PRESSURE, &odrP);
  if (ret != BSP_ERROR_WRONG_PARAM)
  {
    Error_Handler();
  }
  ret = BSP_ENV_SENSOR_GetOutputDataRate(ENV_SENSOR_SHT40AD1B_0, ENV_HUMIDITY, &odrH);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if ((odrT != 0) || (odrP != 555) || (odrH != 0))
  {
    Error_Handler();
  }
  
#endif /* defined(USE_ENV_SENSOR_SHT40AD1B_0) */
}

/**
 * @brief  Check the Motion active capabilities and set the ADV data accordingly
 * @param  None
 * @retval None
 */
static void MOTION_GetCaps(void)
{
  uint8_t id = 0xAB;
  uint32_t ret;
  ret = BSP_MOTION_SENSOR_ReadID(MOTION_SENSOR_ISM330DHCX_0, &id);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if (id != 107)
  {
    Error_Handler();
  }

  ret = BSP_MOTION_SENSOR_GetCapabilities(MOTION_SENSOR_ISM330DHCX_0, &motion_capabilities);
  if (ret != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  LOG_INFO_APP("\n Motion Sensors Capabilities: (1 = Supported / 0 = Not Supported) \n\
               LowPower:%d\n\
               Acc:%d\n\
               Magneto:%d\n\
               Gyro:%d\n",
               motion_capabilities.LowPower,
               motion_capabilities.Acc,
               motion_capabilities.Magneto,
               motion_capabilities.Gyro);
  
  /* LowPower is not supported by the sensor */
  if (motion_capabilities.LowPower != 0)
  {
    Error_Handler();
  }
  /* Magneto is not supported by the sensor */
  if ((motion_capabilities.Acc != 1) || (motion_capabilities.Gyro != 1) || (motion_capabilities.Magneto != 0))
  {
    Error_Handler();
  }
  if ((motion_capabilities.AccMaxOdr == 0) || (motion_capabilities.GyroMaxOdr == 0) || (motion_capabilities.MagMaxOdr != 0))
  {
    Error_Handler();
  }
  if ((motion_capabilities.AccMaxFS == 0) || (motion_capabilities.GyroMaxFS == 0) || (motion_capabilities.MagMaxFS != 0))
  {
    Error_Handler();
  }
  return;
}
/* USER CODE END FD_LOCAL_FUNCTIONS */
