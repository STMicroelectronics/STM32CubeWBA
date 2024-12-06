## <b>HASH_SHA1_DMA_TrustZone example with TZEN=1 Description</b>

How to use a secure HASH SHA-1 computation service based on secure DMA channel
when TrustZone security is activated (Option bit TZEN=1).  

The purpose of this example is to declare in secure example part a HASH SHA-1
computation service and have it available for non-secure example part through a
a Non-Secure Callable function. This service allows to demonstrate the usage of
a secure DMA channel for the secure HASH peripheral with input and output data
from non-secure embedded SRAM1.  

The non-secure example part waits for 1 second and then calls the secure HASH
SHA-1 service and checks that the SHA-1 digest output is as expected and it turns
on the LED1 in case of success else the LED3 is switched on.  

This project is composed of two sub-projects:

 - one for the secure application part (Project_s)
 - one for the non-secure application part (Project_ns).  
  
This project mainly shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:

  - the first half for the secure application and  
  - the second half for the non-secure application.    
  
User Option Bytes configuration:  
Please note the internal Flash is fully secure by default in TZEN=1 and User Option Bytes
SECWM_PSTRT/SECWM_PEND should be set according to the application configuration.  
 
Here the proper User Option Bytes setup in line with the project linker/scatter file is as follows:
                                                               
      TZEN=1  
      SECWM_PSTRT=0x0        |      SECWM_PEND=0x3F        :     meaning only first 64 pages of Flash set as secure    
  
Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault as demonstrated in non secure application by uncommenting some
code instructions in Secure/main.c (LED) and NonSecure/main.c (illegal access).  
 
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

Security, TrustZone, HASH, SHA1, digest, DMA, GTZC, MPCWM, Non-Secure Callable

### <b>Directory contents</b>

  - HASH/HASH_SHA1_DMA_TrustZone/Inc/stm32wbaxx_nucleo_conf.h    BSP configuration file
  - HASH/HASH_SHA1_DMA_TrustZone/Src/main.c                      Main program
  - HASH/HASH_SHA1_DMA_TrustZone/Src/system_stm32wbaxx.c         STM32WBAxx system clock configuration file
  - HASH/HASH_SHA1_DMA_TrustZone/Src/stm32wbaxx_it.c             Interrupt handlers
  - HASH/HASH_SHA1_DMA_TrustZone/Src/stm32wbaxx_hal_msp.c        HAL MSP module
  - HASH/HASH_SHA1_DMA_TrustZone/Inc/main.h                      Main program header file
  - HASH/HASH_SHA1_DMA_TrustZone/Inc/stm32wbaxx_hal_conf.h       HAL Configuration file
  - HASH/HASH_SHA1_DMA_TrustZone/Inc/stm32wbaxx_it.h             Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices with security enabled (TZEN=1).

  - This example has been tested with STMicroelectronics NUCLEO-WBA55CG
    board and can be easily tailored to any other supported device
    and development board.

	
  - User Option Bytes requirement (with STM32CubeProgrammer tool)
    - TZEN = 1                            System with TrustZone-M enabled
    - SECWM_PSTRT=0x0   SECWM_PEND=0x3F   Only first 64 pages of Flash set as secure

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=1) (option byte)

### IAR  

 - Open your toolchain  
 - Open Multi-projects workspace file Project.eww  
 - Rebuild xxxxx_S project  
 - Rebuild xxxxx_NS project  
 - Set the "xxxxx_NS" as active application (Set as Active)  
 - Flash the non-secure binary with Project->Download->Download active application  
   (this shall download the \EWARM\xxxxx_NS\Exe\xxxxx_NS.out to flash memory)  
 - Set the "xxxxx_S" as active application (Set as Active)  
 - Flash the secure binary with Project->Download->Download active application  
   (this shall download the \Secure_nsclib\xxxxx_S.out to flash memory)  
 - Run the example  


### MDK-ARM 

 - Open your toolchain
 - Open Multi-projects workspace file Project.uvmpw
 - Select the xxxxx_S project as Active Project (Set as Active Project)
 - Build xxxxx_S project
 - Select the xxxxx_NS project as Active Project (Set as Active Project)
 - Build xxxxx_NS project
 - Load the non-secure binary (F8)
   (this shall download the \MDK-ARM\xxxxx_ns\Exe\Project_ns.axf to flash memory)
 - Select the xxxxx_S project as Active Project (Set as Active Project)
 - Load the secure binary (F8)
   (this shall download the \MDK-ARM\xxxxx_s\Exe\Project_s.axf to flash memory)
 - Run the example


### STM32CubeIDE  

 - Open STM32CubeIDE  
 - File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.  
 - Build configuration: Set the same active build configuration: Debug (default) or Release for both projects xxxxx_S & xxxxx_NS  
 - Select and build the xxxxx_NS project, this will automatically trigger the build of xxxxx_S project  
 - Select the xxxxx_S project and select "Debug configuration" or "Run configuration" in function of the active build configuration  
   - Double click on "STM32 Cortex-M C/C++ Application"  
   - Select  "Startup" >  "Add" >  
     - Select the xxxxx_NS project  
 - Click Debug/Run to debug/run the example  


