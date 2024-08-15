/**
  ******************************************************************************
  * @file    codec_if.c
  * @author  MCD Application Team
  * @brief   File for codec manager integration : process and high speed timer
  * This timer needs to run on the same clock domain as the audio interface
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "codec_if.h"
#include "app_common.h"
#include "main.h"
#include "app_conf.h"
#include "log_module.h"

#if CODEC_LC3_NUM_ENCODER_CHANNEL == 0
#include "LC3_encoder.h"
#endif /* CODEC_LC3_NUM_ENCODER_CHANNEL */

#if CODEC_LC3_NUM_DECODER_CHANNEL == 0
#include "LC3_decoder.h"
#endif /* CODEC_LC3_NUM_DECODER_CHANNEL */

/* Defines ------------------------------------------------------------------*/
/**
  * @brief  LINK_LC3_ENCODER and LINK_LC3_DECODER tell the linker if the subpart of the LC3 lib must be linked,
  *         Values can be set to 0 if feature is not needed, allowing FLASH footprint optimization
  */
#if CODEC_LC3_NUM_ENCODER_CHANNEL != 0
#define LINK_LC3_ENCODER 1
#else
#define LINK_LC3_ENCODER 0
#endif /* CODEC_LC3_NUM_ENCODER_CHANNEL */

#if CODEC_LC3_NUM_DECODER_CHANNEL != 0
#define LINK_LC3_DECODER 1
#else
#define LINK_LC3_DECODER 0
#endif /* CODEC_LC3_NUM_DECODER_CHANNEL */

/**
  * Expected core clock when executing LC3 codec for each audio frequency: check dependencies with audio peripheral
  *                   kHz:   8,   N/A,  16,  N/A,  24,    32,     44.1,    48,   N/A----------
  */
#define LC3_EXE_CLOCK_MHZ {98.304, 0, 98.304, 0, 98.304, 98.304, 90.3168, 98.304, 0, 0, 0, 0, 0};

/**
  * @brief  Values used for timer driver
  */
#define CLOCK_PRESCALER         98               /* (98.304MHz / 98) provides approximately 1us timer */
#define TIMER_IT_PRIO           4

#define AS_CAPTURE_PRESCALER    0
#define AS_AUTORELOAD           0x000FFFFF      /* maximum value */
#define AS_COMPARE_MAX          1000000         /* 1s */
#define AS_ACCEPTABLE_WINDOWS   100u            /* windows in ticks around the interrupt for generating the event to the codec */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t timestamp;
  uint8_t is_active;
} AUDIO_Timer_t;

/* Private macros ------------------------------------------------------------*/
#define CRITICAL_BEGIN( )       M_BEGIN uint32_t primask_ = __get_PRIMASK( ); \
                                __disable_irq( )

#define CRITICAL_END( )         __set_PRIMASK( primask_ ); M_END

#define IS_RCC_AS_CLOCKED       ((READ_BIT(RCC->CR, RCC_CR_PLL1ON) != 0) && \
                                 (READ_BIT(RCC->PLL1CFGR, RCC_PLL1CFGR_PLL1PEN) != 0))

#define IS_RCC_AS_RUNING        (IS_RCC_AS_CLOCKED && (READ_BIT(RCC->ASCR, RCC_ASCR_CEN_Msk) != 0))

#define WAIT_RCC_AS_EDGE( )     uint32_t tmp = RCC->ASCNTR; \
                                while (tmp == RCC->ASCNTR)

#define WAIT_3_CYCLES( )        __NOP(); \
                                __NOP(); \
                                __NOP()

/* Private variables ---------------------------------------------------------*/
static uint32_t AudioTimerCnt = 0;
uint8_t IsTimerAutoreloading = 0;

static uint8_t Codec_irq_mask_req = 0x00;

static AUDIO_Timer_t Audio_Timer_List[MAX_TIMER_NB] = {0};

