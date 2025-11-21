




 














 















 







 

 
 
           
                                    


      
 




 

 

 

 




 

 









 

 

 

 




 

 
 

 
 
 

 

 


 

 

 
 

 
 

 
 



 
 
 





 
 

 



 

 

 



 



 

 


 






 



 




 

 

 

 


 

 

 

 

 

 





 
 


 






 





 











 
 




 

 

 
 


 


                                      



 



















 

 


 

















 

 


 
 

 


 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 

 




 






 




 







 



 



 






 


 

 


 

 



 
 

 








 

 




 


 




 


 


 


 


 


 


















 

 


 


















 

 


 


























 



 



 





 



 



 




 




 
   



 



 



 























 





 




 




 


 
 
 

typedef enum
{
 
  Reset_IRQn                 = -15,     
  NonMaskableInt_IRQn        = -14,     
  HardFault_IRQn             = -13,     
  MemoryManagement_IRQn      = -12,    
 
  BusFault_IRQn              = -11,    
 
  UsageFault_IRQn            = -10,     
  SVCall_IRQn                =  -5,     
  DebugMonitor_IRQn          =  -4,     
  PendSV_IRQn                =  -2,     
  SysTick_IRQn               =  -1,     

 
  PVD_PVM_IRQn               = 0,       
  DTS_IRQn                   = 2,       
  IWDG_IRQn                  = 3,       
  WWDG_IRQn                  = 4,       
  RCC_IRQn                   = 5,       
  FLASH_IRQn                 = 8,       
  RAMECC_IRQn                = 9,       
  FPU_IRQn                   = 10,      
  TAMP_IRQn                  = 13,      
  EXTI0_IRQn                 = 16,      
  EXTI1_IRQn                 = 17,      
  EXTI2_IRQn                 = 18,      
  EXTI3_IRQn                 = 19,      
  EXTI4_IRQn                 = 20,      
  EXTI5_IRQn                 = 21,      
  EXTI6_IRQn                 = 22,      
  EXTI7_IRQn                 = 23,      
  EXTI8_IRQn                 = 24,      
  EXTI9_IRQn                 = 25,      
  EXTI10_IRQn                = 26,      
  EXTI11_IRQn                = 27,      
  EXTI12_IRQn                = 28,      
  EXTI13_IRQn                = 29,      
  EXTI14_IRQn                = 30,      
  EXTI15_IRQn                = 31,      
  RTC_IRQn                   = 32,      
  SAES_IRQn                  = 33,      
  CRYP_IRQn                  = 34,      
  PKA_IRQn                   = 35,      
  HASH_IRQn                  = 36,      
  RNG_IRQn                   = 37,      
  ADC1_2_IRQn                = 38,      
  GPDMA1_Channel0_IRQn       = 39,      
  GPDMA1_Channel1_IRQn       = 40,      
  GPDMA1_Channel2_IRQn       = 41,      
  GPDMA1_Channel3_IRQn       = 42,      
  GPDMA1_Channel4_IRQn       = 43,      
  GPDMA1_Channel5_IRQn       = 44,      
  GPDMA1_Channel6_IRQn       = 45,      
  GPDMA1_Channel7_IRQn       = 46,      
  TIM1_BRK_IRQn              = 47,      
  TIM1_UP_IRQn               = 48,      
  TIM1_TRG_COM_IRQn          = 49,      
  TIM1_CC_IRQn               = 50,      
  TIM2_IRQn                  = 51,      
  TIM3_IRQn                  = 52,      
  TIM4_IRQn                  = 53,      
  TIM5_IRQn                  = 54,      
  TIM6_IRQn                  = 55,      
  TIM7_IRQn                  = 56,      
  TIM9_IRQn                  = 57,      
  SPI1_IRQn                  = 58,      
  SPI2_IRQn                  = 59,      
  SPI3_IRQn                  = 60,      
  SPI4_IRQn                  = 61,      
  SPI5_IRQn                  = 62,      
  SPI6_IRQn                  = 63,      
  HPDMA1_Channel0_IRQn       = 64,      
  HPDMA1_Channel1_IRQn       = 65,      
  HPDMA1_Channel2_IRQn       = 66,      
  HPDMA1_Channel3_IRQn       = 67,      
  HPDMA1_Channel4_IRQn       = 68,      
  HPDMA1_Channel5_IRQn       = 69,      
  HPDMA1_Channel6_IRQn       = 70,      
  HPDMA1_Channel7_IRQn       = 71,      
  SAI1_A_IRQn                = 72,      
  SAI1_B_IRQn                = 73,      
  SAI2_A_IRQn                = 74,      
  SAI2_B_IRQn                = 75,      
  I2C1_EV_IRQn               = 76,      
  I2C1_ER_IRQn               = 77,      
  I2C2_EV_IRQn               = 78,      
  I2C2_ER_IRQn               = 79,      
  I2C3_EV_IRQn               = 80,      
  I2C3_ER_IRQn               = 81,      
  USART1_IRQn                = 82,      
  USART2_IRQn                = 83,      
  USART3_IRQn                = 84,      
  UART4_IRQn                 = 85,      
  UART5_IRQn                 = 86,      
  UART7_IRQn                 = 87,      
  UART8_IRQn                 = 88,      
  I3C1_EV_IRQn               = 89,      
  I3C1_ER_IRQn               = 90,      
  OTG_HS_IRQn                = 91,      
  ETH_IRQn                   = 92,      
  CORDIC_IRQn                = 93,      
  GFXTIM_IRQn                = 94,      
  DCMIPP_IRQn                = 95,      
  LTDC_IRQn                  = 96,      
  LTDC_ER_IRQn               = 97,      
  DMA2D_IRQn                 = 98,      
  JPEG_IRQn                  = 99,      
  GFXMMU_IRQn                = 100,     
  I3C1_WKUP_IRQn             = 101,     
  MCE1_IRQn                  = 102,     
  MCE2_IRQn                  = 103,     
  MCE3_IRQn                  = 104,     
  XSPI1_IRQn                 = 105,     
  XSPI2_IRQn                 = 106,     
  FMC_IRQn                   = 107,     
  SDMMC1_IRQn                = 108,     
  SDMMC2_IRQn                = 109,     
  OTG_FS_IRQn                = 112,     
  TIM12_IRQn                 = 113,     
  TIM13_IRQn                 = 114,     
  TIM14_IRQn                 = 115,     
  TIM15_IRQn                 = 116,     
  TIM16_IRQn                 = 117,     
  TIM17_IRQn                 = 118,     
  LPTIM1_IRQn                = 119,     
  LPTIM2_IRQn                = 120,     
  LPTIM3_IRQn                = 121,     
  LPTIM4_IRQn                = 122,     
  LPTIM5_IRQn                = 123,     
  SPDIF_RX_IRQn              = 124,     
  MDIOS_IRQn                 = 125,     
  ADF1_FLT0_IRQn             = 126,     
  CRS_IRQn                   = 127,     
  UCPD1_IRQn                 = 128,     
  CEC_IRQn                   = 129,     
  PSSI_IRQn                  = 130,     
  LPUART1_IRQn               = 131,     
  WAKEUP_PIN_IRQn            = 132,     
  GPDMA1_Channel8_IRQn       = 133,     
  GPDMA1_Channel9_IRQn       = 134,     
  GPDMA1_Channel10_IRQn      = 135,     
  GPDMA1_Channel11_IRQn      = 136,     
  GPDMA1_Channel12_IRQn      = 137,     
  GPDMA1_Channel13_IRQn      = 138,     
  GPDMA1_Channel14_IRQn      = 139,     
  GPDMA1_Channel15_IRQn      = 140,     
  HPDMA1_Channel8_IRQn       = 141,     
  HPDMA1_Channel9_IRQn       = 142,     
  HPDMA1_Channel10_IRQn      = 143,     
  HPDMA1_Channel11_IRQn      = 144,     
  HPDMA1_Channel12_IRQn      = 145,     
  HPDMA1_Channel13_IRQn      = 146,     
  HPDMA1_Channel14_IRQn      = 147,     
  HPDMA1_Channel15_IRQn      = 148,     
  GPU2D_IRQn                 = 149,     
  GPU2D_ER_IRQn              = 150,     
  ICACHE_IRQn                = 151,     
  FDCAN1_IT0_IRQn            = 152,     
  FDCAN1_IT1_IRQn            = 153,     
  FDCAN2_IT0_IRQn            = 154,     
  FDCAN2_IT1_IRQn            = 155      
} IRQn_Type;


 
 
 



 

   


 




 
















 

  #pragma system_include          


 
 

  #pragma system_include

 
 

 

  #pragma system_include














 


 
 


  #pragma system_include

 



 

 

 

 
#pragma rtmodel = "__dlib_version", "6"

 


 



























 


  #pragma system_include

 
 
 


  #pragma system_include

 

   

 
 


   #pragma system_include






 




 


 


 


 

 


 

 

 

 

 

 

 

 

 

 
















 



















 











 























 





 



 










 














 













 








 













 













 















 











 








 








 






 





 












 





 













 






 


   


  







 







 




 






 




 




 













 

   




 







 







 







 










 





 

















 


 


 













 

   


 


 



 

 

 
  typedef unsigned int _Wchart;
  typedef unsigned int _Wintt;

 

 
typedef unsigned int     _Sizet;

 
typedef signed char   __int8_t;
typedef unsigned char  __uint8_t;
typedef signed short int   __int16_t;
typedef unsigned short int  __uint16_t;
typedef signed int   __int32_t;
typedef unsigned int  __uint32_t;
   typedef signed long long int   __int64_t;
   typedef unsigned long long int  __uint64_t;
typedef signed int   __intptr_t;
typedef unsigned int  __uintptr_t;

 
typedef struct _Mbstatet
{  
    unsigned int _Wchar;   
    unsigned int _State;   

} _Mbstatet;

 

 
  typedef struct __va_list __Va_list;



 
typedef struct
{
    long long _Off;     
  _Mbstatet _Wstate;
} _Fpost;


 

 
  
   
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Locksyslock_StaticGuard(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Malloc(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Stream(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_Debug(void);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlocksyslock_StaticGuard(void);


  typedef void *__iar_Rmtx;

  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Initdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Dstdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Lockdynamiclock(__iar_Rmtx *);
  _Pragma("object_attribute = __weak") __intrinsic __nounwind void __iar_Unlockdynamiclock(__iar_Rmtx *);

  






 


 
  typedef signed char          int8_t;
  typedef unsigned char        uint8_t;

  typedef signed short int         int16_t;
  typedef unsigned short int       uint16_t;

  typedef signed int         int32_t;
  typedef unsigned int       uint32_t;

  typedef signed long long int         int64_t;
  typedef unsigned long long int       uint64_t;


 
typedef signed char      int_least8_t;
typedef unsigned char    uint_least8_t;

typedef signed short int     int_least16_t;
typedef unsigned short int   uint_least16_t;

typedef signed int     int_least32_t;
typedef unsigned int   uint_least32_t;

 
  typedef signed long long int   int_least64_t;
  typedef unsigned long long int uint_least64_t;



 
typedef signed int       int_fast8_t;
typedef unsigned int     uint_fast8_t;

typedef signed int      int_fast16_t;
typedef unsigned int    uint_fast16_t;

typedef signed int      int_fast32_t;
typedef unsigned int    uint_fast32_t;

  typedef signed long long int    int_fast64_t;
  typedef unsigned long long int  uint_fast64_t;

 
typedef signed long long int          intmax_t;
typedef unsigned long long int        uintmax_t;


 
typedef signed int          intptr_t;
typedef unsigned int        uintptr_t;

 
typedef int __data_intptr_t; typedef unsigned int __data_uintptr_t;

 






















 











 














 




 



 

 




 
















 

  #pragma system_include          


 

 




 


 




 
















 





 
 




 
























#pragma system_include






 

 
















#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic uint16_t __iar_uint16_read(void const *ptr)
{
  return *(__packed uint16_t*)(ptr);
}
#pragma language=restore


#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic void __iar_uint16_write(void const *ptr, uint16_t val)
{
  *(__packed uint16_t*)(ptr) = val;;
}
#pragma language=restore

#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic uint32_t __iar_uint32_read(void const *ptr)
{
  return *(__packed uint32_t*)(ptr);
}
#pragma language=restore

#pragma language=save
#pragma language=extended
_Pragma("inline=forced") __intrinsic void __iar_uint32_write(void const *ptr, uint32_t val)
{
  *(__packed uint32_t*)(ptr) = val;;
}
#pragma language=restore

#pragma language=save
#pragma language=extended
__packed struct  __iar_u32 { uint32_t v; };
#pragma language=restore




















 



  #pragma system_include



 


 


#pragma language=save
#pragma language=extended

__intrinsic __nounwind void    __iar_builtin_no_operation(void);

__intrinsic __nounwind void    __iar_builtin_disable_interrupt(void);
__intrinsic __nounwind void    __iar_builtin_enable_interrupt(void);

typedef unsigned int __istate_t;

__intrinsic __nounwind __istate_t __iar_builtin_get_interrupt_state(void);
__intrinsic __nounwind void __iar_builtin_set_interrupt_state(__istate_t);

 
__intrinsic __nounwind unsigned int __iar_builtin_get_PSR( void );
__intrinsic __nounwind unsigned int __iar_builtin_get_IPSR( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_MSP( void );
__intrinsic __nounwind void         __iar_builtin_set_MSP( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_PSP( void );
__intrinsic __nounwind void         __iar_builtin_set_PSP( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_PRIMASK( void );
__intrinsic __nounwind void         __iar_builtin_set_PRIMASK( unsigned int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_CONTROL( void );
__intrinsic __nounwind void         __iar_builtin_set_CONTROL( unsigned int );

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_FAULTMASK( void );
__intrinsic __nounwind void         __iar_builtin_set_FAULTMASK(unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_get_BASEPRI( void );
__intrinsic __nounwind void         __iar_builtin_set_BASEPRI( unsigned int );

 
__intrinsic __nounwind void __iar_builtin_disable_irq(void);
__intrinsic __nounwind void __iar_builtin_enable_irq(void);

__intrinsic __nounwind void __iar_builtin_disable_fiq(void);
__intrinsic __nounwind void __iar_builtin_enable_fiq(void);


 

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SWP( unsigned int, volatile unsigned int * );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned char __iar_builtin_SWPB( unsigned char, volatile unsigned char * );

typedef unsigned int __ul;
typedef unsigned int __iar_builtin_uint;


 

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_CDP (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRd, unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opc2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_CDP2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRd, unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opc2);

 
__intrinsic __nounwind void          __iar_builtin_MCR( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1, __iar_builtin_uint src,
                                unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind unsigned int __iar_builtin_MRC( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1,
                                unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind void          __iar_builtin_MCR2( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1, __iar_builtin_uint src,
                                 unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );
__intrinsic __nounwind unsigned int __iar_builtin_MRC2( unsigned __constrange(0,15) coproc, unsigned __constrange(0,7) opcode_1,
                                 unsigned __constrange(0,15) CRn, unsigned __constrange(0,15) CRm, unsigned __constrange(0,7) opcode_2 );

__intrinsic __nounwind void __iar_builtin_MCRR (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned long long src, unsigned __constrange(0,15) CRm);
__intrinsic __nounwind void __iar_builtin_MCRR2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned long long src, unsigned __constrange(0,15) CRm);

__intrinsic __nounwind unsigned long long __iar_builtin_MRRC (unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRm);
__intrinsic __nounwind unsigned long long __iar_builtin_MRRC2(unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) opc1, unsigned __constrange(0,15) CRm);

 
__intrinsic __nounwind void __iar_builtin_LDC  ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDCL ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDC2 ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);
__intrinsic __nounwind void __iar_builtin_LDC2L( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src);

 
__intrinsic __nounwind void __iar_builtin_STC  ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STCL ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STC2 ( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);
__intrinsic __nounwind void __iar_builtin_STC2L( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst);

 
__intrinsic __nounwind void __iar_builtin_LDC_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                              unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDCL_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDC2_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_LDC2L_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint const *src,
                                unsigned __constrange(0,255) option);

 
__intrinsic __nounwind void __iar_builtin_STC_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                              unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STCL_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STC2_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                               unsigned __constrange(0,255) option);

__intrinsic __nounwind void __iar_builtin_STC2L_noidx( unsigned __constrange(0,15) coproc, unsigned __constrange(0,15) CRn, volatile __iar_builtin_uint *dst,
                                unsigned __constrange(0,255) option);


 
__intrinsic __nounwind unsigned int       __iar_builtin_rsr(__spec_string const char * special_register);
__intrinsic __nounwind unsigned long long __iar_builtin_rsr64(__spec_string const char * special_register);
__intrinsic __nounwind void*              __iar_builtin_rsrp(__spec_string const char * special_register);

 
__intrinsic __nounwind void __iar_builtin_wsr(__spec_string const char * special_register, unsigned int value);
__intrinsic __nounwind void __iar_builtin_wsr64(__spec_string const char * special_register, unsigned long long value);
__intrinsic __nounwind void __iar_builtin_wsrp(__spec_string const char * special_register, const void *value);

 
__intrinsic __nounwind unsigned int __iar_builtin_get_APSR( void );
__intrinsic __nounwind void         __iar_builtin_set_APSR( unsigned int );

 
__intrinsic __nounwind unsigned int __iar_builtin_get_CPSR( void );
__intrinsic __nounwind void         __iar_builtin_set_CPSR( unsigned int );

 
__intrinsic __nounwind unsigned int __iar_builtin_get_FPSCR( void );
__intrinsic __nounwind void __iar_builtin_set_FPSCR( unsigned int );

 
 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CLZ(unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_ROR(unsigned int, unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_RRX(unsigned int);

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QADD( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDADD( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QSUB( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDSUB( signed int, signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_QDOUBLE( signed int );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int        __iar_builtin_QFlag( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int   __iar_builtin_acle_QFlag(void);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void  __iar_builtin_set_QFlag(int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void  __iar_builtin_ignore_QFlag(void);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int         __iar_builtin_QCFlag( void );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind void __iar_builtin_reset_QC_flag( void );

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_SMUL( signed short, signed short );

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_REV( unsigned int );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind signed int __iar_builtin_REVSH( short );

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_REV16( unsigned int );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_RBIT( unsigned int );

__intrinsic __nounwind unsigned char  __iar_builtin_LDREXB( volatile unsigned char const * );
__intrinsic __nounwind unsigned short __iar_builtin_LDREXH( volatile unsigned short const * );
__intrinsic __nounwind unsigned int  __iar_builtin_LDREX ( volatile unsigned int const * );
__intrinsic __nounwind unsigned long long __iar_builtin_LDREXD( volatile unsigned long long const * );

__intrinsic __nounwind unsigned int  __iar_builtin_STREXB( unsigned char, volatile unsigned char * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREXH( unsigned short, volatile unsigned short * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREX ( unsigned int, volatile unsigned int * );
__intrinsic __nounwind unsigned int  __iar_builtin_STREXD( unsigned long long, volatile unsigned long long * );

__intrinsic __nounwind void __iar_builtin_CLREX( void );

__intrinsic __nounwind void __iar_builtin_SEV( void );
__intrinsic __nounwind void __iar_builtin_WFE( void );
__intrinsic __nounwind void __iar_builtin_WFI( void );
__intrinsic __nounwind void __iar_builtin_YIELD( void );

__intrinsic __nounwind void __iar_builtin_PLI( volatile void const * );
__intrinsic __nounwind void __iar_builtin_PLD( volatile void const * );

__intrinsic __nounwind void __iar_builtin_PLIx( volatile void const *, unsigned int __constrange(0,2), unsigned int __constrange(0,1));
__intrinsic __nounwind void __iar_builtin_PLDx( volatile void const *, unsigned int __constrange(0, 1), unsigned int __constrange(0, 2), unsigned int __constrange(0, 1));
__intrinsic __nounwind void __iar_builtin_PLDW( volatile void const * );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind signed int   __iar_builtin_SSAT     (signed int val, unsigned int __constrange( 1, 32 ) sat );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAT     (signed int val, unsigned int __constrange( 0, 31 ) sat );

 
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SEL( unsigned int op1, unsigned int op2 );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADD8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADD16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUB8    (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUB16   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UADDSUBX (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USUBADDX (unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADD8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADD16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUB8   (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUB16  (unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQADDSUBX(unsigned int pair1, unsigned int pair2);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSUBADDX(unsigned int pair1, unsigned int pair2);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAD8(unsigned int x, unsigned int y );
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USADA8(unsigned int x, unsigned int y,
                                   unsigned int acc );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSAT16   (unsigned int pair,
                                      unsigned int __constrange( 1, 16 ) sat );
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAT16   (unsigned int pair,
                                      unsigned int __constrange( 0, 15 ) sat );

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUAD (unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUSD (unsigned int x, unsigned int y);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUADX(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMUSDX(unsigned int x, unsigned int y);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAD (unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLSD (unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLADX(unsigned int x, unsigned int y, int sum);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLSDX(unsigned int x, unsigned int y, int sum);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALD (unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALDX(unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLSLD (unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLSLDX(unsigned int pair1,
                                 unsigned int pair2,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_PKHBT(unsigned int x,
                                  unsigned int y,
                                  unsigned __constrange(0,31) count);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_PKHTB(unsigned int x,
                                  unsigned int y,
                                  unsigned __constrange(0,32) count);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLABB(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLABT(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLATB(unsigned int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLATT(unsigned int x, unsigned int y, int acc);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAWB(int x, unsigned int y, int acc);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMLAWT(int x, unsigned int y, int acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLA (int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLAR(int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLS (int x, int y, int acc);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMLSR(int x, int y, int acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMUL (int x, int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMMULR(int x, int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULBB(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULBT(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULTB(unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULTT(unsigned int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULWB(int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SMULWT(int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SXTAB (int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind int __iar_builtin_SXTAH (int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAB (unsigned int x, unsigned int y);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAH (unsigned int x, unsigned int y);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned long long __iar_builtin_UMAAL(unsigned int x,
                                       unsigned int y,
                                       unsigned int a,
                                       unsigned int b);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALBB(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALBT(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALTB(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind long long __iar_builtin_SMLALTT(unsigned int x,
                                 unsigned int y,
                                 long long acc);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTB16(unsigned int x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UXTAB16(unsigned int acc, unsigned int x);

_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SXTB16(unsigned int x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SXTAB16(unsigned int acc, unsigned int x);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SSAX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHASX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_SHSAX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_QSAX(unsigned int, unsigned int);

_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_USAX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHASX(unsigned int, unsigned int);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UHSAX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQASX(unsigned int, unsigned int);
_Pragma("function_effects = hidden_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_UQSAX(unsigned int, unsigned int);

 
__intrinsic __nounwind void __iar_builtin_DMB(void);
__intrinsic __nounwind void __iar_builtin_DSB(void);
__intrinsic __nounwind void __iar_builtin_ISB(void);
__intrinsic __nounwind void __iar_builtin_DMBx(unsigned int __constrange(1, 15));
__intrinsic __nounwind void __iar_builtin_DSBx(unsigned int __constrange(1, 15));
__intrinsic __nounwind void __iar_builtin_ISBx(unsigned int __constrange(1, 15));

 
__intrinsic __nounwind unsigned int __iar_builtin_TT(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTT(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTA(unsigned int);
__intrinsic __nounwind unsigned int __iar_builtin_TTAT(unsigned int);

__intrinsic __nounwind unsigned int __iar_builtin_get_SB(void);
__intrinsic __nounwind void __iar_builtin_set_SB(unsigned int);

__intrinsic __nounwind unsigned int __get_LR(void);
__intrinsic __nounwind void __set_LR(unsigned int);

__intrinsic __nounwind unsigned int __get_SP(void);
__intrinsic __nounwind void __set_SP(unsigned int);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VSQRT_F32(float x);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VSQRT_F64(double x);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFMA_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFMS_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFNMA_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VFNMS_F32(float x, float y, float z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFMA_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFMS_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFNMA_F64(double x, double y, double z);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VFNMS_F64(double x, double y, double z);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32B(unsigned int crc, unsigned char data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32H(unsigned int crc, unsigned short data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32W(unsigned int crc, unsigned int data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CB(unsigned int crc, unsigned char data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CH(unsigned int crc, unsigned short data);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind unsigned int __iar_builtin_CRC32CW(unsigned int crc, unsigned int data);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VMAXNM_F32(float a, float b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VMINNM_F32(float a, float b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VMAXNM_F64(double a, double b);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VMINNM_F64(double a, double b);

 
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTA_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTM_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTN_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTP_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTX_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTR_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind float __iar_builtin_VRINTZ_F32(float a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTA_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTM_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTN_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTP_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTX_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTR_F64(double a);
_Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind double __iar_builtin_VRINTZ_F64(double a);

#pragma language=restore









    

    



_Pragma("inline=forced") static inline void __set_CONTROL(uint32_t control)
{
  __iar_builtin_wsr("CONTROL", control);
  __iar_builtin_ISB();
}


    
    


_Pragma("inline=forced") static inline void __TZ_set_CONTROL_NS(uint32_t control)
{
  __iar_builtin_wsr("CONTROL_NS", control);
  __iar_builtin_ISB();
}


    







  _Pragma("inline=forced") __intrinsic int16_t __REVSH(int16_t val)
  {
    return (int16_t) __iar_builtin_REVSH(val);
  }












  _Pragma("inline=forced") __intrinsic uint8_t __LDRBT(volatile uint8_t *addr)
  {
    uint32_t res;
    __asm volatile ("LDRBT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return ((uint8_t)res);
  }

  _Pragma("inline=forced") __intrinsic uint16_t __LDRHT(volatile uint16_t *addr)
  {
    uint32_t res;
    __asm volatile ("LDRHT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return ((uint16_t)res);
  }

  _Pragma("inline=forced") __intrinsic uint32_t __LDRT(volatile uint32_t *addr)
  {
    uint32_t res;
    __asm volatile ("LDRT %0, [%1]" : "=r" (res) : "r" (addr) : "memory");
    return res;
  }

  _Pragma("inline=forced") __intrinsic void __STRBT(uint8_t value, volatile uint8_t *addr)
  {
    __asm volatile ("STRBT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
  }

  _Pragma("inline=forced") __intrinsic void __STRHT(uint16_t value, volatile uint16_t *addr)
  {
    __asm volatile ("STRHT %1, [%0]" : : "r" (addr), "r" ((uint32_t)value) : "memory");
  }

  _Pragma("inline=forced") __intrinsic void __STRT(uint32_t value, volatile uint32_t *addr)
  {
    __asm volatile ("STRT %1, [%0]" : : "r" (addr), "r" (value) : "memory");
  }




#pragma diag_default=Pe940
#pragma diag_default=Pe177







 










 

 






 

 

 













 



 






 



 
typedef union
{
  struct
  {
    uint32_t _reserved0:16;               
    uint32_t GE:4;                        
    uint32_t _reserved1:7;                
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} APSR_Type;

 









 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:23;               
  } b;                                    
  uint32_t w;                             
} IPSR_Type;

 




 
typedef union
{
  struct
  {
    uint32_t ISR:9;                       
    uint32_t _reserved0:1;                
    uint32_t ICI_IT_1:6;                  
    uint32_t GE:4;                        
    uint32_t _reserved1:4;                
    uint32_t T:1;                         
    uint32_t ICI_IT_2:2;                  
    uint32_t Q:1;                         
    uint32_t V:1;                         
    uint32_t C:1;                         
    uint32_t Z:1;                         
    uint32_t N:1;                         
  } b;                                    
  uint32_t w;                             
} xPSR_Type;

 













 
typedef union
{
  struct
  {
    uint32_t nPRIV:1;                     
    uint32_t SPSEL:1;                     
    uint32_t FPCA:1;                      
    uint32_t _reserved0:29;               
  } b;                                    
  uint32_t w;                             
} CONTROL_Type;

 



 







 



 
typedef struct
{
  volatile uint32_t ISER[8U];                
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];                
        uint32_t RESERVED1[24U];
  volatile uint32_t ISPR[8U];                
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];                
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];                
        uint32_t RESERVED4[56U];
  volatile uint8_t  IP[240U];                
        uint32_t RESERVED5[644U];
  volatile  uint32_t STIR;                    
}  NVIC_Type;

 

 







 



 
typedef struct
{
  volatile const  uint32_t CPUID;                   
  volatile uint32_t ICSR;                    
  volatile uint32_t VTOR;                    
  volatile uint32_t AIRCR;                   
  volatile uint32_t SCR;                     
  volatile uint32_t CCR;                     
  volatile uint8_t  SHPR[12U];               
  volatile uint32_t SHCSR;                   
  volatile uint32_t CFSR;                    
  volatile uint32_t HFSR;                    
  volatile uint32_t DFSR;                    
  volatile uint32_t MMFAR;                   
  volatile uint32_t BFAR;                    
  volatile uint32_t AFSR;                    
  volatile const  uint32_t ID_PFR[2U];              
  volatile const  uint32_t ID_DFR;                  
  volatile const  uint32_t ID_AFR;                  
  volatile const  uint32_t ID_MFR[4U];              
  volatile const  uint32_t ID_ISAR[5U];             
        uint32_t RESERVED0[1U];
  volatile const  uint32_t CLIDR;                   
  volatile const  uint32_t CTR;                     
  volatile const  uint32_t CCSIDR;                  
  volatile uint32_t CSSELR;                  
  volatile uint32_t CPACR;                   
        uint32_t RESERVED3[93U];
  volatile  uint32_t STIR;                    
        uint32_t RESERVED4[15U];
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
        uint32_t RESERVED5[1U];
  volatile  uint32_t ICIALLU;                 
        uint32_t RESERVED6[1U];
  volatile  uint32_t ICIMVAU;                 
  volatile  uint32_t DCIMVAC;                 
  volatile  uint32_t DCISW;                   
  volatile  uint32_t DCCMVAU;                 
  volatile  uint32_t DCCMVAC;                 
  volatile  uint32_t DCCSW;                   
  volatile  uint32_t DCCIMVAC;                
  volatile  uint32_t DCCISW;                  
  volatile  uint32_t BPIALL;                  
        uint32_t RESERVED7[5U];
  volatile uint32_t ITCMCR;                  
  volatile uint32_t DTCMCR;                  
  volatile uint32_t AHBPCR;                  
  volatile uint32_t CACR;                    
  volatile uint32_t AHBSCR;                  
        uint32_t RESERVED8[1U];
  volatile uint32_t ABFSR;                   
} SCB_Type;

 





 










 

 







 



 









 














 



 






 







 






 



 





 


 





 







 


 

 


 


 


 




 




 


 




 



 






 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const  uint32_t ICTR;                    
  volatile uint32_t ACTLR;                   
} SCnSCB_Type;

 

 











 







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t LOAD;                    
  volatile uint32_t VAL;                     
  volatile const  uint32_t CALIB;                   
} SysTick_Type;

 




 

 

 



 







 



 
typedef struct
{
  volatile  union
  {
    volatile  uint8_t    u8;                  
    volatile  uint16_t   u16;                 
    volatile  uint32_t   u32;                 
  }  PORT [32U];                          
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;                     
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;                     
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;                     
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
        uint32_t RESERVED5[6U];
  volatile const  uint32_t PID4;                    
  volatile const  uint32_t PID5;                    
  volatile const  uint32_t PID6;                    
  volatile const  uint32_t PID7;                    
  volatile const  uint32_t PID0;                    
  volatile const  uint32_t PID1;                    
  volatile const  uint32_t PID2;                    
  volatile const  uint32_t PID3;                    
  volatile const  uint32_t CID0;                    
  volatile const  uint32_t CID1;                    
  volatile const  uint32_t CID2;                    
  volatile const  uint32_t CID3;                    
} ITM_Type;

 

 









 



   







 



 
typedef struct
{
  volatile uint32_t CTRL;                    
  volatile uint32_t CYCCNT;                  
  volatile uint32_t CPICNT;                  
  volatile uint32_t EXCCNT;                  
  volatile uint32_t SLEEPCNT;                
  volatile uint32_t LSUCNT;                  
  volatile uint32_t FOLDCNT;                 
  volatile const  uint32_t PCSR;                    
  volatile uint32_t COMP0;                   
  volatile uint32_t MASK0;                   
  volatile uint32_t FUNCTION0;               
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;                   
  volatile uint32_t MASK1;                   
  volatile uint32_t FUNCTION1;               
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;                   
  volatile uint32_t MASK2;                   
  volatile uint32_t FUNCTION2;               
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;                   
  volatile uint32_t MASK3;                   
  volatile uint32_t FUNCTION3;               
        uint32_t RESERVED3[981U];
  volatile  uint32_t LAR;                     
  volatile const  uint32_t LSR;                     
} DWT_Type;

 


















 

 

 

 

 

 

 









   







 



 
typedef struct
{
  volatile const  uint32_t SSPSR;                   
  volatile uint32_t CSPSR;                   
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;                    
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;                    
        uint32_t RESERVED2[131U];
  volatile const  uint32_t FFSR;                    
  volatile uint32_t FFCR;                    
  volatile const  uint32_t FSCR;                    
        uint32_t RESERVED3[759U];
  volatile const  uint32_t TRIGGER;                 
  volatile const  uint32_t FIFO0;                   
  volatile const  uint32_t ITATBCTR2;               
        uint32_t RESERVED4[1U];
  volatile const  uint32_t ITATBCTR0;               
  volatile const  uint32_t FIFO1;                   
  volatile uint32_t ITCTRL;                  
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;                
  volatile uint32_t CLAIMCLR;                
        uint32_t RESERVED7[8U];
  volatile const  uint32_t DEVID;                   
  volatile const  uint32_t DEVTYPE;                 
} TPI_Type;

 

 

 




 


 

 







 


 







 


 

 






 


   







 



 
typedef struct
{
  volatile const  uint32_t TYPE;                    
  volatile uint32_t CTRL;                    
  volatile uint32_t RNR;                     
  volatile uint32_t RBAR;                    
  volatile uint32_t RASR;                    
  volatile uint32_t RBAR_A1;                 
  volatile uint32_t RASR_A1;                 
  volatile uint32_t RBAR_A2;                 
  volatile uint32_t RASR_A2;                 
  volatile uint32_t RBAR_A3;                 
  volatile uint32_t RASR_A3;                 
} MPU_Type;


 



 



 

 



 










 







 



 
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;                   
  volatile uint32_t FPCAR;                   
  volatile uint32_t FPDSCR;                  
  volatile const  uint32_t MVFR0;                   
  volatile const  uint32_t MVFR1;                   
  volatile const  uint32_t MVFR2;                   
} FPU_Type;

 









 

 




 








 




 


 







 



 
typedef struct
{
  volatile uint32_t DHCSR;                   
  volatile  uint32_t DCRSR;                   
  volatile uint32_t DCRDR;                   
  volatile uint32_t DEMCR;                   
} CoreDebug_Type;

 












 


 













 







 






 






 

 







 

 




 










 


 



 





 





 










 
static inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);              

  reg_value  =  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;                                                    
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));  
  reg_value  =  (reg_value                                   |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U)  );               
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR =  reg_value;
}






 
static inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}







 
static inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    __asm volatile("":::"memory");
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __asm volatile("":::"memory");
  }
}









 
static inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __iar_builtin_DSB();
    __iar_builtin_ISB();
  }
}









 
static inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}







 
static inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}







 
static inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}









 
static inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}










 
static inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4U)) & (uint32_t)0xFFUL);
  }
}










 
static inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)]                >> (8U - 4U)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHPR[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4U)));
  }
}












 
static inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}












 
static inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);    
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4U)) ? (uint32_t)(4U) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(4U)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4U));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}










 
static inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  vectors[(int32_t)IRQn + 16] = vector;
  __iar_builtin_DSB();
}









 
static inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t *vectors = (uint32_t *)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return vectors[(int32_t)IRQn + 16];
}





 
__attribute__((__noreturn__)) static inline void __NVIC_SystemReset(void)
{
  __iar_builtin_DSB();                                                          
 
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR  = (uint32_t)((0x5FAUL << 16U)    |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U)    );          
  __iar_builtin_DSB();                                                           

  for(;;)                                                            
  {
    __iar_builtin_no_operation();
  }
}

 


 







 
















 
 
  #pragma system_include          
 







 








   









 












                          







  









  











  



 



 



 



 




 
typedef struct {
  uint32_t RBAR; 
  uint32_t RASR; 
} ARM_MPU_Region_t;
    


 
static inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  __iar_builtin_DMB();
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);
  __iar_builtin_DSB();
  __iar_builtin_ISB();
}


 
static inline void ARM_MPU_Disable(void)
{
  __iar_builtin_DMB();
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL  &= ~(1UL );
  __iar_builtin_DSB();
  __iar_builtin_ISB();
}



 
static inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}




    
static inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





    
static inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}





 
static inline void ARM_MPU_OrderedMemcpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i) 
  {
    dst[i] = src[i];
  }
}




 
static inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt) 
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}




 





 








 
static inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->MVFR0;
  if      ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x220U)
  {
    return 2U;            
  }
  else if ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;            
  }
  else
  {
    return 0U;            
  }
}

 


 






 
















 

  #pragma system_include          







 

 






 
_Pragma("inline=forced") static inline void SCB_EnableICache (void)
{
    if (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR & (1UL << 17U)) return;   

    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 17U);   
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_DisableICache (void)
{
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 17U);   
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;                      
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_InvalidateICache (void)
{
    __iar_builtin_DSB();
    __iar_builtin_ISB();
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIALLU = 0UL;
    __iar_builtin_DSB();
    __iar_builtin_ISB();
}









 
_Pragma("inline=forced") static inline void SCB_InvalidateICache_by_Addr (volatile void *addr, int32_t isize)
{
    if ( isize > 0 ) {
       int32_t op_size = isize + (((uint32_t)addr) & (32U - 1U));
      uint32_t op_addr = (uint32_t)addr  ;

      __iar_builtin_DSB();

      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->ICIMVAU = op_addr;              
        op_addr += 32U;
        op_size -= 32U;
      } while ( op_size > 0 );

      __iar_builtin_DSB();
      __iar_builtin_ISB();
    }
}





 
_Pragma("inline=forced") static inline void SCB_EnableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    if (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR & (1UL << 16U)) return;   

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;                        
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);
    __iar_builtin_DSB();

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR |=  (uint32_t)(1UL << 16U);   

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_DisableDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;                        
    __iar_builtin_DSB();

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCR &= ~(uint32_t)(1UL << 16U);   
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_InvalidateDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;                        
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_CleanDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;                        
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCSW = (((sets << 5U) & (0x1FFUL << 5U)) |
                      ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}





 
_Pragma("inline=forced") static inline void SCB_CleanInvalidateDCache (void)
{
    uint32_t ccsidr;
    uint32_t sets;
    uint32_t ways;

    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CSSELR = 0U;                        
    __iar_builtin_DSB();

    ccsidr = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->CCSIDR;

                                             
    sets = (uint32_t)((((ccsidr) & (0x7FFFUL << 13U) ) >> 13U ));
    do {
      ways = (uint32_t)((((ccsidr) & (0x3FFUL << 3U)) >> 3U));
      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCISW = (((sets << 5U) & (0x1FFUL << 5U)) |
                       ((ways << 30U) & (3UL << 30U))  );
      } while (ways-- != 0U);
    } while(sets-- != 0U);

    __iar_builtin_DSB();
    __iar_builtin_ISB();
}









 
_Pragma("inline=forced") static inline void SCB_InvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (32U - 1U));
      uint32_t op_addr = (uint32_t)addr  ;

      __iar_builtin_DSB();

      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCIMVAC = op_addr;              
        op_addr += 32U;
        op_size -= 32U;
      } while ( op_size > 0 );

      __iar_builtin_DSB();
      __iar_builtin_ISB();
    }
}









 
_Pragma("inline=forced") static inline void SCB_CleanDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (32U - 1U));
      uint32_t op_addr = (uint32_t)addr  ;

      __iar_builtin_DSB();

      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCMVAC = op_addr;              
        op_addr += 32U;
        op_size -= 32U;
      } while ( op_size > 0 );

      __iar_builtin_DSB();
      __iar_builtin_ISB();
    }
}









 
_Pragma("inline=forced") static inline void SCB_CleanInvalidateDCache_by_Addr (volatile void *addr, int32_t dsize)
{
    if ( dsize > 0 ) {
       int32_t op_size = dsize + (((uint32_t)addr) & (32U - 1U));
      uint32_t op_addr = (uint32_t)addr  ;

      __iar_builtin_DSB();

      do {
        ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->DCCIMVAC = op_addr;             
        op_addr +=          32U;
        op_size -=          32U;
      } while ( op_size > 0 );

      __iar_builtin_DSB();
      __iar_builtin_ISB();
    }
}

 



 





 












 
static inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);                                                    
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD  = (uint32_t)(ticks - 1UL);                          
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4U) - 1UL);  
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL   = 0UL;                                              
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL  = (1UL << 2U) |
                   (1UL << 1U)   |
                   (1UL );                          
  return (0UL);                                                      
}


 



 





 

