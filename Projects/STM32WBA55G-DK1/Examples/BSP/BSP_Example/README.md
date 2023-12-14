## <b>BSP_Example Description</b>

How to use the different BSP drivers of the board.

This project is targeted to run on STM32WBA55xx devices on STM32WBA55G-DK1 board from STMicroelectronics.

The project configures the maximum system clock frequency at 100Mhz.
- Note: The system clock frequency is switched to 98MHz when Audio is used for bitrate precision.

This example shows how to use the different functionalities of components available on the board by switching between all tests using Joystick SEL button.

Error_Handler is called whenever any error is detected.

- Press Joystick SEL (center Button) to start first Test.

#### <b>LED</b>
- This example shows how to switch on, switch off and toggle LED3.
- Joystick direction [ LEFT: Toggle LED3 | RIGHT: Toggle LED3 | DOWN: Disable LED3| UP: Enable LED3 | Sel: Exit ]

#### <b>JOYSTICK</b>
- Use the joystick to display direction on LCD (UP/DOWN/RIGHT/LEFT) in GPIO Mode. Joystick SEL to Exit.
- Use the joystick to display direction on LCD (UP/DOWN/RIGHT/LEFT) in GPIO Mode. Joystick SEL to Exit.

#### <b>LCD</b>
- This example shows how to use the different LCD features to display string with different fonts, to display different shapes and to draw a bitmap.

#### <b>AUDIO PLAY</b>
- You need to plug a headphone to the audio jack (CN3).
- This example shows how to play/pause/resume or change the volume (min 0% / max 100 %) of an audio playback using Joystick direction.
- Joystick direction [ LEFT: Audio Pause | RIGHT: Audio Resume | DOWN: Audio Vol- | UP: Audio Vol+ | Sel: Exit ]

#### <b>AUDIO RECORD</b>
- You need to plug a headphone to the audio jack (CN3).
- This example will record sound from Digital mic (U21 on MB1802) and and loopback it to headphone.
- Joystick direction [ LEFT: Audio Pause | RIGHT: Audio Resume | DOWN: Audio Stop | UP: Audio Start | Sel: Exit ]

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick Interrupt must have higher priority (numerically lower)
    than the peripheral interruot. Otherwise the caller ISR process will be blocked.
    To change the SysTick Interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

BSP, Led, Button, Joystick, LCD, Audio

### <b>Directory contents</b>

  - BSP/Src/audio_play.c                 AUDIO PLAY features
  - BSP/Src/audio_record.c               AUDIO RECORD features
  - BSP/Src/joystick.c                   joystick feature
  - BSP/Src/lcd.c                        LCD drawing features
  - BSP/Src/led.c                        Led display features
  - BSP/Src/main.c                       Main program
  - BSP/Src/stm32u5xx_hal_msp.c          HAL MSP module
  - BSP/Src/system_stm32wbaxx.c          STM32WBAxx system clock configuration file
  - BSP/Src/stm32wbaxx_it.c              Interrupt handlers
  - BSP/Inc/audi_16khz_waw.h             Audio wave extract.
  - BSP/Inc/main.h                       Main program header file
  - BSP/Inc/stm32wba55g_discovery_conf.h Board Configuration file
  - BSP/Inc/stm32wbaxx_hal_errno.h       HAL BSP Error Configuration file
  - BSP/Inc/stm32wbaxx_hal_conf.h        HAL Configuration file
  - BSP/Inc/stm32wbaxx_it.h              Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This template runs on STM32WBA55CGU devices without security enabled (TZEN=0).
  - This template has been tested with STMicroelectronics STM32WBA55G-DK1 (MB1802 and MB1803)
    board and can be easily tailored to any other sUPported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain
- Rebuild all files and load your image into target memory
- Run the example