static const float Codec_Exe_clock_Mhz[SAMPLE_FREQ_NUMBER] = LC3_EXE_CLOCK_MHZ;

/* Private functions prototype------------------------------------------------*/
static void TIMAudio_Init(void);

/* Private user code ---------------------------------------------------------*/

/******************************************************************************/
/***************************** processing functions ***************************/
/******************************************************************************/

/**
  * @brief Function called by the codec manager for initializing process
  * @param none
  * @retval none
  */
void CODEC_ProcessInit(void)
{
  HAL_NVIC_SetPriority(CODEC_MNGR_INTR_NUM, CODEC_MNGR_INTR_PRIO, 0);
  NVIC_EnableIRQ(CODEC_MNGR_INTR_NUM);
}

/**
  * @brief Function called by the codec manager for requesting CODEC_ManagerProcess to be called
  * @param none
  * @retval none
  */
void CODEC_ProcessReq(void)
{
  HAL_NVIC_SetPendingIRQ(CODEC_MNGR_INTR_NUM);
}

/**
  * @brief  Function to request masking and unmasking codec interrupt
  * @note  any masking request should be considered prior unmasking
  * @param src : requestor of the demand
  * @param req : request type; enable or disable the interrupt
  * @retval none
  */
void CODEC_ReqIRQState(CODEC_MaskSrc_t src, CODEC_IRQReq_t req)
{
  CRITICAL_BEGIN();
  /* ask to mask the IT*/
  if ( req == CODEC_IRQ_DISABLE )
  {
    Codec_irq_mask_req |= src;
    NVIC_DisableIRQ(CODEC_MNGR_INTR_NUM);
  }
  else
  {
    Codec_irq_mask_req &= ~src;
    /*demask the IT if nobody else has asked to mask it*/
    if (Codec_irq_mask_req == 0)
    {
      NVIC_EnableIRQ(CODEC_MNGR_INTR_NUM);
    }
  }
  CRITICAL_END();
}

/******************************************************************************/
/****************************** tracing events ********************************/
/******************************************************************************/

/**
  * @brief  Function called at specific event used for debugging purpose
  * @note This function is called only if CODEC_DEBUG_GPIO is set to 1
  * @param evnt : event of type CODEC_TraceEvnt_t
  * @retval none
  */
void CODEC_TraceEvnt(CODEC_TraceEvnt_t evnt)
{
  switch(evnt)
  {
    case APPLI_SEND_DATA:
      break;
    case APPLI_RECEIVE_DATA_REQ:
      break;
    case CODEC_TO_LL_DATA_EVT:
      break;
    case LL_TO_CODEC_DATA_EVT:
      break;
    case START_ENCODING_EVT:
      break;
    case STOP_ENCODING_EVT:
      break;
    case START_DECODING_EVT:
      break;
    case STOP_DECODING_EVT:
      break;
    case PLC_TRIGGERED_EVT:
      break;
    case CALIB_CLBK_EVT:
      break;
    case CORRECTOR_PROCESS_EVT:
      break;
    case TRIGGER_EVT:
      break;
    case FIFO_UNDERRUN_EVT:
      break;
    case FIFO_OVERRUN_EVT:
      break;
    case SYNC_EVT:
      break;
    case IRREGULARITY_EVT:
      break;
    default:
      break;
  }
}

/**
 * @brief Function called by the codec to print some logs
 * @note This function is called only if logs has been enabled at compilation
 * @param format: a list of printable arguments
 *
 */
void CODEC_DBG_Log(char *format,...)
{
  char msg[255];
  va_list args;

  va_start(args, format );
  vsnprintf(msg, 255-1, format, args);
  va_end(args);

  Log_Module_Print( LOG_VERBOSE_INFO, LOG_REGION_APP, msg);
}

/******************************************************************************/
/***************************** clock functions ********************************/
/******************************************************************************/

/**
  * @brief Function to be called for initializing the dedicated audio timer
  * @param none
  * @retval status
  */