extern volatile int32_t ITM_RxBuffer;                               









 
static inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&       
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL               ) != 0UL)   )      
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __iar_builtin_no_operation();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}







 
static inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;                            

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);        
  }

  return (ch);
}







 
static inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);                               
  }
  else
  {
    return (1);                               
  }
}

 






















 



 



 





 


 



 
  






 
extern uint32_t SystemCoreClock;         



 



 





 
extern void SystemInit (void);






 
extern void SystemCoreClockUpdate (void);




 





 



 


 
 
 




 



 

typedef struct
{
  volatile uint32_t ISR;           
  volatile uint32_t IER;           
  volatile uint32_t CR;            
  volatile uint32_t CFGR;          
  volatile uint32_t CFGR2;         
  volatile uint32_t SMPR1;         
  volatile uint32_t SMPR2;         
       uint32_t RESERVED1;     
  volatile uint32_t TR1;           
  volatile uint32_t TR2;           
  volatile uint32_t TR3;           
       uint32_t RESERVED2;     
  volatile uint32_t SQR1;          
  volatile uint32_t SQR2;          
  volatile uint32_t SQR3;          
  volatile uint32_t SQR4;          
  volatile uint32_t DR;            
       uint32_t RESERVED3;     
       uint32_t RESERVED4;     
  volatile uint32_t JSQR;          
       uint32_t RESERVED5[4];  
  volatile uint32_t OFR1;          
  volatile uint32_t OFR2;          
  volatile uint32_t OFR3;          
  volatile uint32_t OFR4;          
       uint32_t RESERVED6[4];  
  volatile uint32_t JDR1;          
  volatile uint32_t JDR2;          
  volatile uint32_t JDR3;          
  volatile uint32_t JDR4;          
       uint32_t RESERVED7[4];  
  volatile uint32_t AWD2CR;        
  volatile uint32_t AWD3CR;        
       uint32_t RESERVED8;     
       uint32_t RESERVED9;     
  volatile uint32_t DIFSEL;        
  volatile uint32_t CALFACT;       
       uint32_t RESERVED10[4]; 
  volatile uint32_t OR;            
} ADC_TypeDef;

typedef struct
{
  volatile uint32_t CSR;           
  uint32_t      RESERVED1;     
  volatile uint32_t CCR;           
  volatile uint32_t CDR;           
} ADC_Common_TypeDef;



 
typedef struct
{
  uint32_t RESERVED1[9];        
  volatile uint32_t FNMOD2;         
  uint32_t RESERVED3[54];       
  volatile uint32_t READQOS;        
  volatile uint32_t WRITEQOS;       
  volatile uint32_t FNMOD;          
} AXIM_ASIB_TypeDef;

typedef struct
{
  uint32_t RESERVED1[2];        
  volatile uint32_t FNMODBMISS;     
  uint32_t RESERVED2[6];        
  volatile uint32_t FNMOD2;         
  uint32_t RESERVED3[1];        
  volatile uint32_t FNMODLB;        
  uint32_t RESERVED5[54];       
  volatile uint32_t FNMOD;          
} AXIM_AMIB_TypeDef;



 

typedef struct
{
  volatile uint32_t CSR;          
  volatile uint32_t CCR;          
} VREFBUF_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t CFGR;           
  volatile uint32_t TXDR;           
  volatile uint32_t RXDR;           
  volatile uint32_t ISR;            
  volatile uint32_t IER;            
} CEC_TypeDef;



 
typedef struct
{
  volatile uint32_t CSR;            
  volatile uint32_t WDATA;          
  volatile uint32_t RDATA;          
} CORDIC_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t CGCR;           
  volatile uint32_t TCR;            
  volatile uint32_t TDR;            
  volatile uint32_t EVCR;           
  volatile uint32_t EVSR;           
  uint32_t RESERVED1[2];        
  volatile uint32_t WDGTCR;         
  uint32_t RESERVED2[3];        
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
  volatile uint32_t IER;            
  volatile uint32_t TSR;            
  volatile uint32_t LCCRR;          
  volatile uint32_t FCCRR;          
  uint32_t RESERVED3[2];        
  volatile uint32_t ATR;            
  volatile uint32_t AFCR;           
  volatile uint32_t ALCR;           
  uint32_t RESERVED4[1];        
  volatile uint32_t AFCC1R;         
  uint32_t RESERVED5[3];        
  volatile uint32_t ALCC1R;         
  volatile uint32_t ALCC2R;         
  uint32_t RESERVED6[2];        
  volatile uint32_t RFC1R;          
  volatile uint32_t RFC1RR;         
  volatile uint32_t RFC2R;          
  volatile uint32_t RFC2RR;         
  uint32_t RESERVED7[4];        
  volatile uint32_t WDGCR;          
  volatile uint32_t WDGRR;          
  volatile uint32_t WDGPAR;         
  uint32_t RESERVED8[209];      
  volatile uint32_t HWCFGR;         
  volatile uint32_t VERR;           
  volatile uint32_t IPIDR;          
  volatile uint32_t SIDR;           
} GFXTIM_TypeDef;




 
typedef struct
{
  volatile uint32_t DR;             
  volatile uint32_t IDR;            
  volatile uint32_t CR;             
       uint32_t RESERVED1;      
  volatile uint32_t INIT;           
  volatile uint32_t POL;            
} CRC_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t CFGR;           
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
} CRS_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;          
  volatile uint32_t SR;          
  volatile uint32_t DIN;         
  volatile uint32_t DOUT;        
  volatile uint32_t DMACR;       
  volatile uint32_t IMSCR;       
  volatile uint32_t RISR;        
  volatile uint32_t MISR;        
  volatile uint32_t K0LR;        
  volatile uint32_t K0RR;        
  volatile uint32_t K1LR;        
  volatile uint32_t K1RR;        
  volatile uint32_t K2LR;        
  volatile uint32_t K2RR;        
  volatile uint32_t K3LR;        
  volatile uint32_t K3RR;        
  volatile uint32_t IV0LR;       
  volatile uint32_t IV0RR;       
  volatile uint32_t IV1LR;       
  volatile uint32_t IV1RR;       
  volatile uint32_t CSGCMCCM0R;  
  volatile uint32_t CSGCMCCM1R;  
  volatile uint32_t CSGCMCCM2R;  
  volatile uint32_t CSGCMCCM3R;  
  volatile uint32_t CSGCMCCM4R;  
  volatile uint32_t CSGCMCCM5R;  
  volatile uint32_t CSGCMCCM6R;  
  volatile uint32_t CSGCMCCM7R;  
  volatile uint32_t CSGCM0R;     
  volatile uint32_t CSGCM1R;     
  volatile uint32_t CSGCM2R;     
  volatile uint32_t CSGCM3R;     
  volatile uint32_t CSGCM4R;     
  volatile uint32_t CSGCM5R;     
  volatile uint32_t CSGCM6R;     
  volatile uint32_t CSGCM7R;     
} CRYP_TypeDef;



 
typedef struct
{
  volatile uint32_t IDCODE;         
  volatile uint32_t CR;             
  uint32_t RESERVED1[5];        
  volatile uint32_t AHB5FZR;        
  uint32_t RESERVED2;           
  volatile uint32_t AHB1FZR;        
  uint32_t RESERVED3[5];        
  volatile uint32_t APB1FZR;        
  uint32_t RESERVED4[3];        
  volatile uint32_t APB2FZR;        
  uint32_t RESERVED5;           
  volatile uint32_t APB4FZR;        
  uint32_t RESERVED6[41];       
  volatile uint32_t SR;             
  volatile uint32_t DBG_AUTH_HOST;  
  volatile uint32_t DBG_AUTH_DEV;   
  volatile uint32_t DBG_AUTH_ACK;   
  uint32_t RESERVED7[945];      
  volatile uint32_t PIDR4;          
  uint32_t RESERVED8[3];        
  volatile uint32_t PIDR0;          
  volatile uint32_t PIDR1;          
  volatile uint32_t PIDR2;          
  volatile uint32_t PIDR3;          
  volatile uint32_t CIDR0;          
  volatile uint32_t CIDR1;          
  volatile uint32_t CIDR2;          
  volatile uint32_t CIDR3;          
} DBGMCU_TypeDef;



 
typedef struct
{
  volatile uint32_t IPGR1;          
  volatile uint32_t IPGR2;          
  volatile uint32_t IPGR3;          
  uint32_t RESERVED1[4];        
  volatile uint32_t IPGR8;          
  volatile uint32_t IPC1R1;         
  volatile uint32_t IPC1R2;         
  volatile uint32_t IPC1R3;         
  volatile uint32_t RESERVED2[54];  
  volatile uint32_t PRCR;           
  volatile uint32_t PRESCR;         
  volatile uint32_t PRESUR;         
  volatile uint32_t RESERVED3[57];  
  volatile uint32_t PRIER;          
  volatile uint32_t PRSR;           
  volatile uint32_t PRFCR;          
  uint32_t RESERVED4;           
  volatile uint32_t CMCR;           
  volatile uint32_t CMFRCR;         
  volatile uint32_t RESERVED5[121];  
  volatile uint32_t CMIER;          
  volatile uint32_t CMSR1;          
  volatile uint32_t CMSR2;          
  volatile uint32_t CMFCR;          
  uint32_t RESERVED6;           
  volatile uint32_t P0FSCR;         
  volatile uint32_t RESERVED7[62];  
  volatile uint32_t P0FCTCR;        
  volatile uint32_t P0SCSTR;        
  volatile uint32_t P0SCSZR;        
  volatile uint32_t RESERVED8[41];  
  volatile uint32_t P0DCCNTR;       
  volatile uint32_t P0DCLMTR;       
  volatile uint32_t RESERVED9[2];   
  volatile uint32_t P0PPCR;         
  volatile uint32_t P0PPM0AR1;      
  volatile uint32_t P0PPM0AR2;      
  volatile uint32_t RESERVED10[10];  
  volatile uint32_t P0IER;          
  volatile uint32_t P0SR;           
  volatile uint32_t P0FCR;          
  volatile uint32_t RESERVED11[64];  
  volatile uint32_t P0CFCTCR;       
  volatile uint32_t P0CSCSTR;       
  volatile uint32_t P0CSCSZR;       
  volatile uint32_t RESERVED12[45];  
  volatile uint32_t P0CPPCR;        
  volatile uint32_t P0CPPM0AR1;     
  volatile uint32_t P0CPPM0AR2;     
} DCMIPP_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;    
  volatile uint32_t CFGR;  
} DLYB_TypeDef;



 
typedef struct
{
       uint32_t RESERVED1;      
  volatile uint32_t PRIVCFGR;       
  volatile uint32_t RCFGLOCKR;      
  volatile uint32_t MISR;           
} DMA_TypeDef;

typedef struct
{
  volatile uint32_t CLBAR;          
       uint32_t RESERVED1[2];   
  volatile uint32_t CFCR;           
  volatile uint32_t CSR;            
  volatile uint32_t CCR;            
       uint32_t RESERVED2[10];  
  volatile uint32_t CTR1;           
  volatile uint32_t CTR2;           
  volatile uint32_t CBR1;           
  volatile uint32_t CSAR;           
  volatile uint32_t CDAR;           
  volatile uint32_t CTR3;           
  volatile uint32_t CBR2;           
       uint32_t RESERVED3[8];   
  volatile uint32_t CLLR;           
} DMA_Channel_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t ISR;            
  volatile uint32_t IFCR;           
  volatile uint32_t FGMAR;          
  volatile uint32_t FGOR;           
  volatile uint32_t BGMAR;          
  volatile uint32_t BGOR;           
  volatile uint32_t FGPFCCR;        
  volatile uint32_t FGCOLR;         
  volatile uint32_t BGPFCCR;        
  volatile uint32_t BGCOLR;         
  volatile uint32_t FGCMAR;         
  volatile uint32_t BGCMAR;         
  volatile uint32_t OPFCCR;         
  volatile uint32_t OCOLR;          
  volatile uint32_t OMAR;           
  volatile uint32_t OOR;            
  volatile uint32_t NLR;            
  volatile uint32_t LWR;            
  volatile uint32_t AMTCR;          
  uint32_t      RESERVED[236];  
  volatile uint32_t FGCLUT[256];    
  volatile uint32_t BGCLUT[256];    
} DMA2D_TypeDef;



 
typedef struct
{
  volatile uint32_t CFGR1;          
  uint32_t RESERVED0;           
  volatile uint32_t T0VALR1;        
  uint32_t RESERVED1;           
  volatile uint32_t RAMPVALR;       
  volatile uint32_t ITR1;           
  uint32_t RESERVED2;           
  volatile uint32_t DR;             
  volatile uint32_t SR;             
  volatile uint32_t ITENR;          
  volatile uint32_t ICIFR;          
  volatile uint32_t OR;             
} DTS_TypeDef;



 
typedef struct
{
  volatile uint32_t MACCR;
  volatile uint32_t MACECR;
  volatile uint32_t MACPFR;
  volatile uint32_t MACWTR;
  volatile uint32_t MACHT0R;
  volatile uint32_t MACHT1R;
  uint32_t      RESERVED1[14];
  volatile uint32_t MACVTR;
  uint32_t      RESERVED2;
  volatile uint32_t MACVHTR;
  uint32_t      RESERVED3;
  volatile uint32_t MACVIR;
  volatile uint32_t MACIVIR;
  uint32_t      RESERVED4[2];
  volatile uint32_t MACTFCR;
  uint32_t      RESERVED5[7];
  volatile uint32_t MACRFCR;
  uint32_t      RESERVED6[7];
  volatile uint32_t MACISR;
  volatile uint32_t MACIER;
  volatile uint32_t MACRXTXSR;
  uint32_t      RESERVED7;
  volatile uint32_t MACPCSR;
  volatile uint32_t MACRWKPFR;
  uint32_t      RESERVED8[2];
  volatile uint32_t MACLCSR;
  volatile uint32_t MACLTCR;
  volatile uint32_t MACLETR;
  volatile uint32_t MAC1USTCR;
  uint32_t      RESERVED9[12];
  volatile uint32_t MACVR;
  volatile uint32_t MACDR;
  uint32_t      RESERVED10;
  volatile uint32_t MACHWF0R;
  volatile uint32_t MACHWF1R;
  volatile uint32_t MACHWF2R;
  uint32_t      RESERVED11[54];
  volatile uint32_t MACMDIOAR;
  volatile uint32_t MACMDIODR;
  uint32_t      RESERVED12[2];
  volatile uint32_t MACARPAR;
  uint32_t      RESERVED13[59];
  volatile uint32_t MACA0HR;
  volatile uint32_t MACA0LR;
  volatile uint32_t MACA1HR;
  volatile uint32_t MACA1LR;
  volatile uint32_t MACA2HR;
  volatile uint32_t MACA2LR;
  volatile uint32_t MACA3HR;
  volatile uint32_t MACA3LR;
  uint32_t      RESERVED14[248];
  volatile uint32_t MMCCR;
  volatile uint32_t MMCRIR;
  volatile uint32_t MMCTIR;
  volatile uint32_t MMCRIMR;
  volatile uint32_t MMCTIMR;
  uint32_t      RESERVED15[14];
  volatile uint32_t MMCTSCGPR;
  volatile uint32_t MMCTMCGPR;
  uint32_t      RESERVED16[5];
  volatile uint32_t MMCTPCGR;
  uint32_t      RESERVED17[10];
  volatile uint32_t MMCRCRCEPR;
  volatile uint32_t MMCRAEPR;
  uint32_t      RESERVED18[10];
  volatile uint32_t MMCRUPGR;
  uint32_t      RESERVED19[9];
  volatile uint32_t MMCTLPIMSTR;
  volatile uint32_t MMCTLPITCR;
  volatile uint32_t MMCRLPIMSTR;
  volatile uint32_t MMCRLPITCR;
  uint32_t      RESERVED20[65];
  volatile uint32_t MACL3L4C0R;
  volatile uint32_t MACL4A0R;
  uint32_t      RESERVED21[2];
  volatile uint32_t MACL3A0R0R;
  volatile uint32_t MACL3A1R0R;
  volatile uint32_t MACL3A2R0R;
  volatile uint32_t MACL3A3R0R;
  uint32_t      RESERVED22[4];
  volatile uint32_t MACL3L4C1R;
  volatile uint32_t MACL4A1R;
  uint32_t      RESERVED23[2];
  volatile uint32_t MACL3A0R1R;
  volatile uint32_t MACL3A1R1R;
  volatile uint32_t MACL3A2R1R;
  volatile uint32_t MACL3A3R1R;
  uint32_t      RESERVED24[108];
  volatile uint32_t MACTSCR;
  volatile uint32_t MACSSIR;
  volatile uint32_t MACSTSR;
  volatile uint32_t MACSTNR;
  volatile uint32_t MACSTSUR;
  volatile uint32_t MACSTNUR;
  volatile uint32_t MACTSAR;
  uint32_t      RESERVED25;
  volatile uint32_t MACTSSR;
  uint32_t      RESERVED26[3];
  volatile uint32_t MACTTSSNR;
  volatile uint32_t MACTTSSSR;
  uint32_t      RESERVED27[2];
  volatile uint32_t MACACR;
  uint32_t      RESERVED28;
  volatile uint32_t MACATSNR;
  volatile uint32_t MACATSSR;
  volatile uint32_t MACTSIACR;
  volatile uint32_t MACTSEACR;
  volatile uint32_t MACTSICNR;
  volatile uint32_t MACTSECNR;
  uint32_t      RESERVED29[4];
  volatile uint32_t MACPPSCR;
  uint32_t      RESERVED30[3];
  volatile uint32_t MACPPSTTSR;
  volatile uint32_t MACPPSTTNR;
  volatile uint32_t MACPPSIR;
  volatile uint32_t MACPPSWR;
  uint32_t      RESERVED31[12];
  volatile uint32_t MACPOCR;
  volatile uint32_t MACSPI0R;
  volatile uint32_t MACSPI1R;
  volatile uint32_t MACSPI2R;
  volatile uint32_t MACLMIR;
  uint32_t      RESERVED32[11];
  volatile uint32_t MTLOMR;
  uint32_t      RESERVED33[7];
  volatile uint32_t MTLISR;
  uint32_t      RESERVED34[55];
  volatile uint32_t MTLTQOMR;
  volatile uint32_t MTLTQUR;
  volatile uint32_t MTLTQDR;
  uint32_t      RESERVED35[8];
  volatile uint32_t MTLQICSR;
  volatile uint32_t MTLRQOMR;
  volatile uint32_t MTLRQMPOCR;
  volatile uint32_t MTLRQDR;
  uint32_t      RESERVED36[177];
  volatile uint32_t DMAMR;
  volatile uint32_t DMASBMR;
  volatile uint32_t DMAISR;
  volatile uint32_t DMADSR;
  uint32_t      RESERVED37[60];
  volatile uint32_t DMACCR;
  volatile uint32_t DMACTCR;
  volatile uint32_t DMACRCR;
  uint32_t      RESERVED38[2];
  volatile uint32_t DMACTDLAR;
  uint32_t      RESERVED39;
  volatile uint32_t DMACRDLAR;
  volatile uint32_t DMACTDTPR;
  uint32_t      RESERVED40;
  volatile uint32_t DMACRDTPR;
  volatile uint32_t DMACTDRLR;
  volatile uint32_t DMACRDRLR;
  volatile uint32_t DMACIER;
  volatile uint32_t DMACRIWTR;
volatile uint32_t DMACSFCSR;
  uint32_t      RESERVED41;
  volatile uint32_t DMACCATDR;
  uint32_t      RESERVED42;
  volatile uint32_t DMACCARDR;
  uint32_t      RESERVED43;
  volatile uint32_t DMACCATBR;
  uint32_t      RESERVED44;
  volatile uint32_t DMACCARBR;
  volatile uint32_t DMACSR;
uint32_t      RESERVED45[2];
volatile uint32_t DMACMFCR;
}ETH_TypeDef;



 
typedef struct
{
  volatile uint32_t RTSR1;          
  volatile uint32_t FTSR1;          
  volatile uint32_t SWIER1;         
       uint32_t RESERVED1[5];   
  volatile uint32_t RTSR2;          
  volatile uint32_t FTSR2;          
  volatile uint32_t SWIER2;         
       uint32_t RESERVED2[21];  
  volatile uint32_t IMR1;           
  volatile uint32_t EMR1;           
  volatile uint32_t PR1;            
       uint32_t RESERVED3;      
  volatile uint32_t IMR2;           
  volatile uint32_t EMR2;           
  volatile uint32_t PR2;            
       uint32_t RESERVED4;      
  volatile uint32_t IMR3;           
  volatile uint32_t EMR3;           
} EXTI_TypeDef;



 
typedef struct
{
  volatile uint32_t CREL;           
  volatile uint32_t ENDN;           
       uint32_t RESERVED1;      
  volatile uint32_t DBTP;           
  volatile uint32_t TEST;           
  volatile uint32_t RWD;            
  volatile uint32_t CCCR;           
  volatile uint32_t NBTP;           
  volatile uint32_t TSCC;           
  volatile uint32_t TSCV;           
  volatile uint32_t TOCC;           
  volatile uint32_t TOCV;           
       uint32_t RESERVED2[4];   
  volatile uint32_t ECR;            
  volatile uint32_t PSR;            
  volatile uint32_t TDCR;           
       uint32_t RESERVED3;      
  volatile uint32_t IR;             
  volatile uint32_t IE;             
  volatile uint32_t ILS;            
  volatile uint32_t ILE;            
       uint32_t RESERVED4[8];   
  volatile uint32_t RXGFC;          
  volatile uint32_t XIDAM;          
  volatile uint32_t HPMS;           
       uint32_t RESERVED5;      
  volatile uint32_t RXF0S;          
  volatile uint32_t RXF0A;          
  volatile uint32_t RXF1S;          
  volatile uint32_t RXF1A;          
       uint32_t RESERVED6[8];   
  volatile uint32_t TXBC;           
  volatile uint32_t TXFQS;          
  volatile uint32_t TXBRP;          
  volatile uint32_t TXBAR;          
  volatile uint32_t TXBCR;          
  volatile uint32_t TXBTO;          
  volatile uint32_t TXBCF;          
  volatile uint32_t TXBTIE;         
  volatile uint32_t TXBCIE;         
  volatile uint32_t TXEFS;          
  volatile uint32_t TXEFA;          
} FDCAN_GlobalTypeDef;



 
typedef struct
{
  volatile uint32_t CKDIV;          
       uint32_t RESERVED1[128]; 
  volatile uint32_t OPTR;           
       uint32_t RESERVED2[58];  
  volatile uint32_t HWCFG;          
  volatile uint32_t VERR;           
  volatile uint32_t IPIDR;          
  volatile uint32_t SIDR;           
} FDCAN_Config_TypeDef;



 
typedef struct
{
  volatile uint32_t ACR;            
  volatile uint32_t KEYR;           
       uint32_t RESERVED1[2];   
  volatile uint32_t CR;             
  volatile uint32_t SR;             
       uint32_t RESERVED2[2];   
  volatile uint32_t IER;            
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
       uint32_t RESERVED3;      
  volatile uint32_t CRCCR;          
  volatile uint32_t CRCSADDR;       
  volatile uint32_t CRCEADDR;       
  volatile uint32_t CRCDATAR;       
  volatile uint32_t ECCSFADDR;      
  volatile uint32_t ECCDFADDR;      
       uint32_t RESERVED4[46];  
  volatile uint32_t OPTKEYR;        
  volatile uint32_t OPTCR;          
  volatile uint32_t OPTISR;         
  volatile uint32_t OPTICR;         
  volatile uint32_t OBKCR;          
       uint32_t RESERVED5;      
  volatile uint32_t OBKDR[8];       
       uint32_t RESERVED6[50];  
  volatile uint32_t NVSR;           
  volatile uint32_t NVSRP;          
  volatile uint32_t ROTSR;          
  volatile uint32_t ROTSRP;         
  volatile uint32_t OTPLSR;         
  volatile uint32_t OTPLSRP;        
  volatile uint32_t WRPSR;          
  volatile uint32_t WRPSRP;         
       uint32_t RESERVED7[4];   
  volatile uint32_t HDPSR;          
  volatile uint32_t HDPSRP;         
       uint32_t RESERVED8[6];   
  volatile uint32_t EPOCHSR;        
  volatile uint32_t EPOCHSRP;       
       uint32_t RESERVED9[2];   
  volatile uint32_t OBW1SR;         
  volatile uint32_t OBW1SRP;        
  volatile uint32_t OBW2SR;         
  volatile uint32_t OBW2SRP;        
} FLASH_TypeDef;



 
typedef struct {
  volatile uint32_t BTCR[8];        
} FMC_Bank1_TypeDef;



 
typedef struct {
  volatile uint32_t BWTR[7];        
} FMC_Bank1E_TypeDef;



 
typedef struct {
  volatile uint32_t PCR;            
  volatile uint32_t SR;             
  volatile uint32_t PMEM;           
  volatile uint32_t PATT;           
       uint32_t RESERVED0;      
  volatile uint32_t ECCR;           
} FMC_Bank3_TypeDef;



 
typedef struct
{
  volatile uint32_t SDCR[2];        
  volatile uint32_t SDTR[2];        
  volatile uint32_t SDCMR;          
  volatile uint32_t SDRTR;          
  volatile uint32_t SDSR;           
} FMC_Bank5_6_TypeDef;



 
typedef struct
{
  volatile uint32_t MODER;          
  volatile uint32_t OTYPER;         
  volatile uint32_t OSPEEDR;        
  volatile uint32_t PUPDR;          
  volatile uint32_t IDR;            
  volatile uint32_t ODR;            
  volatile uint32_t BSRR;           
  volatile uint32_t LCKR;           
  volatile uint32_t AFR[2];         
  volatile uint32_t BRR;            
} GPIO_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;               
  volatile uint32_t SR;               
  volatile uint32_t FCR;              
       uint32_t RESERVED0;        
  volatile uint32_t DVR;              
  volatile uint32_t DAR;              
       uint32_t RESERVED1[2];     
  volatile uint32_t B0CR;             
  volatile uint32_t B1CR;             
  volatile uint32_t B2CR;             
  volatile uint32_t B3CR;             
       uint32_t RESERVED2[1012];  
  volatile uint32_t LUT[2048];       
 
} GFXMMU_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;                
  volatile uint32_t DIN;               
  volatile uint32_t STR;               
  volatile uint32_t HR[5];             
  volatile uint32_t IMR;               
  volatile uint32_t SR;                
       uint32_t RESERVED[52];      
  volatile uint32_t CSR[103];          
} HASH_TypeDef;



 
typedef struct
{
  volatile uint32_t HR[16];             
} HASH_DIGEST_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;          
  volatile uint32_t CR2;          
  volatile uint32_t OAR1;         
  volatile uint32_t OAR2;         
  volatile uint32_t TIMINGR;      
  volatile uint32_t TIMEOUTR;     
  volatile uint32_t ISR;          
  volatile uint32_t ICR;          
  volatile uint32_t PECR;         
  volatile uint32_t RXDR;         
  volatile uint32_t TXDR;         
} I2C_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;              
  volatile uint32_t CFGR;            
  uint32_t      RESERVED1[2];    
  volatile uint32_t RDR;             
  volatile uint32_t RDWR;            
  volatile uint32_t TDR;             
  volatile uint32_t TDWR;            
  volatile uint32_t IBIDR;           
  volatile uint32_t TGTTDR;          
  uint32_t      RESERVED2[2];    
  volatile uint32_t SR;              
  volatile uint32_t SER;             
  uint32_t      RESERVED3[2];    
  volatile uint32_t RMR;             
  uint32_t      RESERVED4[3];    
  volatile uint32_t EVR;             
  volatile uint32_t IER;             
  volatile uint32_t CEVR;            
  uint32_t RESERVED5;            
  volatile uint32_t DEVR0;           
  volatile uint32_t DEVRX[4];        
  uint32_t      RESERVED6[7];    
  volatile uint32_t MAXRLR;          
  volatile uint32_t MAXWLR;          
  uint32_t      RESERVED7[2];    
  volatile uint32_t TIMINGR0;        
  volatile uint32_t TIMINGR1;        
  volatile uint32_t TIMINGR2;        
  uint32_t      RESERVED9[5];    
  volatile uint32_t BCR;             
  volatile uint32_t DCR;             
  volatile uint32_t GETCAPR;         
  volatile uint32_t CRCAPR;          
  volatile uint32_t GETMXDSR;        
  volatile uint32_t EPIDR;           
} I3C_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;              
  volatile uint32_t SR;              
  volatile uint32_t IER;             
  volatile uint32_t FCR;             
  volatile uint32_t HMONR;           
  volatile uint32_t MMONR;           
} ICACHE_TypeDef;



 
typedef struct
{
  volatile uint32_t KR;             
  volatile uint32_t PR;             
  volatile uint32_t RLR;            
  volatile uint32_t SR;             
  volatile uint32_t WINR;           
  volatile uint32_t EWCR;           
} IWDG_TypeDef;



 
typedef struct
{
  volatile uint32_t CONFR0;           
  volatile uint32_t CONFR1;           
  volatile uint32_t CONFR2;           
  volatile uint32_t CONFR3;           
  volatile uint32_t CONFR4;           
  volatile uint32_t CONFR5;           
  volatile uint32_t CONFR6;           
  volatile uint32_t CONFR7;           
       uint32_t RESERVED0[4];     
  volatile uint32_t CR;               
  volatile uint32_t SR;               
  volatile uint32_t CFR;              
       uint32_t RESERVED1;        
  volatile uint32_t DIR;              
  volatile uint32_t DOR;              
       uint32_t RESERVED2[2];     
  volatile uint32_t QMEM0[16];        
  volatile uint32_t QMEM1[16];        
  volatile uint32_t QMEM2[16];        
  volatile uint32_t QMEM3[16];        
  volatile uint32_t HUFFMIN[16];      
  volatile uint32_t HUFFBASE[32];     
  volatile uint32_t HUFFSYMB[84];     
  volatile uint32_t DHTMEM[103];      
       uint32_t RESERVED3;        
  volatile uint32_t HUFFENC_AC0[88];  
  volatile uint32_t HUFFENC_AC1[88];  
  volatile uint32_t HUFFENC_DC0[8];   
  volatile uint32_t HUFFENC_DC1[8];   
} JPEG_TypeDef;



 
typedef struct
{
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
  volatile uint32_t DIER;           
  volatile uint32_t CFGR;           
  volatile uint32_t CR;             
  volatile uint32_t CCR1;           
  volatile uint32_t ARR;            
  volatile uint32_t CNT;            
  volatile uint32_t RESERVED0;      
  volatile uint32_t CFGR2;          
  volatile uint32_t RCR;            
  volatile uint32_t CCMR1;          
  volatile uint32_t RESERVED1;      
  volatile uint32_t CCR2;           
} LPTIM_TypeDef;



 
typedef struct
{
  volatile uint32_t REGCR;             
  volatile uint32_t SADDR;             
  volatile uint32_t EADDR;             
  volatile uint32_t ATTR;              
} MCE_Region_TypeDef;

