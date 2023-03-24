## <b>GTZC_TZSC_MPCBB_TrustZone Example Description</b>

How to use HAL GTZC MPCBB to build any example with SecureFault detection when TrustZone
security is activated (Option bit TZEN=1).

The purpose of this example is to declare the 2nd half of SRAM1 (from 0x20008000) as non-secure
with HAL GTZC MPCBB services and to demonstrate that a security violation is detected when
non-secure application accesses secure SRAM1 memory.

The non-secure application toggles LD3 every second until the security violation is detected.

The security violation detection at SAU level is triggered by two means:
- automatically after 5 seconds
- before through the USER push-button key press
Any key press on USER push-button initiates an access in secure SRAM1 (SRAM1 base address).
The secure fault is detected from the secure application SecureFault_Handler which calls a
non-secure registered callback function to inform the non-secure application. The non-secure
application switches on the error LD2.

This project is composed of two sub-projects:
- one for the secure application part (xxxxx_S)
- one for the non-secure application part (xxxxx_NS).

Please remember that on system with security enabled:
- the system always boots in secure and the secure application is responsible for
launching the non-secure application.
- the SystemInit() function in secure application sets up the SAU/IDAU, FPU and
secure/non-secure interrupts allocations defined in partition_stm32wba52xx.h file.

This project shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:
 - the first half for the secure application and
 - the second half for the non-secure application.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2.  The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

Security, TrustZone, GTZC, MPCBB, Memory protection, Block-Based, Memory, internal SRAM, illegal access

### <b>Directory contents</b>

  - Secure/Src/main.c                     Secure Main program
  - Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - Secure/Src/stm32wbaxx_hal_msp.c       Secure HAL MSP module
  - Secure/Src/stm32wbaxx_it.c            Secure Interrupt handlers
  - Secure/Src/system_stm32wbaxx_s.c      Secure STM32WBAxx system clock configuration file
  - Secure/Inc/main.h                     Secure Main program header file
  - Secure/Inc/partition_stm32wba52xx.h   STM32WBA Device System Configuration file
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

  - This example runs on STM32WBA52CGUx devices with security enabled (TZEN=1).
    
  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.      

  - User Option Bytes requirement (with STM32CubeProgrammer tool)
     TZEN = 1                            System with TrustZone-M enabled
     SECWM1_PSTRT=0x0  SECWM1_PEND=0x3F  64 of 128 pages of internal Flash set as secure

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the example
