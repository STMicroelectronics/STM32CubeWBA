
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>RCC_ClockConfig_TrustZone Example Description</b>

How to configure he system clock (SYSCLK) in Run mode from the secure application
upon request from the non-secure application, using the RCC HAL API when TrustZone security is activated (Option bit TZEN=1).

In this example, after startup, the SYSCLK is configured to the max frequency using the PLL with
HSE as clock source by the secure application and the PLL, SYSCLK and AHB/APB prescaler are set as secure. 
The secure application provides then non-secure callable functions to either configure the SYSCLK to maximum
frequency using the PLL with HSI or HSE as clock source. The B1 push-button (connected to External line 13) 
will be used by the non-secure application to change alternatively the PLL source:
- from HSI to HSE
- from HSE to HSI.

Each time the B1 push-button is pressed External line 13 interrupt is generated in the
non-secure application and a flag raised to instruct the non-secure main program to request
a switch of SYSCLK configuration to the secure application.

- If the HSE oscillator is selected as PLL source, the following steps will be followed to switch
   the PLL source to HSI oscillator:
     a- Switch the system clock source to HSE to allow modification of the PLL configuration
     b- Enable HSI Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure the PCLK1, PCLK2 and PCLK7 clocks dividers
     d- Disable the HSE oscillator (optional, if the HSE is no more needed by the application)

- If the HSI oscillator is selected as PLL source, the following steps will be followed to switch
   the PLL source to HSE oscillator:
     a- Switch the system clock source to HSI to allow modification of the PLL configuration
     b- Enable HSE Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure the HCLK, PCLK1, PCLK2 and PCLK7
        clocks dividers 
     d- Disable the HSI oscillator (optional, if the HSI is no more needed by the application)

LD2 is toggled with a timing defined by the HAL_Delay() API: slower for HSI or faster for HSE
LD3 is switched on by the secure application in case of error.

#### <b>Notes</b>

1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2.  The secure and non-secure applications need to ensure that the SysTick time base is always
      set to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Security, TrustZone, RCC, System, Clock Configuration, System clock, Oscillator, PLL

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


:::
:::