typedef struct
{
  volatile uint32_t CCCFGR;            
  volatile uint32_t CCNR0;             
  volatile uint32_t CCNR1;             
  volatile uint32_t CCKEYR0;           
  volatile uint32_t CCKEYR1;           
  volatile uint32_t CCKEYR2;           
  volatile uint32_t CCKEYR3;           
} MCE_Context_TypeDef;

typedef struct
{
  volatile uint32_t CR;                
  volatile uint32_t SR;                
  volatile uint32_t IASR;              
  volatile uint32_t IACR;              
  volatile uint32_t IAIER;             
  uint32_t RESERVED1[2];           
  volatile uint32_t PRIVCFGR;          
  volatile uint32_t IAESR;             
  volatile uint32_t IADDR;             
  uint32_t RESERVED2[118];         
  volatile uint32_t MKEYR0;            
  volatile uint32_t MKEYR1;            
  volatile uint32_t MKEYR2;            
  volatile uint32_t MKEYR3;            
  uint32_t RESERVED3[4];           
  volatile uint32_t FMKEYR0;           
  volatile uint32_t FMKEYR1;           
  volatile uint32_t FMKEYR2;           
  volatile uint32_t FMKEYR3;           
} MCE_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t WRFR;
  volatile uint32_t CWRFR;
  volatile uint32_t RDFR;
  volatile uint32_t CRDFR;
  volatile uint32_t SR;
  volatile uint32_t CLRFR;
  uint32_t RESERVED[57];
  volatile uint32_t DINR0;
  volatile uint32_t DINR1;
  volatile uint32_t DINR2;
  volatile uint32_t DINR3;
  volatile uint32_t DINR4;
  volatile uint32_t DINR5;
  volatile uint32_t DINR6;
  volatile uint32_t DINR7;
  volatile uint32_t DINR8;
  volatile uint32_t DINR9;
  volatile uint32_t DINR10;
  volatile uint32_t DINR11;
  volatile uint32_t DINR12;
  volatile uint32_t DINR13;
  volatile uint32_t DINR14;
  volatile uint32_t DINR15;
  volatile uint32_t DINR16;
  volatile uint32_t DINR17;
  volatile uint32_t DINR18;
  volatile uint32_t DINR19;
  volatile uint32_t DINR20;
  volatile uint32_t DINR21;
  volatile uint32_t DINR22;
  volatile uint32_t DINR23;
  volatile uint32_t DINR24;
  volatile uint32_t DINR25;
  volatile uint32_t DINR26;
  volatile uint32_t DINR27;
  volatile uint32_t DINR28;
  volatile uint32_t DINR29;
  volatile uint32_t DINR30;
  volatile uint32_t DINR31;
  volatile uint32_t DOUTR0;
  volatile uint32_t DOUTR1;
  volatile uint32_t DOUTR2;
  volatile uint32_t DOUTR3;
  volatile uint32_t DOUTR4;
  volatile uint32_t DOUTR5;
  volatile uint32_t DOUTR6;
  volatile uint32_t DOUTR7;
  volatile uint32_t DOUTR8;
  volatile uint32_t DOUTR9;
  volatile uint32_t DOUTR10;
  volatile uint32_t DOUTR11;
  volatile uint32_t DOUTR12;
  volatile uint32_t DOUTR13;
  volatile uint32_t DOUTR14;
  volatile uint32_t DOUTR15;
  volatile uint32_t DOUTR16;
  volatile uint32_t DOUTR17;
  volatile uint32_t DOUTR18;
  volatile uint32_t DOUTR19;
  volatile uint32_t DOUTR20;
  volatile uint32_t DOUTR21;
  volatile uint32_t DOUTR22;
  volatile uint32_t DOUTR23;
  volatile uint32_t DOUTR24;
  volatile uint32_t DOUTR25;
  volatile uint32_t DOUTR26;
  volatile uint32_t DOUTR27;
  volatile uint32_t DOUTR28;
  volatile uint32_t DOUTR29;
  volatile uint32_t DOUTR30;
  volatile uint32_t DOUTR31;
} MDIOS_TypeDef;



 
typedef struct
{
  volatile uint32_t GCR;             
  volatile uint32_t CKGCR;           
} MDF_TypeDef;



 
typedef struct
{
  volatile uint32_t SITFCR;          
  volatile uint32_t BSMXCR;          
  volatile uint32_t DFLTCR;          
  volatile uint32_t DFLTCICR;        
  volatile uint32_t DFLTRSFR;        
  uint32_t      RESERVED0[4];    
  volatile uint32_t DLYCR;           
  uint32_t      RESERVED1[1];    
  volatile uint32_t DFLTIER;         
  volatile uint32_t DFLTISR;         
  uint32_t      RESERVED2[1];    
  volatile uint32_t SADCR;           
  volatile uint32_t SADCFGR;         
  volatile uint32_t SADSDLVR;        
  volatile uint32_t SADANLVR;        
  uint32_t      RESERVED3[10];   
  volatile uint32_t DFLTDR;          
} MDF_Filter_TypeDef;



 

typedef struct
{
  uint32_t      RESERVED0[2];   
  volatile uint32_t SSCR;           
  volatile uint32_t BPCR;           
  volatile uint32_t AWCR;           
  volatile uint32_t TWCR;           
  volatile uint32_t GCR;            
  uint32_t      RESERVED1[2];   
  volatile uint32_t SRCR;           
  uint32_t      RESERVED2[1];   
  volatile uint32_t BCCR;           
  uint32_t      RESERVED3[1];   
  volatile uint32_t IER;            
  volatile uint32_t ISR;            
  volatile uint32_t ICR;            
  volatile uint32_t LIPCR;          
  volatile uint32_t CPSR;           
  volatile uint32_t CDSR;           
} LTDC_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t WHPCR;          
  volatile uint32_t WVPCR;          
  volatile uint32_t CKCR;           
  volatile uint32_t PFCR;           
  volatile uint32_t CACR;           
  volatile uint32_t DCCR;           
  volatile uint32_t BFCR;           
  uint32_t      RESERVED0[2];   
  volatile uint32_t CFBAR;          
  volatile uint32_t CFBLR;          
  volatile uint32_t CFBLNR;         
  uint32_t      RESERVED1[3];   
  volatile uint32_t CLUTWR;         

} LTDC_Layer_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;              
       uint32_t RESERVED1;       
  volatile uint32_t DCR1;            
  volatile uint32_t DCR2;            
  volatile uint32_t DCR3;            
  volatile uint32_t DCR4;            
       uint32_t RESERVED2[2];    
  volatile uint32_t SR;              
  volatile uint32_t FCR;             
       uint32_t RESERVED3[6];    
  volatile uint32_t DLR;             
       uint32_t RESERVED4;       
  volatile uint32_t AR;              
       uint32_t RESERVED5;       
  volatile uint32_t DR;              
       uint32_t RESERVED6[11];   
  volatile uint32_t PSMKR;           
       uint32_t RESERVED7;       
  volatile uint32_t PSMAR;           
       uint32_t RESERVED8;       
  volatile uint32_t PIR;             
       uint32_t RESERVED9[27];   
  volatile uint32_t CCR;             
       uint32_t RESERVED10;      
  volatile uint32_t TCR;             
       uint32_t RESERVED11;      
  volatile uint32_t IR;              
       uint32_t RESERVED12[3];   
  volatile uint32_t ABR;             
       uint32_t RESERVED13[3];   
  volatile uint32_t LPTR;            
       uint32_t RESERVED14[3];   
  volatile uint32_t WPCCR;           
       uint32_t RESERVED15;      
  volatile uint32_t WPTCR;           
       uint32_t RESERVED16;      
  volatile uint32_t WPIR;            
       uint32_t RESERVED17[3];   
  volatile uint32_t WPABR;           
       uint32_t RESERVED18[7];   
  volatile uint32_t WCCR;            
       uint32_t RESERVED19;      
  volatile uint32_t WTCR;            
       uint32_t RESERVED20;      
  volatile uint32_t WIR;             
       uint32_t RESERVED21[3];   
  volatile uint32_t WABR;            
       uint32_t RESERVED22[23];  
  volatile uint32_t HLCR;            
       uint32_t RESERVED23[3];   
  volatile uint32_t CALFCR;          
       uint32_t RESERVED24;      
  volatile uint32_t CALMR;           
       uint32_t RESERVED25;      
  volatile uint32_t CALSOR;          
       uint32_t RESERVED26;      
  volatile uint32_t CALSIR;          
} XSPI_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;      
} XSPIM_TypeDef;



 

typedef struct
{
  volatile uint32_t CR1;            
  volatile uint32_t SR1;            
  volatile uint32_t CSR1;           
  volatile uint32_t CSR2;           
  volatile uint32_t CSR3;           
  volatile uint32_t CSR4;           
  uint32_t      RESERVED0[2];   
  volatile uint32_t WKUPCR;         
  volatile uint32_t WKUPFR;         
  volatile uint32_t WKUPEPR;        
  volatile uint32_t UCPDR;          
  volatile uint32_t APCR;           
  volatile uint32_t PUCRN;          
  volatile uint32_t PDCRN;          
  volatile uint32_t PUCRO;          
  volatile uint32_t PDCRO;          
  volatile uint32_t PDCRP;          
  uint32_t      RESERVED2[2];   
  volatile uint32_t PDR1;           
} PWR_TypeDef;



 

typedef struct
{
  volatile uint32_t CR;           
  volatile uint32_t SR;           
  volatile uint32_t CLRFR;        
  uint32_t Reserved1[253];    
  volatile uint32_t RAM[1334];    
} PKA_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t SR;             
  volatile uint32_t RIS;            
  volatile uint32_t IER;            
  volatile uint32_t MIS;            
  volatile uint32_t ICR;            
  volatile uint32_t RESERVED1[4];   
  volatile uint32_t DR;             
} PSSI_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;            
  volatile uint32_t SR;            
  volatile uint32_t FAR;           
  volatile uint32_t FDRL;          
  volatile uint32_t FDRH;          
  volatile uint32_t FECR;          
} RAMECC_MonitorTypeDef;

typedef struct
{
  volatile uint32_t IER;           
} RAMECC_TypeDef;




 

typedef struct
{
  volatile uint32_t CR;             
  volatile uint32_t HSICFGR;        
  volatile uint32_t CRRCR;          
  volatile uint32_t CSICFGR;        
  volatile uint32_t CFGR;           
  uint32_t      RESERVED0;      
  volatile uint32_t CDCFGR;         
  volatile uint32_t BMCFGR;         
  volatile uint32_t APBCFGR;        
  uint32_t      RESERVED1;      
  volatile uint32_t PLLCKSELR;      
  volatile uint32_t PLLCFGR;        
  volatile uint32_t PLL1DIVR1;      
  volatile uint32_t PLL1FRACR;      
  volatile uint32_t PLL2DIVR1;      
  volatile uint32_t PLL2FRACR;      
  volatile uint32_t PLL3DIVR1;      
  volatile uint32_t PLL3FRACR;      
  uint32_t      RESERVED2;      
  volatile uint32_t CCIPR1;         
  volatile uint32_t CCIPR2;         
  volatile uint32_t CCIPR3;         
  volatile uint32_t CCIPR4;         
  uint32_t      RESERVED3;      
  volatile uint32_t CIER;           
  volatile uint32_t CIFR;           
  volatile uint32_t CICR;           
  uint32_t      RESERVED4;      
  volatile uint32_t BDCR;           
  volatile uint32_t CSR;            
  uint32_t      RESERVED5;      
  volatile uint32_t AHB5RSTR;       
  volatile uint32_t AHB1RSTR;       
  volatile uint32_t AHB2RSTR;       
  volatile uint32_t AHB4RSTR;       
  volatile uint32_t APB5RSTR;       
  volatile uint32_t APB1RSTR1;      
  volatile uint32_t APB1RSTR2;      
  volatile uint32_t APB2RSTR;       
  volatile uint32_t APB4RSTR;       
  uint32_t      RESERVED6;      
  volatile uint32_t AHB3RSTR;       
  uint32_t      RESERVED7[2];   
  volatile uint32_t CKGDISR  ;      
  uint32_t      RESERVED8[3];   
  volatile uint32_t PLL1DIVR2;      
  volatile uint32_t PLL2DIVR2;      
  volatile uint32_t PLL3DIVR2;      
  volatile uint32_t PLL1SSCGR;      
  volatile uint32_t PLL2SSCGR;      
  volatile uint32_t PLL3SSCGR;      
  uint32_t      RESERVED9[10];  
  volatile uint32_t CKPROTR;        
  uint32_t      RESERVED10[11];  
  volatile uint32_t RSR;            
  volatile uint32_t AHB5ENR;        
  volatile uint32_t AHB1ENR;        
  volatile uint32_t AHB2ENR;        
  volatile uint32_t AHB4ENR;        
  volatile uint32_t APB5ENR;        
  volatile uint32_t APB1ENR1;       
  volatile uint32_t APB1ENR2;       
  volatile uint32_t APB2ENR;        
  volatile uint32_t APB4ENR;        
  volatile uint32_t AHB3ENR;        
  volatile uint32_t AHB5LPENR;      
  volatile uint32_t AHB1LPENR;      
  volatile uint32_t AHB2LPENR;      
  volatile uint32_t AHB4LPENR;      
  volatile uint32_t AHB3LPENR;      
  volatile uint32_t APB1LPENR1;     
  volatile uint32_t APB1LPENR2;     
  volatile uint32_t APB2LPENR;      
  volatile uint32_t APB4LPENR;      
  volatile uint32_t APB5LPENR;      
} RCC_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;       
  volatile uint32_t SR;       
  volatile uint32_t DR;       
  uint32_t RESERVED;
  volatile uint32_t HTCR;     
} RNG_TypeDef;



 



 
typedef struct
{
  volatile uint32_t TR;           
  volatile uint32_t DR;           
  volatile uint32_t SSR;          
  volatile uint32_t ICSR;         
  volatile uint32_t PRER;         
  volatile uint32_t WUTR;         
  volatile uint32_t CR;           
  volatile uint32_t PRIVCFGR;     
  volatile uint32_t RESERVED0;    
  volatile uint32_t WPR;          
  volatile uint32_t CALR;         
  volatile uint32_t SHIFTR;       
  volatile uint32_t TSTR;         
  volatile uint32_t TSDR;         
  volatile uint32_t TSSSR;        
       uint32_t RESERVED1;    
  volatile uint32_t ALRMAR;       
  volatile uint32_t ALRMASSR;     
  volatile uint32_t ALRMBR;       
  volatile uint32_t ALRMBSSR;     
  volatile uint32_t SR;           
  volatile uint32_t MISR;         
  volatile uint32_t RESERVED2;    
  volatile uint32_t SCR;          
       uint32_t RESERVED3[4]; 
  volatile uint32_t ALRABINR;     
  volatile uint32_t ALRBBINR;     
} RTC_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;               
  volatile uint32_t SR;               
  volatile uint32_t DINR;             
  volatile uint32_t DOUTR;            
  volatile uint32_t KEYR0;            
  volatile uint32_t KEYR1;            
  volatile uint32_t KEYR2;            
  volatile uint32_t KEYR3;            
  volatile uint32_t IVR0;             
  volatile uint32_t IVR1;             
  volatile uint32_t IVR2;             
  volatile uint32_t IVR3;             
  volatile uint32_t KEYR4;            
  volatile uint32_t KEYR5;            
  volatile uint32_t KEYR6;            
  volatile uint32_t KEYR7;            
  volatile uint32_t SUSP0R;           
  volatile uint32_t SUSP1R;           
  volatile uint32_t SUSP2R;           
  volatile uint32_t SUSP3R;           
  volatile uint32_t SUSP4R;           
  volatile uint32_t SUSP5R;           
  volatile uint32_t SUSP6R;           
  volatile uint32_t SUSP7R;           
       uint32_t RESERVED1[168];   
  volatile uint32_t IER;              
  volatile uint32_t ISR;              
  volatile uint32_t ICR;              
} SAES_TypeDef;



 
typedef struct
{
  volatile uint32_t GCR;           
  uint32_t      RESERVED[16];  
  volatile uint32_t PDMCR;         
  volatile uint32_t PDMDLY;        
} SAI_TypeDef;

typedef struct
{
  volatile uint32_t CR1;          
  volatile uint32_t CR2;          
  volatile uint32_t FRCR;         
  volatile uint32_t SLOTR;        
  volatile uint32_t IMR;          
  volatile uint32_t SR;           
  volatile uint32_t CLRFR;        
  volatile uint32_t DR;           
} SAI_Block_TypeDef;



 
typedef struct
{
  volatile uint32_t BOOTSR;         
  uint32_t      RESERVED1[3];   
  volatile uint32_t HDPLCR;         
  volatile uint32_t HDPLSR;         
  uint32_t      RESERVED2[2];   
  volatile uint32_t DBGCR;          
  volatile uint32_t DBGLOCKR;       
  uint32_t      RESERVED3[3];   
  volatile uint32_t RSSCMDR;        
  uint32_t      RESERVED4[50];  
  volatile uint32_t PMCR;           
  volatile uint32_t FPUIMR;         
  volatile uint32_t MESR;           
  uint32_t      RESERVED5;      
  volatile uint32_t CCCSR;          
  volatile uint32_t CCVALR;         
  volatile uint32_t CCSWVALR;       
  uint32_t      RESERVED6;      
  volatile uint32_t BKLOCKR;        
  uint32_t      RESERVED7[3];   
  volatile uint32_t EXTICR[4];      
} SBS_TypeDef;



 
typedef struct
{
  volatile uint32_t POWER;         
  volatile uint32_t CLKCR;         
  volatile uint32_t ARG;           
  volatile uint32_t CMD;           
  volatile uint32_t RESPCMD;       
  volatile uint32_t RESP1;         
  volatile uint32_t RESP2;         
  volatile uint32_t RESP3;         
  volatile uint32_t RESP4;         
  volatile uint32_t DTIMER;        
  volatile uint32_t DLEN;          
  volatile uint32_t DCTRL;         
  volatile uint32_t DCOUNT;        
  volatile uint32_t STA;           
  volatile uint32_t ICR;           
  volatile uint32_t MASK;          
  volatile uint32_t ACKTIME;       
       uint32_t RESERVED0[3];  
  volatile uint32_t IDMACTRL;      
  volatile uint32_t IDMABSIZE;     
  volatile uint32_t IDMABASER;     
       uint32_t RESERVED1[2];  
  volatile uint32_t IDMALAR;       
  volatile uint32_t IDMABAR;       
       uint32_t RESERVED2[5];  
  volatile uint32_t FIFO;          
} SDMMC_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;           
  volatile uint32_t IMR;          
  volatile uint32_t SR;           
  volatile uint32_t IFCR;         
  volatile uint32_t DR;           
  volatile uint32_t CSR;          
  volatile uint32_t DIR;          
} SPDIFRX_TypeDef;



 
typedef struct
{
  volatile uint32_t CR1;            
  volatile uint32_t CR2;            
  volatile uint32_t CFG1;           
  volatile uint32_t CFG2;           
  volatile uint32_t IER;            
  volatile uint32_t SR;             
  volatile uint32_t IFCR;           
  uint32_t      RESERVED0;      
  volatile uint32_t TXDR;           
  uint32_t      RESERVED1[3];   
  volatile uint32_t RXDR;           
  uint32_t      RESERVED2[3];   
  volatile uint32_t CRCPOLY;        
  volatile uint32_t TXCRC;          
  volatile uint32_t RXCRC;          
  volatile uint32_t UDRDR;          
  volatile uint32_t I2SCFGR;        
} SPI_TypeDef;



 
typedef struct
{
  volatile uint32_t CR1;            
  volatile uint32_t CR2;            
  volatile uint32_t CR3;            
  volatile uint32_t FLTCR;          
  volatile uint32_t ATCR1;          
  volatile uint32_t ATSEEDR;        
  volatile uint32_t ATOR;           
  volatile uint32_t ATCR2;          
  volatile uint32_t CFGR;           
  volatile uint32_t PRIVCFGR;       
       uint32_t RESERVED2;      
  volatile uint32_t IER;            
  volatile uint32_t SR;             
  volatile uint32_t MISR;           
  volatile uint32_t RESERVED3;      
  volatile uint32_t SCR;            
  volatile uint32_t COUNT1R;        
       uint32_t RESERVED4[4];   
  volatile uint32_t RPCFGR;         
       uint32_t RESERVED5[42];  
  volatile uint32_t BKP0R;          
  volatile uint32_t BKP1R;          
  volatile uint32_t BKP2R;          
  volatile uint32_t BKP3R;          
  volatile uint32_t BKP4R;          
  volatile uint32_t BKP5R;          
  volatile uint32_t BKP6R;          
  volatile uint32_t BKP7R;          
  volatile uint32_t BKP8R;          
  volatile uint32_t BKP9R;          
  volatile uint32_t BKP10R;         
  volatile uint32_t BKP11R;         
  volatile uint32_t BKP12R;         
  volatile uint32_t BKP13R;         
  volatile uint32_t BKP14R;         
  volatile uint32_t BKP15R;         
  volatile uint32_t BKP16R;         
  volatile uint32_t BKP17R;         
  volatile uint32_t BKP18R;         
  volatile uint32_t BKP19R;         
  volatile uint32_t BKP20R;         
  volatile uint32_t BKP21R;         
  volatile uint32_t BKP22R;         
  volatile uint32_t BKP23R;         
  volatile uint32_t BKP24R;         
  volatile uint32_t BKP25R;         
  volatile uint32_t BKP26R;         
  volatile uint32_t BKP27R;         
  volatile uint32_t BKP28R;         
  volatile uint32_t BKP29R;         
  volatile uint32_t BKP30R;         
  volatile uint32_t BKP31R;         
} TAMP_TypeDef;



 
typedef struct
{
  volatile uint32_t CR1;             
  volatile uint32_t CR2;             
  volatile uint32_t SMCR;            
  volatile uint32_t DIER;            
  volatile uint32_t SR;              
  volatile uint32_t EGR;             
  volatile uint32_t CCMR1;           
  volatile uint32_t CCMR2;           
  volatile uint32_t CCER;            
  volatile uint32_t CNT;             
  volatile uint32_t PSC;             
  volatile uint32_t ARR;             
  volatile uint32_t RCR;             
  volatile uint32_t CCR1;            
  volatile uint32_t CCR2;            
  volatile uint32_t CCR3;            
  volatile uint32_t CCR4;            
  volatile uint32_t BDTR;            
  volatile uint32_t CCR5;            
  volatile uint32_t CCR6;            
  volatile uint32_t CCMR3;           
  volatile uint32_t DTR2;            
  volatile uint32_t ECR;             
  volatile uint32_t TISEL;           
  volatile uint32_t AF1;             
  volatile uint32_t AF2;             
       uint32_t RESERVED0[221];  
  volatile uint32_t DCR;             
  volatile uint32_t DMAR;            
} TIM_TypeDef;




 
typedef struct
{
  volatile uint32_t CFG1;         
  volatile uint32_t CFG2;         
  volatile uint32_t CFG3;         
  volatile uint32_t CR;           
  volatile uint32_t IMR;          
  volatile uint32_t SR;           
  volatile uint32_t ICR;          
  volatile uint32_t TX_ORDSET;    
  volatile uint32_t TX_PAYSZ;     
  volatile uint32_t TXDR;         
  volatile uint32_t RX_ORDSET;    
  volatile uint32_t RX_PAYSZ;     
  volatile uint32_t RXDR;         
  volatile uint32_t RX_ORDEXT1;   
  volatile uint32_t RX_ORDEXT2;   
} UCPD_TypeDef;



 
typedef struct
{
  volatile uint32_t GOTGCTL;              
  volatile uint32_t GOTGINT;              
  volatile uint32_t GAHBCFG;              
  volatile uint32_t GUSBCFG;              
  volatile uint32_t GRSTCTL;              
  volatile uint32_t GINTSTS;              
  volatile uint32_t GINTMSK;              
  volatile uint32_t GRXSTSR;              
  volatile uint32_t GRXSTSP;              
  volatile uint32_t GRXFSIZ;              
  volatile uint32_t DIEPTXF0_HNPTXFSIZ;   
  volatile uint32_t HNPTXSTS;             
  volatile uint32_t Reserved30[2];        
  volatile uint32_t GCCFG;                
  volatile uint32_t CID;                  
  volatile uint32_t GSNPSID;              
  volatile uint32_t GHWCFG1;              
  volatile uint32_t GHWCFG2;              
  volatile uint32_t GHWCFG3;              
  volatile uint32_t  Reserved6;           
  volatile uint32_t GLPMCFG;              
  volatile uint32_t GPWRDN;               
  volatile uint32_t GDFIFOCFG;            
  volatile uint32_t GADPCTL;              
  volatile uint32_t  Reserved43[39];      
  volatile uint32_t HPTXFSIZ;             
  volatile uint32_t DIEPTXF[0x0F];        
} USB_OTG_GlobalTypeDef;



 
typedef struct
{
  volatile uint32_t DCFG;                 
  volatile uint32_t DCTL;                 
  volatile uint32_t DSTS;                 
  uint32_t Reserved0C;                
  volatile uint32_t DIEPMSK;              
  volatile uint32_t DOEPMSK;              
  volatile uint32_t DAINT;                
  volatile uint32_t DAINTMSK;             
  uint32_t  Reserved20;               
  uint32_t Reserved9;                 
  volatile uint32_t DVBUSDIS;             
  volatile uint32_t DVBUSPULSE;           
  volatile uint32_t DTHRCTL;              
  volatile uint32_t DIEPEMPMSK;           
  volatile uint32_t DEACHINT;             
  volatile uint32_t DEACHMSK;             
  uint32_t Reserved40;                
  volatile uint32_t DINEP1MSK;            
  uint32_t  Reserved44[15];           
  volatile uint32_t DOUTEP1MSK;           
} USB_OTG_DeviceTypeDef;




 
typedef struct
{
  volatile uint32_t DIEPCTL;              
  volatile uint32_t Reserved04;           
  volatile uint32_t DIEPINT;              
  volatile uint32_t Reserved0C;           
  volatile uint32_t DIEPTSIZ;             
  volatile uint32_t DIEPDMA;              
  volatile uint32_t DTXFSTS;              
  volatile uint32_t Reserved18;           
} USB_OTG_INEndpointTypeDef;



 
typedef struct
{
  volatile uint32_t DOEPCTL;              
  volatile uint32_t Reserved04;           
  volatile uint32_t DOEPINT;              
  volatile uint32_t Reserved0C;           
  volatile uint32_t DOEPTSIZ;             
  volatile uint32_t DOEPDMA;              
  volatile uint32_t Reserved18[2];        
} USB_OTG_OUTEndpointTypeDef;



 
typedef struct
{
  volatile uint32_t HCFG;                  
  volatile uint32_t HFIR;                  
  volatile uint32_t HFNUM;                 
  uint32_t Reserved40C;                
  volatile uint32_t HPTXSTS;               
  volatile uint32_t HAINT;                 
  volatile uint32_t HAINTMSK;              
} USB_OTG_HostTypeDef;



 
typedef struct
{
  volatile uint32_t HCCHAR;                
  volatile uint32_t HCSPLT;                
  volatile uint32_t HCINT;                 
  volatile uint32_t HCINTMSK;              
  volatile uint32_t HCTSIZ;                
  volatile uint32_t HCDMA;                 
  uint32_t Reserved[2];                
} USB_OTG_HostChannelTypeDef;



 
typedef struct
{
  volatile uint32_t CR1;          
  volatile uint32_t CR2;          
  volatile uint32_t CR3;          
  volatile uint32_t BRR;          
  volatile uint32_t GTPR;         
  volatile uint32_t RTOR;         
  volatile uint32_t RQR;          
  volatile uint32_t ISR;          
  volatile uint32_t ICR;          
  volatile uint32_t RDR;          
  volatile uint32_t TDR;          
  volatile uint32_t PRESC;        
} USART_TypeDef;



 
typedef struct
{
  volatile uint32_t CR;           
  volatile uint32_t CFR;          
  volatile uint32_t SR;           
} WWDG_TypeDef;

   

 
 
 



 




 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 














 
