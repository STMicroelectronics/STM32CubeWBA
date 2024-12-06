## <b>DMA_MemToMem_TrustZone Example Description</b>

How to use HAL DMA to perform memory to memory data transfers over secure and non-secure
DMA channels when TrustZone security is activated (Option bit TZEN=1).

The purpose of this example is to declare secure and non-secure DMA channels and
to use these channels for data transfers within or between the secure and non-secure
applications depending on the memory security attribute.

The example follows this sequence:

Secure Application :
The secure application defines 3 secure DMA channels for memory to memory data
transfers:
 - DMA1_Channel1 for data transfer over DMA between secure memory source and secure
   memory destination
 - DMA1_Channel2 for data transfer over DMA between secure memory source and non-secure
   memory destination (this transfer requests to call Non-secure callable function)
 - DMA1_Channel3 for data transfer over DMA between non-secure memorysource and secure
   memory destination (this transfer requests to call Non-secure callable function)

The secure application launches a DMA transfer to copy reference data from secure
Flash to secure RAM with DMA1_Channel1 secure channel with interrupt. When the
transfer completes the secure application switches on LD2 and launches the
non-secure application.

Three Non-secure callable APIs are provided in order to:
 - allows the non-secure to use the second secure DMA channel
 - allows the non-secure to use the third secure DMA channel
 - allows the non-secure to ask for a secure check on exchanged data

Non-Secure Application :
The non-secure application defines 1 non-secure DMA channel for memory to memory
data transfers:
 - DMA1_Channel4 for data transfer over DMA between non-secure memory
   source and destination

Once started, the non-secure application requests the secure application for
a data copy of the reference secure data into the non-secure memory (with
a non-secure call function). The non-secure application uses after the
non-secure DMA channel for another local copy of the reference data in
non-secure memory buffers (with a non-secure call function).
This last local data copy is then shared to the secure application with
a non-secure call function (the secure application stores this data).
Finally the non-secure application calls the last non-secure
callable service to get a comparison status on the original secure data buffer
and the latest data buffer shared by the non-secure.

All DMA transfers are done with interrupts: the secure partition_stm32wba55xx.h
makes sure that:
 - the 3 associated secure DMA channel interrupts are set as secured (default
   interrupt state)
 - the non-secure DMA channel interrupt is assigned to non-secure.


The secure LD2 is on when the DMA transfer between secure memory addresses completes.
LD2 toggles in case of error detected by the secure application.

The non-secure LD3 is on when all requested DMA transfers complete and the latest
non-secure data buffer matches the original buffer in secure memory (non-secure
call function). LD3 toggles in case of error detected by the non-secure application.

This project is composed of two sub-projects:
- one for the secure application part (Project_s)
- one for the non-secure application part (Project_ns).

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.

This project mainly shows how to switch from secure application to non-secure application
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

Security, TrustZone, DMA, Data Transfer, Memory to memory, RAM

### <b>Directory contents</b>

  - Secure/Src/main.c                     Secure Main program
  - Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
  - Secure/Src/stm32wbaxx_hal_msp.c       Secure HAL MSP module
  - Secure/Src/stm32wbaxx_it.c            Secure Interrupt handlers
  - Secure/Src/system_stm32wbaxx_s.c      Secure STM32WBAxx system clock configuration file
  - Secure/Inc/main.h                     Secure Main program header file
  - Secure/Inc/partition_stm32wba55xx.h   STM32WBA Device System Configuration file
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

