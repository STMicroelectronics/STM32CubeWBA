## <b>ADC_SingleConversion_TriggerSW_IT Example Description</b>

How to use ADC to convert a single channel at each SW start,
conversion performed using programming model: interrupt.
This example is based on the STM32WBAxx ADC HAL and LL API.
The LL API is used for performance improvement.

Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from SW trigger.
ADC interruption enabled: EOC (end of conversion of ADC group regular).

Example execution:

From the main program execution, a ADC software start is performed
every second, ADC group regular converts the selected channel.
When conversion is completed ADC interruption occurs.
IRQ handler callback function reads conversion data from ADC data register
and stores it into a variable.
IRQ handler is coded using LL driver for optimized latency of interruption
processing.

LD1 is used to monitor program execution status:

- Normal operation: ADC group regular activity can be observed with LED toggle:
  - At each ADC conversion: LED toggle once (every 1sec)
- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData": ADC group regular buffer conversion data (resolution 12 bits)
- "uhADCxConvertedData_Voltage_mVolt": ADC group regular buffer conversion data computed to voltage value (unit: mV)

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

Note: If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

  1 GPIO for LD1

  1 GPIO for analog input: ADC channel 4 on pin PA.07 (Arduino A0 pin 1 CN7, Morpho pin 28 CN3)

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, interruption

### <b>Directory contents</b>

  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - ADC/ADC_SingleConversion_TriggerSW_IT/Inc/main.h                  Header for main.c module
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/stm32wbaxx_it.c          Interrupt handlers
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/main.c                  Main program
  - ADC/ADC_SingleConversion_TriggerSW_IT/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.

  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

