## <b>HASH_SHA224SHA256_DMA example Description</b>

How to use the HASH peripheral to hash data with SHA224 and SHA256 algorithms.  

In this example, DMA is used to transfer data from internal RAM to HASH peripheral.  
The SHA224 message digest length is 28 bytes and the SHA256 message digest length is 32 bytes.    

STM32 board LED is used to monitor the example status:  
  - LDs is ON when correct digest values are calculated  
  - In case of digest computation or initialization issue, LD3 is turned on.   

The expected HASH digests (for SHA224 and SHA256) are already computed using an online HASH tool.  
Those values are compared to those computed by the HASH peripheral.  


In case there is a mismatch the red LED is turned ON.  
In case the SHA224 digest is computed correctly the blue LED LD1 is turned ON.  
In case the SHA256 digest is computed correctly the green LED LD2 is turned ON.  

#### <b>Notes</b>  
1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR.    
	  This implies that if HAL_Delay() is called from a peripheral ISR process, then the SysTick interrupt must have higher priority  
	  (numerically lower)than the peripheral interrupt.  
	  Otherwise the caller ISR process will be blocked.  
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.  

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.  

### <b>Keywords</b>

System, Security, HASH, SHA256, SHA224, digest, DMA

### <b>Directory contents</b>

  - HASH/HASH_SHA224SHA256_DMA/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - HASH/HASH_SHA224SHA256_DMA/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - HASH/HASH_SHA224SHA256_DMA/Inc/main.h		            Header for main.c module 
  - HASH/HASH_SHA224SHA256_DMA/Inc/stm32wbaxx_nucleo_conf.h Nucleo board configuration file  
  - HASH/HASH_SHA224SHA256_DMA/Src/stm32wbaxx_it.c          Interrupt handlers
  - HASH/HASH_SHA224SHA256_DMA/Src/main.c		            Main program
  - HASH/HASH_SHA224SHA256_DMA/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - HASH/HASH_SHA224SHA256_DMA/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with STMicroelectronics NUCLEO-WBA52CG
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :  
 - Open your preferred toolchain   
 - Rebuild all files and load your image into target memory  
 - Run the example