typedef uint32_t (*RSSLIB_JumpHDPlvl2_TypeDef)(uint32_t VectorTableAddr, uint32_t MPUIndex);














 
typedef uint32_t (*RSSLIB_JumpHDPlvl3_TypeDef)(uint32_t VectorTableAddr, uint32_t MPUIndex);



 
typedef struct
{
  uint32_t *pSource;         
  uint32_t Destination;      
  uint32_t Size;             
  uint32_t DoEncryption;     
  uint32_t Crc;              
} RSSLIB_DataProvisioningConf_t;







 
typedef uint32_t (*RSSLIB_DataProvisioning_TypeDef)(RSSLIB_DataProvisioningConf_t *pConfig);











 
typedef uint32_t (*RSSLIB_SetSecOB_TypeDef)(uint32_t ObAddr, uint32_t ObMask, uint32_t ObValue, uint32_t ObPos);








 
typedef uint32_t (*RSSLIB_GetRssStatus_TypeDef)(void);









 
typedef uint32_t (*RSSLIB_SetProductState_TypeDef)(uint32_t ProductState);











 
typedef uint32_t (*RSSLIB_GetProductState_TypeDef)(void);




 
typedef struct
{
  volatile const RSSLIB_SetSecOB_TypeDef SetSecOB;
  uint32_t RESERVED1[4];
  volatile const RSSLIB_GetRssStatus_TypeDef GetRssStatus;
  volatile const RSSLIB_SetProductState_TypeDef SetProductState;
  volatile const RSSLIB_DataProvisioning_TypeDef DataProvisioning;
  volatile const RSSLIB_JumpHDPlvl2_TypeDef JumpHDPLvl2;
  volatile const RSSLIB_JumpHDPlvl3_TypeDef JumpHDPLvl3;
  volatile const RSSLIB_GetProductState_TypeDef GetProductState;
} RSSLIB_pFunc_TypeDef;


   


 
 
 




 


   



 



 


 



 
 
 
 

 
 
 
 
 



 

 

 

 

 










 





 











 









 



 


 


 





 





 





 


 

 







 




 




 




 




 

 

 

 

 

 

 

 


 

 
 


 









 


 
 
 
 
 
 

 

 

 

 

 

 

 

 
 
 
 
 
 


 

 
 
 
 
 

 

 

 

 

 

 


 
 
 
 
 
 

 

 


 
 
 
 
 
 

 

 

 

 


 
 
 
 
 

 



 

 


 
 
 
 
 
 







 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 





 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

   
 


 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 





 








 










 




 




 








 






 














 














 














 




 

 

 


 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 


 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 


 
 
 
 
 
 









 




 







 
















 




 


 

 

 



 


 







 



 






 



 





 





 
 
 
 
 
 

 

 

 

 

 

 


 
 
 
 
 
 


 
 

 

 
 

 
 

 

 
 

 


 






 
 

 
 

 
 

 

 

 

 




 






 




 

 




 

 




 

 













 








 





 




 

 

 

 



 

 

 

 

 

 

 

 

 

 

 

 

 

 


 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 
 
 
 
 
 

 

 

 

 

 

 

 

 


 

 

 

 

 


 

 

 

 

 

 

 

 

 

 

 

 

 

 



 


 

 

 

 

 

 

 

 

 

 

 




 

 

 



 





 



 

 

 

 

 

 

 

 

 

 

 

 


 


 
 
 
 
 
 

 

 

 

 

 

 

 

 

 

 

 


 
 
 
 
 
 

 

 

   

   



 

 



 

 

 

 

 

 

 

 





 

 

 


 

 

 

 
 

 
 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 



 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 

 


   

   

   





 



 
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;

typedef enum
{
  SUCCESS = 0,
  ERROR = !SUCCESS
} ErrorStatus;



 




 








 
 

 

 

 

 

 



 





 



 

















 

 


 
 
 



 


 



 






 



 




 



 





 



 


 



 


 



 




 



 








 



 







 



 




 



 




 



 






 




 


 



 



 



 












 



 


 



 






 



 


 



 



 



 


 



 




 
 




 


 



 



 




 



 



 



 












 



 




 



 

 

 



 



 















 




 





 




 


 



 






 



 









 



 


 



 








 




 




 



 



 



 



 



 






 



 




 





 



 

 



 


 



 




 



 



 


 

 






 



 



 



 



 



 





 



 









 



 



 



 


 



 


 



 


 



 


 



 


 




 



 

 



 


 



 




 




 








 



 



 



 






 



 




 




 




 



 




 



 



 



 





 



 







 




 


 




 




 




 



 




 



 




 










 






























 



 



 



 








 



 









 



 






 



 


 



 




 



 






 




 





 



 









 



 






 



 




 



 


 



 


 



 


 



 


 



 


 



 


 



 



 




 
 

  #pragma system_include

 
 

 

  #pragma system_include














 




 

  #pragma system_include




 

 
 




 

 
 




 

 
 


 



 
typedef unsigned int __iar_FlagUType;
typedef signed int  __iar_FlagSType;

typedef signed int  __iar_ExpType;













 




 



