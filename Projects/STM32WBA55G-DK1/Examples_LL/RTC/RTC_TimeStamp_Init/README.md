## <b>RTC_TimeStamp_Init Example Description</b>

Configuration of the Timestamp using the RTC LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSE as clock source.

The RTC peripheral configuration is ensured by the Configure_RTC() function
(configure of the needed RTC resources according to the used hardware CLOCK,
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

**Note:**

   LSI oscillator clock is used as RTC clock source by default.
       The user can use also LSE as RTC clock source.
       - The user uncomment the adequate line on the main.c file.
         @code
           #define RTC_CLOCK_SOURCE_LSI
           /* #define RTC_CLOCK_SOURCE_LSE */
         @endcode
       - Open the ioc file with STM32CubeMX and select :
         LSE as "Crystal/Ceramic Resonator" in RCC configuration.
         LSE as RTC clock source in Clock configuration.
       - Generate code
       LSI oscillator clock is delivered by a 32 kHz RC.
       LSE (when available on board) is delivered by a 32.768 kHz crystal.

Configure_RTC_TimeStamp() function is then called to initialize the time stamp feature
with interrupt mode. It configures the time stamp pin to be rising edge and enables
the time stamp detection on time stamp pin.

LL_RTC_DATE_Config()and LL_RTC_TIME_Config() functions are then called to initialize the
time and the date.

The associated firmware performs the following:

1. After start-up the program configure the RTC (Time date) and enable the feature
   timeStamp.

2. When applying a rising edge on the time stamp pin (PC.13 Arduino D4 Pin 5, CN10),
   a time stamp event is detected and the calendar is saved in the time stamp structures.
   The current date and time stamp are updated and displayed.

**Note:** On this board, time stamp pin is not connected to Joystick SEL push-button (pin PA.01).
         Therefore, an external connection is needed:

         - to generate a time stamp event using Joystick SEL push-button: connect PC.13 (Arduino D4 Pin 5, CN10) to PA.01 (Arduino A1 Pin 2, CN11).
         - to generate a time stamp event using external signal: connect PC.13 (Arduino D4 Pin 5, CN10) to voltage level 0V or 3.3V.
  
   on the debugger (live watch) in aShowTimeStamp and aShowDateStamp variables .
   The current time and date are updated and displayed on the debugger (live watch) in aShowTime, aShowDate variables .

- LD3 is toggling : This indicates that the system generates an error.

### <b>Keywords</b>

RTC, Timer, Timestamp, Counter, LSE, LSI, Current time, Real Time Clock

### <b>Directory contents</b>

  - RTC/RTC_TimeStamp_Init/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp_Init/Inc/main.h                   Header for main.c module
  - RTC/RTC_TimeStamp_Init/Inc/stm32_assert.h           Template file to include assert_failed function
  - RTC/RTC_TimeStamp_Init/Src/stm32wbaxx_it.c          Interrupt handlers
  - RTC/RTC_TimeStamp_Init/Src/main.c                   Main program
  - RTC/RTC_TimeStamp_Init/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with STM32WBA55G-DK1 board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
