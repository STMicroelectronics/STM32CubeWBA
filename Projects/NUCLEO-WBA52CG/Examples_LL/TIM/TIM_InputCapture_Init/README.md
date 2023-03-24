## <b>TIM_InputCapture_Init Example Description</b>

Use of the TIM peripheral to measure a periodic signal frequency
provided either by an external signal generator or by
another timer instance. This example is based on the STM32WBAxx TIM
LL API. The peripheral initialization uses LL unitary service functions
for optimization purposes (performance and size).

TIM1_CH1 is configured in input capture mode. The TIM1CLK frequency is set to
its maximum value (Prescaler is /2 * 2 = 1) in order to get the best possible resolution.
So the TIM1 counter clock is SystemCoreClock.

SystemCoreClock is set to 100 MHz for STM32WBAxx Devices.

The "uwMeasuredFrequency" variable contains the measured signal frequency.
It is calculated within the capture/compare 1 interrupt service routine.

The minimum frequency value to measure is TIM1 counter clock / TIMx_CCR1 MAX
                                              = 100 MHz / 65535

Due to TIM1 interrupt routine processing time (around 4us), the maximum
frequency value to measure is around 700 KHz.

TIM3_CH1 is configured to generate a PWM signal.  USER push-button can be used to
change the frequency of this signal from 2 kHz up to 20 kHz by steps of 2 kHz.
It is then possible to run this example without a signal generator by connecting
TIM3_CH1 to TIM1_CH1.

Note that LD3 remains ON when connecting TIM1_CH1 with TIM3_CH1 : : LD3 and TIM1_CH1 share the same pin PB8.

### <b>Keywords</b>

Timer, TIM, Frequency, Input, Capture, External Signal, Measurement

### <b>Directory contents</b>

  - TIM/TIM_InputCapture_Init/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - TIM/TIM_InputCapture_Init/Inc/main.h                  Header for main.c module
  - TIM/TIM_InputCapture_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_InputCapture_Init/Src/stm32wbaxx_it.c         Interrupt handlers
  - TIM/TIM_InputCapture_Init/Src/main.c                  Main program
  - TIM/TIM_InputCapture_Init/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG Set-up
    - When no signal generator is used TIM3_CH1 can be connected to TIM1_CH1:
      - TIM1_CH1  PB8: connected to pin 38 of CN4 connector
      - TIM3_CH1  PA2: connected to pin 32 of CN3 connector

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