#pragma diag_suppress = Pe549 









 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isfinite32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X);
    return ((_Ix << 1) >> (23 + 1)) + 1;
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isfinite64(double _X)
{
    signed int _Ix = __iar_fpgethi64(_X);
    return ((_Ix << 1) >> (52 - 31)) + 1;
}


 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnan32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X) << 1;
    return (_Ix >> (23 + 1)) + 1 ? 0 : (_Ix << (31 - 23));
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnan64(double _X)
{
    signed int _Ix = __iar_fpgethi64(_X);
    return ((_Ix << 1) >> (52 - 31)) + 1 ? 0 : _Ix << (64 - 52);
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_iszero32(float _X)
{
    unsigned int _Ix = __iar_fp2bits32(_X);
    return (_Ix << 1) == 0;
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_iszero64(double _X)
{
    unsigned long long int _Ix = __iar_fp2bits64(_X);
    return (_Ix & ~(1ULL << 63)) == 0;
}





 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isinf32(float _X)
{
    signed int _Ix = __iar_fp2bits32(_X);
    return ((_Ix << 1) >> (23 + 1)) + 1
           ? 0
           : ((_Ix << (32 - 23)) == 0);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isinf64(double _X)
{
    signed int _Ix = __iar_fpgethi64(_X);
    return ((_Ix << 1) >> (52 - 31)) + 1 
             ? 0 
             : ((_Ix << (64 - 52)) == 0);
}





 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_issubnormal32(float _X)
{
    unsigned int _Ix = __iar_fp2bits32(_X) & ~(1 << 31);
    return (_Ix != 0) && (_Ix < (1 << 23));
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_issubnormal64(double _X)
{
    unsigned long long int _Ix = __iar_fp2bits64(_X) & ~(1ULL << 63);
    return (_Ix != 0) && (_Ix < (1ULL << 52));
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnormal32(float _X)
{
  signed int _Exp = ((signed int)
                          (__iar_fp2bits32(_X) << 1) >> (23 + 1));
  return ((_Exp + 1) >> 1);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_isnormal64(double _X)
{
    signed int _Exp = 
      ((signed int)(__iar_fpgethi64(_X) << 1) >> (52 - 31));
    return ((_Exp + 1) >> 1);
}




 
#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_signbit32(float _X)
{
  unsigned int _Ix = __iar_fp2bits32(_X);
  return (_Ix >> 31);
}

#pragma no_arith_checks
_Pragma("inline=forced") __intrinsic unsigned int __iar_signbit64(double _X)
{
  unsigned long long int _Ix = __iar_fp2bits64(_X);
  return (_Ix >> 63);
}





  typedef float float_t;
  typedef double double_t;



   
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       acos(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       asin(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       atan(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       atan2(double, double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       ceil(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       cos(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       cosh(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       exp(double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       fabs(double);
  _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind      double       floor(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       fmod(double, double);
  _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind   double       frexp(double, int *);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       ldexp(double, int);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       log(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       log10(double);
  _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind   double       modf(double, double *);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       pow(double, double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sin(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sinh(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       sqrt(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       tan(double);
  _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind      double       tanh(double);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       acosh(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       asinh(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       atanh(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       cbrt(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       copysign(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       erf(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       erfc(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       expm1(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       exp2(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fdim(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       fma(double, double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fmax(double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       fmin(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       hypot(double, double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogb(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       lgamma(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llround(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       log1p(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       log2(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   double       logb(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   long         lrint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns")  __intrinsic __nounwind   long         lround(double);
    _Pragma("function_effects = no_state, always_returns")  __intrinsic __nounwind   double       nan(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nearbyint(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nextafter(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       nexttoward(double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       remainder(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       remquo(double, double, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       rint(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       round(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       scalbn(double, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    double       scalbln(double, long);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  double       tgamma(double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    double       trunc(double);

     
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        acosf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        acoshf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        asinf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        asinhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atanf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atanhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        atan2f(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        ceilf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        coshf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        cosf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        cbrtf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        copysignf(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        erff(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        erfcf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        expf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        expm1f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        exp2f(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fabsf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fdimf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        floorf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        fmaf(float, float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fmaxf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        fminf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        fmodf(float, float);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind float        frexpf(float, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        hypotf(float, float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogbf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        ldexpf(float, int);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        lgammaf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llroundf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        logbf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        logf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log1pf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log2f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        log10f(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lrintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lroundf(float);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind float        modff(float, float *);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        nanf(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nearbyintf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nextafterf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        nexttowardf(float, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        powf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        remainderf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        remquof(float, float, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        rintf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        roundf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        scalbnf(float, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        scalblnf(float, long);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sinf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sinhf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        sqrtf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        tanf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    float        tanhf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  float        tgammaf(float);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    float        truncf(float);

     
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  acoshl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  acosl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  asinhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  asinl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atanl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atanhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  atan2l(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  ceill(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  cbrtl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  copysignl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  coshl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  cosl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  erfl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  erfcl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  expl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  expm1l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  exp2l(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fabsl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fdiml(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  floorl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  fmal(long double, long double,
                                               long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fmaxl(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  fminl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  fmodl(long double, long double);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind long double  frexpl(long double, int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  hypotl(long double, long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    int          ilogbl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  ldexpl(long double, int);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  lgammal(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llrintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long long    llroundl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  logbl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  logl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log1pl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log10l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  log2l(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lrintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long         lroundl(long double);
    _Pragma("function_effects = no_state, no_read(2), always_returns") __intrinsic __nounwind long double  modfl(long double, long double *);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  nanl(const char *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nearbyintl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nextafterl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  nexttowardl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  powl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  remainderl(long double, long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  remquol(long double, long double,
                                                  int *);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  rintl(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  roundl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  scalbnl(long double, int);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  scalblnl(long double, long);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sinhl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sinl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  sqrtl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  tanl(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind    long double  tanhl(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind  long double  tgammal(long double);
    _Pragma("function_effects = no_state, always_returns") __intrinsic __nounwind    long double  truncl(long double);

   
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_cos_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_exp_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log10_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_log2_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_pow_medium(double, double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_sin_medium(double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   double        __iar_tan_medium(double);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_cos_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_exp_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log10_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_log2_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_pow_mediumf(float, float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_sin_mediumf(float);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   float         __iar_tan_mediumf(float);

    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_cos_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_exp_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log10_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_log2_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_pow_mediuml(long double,
                                                           long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_sin_mediuml(long double);
    _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind   long double   __iar_tan_mediuml(long double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_cos_accurate(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_pow_accurate(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_sin_accurate(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_tan_accurate(double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_cos_accuratef(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_pow_accuratef(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_sin_accuratef(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_tan_accuratef(float);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_cos_accuratel(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_pow_accuratel(long double,
                                                              long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_sin_accuratel(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_tan_accuratel(long double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_cos_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_exp_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log10_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_log2_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_pow_small(double, double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_sin_small(double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind double        __iar_tan_small(double);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_cos_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_exp_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log10_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_log2_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_pow_smallf(float, float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_sin_smallf(float);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind float         __iar_tan_smallf(float);

      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_cos_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_exp_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log10_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_log2_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_pow_smalll(long double,
                                                         long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_sin_smalll(long double);
      _Pragma("function_effects = no_state, write_errno, always_returns") __intrinsic __nounwind long double   __iar_tan_smalll(long double);




   
  enum __FPclass { __kInfinity, __kNan, __kFinite, __kDenorm, __kZero };

  #pragma inline=forced
  __intrinsic int __iar_FPclassify32(float x)
  {
    if (!__iar_isfinite32(x))
    {
      if (__iar_isnan32(x))
      {
        return __kNan;
      }
      return __kInfinity;
    }
    if (__iar_iszero32(x))
    {
      return __kZero;
    }
    if (__iar_issubnormal32(x))
    {
      return __kDenorm;
    }
    return __kFinite;
  }

    #pragma inline=forced
    __intrinsic int __iar_FPclassify64(double x)
    {
      if (!__iar_isfinite64(x))
      {
        if (__iar_isnan64(x))
        {
          return __kNan;
        }
        return __kInfinity;
      }
      if (__iar_iszero64(x))
      {
        return __kZero;
      }
      if (__iar_issubnormal64(x))
      {
        return __kDenorm;
      }
      return __kFinite;
    }

   





   




    #pragma inline
    __iar_FlagSType __isnormalf(float _Left)
    {       
      return !__iar_isnan32(_Left) && __iar_isnormal32(_Left);
    }
    #pragma inline
    __iar_FlagSType __isnormal(double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isnormal64(_Left);
    }
    #pragma inline
    __iar_FlagSType __isnormall(long double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isnormal64(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinff(float _Left)
    {       
      return !__iar_isnan32(_Left) && __iar_isinf32(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinf(double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isinf64(_Left);
    }
    #pragma inline
     __iar_FlagSType __isinfl(long double _Left)
    {       
      return !__iar_isnan64(_Left) && __iar_isinf64(_Left);
    }




    #pragma inline
    __iar_FlagSType __isunorderedf(float _x, float _y)
    {
      return __iar_isnan32(_x) || __iar_isnan32(_y);
    }
    #pragma inline
    __iar_FlagSType __isunordered(double _x, double _y)
    {
      return __iar_isnan64(_x) || __iar_isnan64(_y);
    }
    #pragma inline
    __iar_FlagSType __isunorderedl(long double _x, long double _y)
    {
      return __iar_isnan64(_x) || __iar_isnan64(_y);
    }
    #pragma inline
    __iar_FlagSType __islessgreaterf(float _x, float _y)
    {
      if (__isunorderedf(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }
    #pragma inline
    __iar_FlagSType __islessgreater(double _x, double _y)
    {
      if (__isunordered(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }
    #pragma inline
    __iar_FlagSType __islessgreaterl(long double _x, long double _y)
    {
      if (__isunorderedl(_x, _y))
        return 0;
      return _x < _y || _x > _y;
    }







 
 
 

  #pragma system_include

 
 

 

  #pragma system_include














 


 
 


  #pragma system_include

 
 

 

  #pragma system_include














 



 
  typedef _Sizet size_t;

typedef unsigned int __data_size_t;



 


 
  typedef   signed int ptrdiff_t;

  typedef   _Wchart wchar_t;


    typedef union
    {
      long long _ll;
      long double _ld;
      void *_vp;
    } _Max_align_t;
    typedef _Max_align_t max_align_t;






 

 



 
typedef enum
{
  HAL_OK       = 0x00,
  HAL_ERROR    = 0x01,
  HAL_BUSY     = 0x02,
  HAL_TIMEOUT  = 0x03
} HAL_StatusTypeDef;



 
typedef enum
{
  HAL_UNLOCKED = 0x00,
  HAL_LOCKED   = 0x01
} HAL_LockTypeDef;

 



















 







 



 



 




 


 





















 

 


 



 




 

 
 
 


 








 

 



 

 


 








 

 

 


 
extern const uint8_t LL_RCC_PrescTable[16];



 

 


 






 









 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 




 



 


 



 


 



 


 



 



 



 


 



 


 



 





 



 



 



 


 



 


 



 


 



 



 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 

 


 



 






 





 


 



 















 















 











 











 











 











 



 




 

 


 



 







 
static inline void LL_RCC_HSE_EnableCSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (20U))));
}





 
static inline void LL_RCC_HSE_EnableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (18U))));
}





 
static inline void LL_RCC_HSE_DisableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (18U))));
}





 
static inline void LL_RCC_HSE_SelectAnalogClock(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (19U))));
}





 
static inline void LL_RCC_HSE_SelectDigitalClock(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (19U))));
}





 
static inline void LL_RCC_HSE_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (16U))));
}





 
static inline void LL_RCC_HSE_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (16U))));
}





 
static inline uint32_t LL_RCC_HSE_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (17U)))) == (0x1UL << (17U))) ? 1UL : 0UL);
}



 



 





 
static inline void LL_RCC_HSI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_HSI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_HSI_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (2U)))) == (0x1UL << (2U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_HSI_IsDividerReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (5U)))) == (0x1UL << (5U))) ? 1UL : 0UL);
}










 
static inline void LL_RCC_HSI_SetDivider(uint32_t Divider)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR))) & (~((0x3UL << (3U))))) | (Divider))));
}









 
static inline uint32_t LL_RCC_HSI_GetDivider(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x3UL << (3U)))));
}





 
static inline void LL_RCC_HSI_EnableStopMode(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (1U))));
}





 
static inline void LL_RCC_HSI_DisableStopMode(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (1U))));
}





 
static inline uint32_t LL_RCC_HSI_IsEnabledStopMode(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}





 
static inline void LL_RCC_EnableXSPIClockProtection(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_DisableXSPIClockProtection(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_IsEnabledXSPIClockProtection(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}





 
static inline void LL_RCC_EnableFMCClockProtection(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) |= ((0x1UL << (1U))));
}





 
static inline void LL_RCC_DisableFMCClockProtection(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) &= ~((0x1UL << (1U))));
}





 
static inline uint32_t LL_RCC_IsEnabledFMCClockProtection(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}










 
static inline uint32_t LL_RCC_GetXSPI2SwitchPosition(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) & ((0x7UL << (8U)))));
}










 
static inline uint32_t LL_RCC_GetXSPI1SwitchPosition(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) & ((0x7UL << (4U)))));
}











 
static inline uint32_t LL_RCC_GetFMCSwitchPosition(void)
{
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CKPROTR) & ((0x7UL << (12U)))));
}







 
static inline uint32_t LL_RCC_HSI_GetCalibration(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->HSICFGR) & ((0xFFFUL << (0U)))) >> (0U));
}









 
static inline void LL_RCC_HSI_SetCalibTrimming(uint32_t Value)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->HSICFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->HSICFGR))) & (~((0x7FUL << (24U))))) | (Value << (24U)))));
}





 
static inline uint32_t LL_RCC_HSI_GetCalibTrimming(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->HSICFGR) & ((0x7FUL << (24U)))) >> (24U));
}



 



 





 
static inline void LL_RCC_CSI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (7U))));
}





 
static inline void LL_RCC_CSI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (7U))));
}





 
static inline uint32_t LL_RCC_CSI_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (8U)))) == (0x1UL << (8U))) ? 1UL : 0UL);
}





 
static inline void LL_RCC_CSI_EnableStopMode(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (9U))));
}





 
static inline void LL_RCC_CSI_DisableStopMode(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (9U))));
}





 
static inline uint32_t LL_RCC_CSI_IsEnabledStopMode(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}







 
static inline uint32_t LL_RCC_CSI_GetCalibration(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSICFGR) & ((0xFFUL << (0U)))) >> (0U));
}









 
static inline void LL_RCC_CSI_SetCalibTrimming(uint32_t Value)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSICFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSICFGR))) & (~((0x3FUL << (24U))))) | (Value << (24U)))));
}





 
static inline uint32_t LL_RCC_CSI_GetCalibTrimming(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSICFGR) & ((0x3FUL << (24U)))) >> (24U));
}



 



 





 
static inline void LL_RCC_HSI48_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (12U))));
}





 
static inline void LL_RCC_HSI48_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (12U))));
}





 
static inline uint32_t LL_RCC_HSI48_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (13U)))) == (0x1UL << (13U))) ? 1UL : 0UL);
}







 
static inline uint32_t LL_RCC_HSI48_GetCalibration(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CRRCR) & ((0x3FFUL << (0U)))) >> (0U));
}


 



 







 
static inline void LL_RCC_LSE_EnableCSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (5U))));
}







 
static inline void LL_RCC_LSE_DisableCSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (5U))));
}





 
static inline uint32_t LL_RCC_LSE_IsFailureDetected(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) & ((0x1UL << (6U)))) == (0x1UL << (6U))) ? 1UL : 0UL);
}







 
static inline void LL_RCC_LSE_ReArmCSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (12U))));
}





 
static inline void LL_RCC_LSE_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSE_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSE_EnableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (2U))));
}





 
static inline void LL_RCC_LSE_DisableBypass(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (2U))));
}







 
static inline void LL_RCC_LSE_SelectDigitalClock(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (7U))));
}







 
static inline void LL_RCC_LSE_SelectAnalogClock(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (7U))));
}











 
static inline void LL_RCC_LSE_SetDriveCapability(uint32_t LSEDrive)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR))) & (~((0x3UL << (3U))))) | (LSEDrive))));
}









 
static inline uint32_t LL_RCC_LSE_GetDriveCapability(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) & ((0x3UL << (3U)))));
}





 
static inline uint32_t LL_RCC_LSE_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) & ((0x1UL << (1U)))) == (0x1UL << (1U))) ? 1UL : 0UL);
}



 



 





 
static inline void LL_RCC_LSI_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_LSI_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSR) &= ~((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_LSI_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CSR) & ((0x1UL << (1U)))) == (0x1UL << (1U))) ? 1UL : 0UL);
}



 



 










 
static inline void LL_RCC_SetSysClkSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((0x7UL << (0U))))) | (Source))));
}









 
static inline uint32_t LL_RCC_GetSysClkSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR) & ((0x7UL << (3U)))));
}








 
static inline void LL_RCC_SetSysWakeUpClkSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((0x1UL << (6U))))) | (Source))));
}







 
static inline uint32_t LL_RCC_GetSysWakeUpClkSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR) & ((0x1UL << (6U)))));
}








 
static inline void LL_RCC_SetKerWakeUpClkSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((0x1UL << (7U))))) | (Source))));
}







 
static inline uint32_t LL_RCC_GetKerWakeUpClkSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR) & ((0x1UL << (7U)))));
}















 
static inline void LL_RCC_SetSysPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CDCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CDCFGR))) & (~((0xFUL << (0U))))) | (Prescaler))));
}















 
static inline void LL_RCC_SetAHBPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BMCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BMCFGR))) & (~((0xFUL << (0U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB1Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR))) & (~((0x7UL << (0U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB2Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR))) & (~((0x7UL << (4U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB4Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR))) & (~((0x7UL << (8U))))) | (Prescaler))));
}











 
static inline void LL_RCC_SetAPB5Prescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR))) & (~((0x7UL << (12U))))) | (Prescaler))));
}














 
static inline uint32_t LL_RCC_GetSysPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CDCFGR) & ((0xFUL << (0U)))));
}














 
static inline uint32_t LL_RCC_GetAHBPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BMCFGR) & ((0xFUL << (0U)))));
}










 
static inline uint32_t LL_RCC_GetAPB1Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR) & ((0x7UL << (0U)))));
}










 
static inline uint32_t LL_RCC_GetAPB2Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR) & ((0x7UL << (4U)))));
}










 
static inline uint32_t LL_RCC_GetAPB4Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR) & ((0x7UL << (8U)))));
}










 
static inline uint32_t LL_RCC_GetAPB5Prescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->APBCFGR) & ((0x7UL << (12U)))));
}



 



 



















































 
static inline void LL_RCC_ConfigMCO(uint32_t MCOxSource, uint32_t MCOxPrescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((MCOxSource << 16U) | (MCOxPrescaler << 16U)))) | ((MCOxSource & 0xFFFF0000U) | (MCOxPrescaler & 0xFFFF0000U)))));
}



 



 






















































































 
static inline void LL_RCC_SetClockSource(uint32_t ClkSource)
{
  volatile uint32_t *pReg = (uint32_t *)((uint32_t)&((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1 + (((ClkSource) >> 0U ) & 0xFFUL));
  (((*pReg)) = ((((((*pReg))) & (~(((((ClkSource) >> 24U ) & 0xFFUL) << (((ClkSource) >> 8U ) & 0x1FUL))))) | (((((ClkSource) >> 16U) & 0xFFUL) << (((ClkSource) >> 8U ) & 0x1FUL))))));
}









 
static inline void LL_RCC_SetADCClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (24U))))) | (ClkSource))));
}












 
static inline void LL_RCC_SetADFClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x7UL << (20U))))) | (ClkSource))));
}









 
static inline void LL_RCC_SetCECClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2))) & (~((0x3UL << (28U))))) | (ClkSource))));
}









 
static inline void LL_RCC_SetCLKPClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (28U))))) | (ClkSource))));
}








 
static inline void LL_RCC_SetETHPHYClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x1UL << (18U))))) | (ClkSource))));
}









 
static inline void LL_RCC_SetETHREFClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (16U))))) | (ClkSource))));
}









 
static inline void LL_RCC_SetFDCANClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2))) & (~((0x3UL << (22U))))) | (ClkSource))));
}










 
static inline void LL_RCC_SetFMCClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (0U))))) | (ClkSource))));
}















 
static inline void LL_RCC_SetI2CClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}










 
static inline void LL_RCC_SetI3CClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2))) & (~((0x3UL << (12U))))) | (ClkSource))));
}


























 
static inline void LL_RCC_SetLPTIMClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}












 
static inline void LL_RCC_SetLPUARTClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR4)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR4))) & (~((0x7UL << (0U))))) | (ClkSource))));
}










 
static inline void LL_RCC_SetOTGFSClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (14U))))) | (ClkSource))));
}













 
static inline void LL_RCC_SetXSPIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}








 
static inline void LL_RCC_SetPSSIClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x1UL << (27U))))) | (ClkSource))));
}


















 
static inline void LL_RCC_SetSAIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}








 
static inline void LL_RCC_SetSDMMCClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x1UL << (2U))))) | (ClkSource))));
}










 
static inline void LL_RCC_SetSPDIFRXClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2))) & (~((0x3UL << (24U))))) | (ClkSource))));
}































 
static inline void LL_RCC_SetSPIClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}



















 
static inline void LL_RCC_SetUSARTClockSource(uint32_t ClkSource)
{
  LL_RCC_SetClockSource(ClkSource);
}










 
static inline void LL_RCC_SetUSBPHYCClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0x3UL << (12U))))) | (ClkSource))));
}












 
static inline void LL_RCC_SetUSBREFClockSource(uint32_t ClkSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1))) & (~((0xFUL << (8U))))) | (ClkSource))));
}






































































































 
static inline uint32_t LL_RCC_GetClockSource(uint32_t Periph)
{
  const volatile uint32_t *pReg = (uint32_t *)((uint32_t)((uint32_t)(&((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) + (((Periph) >> 0U ) & 0xFFUL)));
  return (uint32_t)(Periph | (((((*pReg) & (((((Periph) >> 24U ) & 0xFFUL) << (((Periph) >> 8U ) & 0x1FUL))))) >> (((Periph) >> 8U ) & 0x1FUL)) << 16U));
}










 
static inline uint32_t LL_RCC_GetADCClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (24U)))));
}













 
static inline uint32_t LL_RCC_GetADFClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x7UL << (20U)))));
}










 
static inline uint32_t LL_RCC_GetCECClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2) & ((0x3UL << (28U)))));
}










 
static inline uint32_t LL_RCC_GetCLKPClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (28U)))));
}









 
static inline uint32_t LL_RCC_GetETHPHYClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x1UL << (18U)))));
}










 
static inline uint32_t LL_RCC_GetETHREFClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (16U)))));
}










 
static inline uint32_t LL_RCC_GetFDCANClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2) & ((0x3UL << (22U)))));
}











 
static inline uint32_t LL_RCC_GetFMCClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (0U)))));
}

















 
static inline uint32_t LL_RCC_GetI2CClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}











 
static inline uint32_t LL_RCC_GetI3CClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2) & ((0x3UL << (12U)))));
}






























 
static inline uint32_t LL_RCC_GetLPTIMClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}













 
static inline uint32_t LL_RCC_GetLPUARTClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR4) & ((0x7UL << (0U)))));
}











 
static inline uint32_t LL_RCC_GetOTGFSClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (14U)))));
}















 
static inline uint32_t LL_RCC_GetXSPIClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}









 
static inline uint32_t LL_RCC_GetPSSIClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x1UL << (27U)))));
}




















 
static inline uint32_t LL_RCC_GetSAIClockSource(uint32_t Periph)
{
  (void)Periph;
  return LL_RCC_GetClockSource(Periph);
}









 
static inline uint32_t LL_RCC_GetSDMMCClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x1UL << (2U)))));
}











 
static inline uint32_t LL_RCC_GetSPDIFRXClockSource(uint32_t Periph)
{
  (void)Periph;
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR2) & ((0x3UL << (24U)))));
}



































 
static inline uint32_t LL_RCC_GetSPIClockSource(uint32_t Periph)
{
  return LL_RCC_GetClockSource(Periph);
}





















 
static inline uint32_t LL_RCC_GetUSARTClockSource(uint32_t Periph)
{
  (void)Periph;
  return LL_RCC_GetClockSource(Periph);
}











 
static inline uint32_t LL_RCC_GetUSBPHYCClockSource(uint32_t Periph)
{
  (void)Periph;
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0x3UL << (12U)))));
}














 
static inline uint32_t LL_RCC_GetUSBREFClockSource(uint32_t Periph)
{
  (void)Periph;
  return (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CCIPR1) & ((0xFUL << (8U)))));
}



 



 













 
static inline void LL_RCC_SetRTCClockSource(uint32_t Source)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR))) & (~((0x3UL << (8U))))) | (Source))));
}









 
static inline uint32_t LL_RCC_GetRTCClockSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) & ((0x3UL << (8U)))));
}





 
static inline void LL_RCC_EnableRTC(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (15U))));
}





 
static inline void LL_RCC_DisableRTC(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (15U))));
}





 
static inline uint32_t LL_RCC_IsEnabledRTC(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) & ((0x1UL << (15U)))) == ((0x1UL << (15U)))) ? 1UL : 0UL);
}





 
static inline void LL_RCC_ForceBackupDomainReset(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) |= ((0x1UL << (16U))));
}





 
static inline void LL_RCC_ReleaseBackupDomainReset(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->BDCR) &= ~((0x1UL << (16U))));
}





































































 
static inline void LL_RCC_SetRTC_HSEPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((0x3FUL << (8U))))) | (Prescaler))));
}




































































 
static inline uint32_t LL_RCC_GetRTC_HSEPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR) & ((0x3FUL << (8U)))));
}



 



 








 
static inline void LL_RCC_SetTIMPrescaler(uint32_t Prescaler)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR))) & (~((0x1UL << (15U))))) | (Prescaler))));
}







 
static inline uint32_t LL_RCC_GetTIMPrescaler(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CFGR) & ((0x1UL << (15U)))));
}



 



 





 
static inline void LL_RCC_USBPHYC_EnablePowerDown(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->AHB1LPENR) |= ((0x1UL << (24U))));
}





 
static inline void LL_RCC_USBPHYC_DisablePowerDown(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->AHB1LPENR) &= ~((0x1UL << (24U))));
}





 
static inline uint32_t LL_RCC_USBPHYC_IsEnabledPowerDown(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->AHB1LPENR) & ((0x1UL << (24U)))) == ((0x1UL << (24U)))) ? 1UL : 0UL);
}



 



 












 
static inline void LL_RCC_PLL_SetSource(uint32_t PLLSource)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR))) & (~((0x3UL << (0U))))) | (PLLSource))));
}









 
static inline uint32_t LL_RCC_PLL_GetSource(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR) & ((0x3UL << (0U)))));
}





 
static inline void LL_RCC_PLL1_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (24U))));
}






 
static inline void LL_RCC_PLL1_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (24U))));
}





 
static inline uint32_t LL_RCC_PLL1_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (25U)))) == ((0x1UL << (25U)))) ? 1UL : 0UL);
}



















 
static inline void LL_RCC_PLL1_ConfigDomain_SYS(uint32_t Source, uint32_t M, uint32_t N, uint32_t P)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR))) & (~((0x3FUL << (4U)) | (0x3UL << (0U))))) | ((M << (4U)) | Source))));
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1))) & (~((0x7FUL << (9U)) | (0x1FFUL << (0U))))) | (((P - 1UL) << (9U)) | ((N - 1UL) << (0U))))));
}






 
static inline void LL_RCC_PLL1P_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (5U))));
}






 
static inline void LL_RCC_PLL1Q_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (6U))));
}






 
static inline void LL_RCC_PLL1R_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (7U))));
}






 
static inline void LL_RCC_PLL1S_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (8U))));
}





 
static inline void LL_RCC_PLL1FRACN_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (0U))));
}





 
static inline uint32_t LL_RCC_PLL1P_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (5U)))) == (0x1UL << (5U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1Q_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (6U)))) == (0x1UL << (6U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1R_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (7U)))) == (0x1UL << (7U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1S_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (8U)))) == (0x1UL << (8U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1FRACN_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (0U)))) == (0x1UL << (0U))) ? 1UL : 0UL);
}






 
static inline void LL_RCC_PLL1P_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (5U))));
}






 
static inline void LL_RCC_PLL1Q_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (6U))));
}






 
static inline void LL_RCC_PLL1R_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (7U))));
}






 
static inline void LL_RCC_PLL1S_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (8U))));
}





 
static inline void LL_RCC_PLL1FRACN_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (0U))));
}









 
static inline uint32_t LL_RCC_PLL1_GetVCOInputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x3UL << (3U)))));
}







 
static inline uint32_t LL_RCC_PLL1_GetVCOOutputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (1U)))));
}











 
static inline void LL_RCC_PLL1_SetVCOInputRange(uint32_t InputRange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x3UL << (3U))))) | (InputRange))));
}









 
static inline void LL_RCC_PLL1_SetVCOOutputRange(uint32_t VCORange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x1UL << (1U))))) | (VCORange))));
}





 
static inline uint32_t LL_RCC_PLL1_GetN(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1) & ((0x1FFUL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetM(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR) & ((0x3FUL << (4U)))) >>  (4U));
}





 
static inline uint32_t LL_RCC_PLL1_GetP(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1) & ((0x7FUL << (9U)))) >>  (9U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetQ(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1) & ((0x7FUL << (16U)))) >>  (16U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetR(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1) & ((0x7FUL << (24U)))) >>  (24U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetS(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR2) & ((0x7UL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetFRACN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1FRACR) & ((0x1FFFUL << (3U)))) >>  (3U));
}






 
static inline void LL_RCC_PLL1_SetN(uint32_t N)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1))) & (~((0x1FFUL << (0U))))) | ((N - 1UL) << (0U)))));
}






 
static inline void LL_RCC_PLL1_SetM(uint32_t M)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR))) & (~((0x3FUL << (4U))))) | (M << (4U)))));
}






 
static inline void LL_RCC_PLL1_SetP(uint32_t P)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1))) & (~((0x7FUL << (9U))))) | ((P - 1UL) << (9U)))));
}






 
static inline void LL_RCC_PLL1_SetQ(uint32_t Q)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1))) & (~((0x7FUL << (16U))))) | ((Q - 1UL) << (16U)))));
}






 
static inline void LL_RCC_PLL1_SetR(uint32_t R)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR1))) & (~((0x7FUL << (24U))))) | ((R - 1UL) << (24U)))));
}






 
static inline void LL_RCC_PLL1_SetS(uint32_t S)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1DIVR2))) & (~((0x7UL << (0U))))) | ((S - 1UL) << (0U)))));
}





 
static inline void LL_RCC_PLL1_SetFRACN(uint32_t FRACN)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1FRACR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1FRACR))) & (~((0x1FFFUL << (3U))))) | (FRACN << (3U)))));
}






 
static inline void LL_RCC_PLL1_EnableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (2U))));
}





 
static inline void LL_RCC_PLL1_DisableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (2U))));
}





 
static inline uint32_t LL_RCC_PLL1_IsEnabledSpreadSpectrum(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (2U)))) == (0x1UL << (2U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1_GetModulationPeriod(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1SSCGR) & ((0x1FFFUL << (0U)))));
}





 
static inline uint32_t LL_RCC_PLL1_GetIncrementStep(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1SSCGR) & ((0x7FFFUL << (16U)))) >> (16U));
}







 
static inline uint32_t LL_RCC_PLL1_GetSpreadSelection(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1SSCGR) & ((0x1UL << (15U)))));
}





 
static inline uint32_t LL_RCC_PLL1_IsEnabledRPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1SSCGR) & ((0x1UL << (14U)))) == 0U) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL1_IsEnabledTPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL1SSCGR) & ((0x1UL << (13U)))) == 0U) ? 1UL : 0UL);
}





 
static inline void LL_RCC_PLL2_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (26U))));
}






 
static inline void LL_RCC_PLL2_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (26U))));
}





 
static inline uint32_t LL_RCC_PLL2_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (27U)))) == (0x1UL << (27U))) ? 1UL : 0UL);
}






 
static inline void LL_RCC_PLL2P_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (16U))));
}






 
static inline void LL_RCC_PLL2Q_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (17U))));
}






 
static inline void LL_RCC_PLL2R_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (18U))));
}






 
static inline void LL_RCC_PLL2S_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (19U))));
}






 
static inline void LL_RCC_PLL2T_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (20U))));
}





 
static inline void LL_RCC_PLL2FRACN_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (11U))));
}





 
static inline uint32_t LL_RCC_PLL2P_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (16U)))) == (0x1UL << (16U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2Q_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (17U)))) == (0x1UL << (17U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2R_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (18U)))) == (0x1UL << (18U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2S_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (19U)))) == (0x1UL << (19U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2T_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (20U)))) == (0x1UL << (20U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2FRACN_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (11U)))) == (0x1UL << (11U))) ? 1UL : 0UL);
}






 
static inline void LL_RCC_PLL2P_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (16U))));
}






 
static inline void LL_RCC_PLL2Q_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (17U))));
}






 
static inline void LL_RCC_PLL2R_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (18U))));
}






 
static inline void LL_RCC_PLL2S_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (19U))));
}






 
static inline void LL_RCC_PLL2T_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (20U))));
}





 
static inline void LL_RCC_PLL2FRACN_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (11U))));
}









 
static inline uint32_t LL_RCC_PLL2_GetVCOInputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x3UL << (14U)))) >> 11U);
}







 
static inline uint32_t LL_RCC_PLL2_GetVCOOutputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (12U)))) >> 11U);
}











 
static inline void LL_RCC_PLL2_SetVCOInputRange(uint32_t InputRange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x3UL << (14U))))) | ((InputRange << 11U)))));
}









 
static inline void LL_RCC_PLL2_SetVCOOutputRange(uint32_t VCORange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x1UL << (12U))))) | ((VCORange << 11U)))));
}





 
static inline uint32_t LL_RCC_PLL2_GetN(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1) & ((0x1FFUL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetM(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR) & ((0x3FUL << (12U)))) >>  (12U));
}





 
static inline uint32_t LL_RCC_PLL2_GetP(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1) & ((0x7FUL << (9U)))) >>  (9U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetQ(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1) & ((0x7FUL << (16U)))) >>  (16U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetR(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1) & ((0x7FUL << (24U)))) >>  (24U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetS(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2) & ((0x7UL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetT(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2) & ((0x7UL << (8U)))) >>  (8U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetFRACN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2FRACR) & ((0x1FFFUL << (3U)))) >>  (3U));
}






 
static inline void LL_RCC_PLL2_SetN(uint32_t N)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1))) & (~((0x1FFUL << (0U))))) | ((N - 1UL) << (0U)))));
}






 
static inline void LL_RCC_PLL2_SetM(uint32_t M)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR))) & (~((0x3FUL << (12U))))) | (M << (12U)))));
}






 
static inline void LL_RCC_PLL2_SetP(uint32_t P)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1))) & (~((0x7FUL << (9U))))) | ((P - 1UL) << (9U)))));
}






 
static inline void LL_RCC_PLL2_SetQ(uint32_t Q)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1))) & (~((0x7FUL << (16U))))) | ((Q - 1UL) << (16U)))));
}






 
static inline void LL_RCC_PLL2_SetR(uint32_t R)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR1))) & (~((0x7FUL << (24U))))) | ((R - 1UL) << (24U)))));
}






 
static inline void LL_RCC_PLL2_SetS(uint32_t S)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2))) & (~((0x7UL << (0U))))) | ((S - 1UL) << (0U)))));
}






 
static inline void LL_RCC_PLL2_SetT(uint32_t T)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2DIVR2))) & (~((0x7UL << (8U))))) | ((T - 1UL) << (8U)))));
}





 
static inline void LL_RCC_PLL2_SetFRACN(uint32_t FRACN)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2FRACR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2FRACR))) & (~((0x1FFFUL << (3U))))) | (FRACN << (3U)))));
}






 
static inline void LL_RCC_PLL2_EnableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (13U))));
}





 
static inline void LL_RCC_PLL2_DisableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (13U))));
}





 
static inline uint32_t LL_RCC_PLL2_IsEnabledSpreadSpectrum(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (13U)))) == (0x1UL << (13U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2_GetModulationPeriod(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2SSCGR) & ((0x1FFFUL << (0U)))));
}





 
static inline uint32_t LL_RCC_PLL2_GetIncrementStep(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2SSCGR) & ((0x7FFFUL << (16U)))) >> (16U));
}







 
static inline uint32_t LL_RCC_PLL2_GetSpreadSelection(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2SSCGR) & ((0x1UL << (15U)))));
}






 
static inline uint32_t LL_RCC_PLL2_IsEnabledRPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2SSCGR) & ((0x1UL << (14U)))) == 0U) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL2_IsEnabledTPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL2SSCGR) & ((0x1UL << (13U)))) == 0U) ? 1UL : 0UL);
}




 
static inline void LL_RCC_PLL3_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) |= ((0x1UL << (28U))));
}






 
static inline void LL_RCC_PLL3_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) &= ~((0x1UL << (28U))));
}





 
static inline uint32_t LL_RCC_PLL3_IsReady(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CR) & ((0x1UL << (29U)))) == (0x1UL << (29U))) ? 1UL : 0UL);
}






 
static inline void LL_RCC_PLL3P_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (27U))));
}






 
static inline void LL_RCC_PLL3Q_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (28U))));
}






 
static inline void LL_RCC_PLL3R_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (29U))));
}






 
static inline void LL_RCC_PLL3S_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (30U))));
}





 
static inline void LL_RCC_PLL3FRACN_Enable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (22U))));
}





 
static inline uint32_t LL_RCC_PLL3P_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (27U)))) == (0x1UL << (27U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3Q_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (28U)))) == (0x1UL << (28U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3R_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (29U)))) == (0x1UL << (29U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3S_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (30U)))) == (0x1UL << (30U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3FRACN_IsEnabled(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (22U)))) == (0x1UL << (22U))) ? 1UL : 0UL);
}






 
static inline void LL_RCC_PLL3P_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (27U))));
}






 
static inline void LL_RCC_PLL3Q_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (28U))));
}






 
static inline void LL_RCC_PLL3R_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (29U))));
}






 
static inline void LL_RCC_PLL3S_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (30U))));
}





 
static inline void LL_RCC_PLL3FRACN_Disable(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (22U))));
}









 
static inline uint32_t LL_RCC_PLL3_GetVCOInputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x3UL << (25U)))) >> 22U);
}







 
static inline uint32_t LL_RCC_PLL3_GetVCOOutputRange(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (23U)))) >> 22U);
}











 
static inline void LL_RCC_PLL3_SetVCOInputRange(uint32_t InputRange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x3UL << (25U))))) | ((InputRange << 22U)))));
}









 
static inline void LL_RCC_PLL3_SetVCOOutputRange(uint32_t VCORange)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR))) & (~((0x1UL << (23U))))) | ((VCORange << 22U)))));
}





 
static inline uint32_t LL_RCC_PLL3_GetN(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1) & ((0x1FFUL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetM(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR) & ((0x3FUL << (20U)))) >>  (20U));
}





 
static inline uint32_t LL_RCC_PLL3_GetP(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1) & ((0x7FUL << (9U)))) >>  (9U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetQ(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1) & ((0x7FUL << (16U)))) >>  (16U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetR(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1) & ((0x7FUL << (24U)))) >>  (24U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetS(void)
{
  return (uint32_t)((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR2) & ((0x7UL << (0U)))) >>  (0U)) + 1UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetFRACN(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3FRACR) & ((0x1FFFUL << (3U)))) >>  (3U));
}






 
static inline void LL_RCC_PLL3_SetN(uint32_t N)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1))) & (~((0x1FFUL << (0U))))) | ((N - 1UL) << (0U)))));
}






 
static inline void LL_RCC_PLL3_SetM(uint32_t M)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCKSELR))) & (~((0x3FUL << (20U))))) | (M << (20U)))));
}






 
static inline void LL_RCC_PLL3_SetP(uint32_t P)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1))) & (~((0x7FUL << (9U))))) | ((P - 1UL) << (9U)))));
}






 
static inline void LL_RCC_PLL3_SetQ(uint32_t Q)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1))) & (~((0x7FUL << (16U))))) | ((Q - 1UL) << (16U)))));
}






 
static inline void LL_RCC_PLL3_SetR(uint32_t R)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR1))) & (~((0x7FUL << (24U))))) | ((R - 1UL) << (24U)))));
}






 
static inline void LL_RCC_PLL3_SetS(uint32_t S)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR2)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3DIVR2))) & (~((0x7UL << (0U))))) | ((S - 1UL) << (0U)))));
}





 
static inline void LL_RCC_PLL3_SetFRACN(uint32_t FRACN)
{
  (((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3FRACR)) = ((((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3FRACR))) & (~((0x1FFFUL << (3U))))) | (FRACN << (3U)))));
}







 
static inline void LL_RCC_PLL3_EnableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) |= ((0x1UL << (24U))));
}





 
static inline void LL_RCC_PLL3_DisableSpreadSpectrum(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) &= ~((0x1UL << (24U))));
}





 
static inline uint32_t LL_RCC_PLL3_IsEnabledSpreadSpectrum(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLLCFGR) & ((0x1UL << (24U)))) == (0x1UL << (24U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3_GetModulationPeriod(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3SSCGR) & ((0x1FFFUL << (0U)))));
}





 
static inline uint32_t LL_RCC_PLL3_GetIncrementStep(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3SSCGR) & ((0x7FFFUL << (16U)))) >> (16U));
}







 
static inline uint32_t LL_RCC_PLL3_GetSpreadSelection(void)
{
  return (uint32_t)(((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3SSCGR) & ((0x1UL << (15U)))));
}






 
static inline uint32_t LL_RCC_PLL3_IsEnabledRPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3SSCGR) & ((0x1UL << (14U)))) == 0U) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_PLL3_IsEnabledTPDFNDithering(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->PLL3SSCGR) & ((0x1UL << (13U)))) == 0U) ? 1UL : 0UL);
}


 




 





 
static inline void LL_RCC_ClearFlag_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_ClearFlag_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (1U))));
}





 
static inline void LL_RCC_ClearFlag_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (2U))));
}





 
static inline void LL_RCC_ClearFlag_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (3U))));
}





 
static inline void LL_RCC_ClearFlag_CSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (4U))));
}





 
static inline void LL_RCC_ClearFlag_HSI48RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (5U))));
}





 
static inline void LL_RCC_ClearFlag_PLL1RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (6U))));
}





 
static inline void LL_RCC_ClearFlag_PLL2RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (7U))));
}





 
static inline void LL_RCC_ClearFlag_PLL3RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (8U))));
}





 
static inline void LL_RCC_ClearFlag_LSECSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (9U))));
}





 
static inline void LL_RCC_ClearFlag_HSECSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CICR) |= ((0x1UL << (10U))));
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (0U)))) == (0x1UL << (0U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LSERDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (1U)))) == (0x1UL << (1U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (2U)))) == (0x1UL << (2U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSERDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (3U)))) == (0x1UL << (3U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_CSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (4U)))) == (0x1UL << (4U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSI48RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (5U)))) == (0x1UL << (5U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLL1RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (6U)))) == (0x1UL << (6U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLL2RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (7U)))) == (0x1UL << (7U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PLL3RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (8U)))) == (0x1UL << (8U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LSECSS(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (9U)))) == (0x1UL << (9U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_HSECSS(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIFR) & ((0x1UL << (10U)))) == (0x1UL << (10U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_LPWRRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (30U)))) == (0x1UL << (30U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_WWDGRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (28U)))) == (0x1UL << (28U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_IWDGRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (26U)))) == (0x1UL << (26U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_SFTRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (24U)))) == (0x1UL << (24U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PORRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (23U)))) == (0x1UL << (23U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_PINRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (22U)))) == (0x1UL << (22U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsActiveFlag_BORRST(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) & ((0x1UL << (21U)))) == ((0x1UL << (21U)))) ? 1UL : 0UL);
}





 
static inline void LL_RCC_ClearResetFlags(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->RSR) |= ((0x1UL << (16U))));
}



 



 





 
static inline void LL_RCC_EnableIT_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (0U))));
}





 
static inline void LL_RCC_EnableIT_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (1U))));
}





 
static inline void LL_RCC_EnableIT_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (2U))));
}





 
static inline void LL_RCC_EnableIT_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (3U))));
}





 
static inline void LL_RCC_EnableIT_CSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (4U))));
}





 
static inline void LL_RCC_EnableIT_HSI48RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (5U))));
}





 
static inline void LL_RCC_EnableIT_PLL1RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (6U))));
}





 
static inline void LL_RCC_EnableIT_PLL2RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (7U))));
}





 
static inline void LL_RCC_EnableIT_PLL3RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (8U))));
}





 
static inline void LL_RCC_EnableIT_LSECSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) |= ((0x1UL << (9U))));
}





 
static inline void LL_RCC_DisableIT_LSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (0U))));
}





 
static inline void LL_RCC_DisableIT_LSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (1U))));
}





 
static inline void LL_RCC_DisableIT_HSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (2U))));
}





 
static inline void LL_RCC_DisableIT_HSERDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (3U))));
}





 
static inline void LL_RCC_DisableIT_CSIRDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (4U))));
}





 
static inline void LL_RCC_DisableIT_HSI48RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (5U))));
}





 
static inline void LL_RCC_DisableIT_PLL1RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (6U))));
}





 
static inline void LL_RCC_DisableIT_PLL2RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (7U))));
}





 
static inline void LL_RCC_DisableIT_PLL3RDY(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (8U))));
}





 
static inline void LL_RCC_DisableIT_LSECSS(void)
{
  ((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) &= ~((0x1UL << (9U))));
}





 
static inline uint32_t LL_RCC_IsEnableIT_LSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (0U)))) == (0x1UL << (0U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_LSERDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (1U)))) == (0x1UL << (1U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_HSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (2U)))) == (0x1UL << (2U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_HSERDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (3U)))) == (0x1UL << (3U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_CSIRDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (4U)))) == (0x1UL << (4U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_HSI48RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (5U)))) == (0x1UL << (5U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_PLL1RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (6U)))) == (0x1UL << (6U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_PLL2RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (7U)))) == (0x1UL << (7U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_PLL3RDY(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (8U)))) == (0x1UL << (8U))) ? 1UL : 0UL);
}





 
static inline uint32_t LL_RCC_IsEnableIT_LSECSS(void)
{
  return ((((((RCC_TypeDef *) ((0x40000000UL + 0x18020000UL) + 0x4400UL))->CIER) & ((0x1UL << (9U)))) == (0x1UL << (9U))) ? 1UL : 0UL);
}


 



 











 
static inline uint32_t LL_RCC_CalcPLLClockFreq(uint32_t PLLInputFreq, uint32_t M, uint32_t N, uint32_t FRACN,
                                                 uint32_t PQR)
{
  float_t freq;

  freq = ((float_t)PLLInputFreq / (float_t)M) * ((float_t)N + ((float_t)FRACN / (float_t)0x2000));

  freq = freq / (float_t)PQR;

  return (uint32_t)freq;
}


 




 



 




 





 



 

 



 



 
typedef struct
{
  uint32_t PLLState;   
 

  uint32_t PLLSource;  
 

  uint32_t PLLM;       
 

  uint32_t PLLN;       
 

  uint32_t PLLP;       

 

  uint32_t PLLQ;       
 

  uint32_t PLLR;       
 

  uint32_t PLLS;       
 

  uint32_t PLLT;       
 

  uint32_t PLLFractional;   
 

} RCC_PLLInitTypeDef;



 
typedef struct
{
  uint32_t OscillatorType;       
 

  uint32_t HSEState;             
 

  uint32_t LSEState;             
 

  uint32_t HSIState;             
 

  uint32_t HSIDiv;               
 

  uint32_t HSICalibrationValue;  
 

  uint32_t LSIState;             
 

  uint32_t HSI48State;           
 

  uint32_t CSIState;             
 

  RCC_PLLInitTypeDef PLL1;        

  RCC_PLLInitTypeDef PLL2;        

  RCC_PLLInitTypeDef PLL3;        

} RCC_OscInitTypeDef;



 
typedef struct
{
  uint32_t ClockType;             
 

  uint32_t SYSCLKSource;          
 

  uint32_t SYSCLKDivider;         
 

  uint32_t AHBCLKDivider;         

 

  uint32_t APB1CLKDivider;        

 

  uint32_t APB2CLKDivider;        

 

  uint32_t APB4CLKDivider;        

 

  uint32_t APB5CLKDivider;        

 

} RCC_ClkInitTypeDef;



 

 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 

 




 


 





 



 


 



 


 



 


 



 


 










 

 

 

 


 



 


 



 


 



 


 



 


 



 

 



 






 




















































 







 






















 







 













 







 













 







 
































 







 























 







 




























































 







 

























 







 























 







 









 







 












 







 








 







 








 







 

















 







 













 







 
































 







 














 







 













 







 






 




 











 




 







 




 









 




 

















 




 














 




 
































 




 















 




 













 




 







 









 





















 








 














 








 














 








 
































 








 


































 








 






























































 








 


























 








 
























 








 










 








 










 








 








 








 








 








 

















 








 


















 








 
































 








 














 








 













 








 






 



 



















 








 















 









 









 

















 









 









 








 









 





















 






















 



 



 



 






















 







 




 



 









 








 







 













 











 





 












































 















 










 







 






 











 












 





 









































 

















 









 











 






 





 











 











 




































 

















 









 










 



 



 










 








 



 



 















 


 



 








 








 


 



 












 













 



 




 













 














 















 
















 




 























 



 



 

 

















 

 


 



 



 

 


 



 



 
