/*
 * Copyright (c) 2009-2020 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.6.0 startup_ARMv81MML.c
 * Git SHA:
 */

#include "stm32wbaxx.h"
/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern void __PROGRAM_START(void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler  (void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
#define DEFAULT_IRQ_HANDLER(handler_name)  \
void handler_name(void); \
__WEAK void handler_name(void) { \
    while(1); \
}

/* Exceptions */
DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SecureFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)
DEFAULT_IRQ_HANDLER(PVD_AVD_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_S_IRQHandler)
DEFAULT_IRQ_HANDLER(TAMP_IRQHandler)
DEFAULT_IRQ_HANDLER(RAMCFG_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_IRQHandler)
DEFAULT_IRQ_HANDLER(FLASH_S_IRQHandler)
DEFAULT_IRQ_HANDLER(GTZC_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_S_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI0_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI2_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI3_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI4_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI5_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI6_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI7_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI8_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI9_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI10_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI11_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI12_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI13_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI14_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI15_IRQHandler)
DEFAULT_IRQ_HANDLER(IWDG_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel0_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel2_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel3_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel4_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel5_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel6_IRQHandler)
DEFAULT_IRQ_HANDLER(LPDMA1_Channel7_IRQHandler)
DEFAULT_IRQ_HANDLER(OTFDEC1_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(USART1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPUART1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM1_IRQHandler)
DEFAULT_IRQ_HANDLER(LPTIM2_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM16_IRQHandler)
DEFAULT_IRQ_HANDLER(TIM17_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_EV_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C3_ER_IRQHandler)
DEFAULT_IRQ_HANDLER(SAI1_IRQHandler)
DEFAULT_IRQ_HANDLER(AES_IRQHandler)
DEFAULT_IRQ_HANDLER(RNG_IRQHandler)
DEFAULT_IRQ_HANDLER(FPU_IRQHandler)
DEFAULT_IRQ_HANDLER(HASH_IRQHandler)
DEFAULT_IRQ_HANDLER(PKA_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI3_IRQHandler)
DEFAULT_IRQ_HANDLER(ICACHE_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC4_IRQHandler)
DEFAULT_IRQ_HANDLER(RADIO_IRQHandler)
DEFAULT_IRQ_HANDLER(WKUP_IRQHandler)
DEFAULT_IRQ_HANDLER(WKUP_S_IRQHandler)
DEFAULT_IRQ_HANDLER(RCC_AUDIOSYNC_IRQHandler)
DEFAULT_IRQ_HANDLER(USB_IRQHandler)
DEFAULT_IRQ_HANDLER(XSPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI19_RADIO_IO_IRQHandler)
DEFAULT_IRQ_HANDLER(EXTI20_RADIO_IO_IRQHandler)
/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const pFunc __VECTOR_TABLE[];
       const pFunc __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (pFunc)(&__INITIAL_SP),           /*      Initial Stack Pointer */
  Reset_Handler,                    /*      Reset Handler */
  NMI_Handler,                      /* -14: NMI Handler */
  HardFault_Handler,                /* -13: Hard Fault Handler */
  MemManage_Handler,                /* -12: MPU Fault Handler */
  BusFault_Handler,                 /* -11: Bus Fault Handler */
  UsageFault_Handler,               /* -10: Usage Fault Handler */
  SecureFault_Handler,              /*  -9: Secure Fault Handler */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  0,                                /*      Reserved */
  SVC_Handler,                      /*  -5: SVCall Handler */
  DebugMon_Handler,                 /*  -4: Debug Monitor Handler */
  0,                                /*      Reserved */
  PendSV_Handler,                   /*  -2: PendSV Handler */
  SysTick_Handler,                  /*  -1: SysTick Handler */
  0,                                /*      Reserved */
  PVD_AVD_IRQHandler,               /*   1: PVD/AVD through EXTI Line detection Interrupt */
  RTC_IRQHandler,                   /*   2: RTC non-secure interrupt */
  RTC_S_IRQHandler,                 /*   3: RTC secure interrupt */
  TAMP_IRQHandler,                  /*   4: Tamper non-secure interrupt  */
  RAMCFG_IRQHandler,                /*   5: RAMCFG global */
  FLASH_IRQHandler,                 /*   6: FLASH non-secure global interrupt */
  FLASH_S_IRQHandler,               /*   7: FLASH secure global interrupt */
  GTZC_IRQHandler,                  /*   8: Global TrustZone Controller interrupt */
  RCC_IRQHandler,                   /*   9: RCC non-secure global interrupt */
  RCC_S_IRQHandler,                 /*  10: RCC secure global interrupt */
  EXTI0_IRQHandler,                 /*  11: EXTI Line0 interrupt */
  EXTI1_IRQHandler,                 /*  12: EXTI Line1 interrupt */
  EXTI2_IRQHandler,                 /*  13: EXTI Line2 interrupt */
  EXTI3_IRQHandler,                 /*  14: EXTI Line3 interrupt */
  EXTI4_IRQHandler,                 /*  15: EXTI Line4 interrupt */
  EXTI5_IRQHandler,                 /*  16: EXTI Line5 interrupt */
  EXTI6_IRQHandler,                 /*  17: EXTI Line6 interrupt */
  EXTI7_IRQHandler,                 /*  18: EXTI Line7 interrupt */
  EXTI8_IRQHandler,                 /*  19: EXTI Line8 interrupt */
  EXTI9_IRQHandler,                 /*  20: EXTI Line9 interrupt */
  EXTI10_IRQHandler,                /*  21: EXTI Line10 interrupt */
  EXTI11_IRQHandler,                /*  22: EXTI Line11 interrupt */
  EXTI12_IRQHandler,                /*  23: EXTI Line12 interrupt */
  EXTI13_IRQHandler,                /*  24: EXTI Line13 interrupt */
  EXTI14_IRQHandler,                /*  25: EXTI Line14 interrupt */
  EXTI15_IRQHandler,                /*  26: EXTI Line15 interrupt */
  IWDG_IRQHandler,                  /*  27: IWDG global interrupt */
  LPDMA1_Channel0_IRQHandler,       /*  28: LPDMA1 Channel 0 global interrupt */
  LPDMA1_Channel1_IRQHandler,       /*  29: LPDMA1 Channel 1 global interrupt */
  LPDMA1_Channel2_IRQHandler,       /*  30: LPDMA1 Channel 2 global interrupt */
  LPDMA1_Channel3_IRQHandler,       /*  31: LPDMA1 Channel 3 global interrupt */
  LPDMA1_Channel4_IRQHandler,       /*  32: LPDMA1 Channel 4 global interrupt */
  LPDMA1_Channel5_IRQHandler,       /*  33: LPDMA1 Channel 5 global interrupt */
  LPDMA1_Channel6_IRQHandler,       /*  34: LPDMA1 Channel 6 global interrupt */
  LPDMA1_Channel7_IRQHandler,       /*  35: LPDMA1 Channel 7 global interrupt */
  OTFDEC1_IRQHandler,               /*  36: OTFDEC1 global interrupt */
  0,                                /*  37: Reserved */
  0,                                /*  38: Reserved */
  0,                                /*  39: Reserved */
  TIM2_IRQHandler,                  /*  40: TIM2 global interrupt */
  I2C1_EV_IRQHandler,               /*  41: I2C1 Event interrupt */
  I2C1_ER_IRQHandler,               /*  42: I2C1 Error interrupt */
  USART1_IRQHandler,                /*  43: USART1 global interrupt */
  LPUART1_IRQHandler,               /*  44: LPUART1 global interrupt */
  LPTIM1_IRQHandler,                /*  45: LPTIM1 global interrupt */
  LPTIM2_IRQHandler,                /*  46: LPTIM2 global interrupt */
  TIM16_IRQHandler,                 /*  47: TIM16 global interrupt */
  TIM17_IRQHandler,                 /*  48: TIM17 global interrupt */
  I2C3_EV_IRQHandler,               /*  49: I2C3 event */
  I2C3_ER_IRQHandler,               /*  50: I2C3 error */
  SAI1_IRQHandler,                  /*  51: Serial Audio Interface 1 global interrupt */
  AES_IRQHandler,                   /*  52: AES global interrupt */
  RNG_IRQHandler,                   /*  53: RNG global interrupt */
  FPU_IRQHandler,                   /*  54: FPU global interrupt */
  HASH_IRQHandler,                  /*  55: HASH global interrupt */
  PKA_IRQHandler,                   /*  56: PKA global interrupt */
  SPI3_IRQHandler,                  /*  57: SPI3 global interrupt */
  ICACHE_IRQHandler,                /*  58: Instruction cache global interrupt */
  ADC4_IRQHandler,                  /*  59: LP ADC (12bits) global interrupt */
  RADIO_IRQHandler,       			/*  60: 2.4GHz RADIO global interrupt */
  WKUP_IRQHandler,       			/*  61: PWR global WKUP pin interrupt */
  WKUP_S_IRQHandler,                /*  62: PWR secure global WKUP pin interrupt */
  RCC_AUDIOSYNC_IRQHandler,         /*  63: RCC audio synchronization interrupt */
  USB_IRQHandler,                   /*  64: USB global interrupt */
  XSPI1_IRQHandler,                 /*  65: XSPI1 global interrupt */
  EXTI19_RADIO_IO_IRQHandler,       /*  66: EXTI line 19 interrupt, 2.4 GHz RADIO io[x] */
  EXTI20_RADIO_IO_IRQHandler,       /*  67: EXTI line 20 interrupt, 2.4 GHz RADIO io[y] */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  __IO uint32_t tmp;

#endif
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  /* disable IRQ is removed */
  /*__disable_irq();*/
  /* Tamp IRQ prio is set to highest , and IRQ is enabled */
  NVIC_SetPriority(TAMP_IRQn, 4);
  NVIC_EnableIRQ(TAMP_IRQn);
#endif
  __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
  SCB->VTOR = (uint32_t) &__VECTOR_TABLE[0];
  /* Lock Secure Vector Table */
  /* Enable SYSCFG interface clock */
  RCC->APB7ENR |= RCC_APB7ENR_SYSCFGEN;
  /* Delay after an RCC peripheral clock enabling */
  tmp = RCC->APB7ENR;
  (void)tmp;
  SYSCFG->CSLCKR |= SYSCFG_CSLCKR_LOCKSVTAIRCR;
#endif

  SystemInit();                             /* CMSIS System Initialization */
  __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
