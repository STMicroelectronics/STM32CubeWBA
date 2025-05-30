## <b>HASH_SHA1SHA224_IT_Suspension example  Description</b>

How to suspend the HASH peripheral when data are fed in interrupt mode.

The example first computes the HASH digest of a low priority block using SHA-1 algorithm in entering input data in interrupt mode.  

The processing is suspended in order to process a higher priority block to compute its SHA-224 digest.  
For the higher priority block, data are entered in interrupt mode as well.  
Suspension request is triggered by an interrupts counter which is incremented each time a 512-bit long block has been fed to the IP.  
Once the counter has reached an arbitrary value, the low priority block processing is suspended.  

It is shown how to save then how to restore later on the low priority block context to resume its processing.  
For both the low and high priority blocks, the computed digests are compared with the expected ones.  

LD1 is turned on when correct digest values are calculated.  
In case of digest computation or initialization issue, LD3 is turned on.  


#### <b>Notes</b>
1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.  

### <b>Keywords</b>  

Security, HASH, SHA1, SHA256, suspend and resume operation.

### <b>Directory contents</b>

  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - HASH/HASH_SHA1SHA224_IT_Suspension/Inc/main.h                  Header for main.c module  
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/stm32wbaxx_it.c          Interrupt handlers
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/main.c                  Main program
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - HASH/HASH_SHA1SHA224_IT_Suspension/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.
     

### <b>How to use it ?</b>   

In order to make the program work, you must do the following :  
 - Open your preferred toolchain   
 - Rebuild all files and load your image into target memory  
 - Run the example  
 