int32_t CODEC_CLK_Init( void )
{
  TIMAudio_Init();
  return 0;
}

/**
  * @brief Function called by the codec manager for getting a timestamp
  * @param none
  * @retval timestamp
  */
uint32_t CODEC_CLK_GetHostTimestamp( void )
{
  uint32_t as_cnt_val = 0;

  if (IS_RCC_AS_RUNING)
  {
    as_cnt_val = RCC->ASCNTR; /* 20 bits value */
  }
  return AudioTimerCnt + as_cnt_val;
}

/**
  * @brief Function called by the codec manager for getting the timer prescaler used from PLL output to timer clock
  * @param none
  * @retval timer prescaler value
  */
uint16_t CODEC_CLK_GetTimerPrescaler( void )
{
  uint16_t timer_prescaler = CLOCK_PRESCALER;
  return timer_prescaler;
}

/**
  * @brief Function called by the codec manager for getting the core clock for a given audio frequency
  * @note  Used for either CPU load estimation as well as timer timebase correction
  * @param freq_index : index of the frequency related to the list defined in the assigned numbers
  * @retval Core clock in MHz
  */
float CODEC_CLK_GetCoreClock( uint8_t freq_index )
{
  return Codec_Exe_clock_Mhz[freq_index];
}

/**
  * @brief Function called by the codec manager for getting the PLL N Frac value
  * @param none
  * @retval nfrac register value
  */
uint16_t CODEC_CLK_GetPLLNfrac( void )
{
  return (RCC->PLL1FRACR >> RCC_PLL1FRACR_PLL1FRACN_Pos);
}

/**
  * @brief Function called by the codec manager clock corrector for setting the PLL N Frac value
  * @param n_frac : value of n frac register
  * @retval none
  */
void CODEC_CLK_SetPLLNfrac( uint16_t n_frac )
{
  volatile uint32_t cnt = 10;
  __HAL_RCC_PLL1_FRACN_DISABLE();
  __HAL_RCC_PLL1_FRACN_CONFIG((uint32_t)(n_frac));
  /* need to delay before enabling the register*/
  while(cnt != 0)
  {
    cnt--;
  }
  __HAL_RCC_PLL1_FRACN_ENABLE();
}

/**
  * @brief Function called by the codec manager for requesting a event with a specified 32 bits timestamps
  * @note The code should call CODEC_CLK_trigger_event_notify() with the corresponding ID for notifying the event
  * @note Function called from LL low priority ISR (sink), Timer ISR or from background (source)
  * @param ID : path identifier
  * @param trigger_ts : 32 bits timestamp
  * @retval status
  */
int32_t CODEC_CLK_RequestTimerEvent( uint8_t ID, uint32_t trigger_ts )
{

  /* check id is valid */
  if (ID >= MAX_TIMER_NB)
  {
    return -1;
  }

  Audio_Timer_List[ID].timestamp = trigger_ts;
  Audio_Timer_List[ID].is_active = 1;

  uint8_t is_closest = 1;

  CRITICAL_BEGIN();
  for (int32_t i=0 ; i < MAX_TIMER_NB ; i++)
  {
    /* check if it exist another timer that must run before */
    if ((Audio_Timer_List[i].is_active == 1) && ((Audio_Timer_List[i].timestamp - trigger_ts) > 0x80000000)){
      is_closest = 0;
    }
  }

  /* The new ID is the next one */
  if (is_closest == 1)
  {
    /* reconfigure audio timer */
    if (IS_RCC_AS_RUNING)
    {
      uint32_t tmp_ascor = MIN((trigger_ts - CODEC_CLK_GetHostTimestamp()), AS_COMPARE_MAX);

      /* reprogram the IP only if necessary */
      if ((RCC->ASCOR != tmp_ascor) || (RCC->ASCAR != AS_AUTORELOAD))
      {
        AudioTimerCnt += RCC->ASCNTR; /* incremented AudioTimerCnt since the cnt register will be reset */

        /* clear CEN lead to resetting all register */
        CLEAR_BIT(RCC->ASCR, RCC_ASCR_CEN);
        WAIT_3_CYCLES();
        WRITE_REG(RCC->ASARR, AS_AUTORELOAD);
        WRITE_REG(RCC->ASCR, ((CLOCK_PRESCALER-1) << RCC_ASCR_PSC_Pos) + (AS_CAPTURE_PRESCALER << RCC_ASCR_CPS_Pos));
        WRITE_REG(RCC->ASIER, RCC_ASIER_COIE | RCC_ASIER_CAIE);
        WRITE_REG(RCC->ASCOR, tmp_ascor);

        SET_BIT(RCC->ASCR, RCC_ASCR_CEN);
      }

      IsTimerAutoreloading = 0;
    }
  }
  CRITICAL_END();

  return 0;
}


