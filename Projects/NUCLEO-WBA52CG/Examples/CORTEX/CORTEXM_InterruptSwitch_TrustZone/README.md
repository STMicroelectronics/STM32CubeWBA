
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>CORTEXM_InterruptSwitch_TrustZone Example Description</b>

How to first use an interrupt in secure application and later assign it to the
non-secure application when TrustZone security is activated (Option bit TZEN=1).

In this example, the interrupt associated to EXTI line 13 (connected to
the B1 push-button) is used by the secure application that is waiting from a user push-button
key press event before assigning then the interrupt to the non-secure application with
NVIC_SetTargetState() API and launching the non-secure application.

At the first B1 push-button key press, the LD2 is switched on by the secure application. Any 
further B1 push-button key press makes the LD3 managed by the non-secure application toggling.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
2.  The secure and non-secure applications need to ensure that the SysTick time base is always
      set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

System, Trustzone, Cortex, Interrupt

### <b>Directory contents</b>

  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/main.c                     Secure Main program
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/stm32wbaxx_hal_msp.c       Secure HAL MSP module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/stm32wbaxx_it.c            Secure Interrupt handlers
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Src/system_stm32wbaxx_s.c      Secure STM32WBAxx system clock configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/main.h                     Secure Main program header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/partition_stm32wbaxx.h     STM32WBA Device System Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/stm32wbaxx_hal_conf.h      Secure HAL Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure/Inc/stm32wbaxx_it.h            Secure Interrupt handlers header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/main.c                  Non-secure Main program
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/stm32wbaxx_hal_msp.c    Non-secure HAL MSP module
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/stm32wbaxx_it.c         Non-secure Interrupt handlers
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Src/system_stm32wbaxx_ns.c  Non-secure STM32WBAxx system clock configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/main.h                  Non-secure Main program header file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/stm32wbaxx_hal_conf.h   Non-secure HAL Configuration file
  - CORTEX/CORTEXM_InterruptSwitch_TrustZone/NonSecure/Inc/stm32wbaxx_it.h         Non-secure Interrupt handlers header file

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