typedef struct
{
  uint32_t PeriphClockSelection;   
 

  uint32_t FmcClockSelection;      
 

  uint32_t Xspi1ClockSelection;    
 

  uint32_t Xspi2ClockSelection;    
 

  uint32_t CkperClockSelection;   
 

  uint32_t AdcClockSelection;      
 

  uint32_t Adf1ClockSelection;     
 

  uint32_t CecClockSelection;      
 

  uint32_t Eth1RefClockSelection;     
 

  uint32_t Eth1PhyClockSelection;   
 

  uint32_t FdcanClockSelection;    
 

  uint32_t I2c1_I3c1ClockSelection;     
 

  uint32_t I2c23ClockSelection;    
 

  uint32_t Lptim1ClockSelection;   
 

  uint32_t Lptim23ClockSelection;  
 

  uint32_t Lptim45ClockSelection;  
 

  uint32_t Lpuart1ClockSelection;  
 

  uint32_t LtdcClockSelection;     
 

  uint32_t PssiClockSelection;     
 

  uint32_t Sai1ClockSelection;     
 

  uint32_t Sai2ClockSelection;     
 

  uint32_t Sdmmc12ClockSelection;  
 

  uint32_t Spi1ClockSelection;     
 

  uint32_t Spi23ClockSelection;    
 

  uint32_t Spi45ClockSelection;    
 

  uint32_t Spi6ClockSelection;     
 

  uint32_t SpdifrxClockSelection;  
 

  uint32_t Usart1ClockSelection;   
 

  uint32_t Usart234578ClockSelection; 
 

  uint32_t UsbPhycClockSelection;  
 

  uint32_t UsbOtgFsClockSelection; 
 

  uint32_t RTCClockSelection;      
 

  uint32_t TIMPresSelection;       
 
} RCC_PeriphCLKInitTypeDef;



 
typedef struct
{
  uint32_t Prescaler;              
 

  uint32_t Source;                 
 

  uint32_t Polarity;               
 

  uint32_t ReloadValue;            

 

  uint32_t ErrorLimitValue;        
 

  uint32_t HSI48CalibrationValue;  
 

} RCC_CRSInitTypeDef;



 
typedef struct
{
  uint32_t ReloadValue;            
 

  uint32_t HSI48CalibrationValue;  
 

  uint32_t FreqErrorCapture;       

 

  uint32_t FreqErrorDirection;     


 

} RCC_CRSSynchroInfoTypeDef;



 


 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 



 



 

 


 







 






 










 









 







 






 







 






 







 






 






 





 







 






 








 







 








 







 








 







 








 







 










 









 










 









 










 









 










 









 




 







 






 







 






 








 





 











 








 












 









 







 





 










 









 











 








 











 








 












 









 












 









 










 









 










 









 










 







 






 








 







 




 




 




 




 




 




 




 




 




 




 




 




 




 










 










 









 











 
 














 














 

 




 




 




 





 




 










 



 



 

 


 


 
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(const RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef  *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);
void     HAL_RCCEx_EnableClockProtection(uint32_t ProtectClk);
void     HAL_RCCEx_DisableClockProtection(uint32_t ProtectClk);


 



 
void     HAL_RCCEx_WakeUpStopCLKConfig(uint32_t WakeUpClk);
void     HAL_RCCEx_KerWakeUpStopCLKConfig(uint32_t WakeUpClk);
void     HAL_RCCEx_EnableLSECSS(void);
void     HAL_RCCEx_DisableLSECSS(void);
void     HAL_RCCEx_EnableLSECSS_IT(void);
void     HAL_RCCEx_LSECSS_IRQHandler(void);
void     HAL_RCCEx_LSECSS_Callback(void);


 



 
void     HAL_RCCEx_CRSConfig(const RCC_CRSInitTypeDef *pInit);
void     HAL_RCCEx_CRSSoftwareSynchronizationGenerate(void);
void     HAL_RCCEx_CRSGetSynchronizationInfo(RCC_CRSSynchroInfoTypeDef *pSynchroInfo);
uint32_t HAL_RCCEx_CRSWaitSynchronization(uint32_t Timeout);
void     HAL_RCCEx_CRS_IRQHandler(void);
void     HAL_RCCEx_CRS_SyncOkCallback(void);
void     HAL_RCCEx_CRS_SyncWarnCallback(void);
void     HAL_RCCEx_CRS_ExpectedSyncCallback(void);
void     HAL_RCCEx_CRS_ErrorCallback(uint32_t Error);


 



 

 


 



 












































 



 



 



 


 


 



 
 
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(const RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);



 



 
 
void     HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void     HAL_RCC_EnableCSS(void);
void     HAL_RCC_DisableCSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
uint32_t HAL_RCC_GetPCLK4Freq(void);
uint32_t HAL_RCC_GetPCLK5Freq(void);
uint32_t HAL_RCC_GetPLL1PFreq(void);
uint32_t HAL_RCC_GetPLL1QFreq(void);
uint32_t HAL_RCC_GetPLL1RFreq(void);
uint32_t HAL_RCC_GetPLL1SFreq(void);
uint32_t HAL_RCC_GetPLL2PFreq(void);
uint32_t HAL_RCC_GetPLL2QFreq(void);
uint32_t HAL_RCC_GetPLL2RFreq(void);
uint32_t HAL_RCC_GetPLL2SFreq(void);
uint32_t HAL_RCC_GetPLL2TFreq(void);
uint32_t HAL_RCC_GetPLL3PFreq(void);
uint32_t HAL_RCC_GetPLL3QFreq(void);
uint32_t HAL_RCC_GetPLL3RFreq(void);
uint32_t HAL_RCC_GetPLL3SFreq(void);
void     HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void     HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);
 
void     HAL_RCC_NMI_IRQHandler(void);
 
void     HAL_RCC_HSECSSCallback(void);
void     HAL_RCC_LSECSSCallback(void);
uint32_t          HAL_RCC_GetResetSource(void);


 



 

 
 
 


 


 





 

 


 



 













































 



 



 



 



















 

 


 



 




 

 



 


 
typedef struct
{
  uint32_t Pin;        
 

  uint32_t Mode;       
 

  uint32_t Pull;       
 

  uint32_t Speed;      
 

  uint32_t Alternate;  
 
} GPIO_InitTypeDef;



 
typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;


 

 


 


 



 









 


 




 


 




 


 



 

 


 






 






 






 






 






 


 

 


 


 



 







 

 
















 

 


 



 




 

 
 
 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 



 




 



 

 


 



 


 



 

 


 



 




 



 




 

 
void              HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, const GPIO_InitTypeDef *GPIO_Init);
void              HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin);



 




 

 
GPIO_PinState     HAL_GPIO_ReadPin(const GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void              HAL_GPIO_WriteMultipleStatePin(GPIO_TypeDef *GPIOx, uint16_t PinReset, uint16_t PinSet);
void              HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void              HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);



 



 



 



 




















 

 


 



 



 


 




 



 
typedef struct
{
  uint32_t Request;               
 

  uint32_t BlkHWRequest;          

 

  uint32_t Direction;             
 

  uint32_t SrcInc;                
 

  uint32_t DestInc;               
 

  uint32_t SrcDataWidth;          
 

  uint32_t DestDataWidth;         
 

  uint32_t Priority;              
 

  uint32_t SrcBurstLength;        

 

  uint32_t DestBurstLength;       

 

  uint32_t TransferAllocatedPort; 
 

  uint32_t TransferEventMode;     
 

  uint32_t Mode;                  
 

} DMA_InitTypeDef;



 
typedef struct
{
  uint32_t Priority;          
 

  uint32_t LinkStepMode;      
 

  uint32_t LinkAllocatedPort; 
 

  uint32_t TransferEventMode; 
 

  uint32_t LinkedListMode;    
 

} DMA_InitLinkedListTypeDef;



 
typedef enum
{
  HAL_DMA_STATE_RESET   = 0x00U,  
  HAL_DMA_STATE_READY   = 0x01U,  
  HAL_DMA_STATE_BUSY    = 0x02U,  
  HAL_DMA_STATE_ERROR   = 0x03U,  
  HAL_DMA_STATE_ABORT   = 0x04U,  
  HAL_DMA_STATE_SUSPEND = 0x05U,  

} HAL_DMA_StateTypeDef;



 
typedef enum
{
  HAL_DMA_FULL_TRANSFER = 0x00U,  
  HAL_DMA_HALF_TRANSFER = 0x01U,  

} HAL_DMA_LevelCompleteTypeDef;



 
typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID     = 0x00U,  
  HAL_DMA_XFER_HALFCPLT_CB_ID = 0x01U,  
  HAL_DMA_XFER_ERROR_CB_ID    = 0x02U,  
  HAL_DMA_XFER_ABORT_CB_ID    = 0x03U,  
  HAL_DMA_XFER_SUSPEND_CB_ID  = 0x04U,  
  HAL_DMA_XFER_ALL_CB_ID      = 0x05U   

} HAL_DMA_CallbackIDTypeDef;



 
typedef struct __DMA_HandleTypeDef
{
  DMA_Channel_TypeDef       *Instance;                              

  DMA_InitTypeDef           Init;                                   

  DMA_InitLinkedListTypeDef InitLinkedList;                         

  HAL_LockTypeDef           Lock;                                   

  uint32_t                  Mode;                                   

  volatile HAL_DMA_StateTypeDef State;                                  

  volatile uint32_t             ErrorCode;                              

  void                      *Parent;                                

  void (* XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);      

  void (* XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);  

  void (* XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);     

  void (* XferAbortCallback)(struct __DMA_HandleTypeDef *hdma);     

  void (* XferSuspendCallback)(struct __DMA_HandleTypeDef *hdma);   

  struct __DMA_QListTypeDef  *LinkedListQueue;                      

} DMA_HandleTypeDef;


 


 



 




 


 




 


 




 


 




 
 

 


 


 




 


 




 


 




 


 




 


 




 


 




 



 




 


 




 


 




 


 




 


 




 






 


 



 




 





 





 















 














 














 














 













 





 





 


 


 
















 

 


 



 



 

 



 



 
typedef struct
{
  uint32_t DataExchange;  
 

  uint32_t DataAlignment; 
 

} DMA_DataHandlingConfTypeDef;



 
typedef struct
{
  uint32_t TriggerMode;      
 

  uint32_t TriggerPolarity;  
 

  uint32_t TriggerSelection; 
 

} DMA_TriggerConfTypeDef;



 
typedef struct
{
  uint32_t RepeatCount;      
 

  int32_t SrcAddrOffset;     






 

  int32_t DestAddrOffset;    






 

  int32_t BlkSrcAddrOffset;  






 

  int32_t BlkDestAddrOffset; 






 

} DMA_RepeatBlockConfTypeDef;



 
typedef enum
{
  HAL_DMA_QUEUE_STATE_RESET = 0x00U,  
  HAL_DMA_QUEUE_STATE_READY = 0x01U,  
  HAL_DMA_QUEUE_STATE_BUSY  = 0x02U   

} HAL_DMA_QStateTypeDef;



 
typedef struct
{
  uint32_t                    NodeType;           
 

  DMA_InitTypeDef             Init;                

  DMA_DataHandlingConfTypeDef DataHandlingConfig;  

  DMA_TriggerConfTypeDef      TriggerConfig;       

  DMA_RepeatBlockConfTypeDef  RepeatBlockConfig;   

  uint32_t                    SrcAddress;          
  uint32_t                    DstAddress;          
  uint32_t                    DataSize;            

} DMA_NodeConfTypeDef;



 
typedef struct
{
  uint32_t LinkRegisters[8U];  
  uint32_t NodeInfo;           

} DMA_NodeTypeDef;



 
typedef struct __DMA_QListTypeDef
{
  DMA_NodeTypeDef                *Head;               

  DMA_NodeTypeDef                *FirstCircularNode;  

  uint32_t                       NodeNumber;          

  volatile HAL_DMA_QStateTypeDef     State;               

  volatile uint32_t                  ErrorCode;           

  volatile uint32_t                  Type;                

} DMA_QListTypeDef;


 

 



 




 


 




 


 




 


 




 


 




 


 




 


 




 
 

 



 




 


 




 


 




 


 



 

 



 




 
HAL_StatusTypeDef HAL_DMAEx_List_Init(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMAEx_List_DeInit(DMA_HandleTypeDef *const hdma);


 




 
HAL_StatusTypeDef HAL_DMAEx_List_Start(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMAEx_List_Start_IT(DMA_HandleTypeDef *const hdma);


 




 
HAL_StatusTypeDef HAL_DMAEx_List_BuildNode(DMA_NodeConfTypeDef const *const pNodeConfig,
                                           DMA_NodeTypeDef *const pNode);
HAL_StatusTypeDef HAL_DMAEx_List_GetNodeConfig(DMA_NodeConfTypeDef *const pNodeConfig,
                                               DMA_NodeTypeDef const *const pNode);

HAL_StatusTypeDef HAL_DMAEx_List_InsertNode(DMA_QListTypeDef *const pQList,
                                            DMA_NodeTypeDef *const pPrevNode,
                                            DMA_NodeTypeDef *const pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Head(DMA_QListTypeDef *const pQList,
                                                 DMA_NodeTypeDef *const pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_InsertNode_Tail(DMA_QListTypeDef *const pQList,
                                                 DMA_NodeTypeDef *const pNewNode);

HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode(DMA_QListTypeDef *const pQList,
                                            DMA_NodeTypeDef *const pNode);
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Head(DMA_QListTypeDef *const pQList);
HAL_StatusTypeDef HAL_DMAEx_List_RemoveNode_Tail(DMA_QListTypeDef *const pQList);

HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode(DMA_QListTypeDef *const pQList,
                                             DMA_NodeTypeDef *const pOldNode,
                                             DMA_NodeTypeDef *const pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Head(DMA_QListTypeDef *const pQList,
                                                  DMA_NodeTypeDef *const pNewNode);
HAL_StatusTypeDef HAL_DMAEx_List_ReplaceNode_Tail(DMA_QListTypeDef *const pQList,
                                                  DMA_NodeTypeDef *const pNewNode);

HAL_StatusTypeDef HAL_DMAEx_List_ResetQ(DMA_QListTypeDef *const pQList);

HAL_StatusTypeDef HAL_DMAEx_List_InsertQ(DMA_QListTypeDef *const pSrcQList,
                                         DMA_NodeTypeDef const *const pPrevNode,
                                         DMA_QListTypeDef *const pDestQList);
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Head(DMA_QListTypeDef *const pSrcQList,
                                              DMA_QListTypeDef *const pDestQList);
HAL_StatusTypeDef HAL_DMAEx_List_InsertQ_Tail(DMA_QListTypeDef *const pSrcQList,
                                              DMA_QListTypeDef *const pDestQList);

HAL_StatusTypeDef HAL_DMAEx_List_SetCircularModeConfig(DMA_QListTypeDef *const pQList,
                                                       DMA_NodeTypeDef *const pFirstCircularNode);
HAL_StatusTypeDef HAL_DMAEx_List_SetCircularMode(DMA_QListTypeDef *const pQList);
HAL_StatusTypeDef HAL_DMAEx_List_ClearCircularMode(DMA_QListTypeDef *const pQList);

HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToDynamic(DMA_QListTypeDef *const pQList);
HAL_StatusTypeDef HAL_DMAEx_List_ConvertQToStatic(DMA_QListTypeDef *const pQList);

HAL_StatusTypeDef HAL_DMAEx_List_LinkQ(DMA_HandleTypeDef *const hdma,
                                       DMA_QListTypeDef *const pQList);
HAL_StatusTypeDef HAL_DMAEx_List_UnLinkQ(DMA_HandleTypeDef *const hdma);


 




 
HAL_StatusTypeDef HAL_DMAEx_ConfigDataHandling(DMA_HandleTypeDef *const hdma,
                                               DMA_DataHandlingConfTypeDef const *const pConfigDataHandling);
HAL_StatusTypeDef HAL_DMAEx_ConfigTrigger(DMA_HandleTypeDef *const hdma,
                                          DMA_TriggerConfTypeDef const *const pConfigTrigger);
HAL_StatusTypeDef HAL_DMAEx_ConfigRepeatBlock(DMA_HandleTypeDef *const hdma,
                                              DMA_RepeatBlockConfTypeDef const *const pConfigRepeatBlock);


 




 
HAL_StatusTypeDef HAL_DMAEx_Suspend(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMAEx_Suspend_IT(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMAEx_Resume(DMA_HandleTypeDef *const hdma);


 




 
uint32_t HAL_DMAEx_GetFifoLevel(DMA_HandleTypeDef const *const hdma);


 



 

 



 



 
typedef struct
{
  uint32_t cllr_offset;        

  uint32_t previousnode_addr;  

  uint32_t currentnode_pos;    

  uint32_t currentnode_addr;   

  uint32_t nextnode_addr;      

} DMA_NodeInQInfoTypeDef;


 

 



 











 

 



 














 


 



 



 



 



 



 




 



 




 
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *const hdma);


 




 
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *const hdma,
                                uint32_t SrcAddress,
                                uint32_t DstAddress,
                                uint32_t SrcDataSize);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *const hdma,
                                   uint32_t SrcAddress,
                                   uint32_t DstAddress,
                                   uint32_t SrcDataSize);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *const hdma,
                                          HAL_DMA_LevelCompleteTypeDef CompleteLevel,
                                          uint32_t Timeout);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *const hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *const hdma,
                                           HAL_DMA_CallbackIDTypeDef CallbackID,
                                           void (*const  pCallback)(DMA_HandleTypeDef *const _hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *const hdma,
                                             HAL_DMA_CallbackIDTypeDef CallbackID);


 




 
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef const *const hdma);
uint32_t             HAL_DMA_GetError(DMA_HandleTypeDef const *const hdma);


 




 

HAL_StatusTypeDef HAL_DMA_ConfigChannelAttributes(DMA_HandleTypeDef *const hdma,
                                                  uint32_t ChannelAttributes);
HAL_StatusTypeDef HAL_DMA_GetConfigChannelAttributes(DMA_HandleTypeDef const *const hdma,
                                                     uint32_t *const pChannelAttributes);

HAL_StatusTypeDef HAL_DMA_LockChannelAttributes(DMA_HandleTypeDef const *const hdma);
HAL_StatusTypeDef HAL_DMA_GetLockChannelAttributes(DMA_HandleTypeDef const *const hdma,
                                                   uint32_t *const pLockState);




 



 


 



 


 


 



 



















 


 



 



 



 



 



 



















 

 


 



 




 

 


 




 
typedef struct
{
  uint8_t  Enable;                
 
  uint8_t  Number;                
 
  uint32_t BaseAddress;           
 
  uint8_t  Size;                  
 
  uint8_t  SubRegionDisable;      
 
  uint8_t  TypeExtField;          
 
  uint8_t  AccessPermission;      
 
  uint8_t  DisableExec;           
 
  uint8_t  IsShareable;           
 
  uint8_t  IsCacheable;           
 
  uint8_t  IsBufferable;          
 
} MPU_Region_InitTypeDef;


 



 

 



 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 

 
 


 




 
 
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);


 




 
 
uint32_t HAL_NVIC_GetPriorityGrouping(void);
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t *pPreemptPriority, uint32_t *pSubPriority);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);
void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(const MPU_Region_InitTypeDef *pMPU_RegionInit);
void HAL_CORTEX_ClearEvent(void);


 



 

 
 
 
 


 



















 



 



 























 

 




 



 


 

 


 



 

typedef struct
{
  uint32_t DataType;                   
 
  uint32_t KeySize;                    
 
  uint32_t *pKey;                       
  uint32_t *pInitVect;                 
 
  uint32_t Algorithm;                  
 
  uint32_t *Header;                    

 
  uint32_t HeaderSize;                  
  uint32_t *B0;                         
  uint32_t DataWidthUnit;               
  uint32_t HeaderWidthUnit;            
 
  uint32_t KeyIVConfigSkip;            


 
  uint32_t KeyMode;                     
  uint32_t KeySelect;                  
 

} CRYP_ConfigTypeDef;




 

typedef enum
{
  HAL_CRYP_STATE_RESET             = 0x00U,   
  HAL_CRYP_STATE_READY             = 0x01U,   
  HAL_CRYP_STATE_BUSY              = 0x02U,   
} HAL_CRYP_STATETypeDef;





 

