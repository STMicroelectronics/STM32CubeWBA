## <b>RTC_Alarm_Init Example Description</b>

Configuration of the RTC LL API to configure and generate an alarm using the RTC peripheral. The peripheral
initialization uses the LL initialization function.

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSE as clock source.

The RTC peripheral configuration is ensured by the Configure_RTC() function
(configure of the needed RTC resources according to the used hardware CLOCK,
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

**Note:** LSI oscillator clock is used as RTC clock source by default.

    The user can use also LSE as RTC clock source.

    - The user uncomment the adequate line on the main.c file.

      @code

        #define RTC_CLOCK_SOURCE_LSI

        /* #define RTC_CLOCK_SOURCE_LSE */

      @endcode

    LSI oscillator clock is delivered by a 32 kHz RC.

    LSE (when available on board) is delivered by a 32.768 kHz crystal.

Configure_RTC_Alarm function is then called to initialize the
time, date and alarm.

In this example, the Time is set to 11:59:55 PM and the Alarm must be generated after
30 seconds on 12:00:25 AM.

LD1 is turned ON when the RTC Alarm is generated correctly.

The current time and date are updated and displayed on the debugger in aShowTime
and aShowDate variables (watch or live watch).

In case of error, LD1 is toggling.

### <b>Keywords</b>

System, RTC, Alarm, wakeup timer, Backup domain, Counter, LSE, LSI

### <b>Directory contents</b>

  - RTC/RTC_Alarm_Init/Inc/stm32wbaxx_it.h     Interrupt handlers header file
  - RTC/RTC_Alarm_Init/Inc/main.h              Header for main.c module
  - RTC/RTC_Alarm_Init/Inc/stm32_assert.h      Template file to include assert_failed function
  - RTC/RTC_Alarm_Init/Src/stm32wbaxx_it.c     Interrupt handlers
  - RTC/RTC_Alarm_Init/Src/main.c              Main program
  - RTC/RTC_Alarm_Init/Src/system_stm32wbaxx.c STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

