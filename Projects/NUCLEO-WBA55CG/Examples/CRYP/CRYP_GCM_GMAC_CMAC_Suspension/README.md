## <b>CRYP_GCM_GMAC_CMAC_Suspension Example Description</b>

How to use the CRYP AES peripheral to suspend then resume the AES GCM and GMAC 
CMAC processing of a message in order to carry out the encryption, decryption 
or authentication tag computation of a higher-priority message(CMAC). 

***
 For GCM algorithm,  
  AES 128 GCM header phase of a low priority message (32-bit data type) is interrupted by the
  AES 256 CMAC processing of a high priority message (32-bit data type).  
  When the latter is done (encryption and tag generation), the low priority GCM header
  phase is resumed.  
 

 For GMAC algorithm,  
  AES 256 GMAC header phase of a low priority message (32-bit data type) is interrupted by the
  AES 256 CMAC processing of a high priority message (32-bit data type).  
  When the latter is done (encryption and tag generation), the low priority GMAC header
  phase is resumed followed by the GMAC final p

The example unrolls as follows.     
 - an AES GCM low-priority message processing is suspended due to the interruphase.  
tion by a high priority messages CMAC processing.   
    Low priority message GCM header then GCM payload phases are in turn suspended then resumed.  
 - Next, an AES GMAC low-priority message processing is suspended due to the interruption by a high priority messages CMAC processing.   
    Low-priority message GMAC header phase is suspended then resumed.        

Low and high-priority blocks AES outputs are compared with expected outputs. 
 
When all ciphering and deciphering operations are successful, LED1 is turned on.
In case of ciphering or deciphering issue, LED3 is turned on.  

 
This project is targeted to run on STM32WBA device on boards from STMicroelectronics.  

 
The reference template project configures the maximum system clock frequency at 100Mhz in non-secure  
application.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

System, Security, AES, GMAC enciphering, GCM enciphering,  CMAC enciphering, Suspension and resume. 

### <b>Directory contents</b>

  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Inc/stm32wbaxx_nucleo_conf.h  BSP configuration file
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Src/main.c                    Main program
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Src/system_stm32wbaxx.c       STM32WBAxx system clock configuration file
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Src/stm32wbaxx_it.c           Interrupt handlers
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Src/stm32wbaxx_hal_msp.c      HAL MSP module
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Inc/main.h                    Main program header file
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Inc/stm32wbaxx_hal_conf.h     HAL Configuration file
  - CRYP/CRYP_GCM_GMAC_CMAC_Suspension/Inc/stm32wbaxx_it.h           Interrupt handlers header file  

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBAxx devices.  

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG  
    board and can be easily tailored to any other supported device  
    and development board.  

### <b>How to use it ?</b> 

In order to make the program work, you must do the following:  
 - Open your preferred toolchain   
 - Rebuild all files and load your image into target memory  
 - Run the example  