static void TIMAudio_Init(void)
{
  /* select clock source */
  /* bit 30  (ASSEL) set to 0 = pll1p (default), or 1 = pll1q*/
  CLEAR_BIT(RCC->CCIPR2, RCC_CCIPR2_ASSEL);

  /* the IP is clocked only if the pll has been started */
  if (IS_RCC_AS_CLOCKED)
  {
    CLEAR_BIT(RCC->ASCR, RCC_ASCR_CEN);
    WAIT_3_CYCLES();
    WRITE_REG(RCC->ASARR, AS_COMPARE_MAX);
    WRITE_REG(RCC->ASCR, ((CLOCK_PRESCALER-1) << RCC_ASCR_PSC_Pos) + (AS_CAPTURE_PRESCALER << RCC_ASCR_CPS_Pos));
    WRITE_REG(RCC->ASIER, RCC_ASIER_COIE | RCC_ASIER_CAIE);
    WRITE_REG(RCC->ASCOR, AS_COMPARE_MAX);

    SET_BIT(RCC->ASCR, RCC_ASCR_CEN);

    IsTimerAutoreloading = 1;
    LOG_INFO_APP("RCC AS is now properly initialized\n");
  }else{
    LOG_INFO_APP("Warning, RCC AS cannot be initialized since it is not clocked\n");
  }

  AudioTimerCnt = 0;

  HAL_NVIC_SetPriority(RCC_AUDIOSYNC_IRQn, TIMER_IT_PRIO, 0);
  HAL_NVIC_EnableIRQ(RCC_AUDIOSYNC_IRQn);
}

static void manage_compare_it(void)
{
  /* Read value that has been set */
  uint32_t current_timestamp = AudioTimerCnt + RCC->ASCOR;
  uint8_t next_id = 0xff;
  uint32_t delta = 0xffffffff;

  for (int32_t i = 0 ; i < MAX_TIMER_NB ; i++)
  {
    if (Audio_Timer_List[i].is_active == 1)
    {
      /* check if this interrupt was concerning this path */
      if ((Audio_Timer_List[i].timestamp - current_timestamp) < AS_ACCEPTABLE_WINDOWS ||
          (Audio_Timer_List[i].timestamp - current_timestamp) > (0u - AS_ACCEPTABLE_WINDOWS))

      {
        Audio_Timer_List[i].is_active = 0;
        CODEC_CLK_trigger_event_notify(i);
      }
      else
      {
        /* check if this path may be the next one */
        uint32_t tmp_delta = Audio_Timer_List[i].timestamp - current_timestamp;
        if (tmp_delta < delta)
        {
          next_id = i;
          delta = tmp_delta;
        }
      }
    }
  }

  /* polling on register to ensure COMPARE and CNT are not equal anymore */
  /* at this time, we expect CNT must be running, and we never get stuck here */
  while (AudioTimerCnt + RCC->ASCNTR == current_timestamp)
  {}

  if (next_id != 0xff)
  {
    CODEC_CLK_RequestTimerEvent(next_id, Audio_Timer_List[next_id].timestamp);
  }
  else
  {
    if (IsTimerAutoreloading == 0)
    {
      CRITICAL_BEGIN();

      WAIT_RCC_AS_EDGE();

      AudioTimerCnt += RCC->ASCNTR+1; /* incremented AudioTimerCnt since the cnt register will be reset */

      /* clear CEN lead to resetting all register */
      CLEAR_BIT(RCC->ASCR, RCC_ASCR_CEN);
      WAIT_3_CYCLES();
      WRITE_REG(RCC->ASARR, AS_COMPARE_MAX);
      WRITE_REG(RCC->ASCR, ((CLOCK_PRESCALER-1) << RCC_ASCR_PSC_Pos) + (AS_CAPTURE_PRESCALER << RCC_ASCR_CPS_Pos));
      WRITE_REG(RCC->ASIER, RCC_ASIER_COIE | RCC_ASIER_CAIE);
      WRITE_REG(RCC->ASCOR, AS_COMPARE_MAX);

      SET_BIT(RCC->ASCR, RCC_ASCR_CEN);

      IsTimerAutoreloading = 1;

      CRITICAL_END();
    }
    else
    {
      AudioTimerCnt += (AS_COMPARE_MAX + 1);
    }
  }
}


