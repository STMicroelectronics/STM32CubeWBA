## <b>ADC_ContinuousConversion_TriggerSW_LowPower Example Description</b>

Use ADC to convert a single channel with ADC low power features
auto wait and auto power-off.
This example is based on the STM32WBAxx ADC HAL API.

Description of ADC low power feature auto wait:

ADC conversions occurrences are limited to the minimum necessary to reduce
power consumption.
This allows to automatically adapt the ADC conversions rate to the speed
of the software that reads the conversion data.
Note: Drawback of this feature: the ADC conversion data read corresponds to
      previous ADC conversion start, independently of delay during which
      ADC was idle.
      Therefore, the ADC conversion data may be outdated: does not correspond 
      to the current voltage level on the selected ADC channel.
These features are intended to be used by applications needing ADC conversions
at few occurrences and without latency constraints.
ADC low power features are used most commonly with programming model: polling.

Example configuration:
ADC is configured to convert a single channel, in continuous conversion mode,
from SW trigger.
ADC low power feature enabled: auto wait and auto power-off.
These 2 low-power modes can be combined.
Note: On this STM32 series, ADC low power feature auto power-off is available,
      additionally to feature auto wait.
      This is not the case on all STM32 series.

Example execution:
From the main program execution, a ADC software start is performed once,
ADC group regular perform conversions of the selected channel
automatically, indefinitely, but with conversions managed by ADC mode low-power:
After the first trigger (software start in this example), following conversions
are not launched automatically: the ADC performs 1 conversion and remains idle
until conversion data has been processed by user software (action of retrieve
conversion value from ADC data register).

On this STM32 series, ADC low power feature auto power-off is available:

ADC is set automatically in mode disable during idle phase.
Then, when ADC conversion is requested, ADC is set automatically in mode enable. 

Software polls for the first conversion completion, then retrieves at will
(every second in this example) conversion data.

LD1 is used to monitor program execution status:

- Normal operation: ADC group regular activity can be observed with LED toggle:

  - ADC conversion data retrieved: LED toggle once (every 1sec)
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
  1 GPIO for analog input: ADC channel 3 on pin PA.06 (Arduino connector CN7 pin A1, Morpho connector CN3 pin 30)

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input

### <b>Directory contents</b>

  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Inc/stm32wbaxx_hal_conf.h        HAL configuration file
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Inc/stm32wbaxx_it.h              Interrupt handlers header file
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Inc/main.h                       Header for main.c module
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Src/stm32wbaxx_it.c              Interrupt handlers
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Src/stm32wbaxx_hal_msp.c         HAL MSP module
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Src/main.c                       Main program
  - ADC/ADC_ContinuousConversion_TriggerSW_LowPower/Src/system_stm32wbaxx.c          STM32WBAxx system source file


### <b>Directory contents</b>

  - This example runs on STM32WBA65RIVx devices.
    
  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


