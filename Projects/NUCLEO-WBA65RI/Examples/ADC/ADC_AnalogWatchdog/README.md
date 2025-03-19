## <b>ADC_AnalogWatchdog Example Description</b>

How to use an ADC peripheral with an ADC analog watchdog to monitor a channel
and detect when the corresponding conversion data is outside the window
thresholds.
This example is based on the STM32WBAxx ADC HAL API.

Example configuration:

ADC is configured to convert a single channel, in continuous conversion mode,
from SW trigger.
Analog watchdog 1 is configured to monitor all channels on ADC group regular
(therefore, including the selected channel).

Analog watchdog thresholds values:
- high threshold is set to Vref+/2 (3.3V/2=1.65V on NUCLEO-WBA65RI), refer to literal ADC_AWD_THRESHOLD_HIGH.
- low threshold is set to 0V, refer to literal ADC_AWD_THRESHOLD_LOW.
ADC interruption enabled: Analog watchdog 1.

Example execution:

From the main program execution, the ADC converts the selected channel continuously.
When conversion data is out of analog watchdog window, ADC interruption occurs
and analog watchdog callback function is called to process the event.
Every 5 seconds, the ADC analog watchdog is rearmed to be ready for another trig.

LD1 is used to monitor program execution status:

- Normal operation: Voltage of ADC channel monitored versus analog watchdog window
  can be observed with LED toggle frequency:
  
  - Voltage within analog watchdog window: toggle frequency slow (2Hz)
  - Voltage below analog watchdog window: toggle frequency fast (10Hz)
  - Voltage above analog watchdog window: toggle frequency very fast (20Hz)
- Error: LED remaining turned on

Note: In this example, analog watchdog low threshold is set to 0V
      for test simplicity purpose, therefore case of voltage below low threshold
      is not achievable unless cutomizing low threshold value.

Note: In case of noise on voltage applied on ADC channel input (due to connectors and wires parasitics),
      ADC analog watchdog may trig at a voltage level with an uncertainty of tens of mV.

Debug: variables to monitor with debugger:

- "ubAnalogWatchdog1Status": analog watchdog state

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).
Note: If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

  1 GPIO for LD1

  1 GPIO for analog input: ADC channel 3 on pin PA.06 (Arduino A1 pin 2 CN7, Morpho pin 30 CN3)

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, trigger, analog watchdog, threshold, window

### <b>Directory contents</b>

    - ADC/ADC_AnalogWatchdog/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
    - ADC/ADC_AnalogWatchdog/Inc/stm32wbaxx_hal_conf.h        HAL configuration file
    - ADC/ADC_AnalogWatchdog/Inc/stm32wbaxx_it.h              Interrupt handlers header file
    - ADC/ADC_AnalogWatchdog/Inc/main.h                       Header for main.c module
    - ADC/ADC_AnalogWatchdog/Src/stm32wbaxx_it.c              Interrupt handlers
    - ADC/ADC_AnalogWatchdog/Src/stm32wbaxx_hal_msp.c         HAL MSP module
    - ADC/ADC_AnalogWatchdog/Src/main.c                       Main program
    - ADC/ADC_AnalogWatchdog/Src/system_stm32wbaxx.c          STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA65RIVx devices.

  - This example has been tested with NUCLEO-WBA65RI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