void RCC_AUDIOSYNC_IRQHandler(void)
{
  uint32_t status_reg = READ_REG(RCC->ASSR);

  if (status_reg & RCC_ASIER_CAIE_Msk)
  {
    /*capture it*/
#if USE_SW_SYNC_METHOD == 0
    /* If the IP has been reset while the hw signal is on, it rings immediately and provide a wrong value */
    if (RCC->ASCAR != 0)
    {
      CODEC_CLK_Provide_ISO_Captured_Timestamp(AudioTimerCnt + RCC->ASCAR);
    }
#endif
    CLEAR_BIT(RCC->ASSR, RCC_ASIER_CAIE_Msk);
  }

  if (status_reg & RCC_ASIER_COIE_Msk)
  {
    /*compare it*/
    manage_compare_it();
    CLEAR_BIT(RCC->ASSR, RCC_ASIER_COIE_Msk);
  }

  if (status_reg & RCC_ASIER_CAEIE_Msk)
  {
    /*error*/
    CLEAR_BIT(RCC->ASSR, RCC_ASIER_CAEIE_Msk);
  }

  HAL_NVIC_ClearPendingIRQ(RCC_AUDIOSYNC_IRQn);
}

/******************************************************************************/
/********************USER SHOULD NOT MODIFY THIS SECTION***********************/
/******************************************************************************/
/*
 * If a feature of the LC3 codec is not used, LINK_LC3_ENCODER or LINK_LC3_DECODER
 * is set to zero, then the entry points of the library (defined as weak) are
 * redefined here for avoiding the linker to import code from the library
 */
/******************************************************************************/

#if (LINK_LC3_ENCODER == 0)
LC3_Status lc3_encoder_process(void *handle, void *input, uint32_t decimation, uint8_t *bytes)
{
  /* should not be reached */
  return LC3_UNKNOWN_ERROR;
}

LC3_Status lc3_encoder_channel_init(void* hSession, void* handle , uint32_t bitrate, LC3_SampleDepth s_bits)
{
  /* should not be reached */
  return LC3_UNKNOWN_ERROR;
}
#endif /* LINK_LC3_ENCODER */

#if (LINK_LC3_DECODER == 0)
LC3_Status lc3_decoder_process(void *handle, uint8_t *bytes, void *output, uint32_t decimation, uint32_t BFI_in)
{
  /* should not be reached */
  return LC3_UNKNOWN_ERROR;
}

LC3_Status lc3_decoder_channel_init(void *hSession, void *handle, uint32_t bitrate, LC3_SampleDepth s_bits)
{
  /* should not be reached */
  return LC3_UNKNOWN_ERROR;
}
#endif /* LINK_LC3_DECODER */