typedef struct
{
  void                              *Instance;         

  CRYP_ConfigTypeDef                Init;              

  uint32_t                          *pCrypInBuffPtr;  
 

  uint32_t                          *pCrypOutBuffPtr; 
 

  volatile uint16_t                     CrypHeaderCount;   

  volatile uint16_t                     CrypInCount;       

  volatile uint16_t                     CrypOutCount;      

  uint16_t                          Size;             
 

  uint32_t                          Phase;             

  DMA_HandleTypeDef                 *hdmain;           

  DMA_HandleTypeDef                 *hdmaout;          

  HAL_LockTypeDef                   Lock;              

  volatile  HAL_CRYP_STATETypeDef       State;             

  volatile uint32_t                     ErrorCode;         

  uint32_t                          Version;           

  uint32_t                          KeyIVConfig;      
 

  uint32_t                          SizesSum;         

 



} CRYP_HandleTypeDef;




 


 


 



 



 




 




 



 




 



 




 



 




 



 




 



 




 



 





 



 

 
 







 



 



 




 




 



 



 



 



 



 



 




 



 




 



 




 

 


 





 




 





 






















 











 














 














 











 



 

 
















 

 


 



 


 

 
 

 


 



 

 


 



 

 


 



 

 


 



 

 


 



 

 


 



 
HAL_StatusTypeDef HAL_CRYPEx_AESGCM_GenerateAuthTAG(CRYP_HandleTypeDef *hcryp, const uint32_t *pAuthTag,
                                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_AESCCM_GenerateAuthTAG(CRYP_HandleTypeDef *hcryp, const uint32_t *pAuthTag,
                                                    uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_EncryptSharedKey(CRYP_HandleTypeDef *hcryp, uint32_t *Key, uint32_t *Output,
                                              uint32_t ID, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_DecryptSharedKey(CRYP_HandleTypeDef *hcryp, uint32_t *Key, uint32_t ID,
                                              uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_UnwrapKey(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYPEx_WrapKey(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint32_t *Output, uint32_t Timeout);



 




 



 


 




 


 



 
HAL_StatusTypeDef HAL_CRYP_Init(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_DeInit(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_MspInit(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_MspDeInit(CRYP_HandleTypeDef *hcryp);
HAL_StatusTypeDef HAL_CRYP_SetConfig(CRYP_HandleTypeDef *hcryp, CRYP_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_CRYP_GetConfig(CRYP_HandleTypeDef *hcryp, CRYP_ConfigTypeDef *pConf);


 



 

 
HAL_StatusTypeDef HAL_CRYP_Encrypt(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_Decrypt(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output,
                                   uint32_t Timeout);
HAL_StatusTypeDef HAL_CRYP_Encrypt_IT(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Decrypt_IT(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Encrypt_DMA(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);
HAL_StatusTypeDef HAL_CRYP_Decrypt_DMA(CRYP_HandleTypeDef *hcryp, uint32_t *Input, uint16_t Size, uint32_t *Output);



 




 
 
void HAL_CRYP_IRQHandler(CRYP_HandleTypeDef *hcryp);
HAL_CRYP_STATETypeDef HAL_CRYP_GetState(const CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_InCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_OutCpltCallback(CRYP_HandleTypeDef *hcryp);
void HAL_CRYP_ErrorCallback(CRYP_HandleTypeDef *hcryp);
uint32_t HAL_CRYP_GetError(const CRYP_HandleTypeDef *hcryp);



 



 

 


 



 















 




 





 




 


 


 



 
 


 



 

 


 



 
 


 



 

 


 



 




 




 




 






















 

 


 



 




 

 



 
typedef enum
{
  HAL_EXTI_COMMON_CB_ID          = 0x00U,
} EXTI_CallbackIDTypeDef;




 
typedef struct
{
  uint32_t Line;                     
  void (* PendingCallback)(void);    
} EXTI_HandleTypeDef;



 
typedef struct
{
  uint32_t Line;      
 
  uint32_t Mode;      
 
  uint32_t Trigger;   
 
  uint32_t GPIOSel;   

 
} EXTI_ConfigTypeDef;



 

 


 



 


 



 


 



 


 




 


 



 

 


 



 

 


 


 



 



 



 



 



 



 

 


 








 


 



 




 
 
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(const EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);


 




 
 
void              HAL_EXTI_IRQHandler(const EXTI_HandleTypeDef *hexti);
uint32_t          HAL_EXTI_GetPending(const EXTI_HandleTypeDef *hexti, uint32_t Edge);
void              HAL_EXTI_ClearPending(const EXTI_HandleTypeDef *hexti, uint32_t Edge);
void              HAL_EXTI_GenerateSWI(const EXTI_HandleTypeDef *hexti);



 



 



 



 





















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t TypeErase;   
 
  uint32_t Sector;      

 
  uint32_t NbSectors;   

 
} FLASH_EraseInitTypeDef;



 

typedef struct
{
  uint32_t OptionType;                  
 
  uint32_t WRPState;                    
 
  uint32_t WRPSector;                   
 
  uint32_t USERType;                    
 
  uint32_t USERConfig1;                 




 
  uint32_t USERConfig2;                 


 
  uint32_t HDPStartPage;                
 
  uint32_t HDPEndPage;                  
 
  uint32_t NVState;                     
 
  uint32_t ROTConfig;                   

 
  uint32_t EPOCH;                       
 
} FLASH_OBProgramInitTypeDef;



 
typedef struct
{
  HAL_LockTypeDef        Lock;               
  uint32_t               ErrorCode;          
  uint32_t               ProcedureOnGoing;  
 
  uint32_t               Address;            
  uint32_t               Sector;            
 
  uint32_t               NbSectorsToErase;   
} FLASH_ProcessTypeDef;



 

 


 



 




 




 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 

 



 






















 





















 



 




 




















 




















 




















 




















 


 

 
















 

 


 



 



 

 


 


 
typedef struct
{
  uint32_t Index;     
 
  uint32_t Size;      
 
  uint32_t HDPLLevel; 
 
} FLASH_KeyConfigTypeDef;



 
typedef struct
{
  uint32_t TypeCRC;      
 

  uint32_t BurstSize;    
 

  uint32_t Sector;       
 

  uint32_t NbSectors;    

 

  uint32_t CRCStartAddr; 
 

  uint32_t CRCEndAddr;   
 

} FLASH_CRCInitTypeDef;



 
typedef struct
{
  uint32_t Area;        
 
  uint32_t Address;     
 
  uint32_t MasterID;    
 
} FLASH_EccInfoTypeDef;



 

 


 


 


 



 


 



 


 



 


 



 


 



 


 



 

 

 


 

 


 
HAL_StatusTypeDef HAL_FLASHEx_Erase(const FLASH_EraseInitTypeDef *pEraseInit, uint32_t *SectorError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(const FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(const FLASH_OBProgramInitTypeDef *pOBInit);
void              HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);


 



 
HAL_StatusTypeDef HAL_FLASHEx_OTPLockConfig(uint32_t OTPLBlock);
uint32_t          HAL_FLASHEx_GetOTPLock(void);
HAL_StatusTypeDef HAL_FLASHEx_KeyConfig(const FLASH_KeyConfigTypeDef *pKeyConfig, const uint32_t *pKey);
HAL_StatusTypeDef HAL_FLASHEx_GetKey(const FLASH_KeyConfigTypeDef *pKeyConfig, uint32_t *pKey);
HAL_StatusTypeDef HAL_FLASHEx_ComputeCRC(const FLASH_CRCInitTypeDef *pCRCInit, uint32_t *CRC_Result);


 



 
void              HAL_FLASHEx_EnableEccCorrectionInterrupt(void);
void              HAL_FLASHEx_DisableEccCorrectionInterrupt(void);
void              HAL_FLASHEx_EnableEccDetectionInterrupt(void);
void              HAL_FLASHEx_DisableEccDetectionInterrupt(void);
void              HAL_FLASHEx_GetEccInfo(FLASH_EccInfoTypeDef *pData);
void              HAL_FLASHEx_ECCD_IRQHandler(void);
__weak void       HAL_FLASHEx_EccDetectionCallback(void);
__weak void       HAL_FLASHEx_EccCorrectionCallback(void);


 



 

 


 
void FLASH_SectorErase(uint32_t Sector);


 

 


 







 



 



 




 


 

 


 
HAL_StatusTypeDef  HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint32_t DataAddress);
HAL_StatusTypeDef  HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint32_t DataAddress);
 
void               HAL_FLASH_IRQHandler(void);
 
void               HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void               HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);


 

 


 
HAL_StatusTypeDef  HAL_FLASH_Unlock(void);
HAL_StatusTypeDef  HAL_FLASH_Lock(void);
 
HAL_StatusTypeDef  HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef  HAL_FLASH_OB_Lock(void);


 

 


 
uint32_t HAL_FLASH_GetError(void);


 



 

 


 
extern FLASH_ProcessTypeDef pFlash;


 

 


 







 

 


 
































 

 


 
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);


 



 



 






















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t DataType;    
 

  uint32_t KeySize;      

  uint8_t *pKey;         

  uint32_t Algorithm;   
 


} HASH_ConfigTypeDef;



 
typedef enum
{
  HAL_HASH_STATE_RESET             = 0x00U,     
  HAL_HASH_STATE_READY             = 0x01U,     
  HAL_HASH_STATE_BUSY              = 0x02U,     
  HAL_HASH_STATE_SUSPENDED         = 0x03U      
} HAL_HASH_StateTypeDef;



 
typedef enum
{
  HAL_HASH_PHASE_READY             = 0x01U,     
  HAL_HASH_PHASE_PROCESS           = 0x02U,     
  HAL_HASH_PHASE_HMAC_STEP_1       = 0x03U,    
 
  HAL_HASH_PHASE_HMAC_STEP_2       = 0x04U,    
 
  HAL_HASH_PHASE_HMAC_STEP_3       = 0x05U     
 

} HAL_HASH_PhaseTypeDef;





 
typedef struct
{
  HASH_TypeDef               *Instance;         

  HASH_ConfigTypeDef           Init;            

  uint8_t const              *pHashInBuffPtr;   

  uint8_t                    *pHashOutBuffPtr;  

  volatile uint32_t              HashInCount;       

  uint32_t                   Size;              

  uint8_t                   *pHashKeyBuffPtr;   

  HAL_HASH_PhaseTypeDef      Phase;             

  DMA_HandleTypeDef          *hdmain;           

  HAL_LockTypeDef            Lock;              

  volatile  uint32_t             ErrorCode;         

  volatile HAL_HASH_StateTypeDef State;             

  volatile  uint32_t             Accumulation;      


} HASH_HandleTypeDef;





 

 



 



 


 



 
 
 


 



 


 



 


 



 


 



 


 



 



 



 

 


 











 








 








 








 








 




 





 




 




 



 

 



 



 
HAL_StatusTypeDef HAL_HASH_Init(HASH_HandleTypeDef *hhash);
HAL_StatusTypeDef HAL_HASH_DeInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspInit(HASH_HandleTypeDef *hhash);
void HAL_HASH_MspDeInit(HASH_HandleTypeDef *hhash);
HAL_StatusTypeDef HAL_HASH_GetConfig(HASH_HandleTypeDef *hhash, HASH_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_HASH_SetConfig(HASH_HandleTypeDef *hhash, HASH_ConfigTypeDef *pConf);

 

HAL_StatusTypeDef HAL_HASH_ProcessSuspend(HASH_HandleTypeDef *hhash);
void HAL_HASH_Resume(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer);
void HAL_HASH_Suspend(HASH_HandleTypeDef *hhash, uint8_t *pMemBuffer);


 



 

HAL_StatusTypeDef HAL_HASH_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                 uint8_t *const pOutBuffer,
                                 uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_Start_IT(HASH_HandleTypeDef *hhash, const  uint8_t *const pInBuffer, uint32_t Size,
                                    uint8_t  *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                     uint8_t  *const pOutBuffer);

HAL_StatusTypeDef HAL_HASH_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                             uint8_t *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size);



 



 
HAL_StatusTypeDef HAL_HASH_HMAC_Start(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                      uint8_t  *const pOutBuffer,
                                      uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_Start_DMA(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                          uint8_t *const pOutBuffer);
HAL_StatusTypeDef HAL_HASH_HMAC_Start_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                         uint8_t *const pOutBuffer);

HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                           uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size,
                                               uint8_t *const pOutBuffer, uint32_t Timeout);
HAL_StatusTypeDef HAL_HASH_HMAC_Accumulate_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer, uint32_t Size);
HAL_StatusTypeDef HAL_HASH_HMAC_AccumulateLast_IT(HASH_HandleTypeDef *hhash, const uint8_t *const pInBuffer,
                                                  uint32_t Size, uint8_t *const pOutBuffer);



 



 
void HAL_HASH_IRQHandler(HASH_HandleTypeDef *hhash);
void HAL_HASH_InCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_DgstCpltCallback(HASH_HandleTypeDef *hhash);
void HAL_HASH_ErrorCallback(HASH_HandleTypeDef *hhash);
HAL_HASH_StateTypeDef HAL_HASH_GetState(const HASH_HandleTypeDef *hhash);
uint32_t HAL_HASH_GetError(const HASH_HandleTypeDef *hhash);



 



 

 


 




 





 





 



 

 


 



 
 


 



 

 


 



 
 



 



 



 


 

























 

 


 



 



 

 

 


 



 


 



 


 




 


 



 


 



 

 


 




 






 






 







 








 






 



 



 

 


 




 
 
HAL_StatusTypeDef HAL_ICACHE_Enable(void);
HAL_StatusTypeDef HAL_ICACHE_Disable(void);
uint32_t HAL_ICACHE_IsEnabled(void);
HAL_StatusTypeDef HAL_ICACHE_ConfigAssociativityMode(uint32_t AssociativityMode);
HAL_StatusTypeDef HAL_ICACHE_DeInit(void);

 
HAL_StatusTypeDef HAL_ICACHE_Invalidate(void);
 
HAL_StatusTypeDef HAL_ICACHE_Invalidate_IT(void);
 
HAL_StatusTypeDef HAL_ICACHE_WaitForInvalidateComplete(void);

 
HAL_StatusTypeDef HAL_ICACHE_Monitor_Start(uint32_t MonitorType);
HAL_StatusTypeDef HAL_ICACHE_Monitor_Stop(uint32_t MonitorType);
HAL_StatusTypeDef HAL_ICACHE_Monitor_Reset(uint32_t MonitorType);
uint32_t HAL_ICACHE_Monitor_GetHitValue(void);
uint32_t HAL_ICACHE_Monitor_GetMissValue(void);



 




 
 
void HAL_ICACHE_IRQHandler(void);
void HAL_ICACHE_InvalidateCompleteCallback(void);
void HAL_ICACHE_ErrorCallback(void);



 




 



 



 
























 

 


 



 



 
typedef struct
{
  uint32_t         ContextID;               
   

  uint32_t         StartAddress;            

  uint32_t         EndAddress;              

  uint32_t         Mode;                    
   
  uint32_t         AccessMode;              
   

  uint32_t         PrivilegedAccess;        
   

} MCE_RegionConfigTypeDef;

typedef struct
{
  uint32_t         Nonce[2];               

  uint32_t         Version;                

  uint32_t         *pKey;                   

} MCE_AESConfigTypeDef;

typedef struct
{
  uint32_t          KeyType;              

  uint32_t          *pKey;                    

} MCE_NoekeonConfigTypeDef;




 
typedef enum
{
  HAL_MCE_STATE_RESET             = 0x00U,   
  HAL_MCE_STATE_READY             = 0x01U,   
  HAL_MCE_STATE_BUSY              = 0x02U,   
} HAL_MCE_StateTypeDef;

typedef struct
{
  MCE_TypeDef             *Instance;         

  HAL_MCE_StateTypeDef    State;             

  HAL_LockTypeDef         Lock;              

  volatile uint32_t           ErrorCode;         


} MCE_HandleTypeDef;


 


 



 


 



 


 



 


 




 


 



 


 



 


 




 


 




 


 



 


 



 


 



 


 



 


 





 

 


 





 











 











 














 















 




 




 


HAL_StatusTypeDef HAL_MCE_Init(MCE_HandleTypeDef *hmce);
HAL_StatusTypeDef HAL_MCE_DeInit(MCE_HandleTypeDef *hmce);
void HAL_MCE_MspInit(MCE_HandleTypeDef *hmce);
void HAL_MCE_MspDeInit(MCE_HandleTypeDef *hmce);


HAL_StatusTypeDef HAL_MCE_ConfigNoekeon(MCE_HandleTypeDef *hmce, const MCE_NoekeonConfigTypeDef *pConfig);
HAL_StatusTypeDef HAL_MCE_ConfigAESContext(MCE_HandleTypeDef *hmce, const MCE_AESConfigTypeDef  *AESConfig,
                                           uint32_t ContextIndex);
HAL_StatusTypeDef HAL_MCE_ConfigRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex,
                                       const MCE_RegionConfigTypeDef *pConfig);
HAL_StatusTypeDef HAL_MCE_SetRegionAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_MCE_EnableAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex);
HAL_StatusTypeDef HAL_MCE_DisableAESContext(MCE_HandleTypeDef *hmce, uint32_t ContextIndex);
HAL_StatusTypeDef HAL_MCE_EnableRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_MCE_DisableRegion(MCE_HandleTypeDef *hmce, uint32_t RegionIndex);
HAL_StatusTypeDef HAL_MCE_LockGlobalConfig(MCE_HandleTypeDef *hmce);
HAL_StatusTypeDef HAL_MCE_LockAESContextConfig(MCE_HandleTypeDef *hmce, uint32_t ContextIndex);
HAL_StatusTypeDef HAL_MCE_LockAESContextKey(MCE_HandleTypeDef *hmce, uint32_t ContextIndex);
HAL_StatusTypeDef HAL_MCE_LockNoekeonMasterKeys(MCE_HandleTypeDef *hmce);
HAL_StatusTypeDef HAL_MCE_LockNoekeonFastKeys(MCE_HandleTypeDef *hmce);
HAL_StatusTypeDef HAL_MCE_GetAESContextCRCKey(MCE_HandleTypeDef *hmce, uint32_t *pCRCKey, uint32_t ContextIndex);


void HAL_MCE_IRQHandler(MCE_HandleTypeDef *hmce);
void HAL_MCE_ErrorCallback(MCE_HandleTypeDef *hmce);

HAL_MCE_StateTypeDef HAL_MCE_GetState(MCE_HandleTypeDef const *hmce);
uint32_t HAL_MCE_GetError(MCE_HandleTypeDef const *hmce);
uint32_t HAL_MCE_KeyCRCComputation(const uint32_t *pKey);






 





 





 





 






 






 






 



 
























 

 


 



 




 

 


 




 
typedef enum
{
  HAL_PKA_STATE_RESET   = 0x00U,   
  HAL_PKA_STATE_READY   = 0x01U,   
  HAL_PKA_STATE_BUSY    = 0x02U,   
  HAL_PKA_STATE_ERROR   = 0x03U,   
}
HAL_PKA_StateTypeDef;



 





 



 




 
typedef struct
{
  PKA_TypeDef                   *Instance;               
  volatile HAL_PKA_StateTypeDef     State;                   
  volatile uint32_t                 ErrorCode;               
} PKA_HandleTypeDef;


 




 

typedef struct
{
  uint32_t scalarMulSize;               
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coefA;                 
  const uint8_t *coefB;                 
  const uint8_t *modulus;               
  const uint8_t *pointX;                
  const uint8_t *pointY;                
  const uint8_t *scalarMul;             
  const uint8_t *primeOrder;            
} PKA_ECCMulInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;              
  uint32_t scalarMulSize;               
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coefA;                 
  const uint8_t *coefB;                 
  const uint8_t *modulus;               
  const uint8_t *pointX;                
  const uint8_t *pointY;                
  const uint8_t *scalarMul;             
  const uint8_t *primeOrder;            
} PKA_ECCMulExInTypeDef;

typedef struct
{
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coefA;                 
  const uint8_t *coefB;                 
  const uint8_t *modulus;               
  const uint8_t *pointX;                
  const uint8_t *pointY;                
  const uint32_t *pMontgomeryParam;     
} PKA_PointCheckInTypeDef;

typedef struct
{
  uint32_t size;                        
  const uint8_t *pOpDp;                 
  const uint8_t *pOpDq;                 
  const uint8_t *pOpQinv;               
  const uint8_t *pPrimeP;               
  const uint8_t *pPrimeQ;               
  const uint8_t *popA;                  
} PKA_RSACRTExpInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;              
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coef;                  
  const uint8_t *modulus;               
  const uint8_t *basePointX;            
  const uint8_t *basePointY;            
  const uint8_t *pPubKeyCurvePtX;       
  const uint8_t *pPubKeyCurvePtY;       
  const uint8_t *RSign;                 
  const uint8_t *SSign;                 
  const uint8_t *hash;                  
  const uint8_t *primeOrder;            
} PKA_ECDSAVerifInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;              
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coef;                  
  const uint8_t *coefB;                 
  const uint8_t *modulus;               
  const uint8_t *integer;               
  const uint8_t *basePointX;            
  const uint8_t *basePointY;            
  const uint8_t *hash;                  
  const uint8_t *privateKey;            
  const uint8_t *primeOrder;            
} PKA_ECDSASignInTypeDef;

typedef struct
{
  uint8_t *RSign;                       
  uint8_t *SSign;                       
} PKA_ECDSASignOutTypeDef;

typedef struct
{
  uint8_t *ptX;                         
  uint8_t *ptY;                         
} PKA_ECDSASignOutExtParamTypeDef, PKA_ECCMulOutTypeDef, PKA_ECCProjective2AffineOutTypeDef,
PKA_ECCDoubleBaseLadderOutTypeDef;

typedef struct
{
  uint8_t *ptX;                         
  uint8_t *ptY;                         
  uint8_t *ptZ;                         
} PKA_ECCCompleteAdditionOutTypeDef;

typedef struct
{
  uint32_t expSize;                     
  uint32_t OpSize;                      
  const uint8_t *pExp;                  
  const uint8_t *pOp1;                  
  const uint8_t *pMod;                  
} PKA_ModExpInTypeDef;

typedef struct
{
  uint32_t expSize;                     
  uint32_t OpSize;                      
  const uint8_t *pOp1;                  
  const uint8_t *pExp;                  
  const uint8_t *pMod;                  
  const uint8_t *pPhi;                  
} PKA_ModExpProtectModeInTypeDef;

typedef struct
{
  uint32_t expSize;                     
  uint32_t OpSize;                      
  const uint8_t *pExp;                  
  const uint8_t *pOp1;                  
  const uint8_t *pMod;                  
  const uint32_t *pMontgomeryParam;     
} PKA_ModExpFastModeInTypeDef;

typedef struct
{
  uint32_t size;                        
  const uint8_t *pOp1;                  
} PKA_MontgomeryParamInTypeDef;

typedef struct
{
  uint32_t size;                        
  const uint32_t *pOp1;                 
  const uint32_t *pOp2;                 
} PKA_AddInTypeDef, PKA_SubInTypeDef, PKA_MulInTypeDef, PKA_CmpInTypeDef;

typedef struct
{
  uint32_t size;                        
  const uint32_t *pOp1;                 
  const uint8_t *pMod;                  
} PKA_ModInvInTypeDef;

typedef struct
{
  uint32_t OpSize;                      
  uint32_t modSize;                     
  const uint32_t *pOp1;                 
  const uint8_t *pMod;                  
} PKA_ModRedInTypeDef;

typedef struct
{
  uint32_t size;                        
  const uint32_t *pOp1;                 
  const uint32_t *pOp2;                 
  const uint8_t  *pOp3;                 
} PKA_ModAddInTypeDef, PKA_ModSubInTypeDef, PKA_MontgomeryMulInTypeDef;

typedef struct
{
  uint32_t primeOrderSize;              
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *coefA;                 
  const uint8_t *modulus;               
  const uint8_t *integerK;              
  const uint8_t *integerM;              
  const uint8_t *basePointX1;           
  const uint8_t *basePointY1;           
  const uint8_t *basePointZ1;           
  const uint8_t *basePointX2;           
  const uint8_t *basePointY2;           
  const uint8_t *basePointZ2;           
} PKA_ECCDoubleBaseLadderInTypeDef;

typedef struct
{
  uint32_t modulusSize;                 
  const uint8_t *modulus;               
  const uint8_t *basePointX;            
  const uint8_t *basePointY;            
  const uint8_t *basePointZ;            
  const uint32_t *pMontgomeryParam;     
} PKA_ECCProjective2AffineInTypeDef;

typedef struct
{
  uint32_t modulusSize;                 
  uint32_t coefSign;                    
  const uint8_t *modulus;               
  const uint8_t *coefA;                 
  const uint8_t *basePointX1;           
  const uint8_t *basePointY1;           
  const uint8_t *basePointZ1;           
  const uint8_t *basePointX2;           
  const uint8_t *basePointY2;           
  const uint8_t *basePointZ2;           
} PKA_ECCCompleteAdditionInTypeDef;


 



 

 


 



 


 




 



 



 



 



 



 



 

 



 




 










 










 










 










 










 




 




 




 


 

 
 


 



 
 
HAL_StatusTypeDef HAL_PKA_Init(PKA_HandleTypeDef *hpka);
HAL_StatusTypeDef HAL_PKA_DeInit(PKA_HandleTypeDef *hpka);
void              HAL_PKA_MspInit(PKA_HandleTypeDef *hpka);
void              HAL_PKA_MspDeInit(PKA_HandleTypeDef *hpka);




 



 
 
 
HAL_StatusTypeDef HAL_PKA_ModExp(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExp_IT(PKA_HandleTypeDef *hpka, PKA_ModExpInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModExpFastMode(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExpFastMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpFastModeInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in,
                                            uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModExpProtectMode_IT(PKA_HandleTypeDef *hpka, PKA_ModExpProtectModeInTypeDef *in);
void HAL_PKA_ModExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes);

HAL_StatusTypeDef HAL_PKA_ECDSASign(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECDSASign_IT(PKA_HandleTypeDef *hpka, PKA_ECDSASignInTypeDef *in);
void HAL_PKA_ECDSASign_GetResult(PKA_HandleTypeDef *hpka, PKA_ECDSASignOutTypeDef *out,
                                 PKA_ECDSASignOutExtParamTypeDef *outExt);

HAL_StatusTypeDef HAL_PKA_ECDSAVerif(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECDSAVerif_IT(PKA_HandleTypeDef *hpka, PKA_ECDSAVerifInTypeDef *in);
uint32_t HAL_PKA_ECDSAVerif_IsValidSignature(PKA_HandleTypeDef const *const hpka);

HAL_StatusTypeDef HAL_PKA_RSACRTExp(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_RSACRTExp_IT(PKA_HandleTypeDef *hpka, PKA_RSACRTExpInTypeDef *in);
void HAL_PKA_RSACRTExp_GetResult(PKA_HandleTypeDef *hpka, uint8_t *pRes);

HAL_StatusTypeDef HAL_PKA_PointCheck(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_PointCheck_IT(PKA_HandleTypeDef *hpka, PKA_PointCheckInTypeDef *in);
uint32_t HAL_PKA_PointCheck_IsOnCurve(PKA_HandleTypeDef const *const hpka);

HAL_StatusTypeDef HAL_PKA_ECCMul(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCMul_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ECCMulEx(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCMulEx_IT(PKA_HandleTypeDef *hpka, PKA_ECCMulExInTypeDef *in);
void HAL_PKA_ECCMul_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCMulOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_Add(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Add_IT(PKA_HandleTypeDef *hpka, PKA_AddInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Sub(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Sub_IT(PKA_HandleTypeDef *hpka, PKA_SubInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Cmp(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Cmp_IT(PKA_HandleTypeDef *hpka, PKA_CmpInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_Mul(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_Mul_IT(PKA_HandleTypeDef *hpka, PKA_MulInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModAdd(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModAdd_IT(PKA_HandleTypeDef *hpka, PKA_ModAddInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModSub(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModSub_IT(PKA_HandleTypeDef *hpka, PKA_ModSubInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModInv(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModInv_IT(PKA_HandleTypeDef *hpka, PKA_ModInvInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_ModRed(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ModRed_IT(PKA_HandleTypeDef *hpka, PKA_ModRedInTypeDef *in);
HAL_StatusTypeDef HAL_PKA_MontgomeryMul(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_MontgomeryMul_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryMulInTypeDef *in);
void HAL_PKA_Arithmetic_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes);

HAL_StatusTypeDef HAL_PKA_MontgomeryParam(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in, uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_MontgomeryParam_IT(PKA_HandleTypeDef *hpka, PKA_MontgomeryParamInTypeDef *in);
void HAL_PKA_MontgomeryParam_GetResult(PKA_HandleTypeDef *hpka, uint32_t *pRes);

HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in,
                                              uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCDoubleBaseLadder_IT(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderInTypeDef *in);
void HAL_PKA_ECCDoubleBaseLadder_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCDoubleBaseLadderOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in,
                                               uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCProjective2Affine_IT(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineInTypeDef *in);
void HAL_PKA_ECCProjective2Affine_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCProjective2AffineOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in,
                                              uint32_t Timeout);
HAL_StatusTypeDef HAL_PKA_ECCCompleteAddition_IT(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionInTypeDef *in);
void HAL_PKA_ECCCompleteAddition_GetResult(PKA_HandleTypeDef *hpka, PKA_ECCCompleteAdditionOutTypeDef *out);

HAL_StatusTypeDef HAL_PKA_Abort(PKA_HandleTypeDef *hpka);
void HAL_PKA_RAMReset(PKA_HandleTypeDef *hpka);
void HAL_PKA_OperationCpltCallback(PKA_HandleTypeDef *hpka);
void HAL_PKA_ErrorCallback(PKA_HandleTypeDef *hpka);
void HAL_PKA_IRQHandler(PKA_HandleTypeDef *hpka);


 



 
 
HAL_PKA_StateTypeDef HAL_PKA_GetState(const PKA_HandleTypeDef *hpka);
uint32_t             HAL_PKA_GetError(const PKA_HandleTypeDef *hpka);


 



 



 




 




















 

 


 



 



 

 



 



 
typedef struct
{
  uint32_t PVDLevel; 

 

  uint32_t Mode;     
 
} PWR_PVDTypeDef;


 

 



 



 
 

 

 


 



 


 



 


 



 
 



 



 


 



 


 



 



 














 
 

 

 

 

 

 


 



 


 



 


 



 
 


 












































 


















 










 










 




 




 




 




 




 




 




 




 




 




 




 




 




 


 

 
















 

 


 



 



 

 


 


 
typedef struct
{
  uint32_t AVDLevel; 
 

  uint32_t Mode;     
 
} PWREx_AVDTypeDef;



 
typedef struct
{
  uint32_t WakeUpPin;   
 

  uint32_t PinPolarity; 
 

  uint32_t PinPull;     
 
} PWREx_WakeupPinTypeDef;



 

 



 



 


 



 


 



 



 



 





 




 



 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 




 



 

 



 




 




 




 




 




 




 




 




 




 




 




 




 




 


 

 



 



 
 
HAL_StatusTypeDef HAL_PWREx_ConfigSupply(uint32_t SupplySource);
uint32_t          HAL_PWREx_GetSupplyConfig(void);

 
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
uint32_t          HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlStopModeVoltageScaling(uint32_t VoltageScaling);
uint32_t          HAL_PWREx_GetStopModeVoltageRange(void);


 



 
 
void HAL_PWREx_EnableFlashPowerDown(void);
void HAL_PWREx_DisableFlashPowerDown(void);

 
void              HAL_PWREx_EnableWakeUpPin(const PWREx_WakeupPinTypeDef *sPinParams);
void              HAL_PWREx_DisableWakeUpPin(uint32_t WakeUpPin);
uint32_t          HAL_PWREx_GetWakeupFlag(uint32_t WakeUpFlag);
HAL_StatusTypeDef HAL_PWREx_ClearWakeupFlag(uint32_t WakeUpFlag);

 
void HAL_PWREx_WAKEUP_PIN_IRQHandler(void);
void HAL_PWREx_WKUP1_Callback(void);
void HAL_PWREx_WKUP2_Callback(void);
void HAL_PWREx_WKUP3_Callback(void);
void HAL_PWREx_WKUP4_Callback(void);


 



 
 
HAL_StatusTypeDef HAL_PWREx_EnableBkUpReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableBkUpReg(void);

 
HAL_StatusTypeDef HAL_PWREx_EnableUSBReg(void);
HAL_StatusTypeDef HAL_PWREx_DisableUSBReg(void);
void              HAL_PWREx_EnableUSBVoltageDetector(void);
void              HAL_PWREx_DisableUSBVoltageDetector(void);
void              HAL_PWREx_EnableUSBHSregulator(void);
void              HAL_PWREx_DisableUSBHSregulator(void);

 
void HAL_PWREx_EnableUCPDStandbyMode(void);
void HAL_PWREx_DisableUCPDStandbyMode(void);
void HAL_PWREx_EnableUCPDDeadBattery(void);
void HAL_PWREx_DisableUCPDDeadBattery(void);

 
void HAL_PWREx_EnableBatteryCharging(uint32_t ResistorValue);
void HAL_PWREx_DisableBatteryCharging(void);

 
void HAL_PWREx_EnableAnalogBooster(void);
void HAL_PWREx_DisableAnalogBooster(void);

 
void HAL_PWREx_EnableXSPIM1(void);
void HAL_PWREx_DisableXSPIM1(void);

 
void HAL_PWREx_EnableXSPIM2(void);
void HAL_PWREx_DisableXSPIM2(void);

 
void     HAL_PWREx_ConfigXSPIPortCap(uint32_t PortCapacitor, uint32_t PortCapacitorSetting);
uint32_t HAL_PWREx_GetConfigXSPIPortCap(uint32_t PortCapacitor);



 



 
 
void     HAL_PWREx_EnableMonitoring(void);
void     HAL_PWREx_DisableMonitoring(void);
uint32_t HAL_PWREx_GetTemperatureLevel(void);
uint32_t HAL_PWREx_GetVBATLevel(void);

 
void HAL_PWREx_ConfigAVD(const PWREx_AVDTypeDef *sConfigAVD);
void HAL_PWREx_EnableAVD(void);
void HAL_PWREx_DisableAVD(void);

 
void HAL_PWREx_PVD_AVD_IRQHandler(void);
void HAL_PWREx_PVD_AVD_Callback(void);


 



 
 
void HAL_PWREx_EnablePullUpPullDownConfig(void);
void HAL_PWREx_DisablePullUpPullDownConfig(void);

 
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullUp(uint32_t GPIO_Port, uint32_t GPIO_Pin);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullUp(uint32_t GPIO_Port, uint32_t GPIO_Pin);
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullDown(uint32_t GPIO_Port, uint32_t GPIO_Pin);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullDown(uint32_t GPIO_Port, uint32_t GPIO_Pin);


 



 
 
 
 
 


 



 
 

 

 

 

 

 

 

 

 

 

 

 

 


 



 



 



 




 


 



 
 
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);


 



 
 
 
void HAL_PWR_ConfigPVD(const PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);

 
void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);

 
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

 
void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);


 



 

 
 
 


 



 


 



 
 


 



 
 

 

 

 

 

 


 



 



 



 



 




















 

 


 



 



 

 




 



 
typedef enum
{
  HAL_RAMECC_STATE_RESET             = 0x00U,   
  HAL_RAMECC_STATE_READY             = 0x01U,   
  HAL_RAMECC_STATE_BUSY              = 0x02U,   
  HAL_RAMECC_STATE_ERROR             = 0x03U,   
} HAL_RAMECC_StateTypeDef;




 

typedef struct
{
  RAMECC_MonitorTypeDef           *Instance;                                                             
  volatile HAL_RAMECC_StateTypeDef    State;                                                                 
  volatile uint32_t                   ErrorCode;                                                             
} RAMECC_HandleTypeDef;



 


 



 



 



 




 



 



 

 


 
















 

















 

















 












 











 





 


 

 




 




 
HAL_StatusTypeDef HAL_RAMECC_Init(RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_DeInit(RAMECC_HandleTypeDef *hramecc);
void              HAL_RAMECC_MspInit(RAMECC_HandleTypeDef *hramecc);
void              HAL_RAMECC_MspDeInit(RAMECC_HandleTypeDef *hramecc);


 




 
HAL_StatusTypeDef HAL_RAMECC_StartMonitor(RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_StopMonitor(RAMECC_HandleTypeDef *hramecc);
HAL_StatusTypeDef HAL_RAMECC_EnableNotification(RAMECC_HandleTypeDef *hramecc, uint32_t Notifications);
HAL_StatusTypeDef HAL_RAMECC_DisableNotification(RAMECC_HandleTypeDef *hramecc, uint32_t Notifications);


 




 
void              HAL_RAMECC_IRQHandler(RAMECC_HandleTypeDef *hramecc);
void              HAL_RAMECC_DetectSingleErrorCallback(RAMECC_HandleTypeDef *hramecc);
void              HAL_RAMECC_DetectDoubleErrorCallback(RAMECC_HandleTypeDef *hramecc);




 




 
uint32_t HAL_RAMECC_GetFailingAddress(const RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetFailingDataLow(const RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetFailingDataHigh(const RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_GetHammingErrorCode(const RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_IsECCSingleErrorDetected(const RAMECC_HandleTypeDef *hramecc);
uint32_t HAL_RAMECC_IsECCDoubleErrorDetected(const RAMECC_HandleTypeDef *hramecc);


 




 
HAL_RAMECC_StateTypeDef HAL_RAMECC_GetState(const RAMECC_HandleTypeDef *hramecc);
uint32_t                HAL_RAMECC_GetError(const RAMECC_HandleTypeDef *hramecc);


 



 
 



 


 

 



 







 



 

 



 


 



 



 



 



















 

 


 



 





 

 



 



 
typedef struct
{
  uint32_t                    ClockErrorDetection;  
} RNG_InitTypeDef;



 



 
typedef enum
{
  HAL_RNG_STATE_RESET     = 0x00U,   
  HAL_RNG_STATE_READY     = 0x01U,   
  HAL_RNG_STATE_BUSY      = 0x02U,   
  HAL_RNG_STATE_TIMEOUT   = 0x03U,   
  HAL_RNG_STATE_ERROR     = 0x04U    

} HAL_RNG_StateTypeDef;



 



 
typedef struct
{
  RNG_TypeDef                 *Instance;     

  RNG_InitTypeDef             Init;          

  HAL_LockTypeDef             Lock;          

  volatile HAL_RNG_StateTypeDef   State;         

  volatile  uint32_t              ErrorCode;     

  uint32_t                    RandomNumber;  


} RNG_HandleTypeDef;




 



 

 


 



 


 



 


 



 


 



 


 



 

 


 




 





 





 










 








 





 





 










 










 



 

 
















 

 


 



 





 

 



 



 

typedef struct
{
  uint32_t        Config1;            
  uint32_t        Config2;            
  uint32_t        Config3;            
  uint32_t        ClockDivider;      
 
  uint32_t        NistCompliance;    
 
  uint32_t        AutoReset;         
 
  uint32_t        HealthTest;           
 
} RNG_ConfigTypeDef;



 

 


 




 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 


 



 



 


 



 



 

 


 



 

 


 



 

 


 



 

 


 










 

 


 



 

 


 



 
HAL_StatusTypeDef HAL_RNGEx_SetConfig(RNG_HandleTypeDef *hrng, const RNG_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_RNGEx_GetConfig(RNG_HandleTypeDef *hrng, RNG_ConfigTypeDef *pConf);
HAL_StatusTypeDef HAL_RNGEx_LockConfig(RNG_HandleTypeDef *hrng);



 



 
HAL_StatusTypeDef HAL_RNGEx_RecoverSeedError(RNG_HandleTypeDef *hrng);



 



 



 



 




 




 


 



 
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);

 



 



 
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_ReadLastRandomNumber(const RNG_HandleTypeDef *hrng);

void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit);



 



 
HAL_RNG_StateTypeDef HAL_RNG_GetState(const RNG_HandleTypeDef *hrng);
uint32_t             HAL_RNG_GetError(const RNG_HandleTypeDef *hrng);


 



 

 


 






 


 

 


 
HAL_StatusTypeDef RNG_RecoverSeedError(RNG_HandleTypeDef *hrng);


 


 




 





















 

 


 



 



 

 


 



 
typedef enum
{
  HAL_RTC_STATE_RESET             = 0x00U,   
  HAL_RTC_STATE_READY             = 0x01U,   
  HAL_RTC_STATE_BUSY              = 0x02U,   
  HAL_RTC_STATE_TIMEOUT           = 0x03U,   
  HAL_RTC_STATE_ERROR             = 0x04U    

} HAL_RTCStateTypeDef;



 
typedef struct
{
  uint32_t HourFormat;        
 

  uint32_t AsynchPrediv;      
 

  uint32_t SynchPrediv;       
 

  uint32_t OutPut;            
 

  uint32_t OutPutRemap;       
 

  uint32_t OutPutPolarity;    
 

  uint32_t OutPutType;        
 

  uint32_t OutPutPullUp;      
 

  uint32_t BinMode;           
 

  uint32_t BinMixBcdU;        
 
} RTC_InitTypeDef;



 
typedef struct
{
  uint8_t Hours;            



 

  uint8_t Minutes;          
 

  uint8_t Seconds;          
 

  uint8_t TimeFormat;       
 

  uint32_t SubSeconds;      





 

  uint32_t SecondFraction;  



 

  uint32_t DayLightSaving;  
 

  uint32_t StoreOperation;  
 
} RTC_TimeTypeDef;



 
typedef struct
{
  uint8_t WeekDay;  
 

  uint8_t Month;    
 

  uint8_t Date;     
 

  uint8_t Year;     
 
} RTC_DateTypeDef;



 
typedef struct
{
  RTC_TimeTypeDef AlarmTime;      

  uint32_t AlarmMask;            
 

  uint32_t AlarmSubSecondMask;   





 

  uint32_t BinaryAutoClr;        



 

  uint32_t AlarmDateWeekDaySel;  
 

  uint8_t AlarmDateWeekDay;      



 

  uint32_t FlagAutoClr;          

 

  uint32_t Alarm;                
 
} RTC_AlarmTypeDef;



 
typedef struct
{
  RTC_TypeDef               *Instance;   

  RTC_InitTypeDef           Init;        

  HAL_LockTypeDef           Lock;        

  volatile HAL_RTCStateTypeDef  State;       


} RTC_HandleTypeDef;




 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 

 



 



 



 



 



 



 



 



 



 


 


 




 


 



 


 



 


 



 


 



 

 


 




 





 





 











 











 





 





 





 





 









 









 









 









 









 









 





 



 

 
















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t Tamper;                      
 

  uint32_t Trigger;                     
 

  uint32_t NoErase;                     
 

  uint32_t MaskFlag;                    
 

  uint32_t Filter;                      
 

  uint32_t SamplingFrequency;           

 

  uint32_t PrechargeDuration;           

 

  uint32_t TamperPullUp;                
 

  uint32_t TimeStampOnTamperDetection;  

 
} RTC_TamperTypeDef;


 




 


 




 
typedef struct
{
  uint32_t Enable;                      
 

  uint32_t Interrupt;                    
 

  uint32_t Output;                      

 

  uint32_t NoErase;                     
 

  uint32_t MaskFlag;                    
 

} RTC_ATampInputTypeDef;


typedef struct
{
  uint32_t ActiveFilter;                
 

  uint32_t ActiveAsyncPrescaler;        

 

  uint32_t TimeStampOnTamperDetection;  

 

  uint32_t ActiveOutputChangePeriod;    
 

  uint32_t Seed[4U];
  
 

  RTC_ATampInputTypeDef TampInput[8U];
  
 
} RTC_ActiveTampersTypeDef;


 



 
typedef struct
{
  uint32_t IntTamper;                   
 

  uint32_t TimeStampOnTamperDetection;  

 

  uint32_t NoErase;                     
 

} RTC_InternalTamperTypeDef;


 



 
typedef struct
{
  uint32_t rtcPrivilegeFull;            
 

  uint32_t rtcPrivilegeFeatures;        



 

  uint32_t tampPrivilegeFull;           

 

  uint32_t backupRegisterPrivZone;      

 

  uint32_t backupRegisterStartZone2;    



 

  uint32_t backupRegisterStartZone3;    


 

  uint32_t MonotonicCounterPrivilege;   

 
} RTC_PrivilegeStateTypeDef;


 



 

 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 



 



 



 




 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 




 


 




 


 




 

 


 



 


 



 



 



 


 



 


 



 


 



 


 



 

 


 













 


















 

 


 





 





 








 








 








 








 









 








 



 

 


 





 





 








 








 








 








 









 









 





 





 








 








 





 





 





 





 




 


 


 





 





 





 





 








 


 

 


 
















 















 


 















 
















 


 



























 




























 



























 




























 


 

 


 








 








 









 







 








 








 


 



 

 


 

 


 

HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_SetTimeStamp_IT(RTC_HandleTypeDef *hrtc, uint32_t TimeStampEdge, uint32_t RTC_TimeStampPin);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetInternalTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTimeStamp(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_GetTimeStamp(const RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTimeStamp,
                                         RTC_DateTypeDef *sTimeStampDate, uint32_t Format);
void              HAL_RTCEx_TimeStampIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTimeStampEvent(const RTC_HandleTypeDef *hrtc, uint32_t Timeout);
void              HAL_RTCEx_TimeStampEventCallback(RTC_HandleTypeDef *hrtc);


 


 


 

HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock);
HAL_StatusTypeDef HAL_RTCEx_SetWakeUpTimer_IT(RTC_HandleTypeDef *hrtc, uint32_t WakeUpCounter, uint32_t WakeUpClock,
                                              uint32_t WakeUpAutoClr);
HAL_StatusTypeDef HAL_RTCEx_DeactivateWakeUpTimer(RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTCEx_GetWakeUpTimer(const RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForWakeUpTimerEvent(const RTC_HandleTypeDef *hrtc, uint32_t Timeout);


 

 


 

HAL_StatusTypeDef HAL_RTCEx_SetSmoothCalib(RTC_HandleTypeDef *hrtc, uint32_t SmoothCalibPeriod,
                                           uint32_t SmoothCalibPlusPulses, uint32_t SmoothCalibMinusPulsesValue);
HAL_StatusTypeDef HAL_RTCEx_SetLowPowerCalib(RTC_HandleTypeDef *hrtc, uint32_t LowPowerCalib);
HAL_StatusTypeDef HAL_RTCEx_SetSynchroShift(RTC_HandleTypeDef *hrtc, uint32_t ShiftAdd1S, uint32_t ShiftSubFS);
HAL_StatusTypeDef HAL_RTCEx_SetCalibrationOutPut(RTC_HandleTypeDef *hrtc, uint32_t CalibOutput);
HAL_StatusTypeDef HAL_RTCEx_DeactivateCalibrationOutPut(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_SetRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateRefClock(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_EnableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DisableBypassShadow(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterIncrement(const RTC_HandleTypeDef *hrtc, uint32_t Instance);
HAL_StatusTypeDef HAL_RTCEx_MonotonicCounterGet(const RTC_HandleTypeDef *hrtc, uint32_t Instance, uint32_t *pValue);
HAL_StatusTypeDef HAL_RTCEx_SetSSRU_IT(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_DeactivateSSRU(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_SSRUIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_SSRUEventCallback(RTC_HandleTypeDef *hrtc);



 

 


 

void              HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForAlarmBEvent(const RTC_HandleTypeDef *hrtc, uint32_t Timeout);


 



 
HAL_StatusTypeDef HAL_RTCEx_SetTamper(const RTC_HandleTypeDef *hrtc, const RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_SetActiveTampers(RTC_HandleTypeDef *hrtc, const RTC_ActiveTampersTypeDef *sAllTamper);
HAL_StatusTypeDef HAL_RTCEx_SetActiveSeed(RTC_HandleTypeDef *hrtc, const uint32_t *pSeed);
HAL_StatusTypeDef HAL_RTCEx_SetTamper_IT(const RTC_HandleTypeDef *hrtc, const RTC_TamperTypeDef *sTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateTamper(const RTC_HandleTypeDef *hrtc, uint32_t Tamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateActiveTampers(const RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTCEx_PollForTamperEvent(const RTC_HandleTypeDef *hrtc, uint32_t Tamper, uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper(const RTC_HandleTypeDef *hrtc,
                                              const RTC_InternalTamperTypeDef *sIntTamper);
HAL_StatusTypeDef HAL_RTCEx_SetInternalTamper_IT(const RTC_HandleTypeDef *hrtc,
                                                 const RTC_InternalTamperTypeDef *sIntTamper);
HAL_StatusTypeDef HAL_RTCEx_DeactivateInternalTamper(const RTC_HandleTypeDef *hrtc, uint32_t IntTamper);
HAL_StatusTypeDef HAL_RTCEx_PollForInternalTamperEvent(const RTC_HandleTypeDef *hrtc, uint32_t IntTamper,
                                                       uint32_t Timeout);
HAL_StatusTypeDef HAL_RTCEx_LockBootHardwareKey(const RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_TamperIRQHandler(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper1EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper2EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper3EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper4EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper5EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper6EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper7EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_Tamper8EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper1EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper2EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper3EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper4EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper5EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper6EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper7EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper8EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper9EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper11EventCallback(RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_InternalTamper15EventCallback(RTC_HandleTypeDef *hrtc);


 



 
void              HAL_RTCEx_BKUPWrite(const RTC_HandleTypeDef *hrtc, uint32_t BackupRegister, uint32_t Data);
uint32_t          HAL_RTCEx_BKUPRead(const RTC_HandleTypeDef *hrtc, uint32_t BackupRegister);
void              HAL_RTCEx_BKUPErase(const RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_BKUPBlock(const RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_BKUPUnblock(const RTC_HandleTypeDef *hrtc);
void              HAL_RTCEx_ConfigEraseDeviceSecrets(const RTC_HandleTypeDef *hrtc, uint32_t DeviceSecretConf);


 



 
HAL_StatusTypeDef HAL_RTCEx_PrivilegeModeSet(const RTC_HandleTypeDef *hrtc,
                                             const RTC_PrivilegeStateTypeDef *privilegeState);
HAL_StatusTypeDef HAL_RTCEx_PrivilegeModeGet(const RTC_HandleTypeDef *hrtc, RTC_PrivilegeStateTypeDef *privilegeState);


 



 

 
 
 
 


 



 







































 



 



 



 



 



 




 


 



 
 
HAL_StatusTypeDef HAL_RTC_Init(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_DeInit(RTC_HandleTypeDef *hrtc);

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc);
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc);

 



 



 
 
HAL_StatusTypeDef HAL_RTC_SetTime(RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetTime(const RTC_HandleTypeDef *hrtc, RTC_TimeTypeDef *sTime, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetDate(RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_GetDate(const RTC_HandleTypeDef *hrtc, RTC_DateTypeDef *sDate, uint32_t Format);
void              HAL_RTC_DST_Add1Hour(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_Sub1Hour(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_SetStoreOperation(const RTC_HandleTypeDef *hrtc);
void              HAL_RTC_DST_ClearStoreOperation(const RTC_HandleTypeDef *hrtc);
uint32_t          HAL_RTC_DST_ReadStoreOperation(const RTC_HandleTypeDef *hrtc);


 



 
 
HAL_StatusTypeDef HAL_RTC_SetAlarm(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_SetAlarm_IT(RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Format);
HAL_StatusTypeDef HAL_RTC_DeactivateAlarm(RTC_HandleTypeDef *hrtc, uint32_t Alarm);
HAL_StatusTypeDef HAL_RTC_GetAlarm(const RTC_HandleTypeDef *hrtc, RTC_AlarmTypeDef *sAlarm, uint32_t Alarm,
                                   uint32_t Format);
void              HAL_RTC_AlarmIRQHandler(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef HAL_RTC_PollForAlarmAEvent(const RTC_HandleTypeDef *hrtc, uint32_t Timeout);
void              HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);



 



 
 
HAL_StatusTypeDef   HAL_RTC_WaitForSynchro(RTC_HandleTypeDef *hrtc);


 



 
 
HAL_RTCStateTypeDef HAL_RTC_GetState(const RTC_HandleTypeDef *hrtc);


 



 

 
 
 


 
 




 

 


 



 





























 



 

 


 
HAL_StatusTypeDef  RTC_EnterInitMode(RTC_HandleTypeDef *hrtc);
HAL_StatusTypeDef  RTC_ExitInitMode(RTC_HandleTypeDef *hrtc);
uint8_t            RTC_ByteToBcd2(uint8_t Value);
uint8_t            RTC_Bcd2ToByte(uint8_t Value);


 



 



 





























 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t BaudRate;                















 

  uint32_t WordLength;              
 

  uint32_t StopBits;                
 

  uint32_t Parity;                  




 

  uint32_t Mode;                    
 

  uint32_t HwFlowCtl;               

 

  uint32_t OverSampling;            

 

  uint32_t OneBitSampling;          

 

  uint32_t ClockPrescaler;          
 

} UART_InitTypeDef;



 
typedef struct
{
  uint32_t AdvFeatureInit;        


 

  uint32_t TxPinLevelInvert;      
 

  uint32_t RxPinLevelInvert;      
 

  uint32_t DataInvert;            

 

  uint32_t Swap;                  
 

  uint32_t OverrunDisable;        
 

  uint32_t DMADisableonRxError;   
 

  uint32_t AutoBaudRateEnable;    
 

  uint32_t AutoBaudRateMode;      

 

  uint32_t MSBFirst;              
 
} UART_AdvFeatureInitTypeDef;








































 
typedef uint32_t HAL_UART_StateTypeDef;



 
typedef enum
{
  UART_CLOCKSOURCE_PCLK1      = 0x00U,     
  UART_CLOCKSOURCE_PCLK2      = 0x01U,     
  UART_CLOCKSOURCE_PCLK4      = 0x02U,     
  UART_CLOCKSOURCE_PLL2Q      = 0x04U,     
  UART_CLOCKSOURCE_PLL3Q      = 0x08U,     
  UART_CLOCKSOURCE_HSI        = 0x10U,     
  UART_CLOCKSOURCE_CSI        = 0x20U,     
  UART_CLOCKSOURCE_LSE        = 0x40U,     
  UART_CLOCKSOURCE_UNDEFINED  = 0x80U      
} UART_ClockSourceTypeDef;









 
typedef uint32_t HAL_UART_RxTypeTypeDef;









 
typedef uint32_t HAL_UART_RxEventTypeTypeDef;



 
typedef struct __UART_HandleTypeDef
{
  USART_TypeDef            *Instance;                 

  UART_InitTypeDef         Init;                      

  UART_AdvFeatureInitTypeDef AdvancedInit;            

  const uint8_t            *pTxBuffPtr;               

  uint16_t                 TxXferSize;                

  volatile uint16_t            TxXferCount;               

  uint8_t                  *pRxBuffPtr;               

  uint16_t                 RxXferSize;                

  volatile uint16_t            RxXferCount;               

  uint16_t                 Mask;                      

  uint32_t                 FifoMode;                 
 

  uint16_t                 NbRxDataToProcess;         

  uint16_t                 NbTxDataToProcess;         

  volatile HAL_UART_RxTypeTypeDef ReceptionType;          

  volatile HAL_UART_RxEventTypeTypeDef RxEventType;       

  void (*RxISR)(struct __UART_HandleTypeDef *huart);  

  void (*TxISR)(struct __UART_HandleTypeDef *huart);  

  DMA_HandleTypeDef        *hdmatx;                   

  DMA_HandleTypeDef        *hdmarx;                   

  HAL_LockTypeDef           Lock;                     

  volatile HAL_UART_StateTypeDef    gState;              

 

  volatile HAL_UART_StateTypeDef    RxState;             
 

  volatile uint32_t                 ErrorCode;            


} UART_HandleTypeDef;




 

 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 





 


 


















 




 



 


 



 


 



 


 



 

 


 




 




 


















 




 




 




 




 




 




 
































 























 























 























 























 



















 











 




 




 




 




 













 













 













 













 


 

 


 



 






 






 






 




 






 




 




 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 





 



 

 
















 

 


 



 



 

 


 



 
typedef struct
{
  uint32_t WakeUpEvent;        


 

  uint16_t AddressLength;      
 

  uint8_t Address;              
} UART_WakeUpTypeDef;



 

 


 



 


 



 


 




 


 




 


 




 


 



 

 
 


 



 

 
HAL_StatusTypeDef HAL_RS485Ex_Init(UART_HandleTypeDef *huart, uint32_t Polarity, uint32_t AssertionTime,
                                   uint32_t DeassertionTime);



 



 

void HAL_UARTEx_WakeupCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart);
void HAL_UARTEx_TxFifoEmptyCallback(UART_HandleTypeDef *huart);



 



 

 
HAL_StatusTypeDef HAL_UARTEx_StopModeWakeUpSourceConfig(UART_HandleTypeDef *huart, UART_WakeUpTypeDef WakeUpSelection);
HAL_StatusTypeDef HAL_UARTEx_EnableStopMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_DisableStopMode(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_MultiProcessorEx_AddressLength_Set(UART_HandleTypeDef *huart, uint32_t AddressLength);

HAL_StatusTypeDef HAL_UARTEx_EnableFifoMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_DisableFifoMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UARTEx_SetTxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold);
HAL_StatusTypeDef HAL_UARTEx_SetRxFifoThreshold(UART_HandleTypeDef *huart, uint32_t Threshold);

HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint16_t *RxLen,
                                           uint32_t Timeout);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UARTEx_ReceiveToIdle_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

HAL_UART_RxEventTypeTypeDef HAL_UARTEx_GetRxEventType(const UART_HandleTypeDef *huart);




 



 

 


 





 










 





 





 





 





 



 

 



 



 




 


 



 

 
HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_LIN_Init(UART_HandleTypeDef *huart, uint32_t BreakDetectLength);
HAL_StatusTypeDef HAL_MultiProcessor_Init(UART_HandleTypeDef *huart, uint8_t Address, uint32_t WakeUpMethod);
HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

 



 



 

 
HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Transmit_DMA(UART_HandleTypeDef *huart, const uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_UART_DMAPause(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAResume(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DMAStop(UART_HandleTypeDef *huart);
 
HAL_StatusTypeDef HAL_UART_Abort(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_Abort_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortTransmit_IT(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_AbortReceive_IT(UART_HandleTypeDef *huart);

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef *huart);
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart);

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);



 



 

 
void HAL_UART_ReceiverTimeout_Config(UART_HandleTypeDef *huart, uint32_t TimeoutValue);
HAL_StatusTypeDef HAL_UART_EnableReceiverTimeout(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_UART_DisableReceiverTimeout(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_LIN_SendBreak(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_EnableMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_MultiProcessor_DisableMuteMode(UART_HandleTypeDef *huart);
void HAL_MultiProcessor_EnterMuteMode(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableTransmitter(UART_HandleTypeDef *huart);
HAL_StatusTypeDef HAL_HalfDuplex_EnableReceiver(UART_HandleTypeDef *huart);



 



 

 
HAL_UART_StateTypeDef HAL_UART_GetState(const UART_HandleTypeDef *huart);
uint32_t              HAL_UART_GetError(const UART_HandleTypeDef *huart);



 



 

 


 
HAL_StatusTypeDef UART_SetConfig(UART_HandleTypeDef *huart);
HAL_StatusTypeDef UART_CheckIdleState(UART_HandleTypeDef *huart);
HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, FlagStatus Status,
                                              uint32_t Tickstart, uint32_t Timeout);
void              UART_AdvFeatureConfig(UART_HandleTypeDef *huart);
HAL_StatusTypeDef UART_Start_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef UART_Start_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);



 

 


 

 
extern const uint16_t UARTPrescTable[12];


 



 



 






















 

 


 




 



 

 


 



 
typedef struct
{
  uint32_t FifoThresholdByte;         


 
  uint32_t MemoryMode;                
 
  uint32_t MemoryType;                
 
  uint32_t MemorySize;                


 
  uint32_t ChipSelectHighTimeCycle;   

 
  uint32_t FreeRunningClock;          
 
  uint32_t ClockMode;                 
 
  uint32_t WrapSize;                  
 
  uint32_t ClockPrescaler;            

 
  uint32_t SampleShifting;            

 
  uint32_t DelayHoldQuarterCycle;     
 
  uint32_t ChipSelectBoundary;        

 
  uint32_t MaxTran;                   


 
  uint32_t Refresh;                   

 
  uint32_t MemorySelect;              
 
} XSPI_InitTypeDef;



 
typedef struct
{
  XSPI_TypeDef               *Instance;      
  XSPI_InitTypeDef           Init;           
  uint8_t                    *pBuffPtr;      
  volatile uint32_t              XferSize;       
  volatile uint32_t              XferCount;      
  DMA_HandleTypeDef          *hdmatx;        
  DMA_HandleTypeDef          *hdmarx;        
  volatile uint32_t              State;          
  volatile uint32_t              ErrorCode;      
  uint32_t                   Timeout;        
} XSPI_HandleTypeDef;



 
typedef struct
{
  uint32_t OperationType;             


 
  uint32_t IOSelect;                  
 
  uint32_t Instruction;               
 
  uint32_t InstructionMode;           
 
  uint32_t InstructionWidth;          
 
  uint32_t InstructionDTRMode;        
 
  uint32_t Address;                   
 
  uint32_t AddressMode;               

 
  uint32_t AddressWidth;              
 
  uint32_t AddressDTRMode;            
 
  uint32_t AlternateBytes;            
 
  uint32_t AlternateBytesMode;        
 
  uint32_t AlternateBytesWidth;       
 
  uint32_t AlternateBytesDTRMode;     
 
  uint32_t DataMode;                  

 
  uint32_t DataLength;                

 
  uint32_t DataDTRMode;               
 
  uint32_t DummyCycles;               
 
  uint32_t DQSMode;                   
 
} XSPI_RegularCmdTypeDef;


 
typedef struct
{
  uint32_t RWRecoveryTimeCycle;       
 
  uint32_t AccessTimeCycle;           
 
  uint32_t WriteZeroLatency;          
 
  uint32_t LatencyMode;               
 
} XSPI_HyperbusCfgTypeDef;



 
typedef struct
{
  uint32_t AddressSpace;              
 
  uint32_t Address;                   
 
  uint32_t AddressWidth;              
 
  uint32_t DataLength;                



 
  uint32_t DQSMode;                   
 
  uint32_t DataMode;                  

 
} XSPI_HyperbusCmdTypeDef;



 
typedef struct
{
  uint32_t MatchValue;                

 
  uint32_t MatchMask;                 
 
  uint32_t MatchMode;                 
 
  uint32_t AutomaticStop;             
 
  uint32_t IntervalTime;              

 
} XSPI_AutoPollingTypeDef;



 
typedef struct
{
  uint32_t TimeOutActivation;         
 
  uint32_t TimeoutPeriodClock;        

 
} XSPI_MemoryMappedTypeDef;



 
typedef struct
{
  uint32_t nCSOverride;               
 
  uint32_t IOPort;                    
 
  uint32_t Req2AckTime;               


 
} XSPIM_CfgTypeDef;



 
typedef struct
{
  uint32_t DelayValueType;            
 
  uint32_t FineCalibrationUnit;       
 
  uint32_t CoarseCalibrationUnit;     
 
  uint32_t MaxCalibration;            

 
} XSPI_HSCalTypeDef;



 

 


 



 


 



 


 



 



 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 




 


 



 


 



 


 



 

 


 



 




 




 











 











 











 













 










 



 

 


 

 


 
HAL_StatusTypeDef     HAL_XSPI_Init(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_MspInit(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_DeInit(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_MspDeInit(XSPI_HandleTypeDef *hxspi);



 

 


 
 
void                  HAL_XSPI_IRQHandler(XSPI_HandleTypeDef *hxspi);

 
HAL_StatusTypeDef     HAL_XSPI_Command(XSPI_HandleTypeDef *hxspi, XSPI_RegularCmdTypeDef *const pCmd,
                                       uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Command_IT(XSPI_HandleTypeDef *hxspi, XSPI_RegularCmdTypeDef *const pCmd);
HAL_StatusTypeDef     HAL_XSPI_HyperbusCfg(XSPI_HandleTypeDef *hxspi, XSPI_HyperbusCfgTypeDef *const pCfg,
                                           uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_HyperbusCmd(XSPI_HandleTypeDef *hxspi, XSPI_HyperbusCmdTypeDef *const pCmd,
                                           uint32_t Timeout);

 
HAL_StatusTypeDef     HAL_XSPI_Transmit(XSPI_HandleTypeDef *hxspi, uint8_t *const pData, uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Receive(XSPI_HandleTypeDef *hxspi, uint8_t *const pData, uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_Transmit_IT(XSPI_HandleTypeDef *hxspi, uint8_t *const pData);
HAL_StatusTypeDef     HAL_XSPI_Receive_IT(XSPI_HandleTypeDef *hxspi, uint8_t *const pData);
HAL_StatusTypeDef     HAL_XSPI_Transmit_DMA(XSPI_HandleTypeDef *hxspi, uint8_t *const pData);
HAL_StatusTypeDef     HAL_XSPI_Receive_DMA(XSPI_HandleTypeDef *hxspi, uint8_t *const pData);

 
HAL_StatusTypeDef     HAL_XSPI_AutoPolling(XSPI_HandleTypeDef *hxspi, XSPI_AutoPollingTypeDef *const pCfg,
                                           uint32_t Timeout);
HAL_StatusTypeDef     HAL_XSPI_AutoPolling_IT(XSPI_HandleTypeDef *hxspi, XSPI_AutoPollingTypeDef *const pCfg);

 
HAL_StatusTypeDef     HAL_XSPI_MemoryMapped(XSPI_HandleTypeDef *hxspi,  XSPI_MemoryMappedTypeDef *const pCfg);

 
void                  HAL_XSPI_ErrorCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_AbortCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_FifoThresholdCallback(XSPI_HandleTypeDef *hxspi);

 
void                  HAL_XSPI_CmdCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_RxCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_TxCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_RxHalfCpltCallback(XSPI_HandleTypeDef *hxspi);
void                  HAL_XSPI_TxHalfCpltCallback(XSPI_HandleTypeDef *hxspi);

 
void                  HAL_XSPI_StatusMatchCallback(XSPI_HandleTypeDef *hxspi);

 
void                  HAL_XSPI_TimeOutCallback(XSPI_HandleTypeDef *hxspi);




 

 


 
HAL_StatusTypeDef     HAL_XSPI_Abort(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_Abort_IT(XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_SetFifoThreshold(XSPI_HandleTypeDef *hxspi, uint32_t Threshold);
uint32_t              HAL_XSPI_GetFifoThreshold(const XSPI_HandleTypeDef *hxspi);
HAL_StatusTypeDef     HAL_XSPI_SetMemoryType(XSPI_HandleTypeDef *hxspi, uint32_t Type);
HAL_StatusTypeDef     HAL_XSPI_SetDeviceSize(XSPI_HandleTypeDef *hxspi, uint32_t Size);
HAL_StatusTypeDef     HAL_XSPI_SetClockPrescaler(XSPI_HandleTypeDef *hxspi, uint32_t Prescaler);
HAL_StatusTypeDef     HAL_XSPI_SetTimeout(XSPI_HandleTypeDef *hxspi, uint32_t Timeout);
uint32_t              HAL_XSPI_GetError(const XSPI_HandleTypeDef *hxspi);
uint32_t              HAL_XSPI_GetState(const XSPI_HandleTypeDef *hxspi);



 

 


 
HAL_StatusTypeDef     HAL_XSPIM_Config(XSPI_HandleTypeDef *const hxspi, XSPIM_CfgTypeDef *const pCfg, uint32_t Timeout);



 

 


 
HAL_StatusTypeDef     HAL_XSPI_GetDelayValue(XSPI_HandleTypeDef *hxspi, XSPI_HSCalTypeDef *const pCfg);
HAL_StatusTypeDef     HAL_XSPI_SetDelayValue(XSPI_HandleTypeDef *hxspi, XSPI_HSCalTypeDef *const pCfg);



 



 
 

 


 

















































 

 



 



 




 





 



 

 


 


 
typedef enum
{
  HAL_TICK_FREQ_10HZ         = 100U,
  HAL_TICK_FREQ_100HZ        = 10U,
  HAL_TICK_FREQ_1KHZ         = 1U,
  HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;


 

 



 


 


 



 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 


 



 



 


 



 


 



 


 



 


 



 


 



 


 


 

 



 


 




























































 



 



 




 



 


 


 






 



 





 



 








 



 









 

 



 
extern volatile uint32_t        uwTick;
extern uint32_t             uwTickPrio;
extern HAL_TickFreqTypeDef  uwTickFreq;


 

 



 



 

 
HAL_StatusTypeDef    HAL_Init(void);
HAL_StatusTypeDef    HAL_DeInit(void);
void                 HAL_MspInit(void);
void                 HAL_MspDeInit(void);
HAL_StatusTypeDef    HAL_InitTick(uint32_t TickPriority);



 



 

 
void                 HAL_IncTick(void);
void                 HAL_Delay(uint32_t Delay);
uint32_t             HAL_GetTick(void);
uint32_t             HAL_GetTickPrio(void);
HAL_StatusTypeDef    HAL_SetTickFreq(HAL_TickFreqTypeDef Freq);
HAL_TickFreqTypeDef  HAL_GetTickFreq(void);
void                 HAL_SuspendTick(void);
void                 HAL_ResumeTick(void);
uint32_t             HAL_GetHalVersion(void);
uint32_t             HAL_GetREVID(void);
uint32_t             HAL_GetDEVID(void);
uint32_t             HAL_GetUIDw0(void);
uint32_t             HAL_GetUIDw1(void);
uint32_t             HAL_GetUIDw2(void);



 



 

 
void                 HAL_DBGMCU_EnableDBGSleepMode(void);
void                 HAL_DBGMCU_DisableDBGSleepMode(void);
void                 HAL_DBGMCU_EnableDBGStopMode(void);
void                 HAL_DBGMCU_DisableDBGStopMode(void);
void                 HAL_DBGMCU_EnableDBGStandbyMode(void);
void                 HAL_DBGMCU_DisableDBGStandbyMode(void);



 



 

 
uint32_t             HAL_SBS_GetBootAddress(void);

void                 HAL_SBS_IncrementHDPLValue(void);
uint32_t             HAL_SBS_GetHDPLValue(void);

void                 HAL_SBS_OpenAccessPort(void);
void                 HAL_SBS_OpenDebug(void);
HAL_StatusTypeDef    HAL_SBS_ConfigDebugLevel(uint32_t Level);
uint32_t             HAL_SBS_GetDebugLevel(void);
void                 HAL_SBS_UnlockDebugConfig(void);
void                 HAL_SBS_LockDebugConfig(void);

void                 HAL_SBS_ConfigRSSCommand(uint32_t Cmd);
uint32_t             HAL_SBS_GetRSSCommand(void);

void                 HAL_SBS_EnableIOAnalogBooster(void);
void                 HAL_SBS_DisableIOAnalogBooster(void);
void                 HAL_SBS_EnableIOAnalogSwitchVdd(void);
void                 HAL_SBS_DisableIOAnalogSwitchVdd(void);
void                 HAL_SBS_ConfigEthernetPHY(uint32_t Config);
void                 HAL_SBS_ConfigAXISRAMWaitState(uint32_t Config);

void                 HAL_SBS_EnableCompensationCell(uint32_t Selection);
void                 HAL_SBS_DisableCompensationCell(uint32_t Selection);
uint32_t             HAL_SBS_GetCompensationCellReadyStatus(uint32_t Selection);
void                 HAL_SBS_ConfigCompensationCell(uint32_t Selection, uint32_t Code, uint32_t NmosValue,
                                                    uint32_t PmosValue);
HAL_StatusTypeDef    HAL_SBS_GetCompensationCell(uint32_t Selection, uint32_t *pCode, uint32_t *pNmosValue,
                                                 uint32_t *pPmosValue);

void                 HAL_SBS_EnableIOSpeedOptimize(uint32_t Selection);
void                 HAL_SBS_DisableIOSpeedOptimize(uint32_t Selection);

void                 HAL_SBS_ConfigTimerBreakInput(uint32_t Input);
uint32_t             HAL_SBS_GetTimerBreakInputConfig(void);

void                 HAL_SBS_EXTIConfig(uint32_t Exti, uint32_t Port);
uint32_t             HAL_SBS_GetEXTIConfig(uint32_t Exti);


 



 

 
void                 HAL_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling);
void                 HAL_VREFBUF_HighImpedanceConfig(uint32_t Mode);
void                 HAL_VREFBUF_TrimmingConfig(uint32_t TrimmingValue);
HAL_StatusTypeDef    HAL_VREFBUF_Enable(void);
void                 HAL_VREFBUF_Disable(void);




 



 

 
void                 HAL_AXIM_ASIB_EnablePacking(AXIM_ASIB_TypeDef *AsibInstance);
void                 HAL_AXIM_ASIB_DisablePacking(AXIM_ASIB_TypeDef *AsibInstance);
void                 HAL_AXIM_ASIB_IssuingConfig(AXIM_ASIB_TypeDef *AsibInstance, uint32_t ReadIssuing,
                                                 uint32_t WriteIssuing);
void                 HAL_AXIM_ASIB_ReadQoSConfig(AXIM_ASIB_TypeDef *AsibInstance, uint32_t QosPriority);
void                 HAL_AXIM_ASIB_WriteQoSConfig(AXIM_ASIB_TypeDef *AsibInstance, uint32_t QosPriority);


void                 HAL_AXIM_AMIB_EnablePacking(AXIM_AMIB_TypeDef *AmibInstance);
void                 HAL_AXIM_AMIB_DisablePacking(AXIM_AMIB_TypeDef *AmibInstance);
void                 HAL_AXIM_AMIB_IssuingConfig(AXIM_AMIB_TypeDef *AmibInstance, uint32_t ReadIssuing,
                                                 uint32_t WriteIssuing);
void                 HAL_AXIM_AMIB_IssuingConfigBusMatrix(AXIM_AMIB_TypeDef *AmibInstance, uint32_t ReadIssuing,
                                                          uint32_t WriteIssuing);
void                 HAL_AXIM_AMIB_EnableLongBurst(AXIM_AMIB_TypeDef *AmibInstance);
void                 HAL_AXIM_AMIB_DisableLongBurst(AXIM_AMIB_TypeDef *AmibInstance);


 



 




 



 



 





 




 






 

enum image_attributes
{
 
  RE_IMAGE_FLASH_SIZE = ((0x00020000)),
 
  RE_IMAGE_FLASH_ADDRESS = ((0x90000000)+((0x0000) + ((0x00020000)))),

  RE_PARTITION_START = ((0x24040000UL)),
  RE_AREA_0_OFFSET = ((0x00000000)),
  RE_AREA_0_SIZE = (((0x00020000))),

  RE_BL2_BOOT_ADDRESS = ((((((0x08000000)) + ((0x00000)))))),
   
  RE_BL2_WRP_OFFSET = ((0x00000)),

  RE_BL2_HDP_START = 0x0,
  RE_BL2_HDP_END = ((0x00000)+(0x10000)-0x1),
  RE_BL2_WRP_SIZE = ((0x10000)),
   

  RE_OVER_WRITE = (0x0),
  RE_TRAILER_MAX_SIZE = (0x2000),
  RE_OEMUROT_ENABLE = (0x0),
  RE_FLASH_PAGE_NBR = (0x7),
  RE_HDP_BLOCK_NBR = (0xFF),
};
