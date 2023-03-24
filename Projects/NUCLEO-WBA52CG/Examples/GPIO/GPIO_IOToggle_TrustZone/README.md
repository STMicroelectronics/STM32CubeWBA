
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>GPIO_IOToggle_TrustZone Example Description</b>

How to use HAL GPIO to toggle secure and unsecure IOs when TrustZone security
is activated (Option bit TZEN=1).

The purpose of this example is to declare a secure IO with HAL GPIO and to toggle it
every second on Secure SysTick timer interrupt basis. All other IOs are released to
non-secure application and a non-secure IO is initialized by the non-secure application
to toggle every two seconds on Non-secure Systick timer interrupt basis.
Since LED toggling is controlled under both secure and non-secure Systick timers,
both Systick timers requires to be initialized with the same System clock frequency.

The secure IO is PB.11 which corresponds to LD2 and the non-secure toggling IO is
PB.08 for LD3.

The secure LD2 toggles every second and remains on in case of error in secure code.
The non-secure LD3 toggles twice faster and remains on in case of error in non-secure code.

This project is composed of two sub-projects:
- one for the secure application part (xxxxx_S)
- one for the non-secure application part (xxxxx_NS).

Please remember that on system with security enabled:
- the system always boots in secure and the secure application is responsible for
launching the non-secure application.
- the SystemInit() function in secure application sets up the SAU/IDAU, FPU and
secure/non-secure interrupts allocations defined in partition_stm32wbaxx.h file.

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
      
2.  The secure and non-secure applications need to ensure that the SysTick time base is always
      set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Security, TrustZone, GPIO, Output, Alternate function, Push-pull, Toggle 

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


:::
:::

