## <b>ADC_TemperatureSensor_Init Example Description</b>

How to use an ADC peripheral to perform a single ADC conversion on the 
internal temperature sensor and calculate the temperature in degrees Celsius. 
This example uses the polling programming model (for interrupt or DMA 
programming models, refer to other examples).

### <b>Description</b>

This example is based on the STM32WBAxx ADC LL API.
The peripheral initialization is done using LL unitary service functions
for optimization purposes (performance and size).

Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.
Sequencer of group regular (default group available on ADC of all STM32 devices)
is configured to convert 1 channel: internal channel temperature sensor.

Example execution:

Program configures ADC and GPIO.
Then, program performs in infinite loop:

- start ADC conversion
- polls for conversion completion
- read conversion data
- compute temperature from raw data to physical values using LL ADC driver helper macro (unit: degree Celsius)
- turn-on LED2 during 0.5 sec

For debug, variables to monitor with debugger watch window:

- "uhADCxConvertedData": ADC group regular conversion data
- "hADCxConvertedData_Temperature_DegreeCelsius": Value of temperature (unit: degree Celsius)

In case of error, LED2 is blinking every 1 sec.

Connection needed:

  - None.

Other peripherals used:

  - 1 GPIO for LED2

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, oversampling

### <b>Directory contents</b>

  - ADC/ADC_Oversampling_Init/Inc/stm32wbaxx_it.h         Interrupt handlers header file
  - ADC/ADC_Oversampling_Init/Inc/main.h                  Header for main.c module
  - ADC/ADC_Oversampling_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - ADC/ADC_Oversampling_Init/Src/stm32wbaxx_it.c         Interrupt handlers
  - ADC/ADC_Oversampling_Init/Src/main.c                  Main program
  - ADC/ADC_Oversampling_Init/Src/system_stm32wbaxx.c     STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

