## <b>FLASH_EraseProgram_TrustZone Example Description</b>

How to configure and use the FLASH HAL API to erase and program the internal
Flash memory when TrustZone security is activated (Option bit TZEN=1).

This project is composed of two sub-projects:
- one for the secure application part (FLASH_EraseProgram_TrustZone_S)
- one for the non-secure application part (FLASH_EraseProgram_TrustZone_NS).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

The system isolation splits the internal Flash and internal SRAM memories into two halves:
 - the first half of Flash is used for the secure application code,
 - the second half of Flash is used for the non-secure application code,
 - the first half of SRAM is used for the secure application data,
 - the second half of SRAM is used for the non-secure application data.

Secure Application :
After Reset, the Flash memory Program/Erase Controller is locked. HAL_FLASH_Unlock() function
is used to enable the FLASH control register access.
Before programming the desired secured addresses, an erase operation is performed using
the flash erase page feature. The erase procedure is done by filling the erase init
structure giving the starting erase page and the number of pagess to erase.
At this stage, all these pages will be erased one by one separately.

Once this operation is finished, page double-word programming operation will be performed
in the secure part of Flash memory. The written data is then read back and checked.

Then the non-secure application is launched.

Non-Secure Application :
Same erase procedure is done on non-secured addresses. 

Once this operation is finished, page double-word programming operation will be performed
in the non-secure part of the Flash memory. The written data is then read back and checked.

The STM32WBAxx_Nucleo (MB1801) board LEDs can be used to monitor the transfer status:
 - LD2 is ON when there are no errors detected after programming in secure area
 - LD2 is blinking with a 1s period when there are errors detected after programming in secure area
 - LD2 is blinking (100ms on, 2s off) when there is an issue during erase or program procedure in secure area
 - LD3 is ON when there are no errors detected after programming in non-secure area
 - LD3 is blinking with a 1s period when there are errors detected after programming in non-secure area
 - LD3 is blinking (100ms on, 2s off) when there is an issue during erase or program procedure in non-secure area

#### <b>Notes</b>

1. If problem occurs on a page, erase will be stopped and faulty page will
      be returned to user (through variable 'PageError').

2. Although code is the same for page erase between secure and non secure project, it matters 
      to mention that secure firmware is only able to erase secure pages within User Flash 
      and non-secure firmware is only able to erase non-secure pages within User Flash.

3. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
4.  The secure and non-secure applications need to ensure that the SysTick time base is always
      set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Security, TrustZone, Memory, Flash, Erase, Program

### <b>Directory contents</b>

  - Secure/Src/main.c                     Secure Main program
  - Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - Secure/Src/stm32wbaxx_hal_msp.c       Secure HAL MSP module
  - Secure/Src/stm32wbaxx_it.c            Secure Interrupt handlers
  - Secure/Src/system_stm32wbaxx_s.c      Secure STM32WBAxx system clock configuration file
  - Secure/Inc/main.h                     Secure Main program header file
  - Secure/Inc/partition_stm32wbaxx.h     STM32WBA Device System Configuration file
  - Secure/Inc/stm32wbaxx_hal_conf.h      Secure HAL Configuration file
  - Secure/Inc/stm32wbaxx_it.h            Secure Interrupt handlers header file
  - Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
  - NonSecure/Src/main.c                  Non-secure Main program
  - NonSecure/Src/stm32wbaxx_hal_msp.c    Non-secure HAL MSP module
  - NonSecure/Src/stm32wbaxx_it.c         Non-secure Interrupt handlers
  - NonSecure/Src/system_stm32wbaxx_ns.c  Non-secure STM32WBAxx system clock configuration file
  - NonSecure/Inc/main.h                  Non-secure Main program header file
  - NonSecure/Inc/stm32wbaxx_hal_conf.h   Non-secure HAL Configuration file
  - NonSecure/Inc/stm32wbaxx_it.h         Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices with security enabled (TZEN=1).
    
  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.      

  - User Option Bytes requirement (with STM32CubeProgrammer tool)
     TZEN = 1                            System with TrustZone-M enabled
     SECWM1_PSTRT=0x0  SECWM1_PEND=0x3F  64 of 128 pages of internal Flash set as secure

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the example
