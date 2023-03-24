## <b>ADC_SingleConversion_TriggerTimer_DMA Example Description</b>

How to use an ADC peripheral to perform a single ADC conversion on a channel 
at each trigger event from a timer. Converted data is transferred by DMA 
into a table in RAM memory.
This example is based on the STM32WBAxx ADC HAL API.

Example configuration:

ADC is configured to convert a single channel, in single conversion mode,
from HW trigger: timer peripheral (timer instance: TIM2).
timer is configured to provide a time base of 1kHz.
DMA is configured to transfer conversion data in an array, in circular mode.
A timer is configured in time base and to generate TRGO events.

Example execution:

From the main program execution, the ADC group regular converts the
selected channel at each trig from timer. DMA transfers conversion data to the array,
indefinitely (DMA in circular mode).

When conversion are completed (DMA half or full buffer complete),
interruption occurs and data is processed in callback functions (for this example purpose,
convert digital data to physical value in mV).

LD1 is used to monitor program execution status:

- Normal operation: ADC group regular buffer activity (buffer complete events):
  LED toggle at frequency of time base / half buffer size (approx. 31Hz with this example default settings)
- Error: LED remaining turned on

Debug: variables to monitor with debugger:

- "uhADCxConvertedData": ADC group regular buffer conversion data (resolution 12 bits)
- "uhADCxConvertedData_Voltage_mVolt": ADC group regular buffer conversion data computed to voltage value (unit: mV)

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

**Note:** If no power supply available, this pin can be let floating (in this case
ADC conversion data will be undetermined).

Other peripherals used:

 - 1 GPIO for LD1
 - 1 GPIO for analog input: ADC channel 2 on pin PA.07 (Arduino A0 connector CN7 pin 1, Morpho connector CN3 pin 28)
 - 1 DMA channel
 - 1 timer instance

### <b>Keywords</b>

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, regular, injected, DMA transfer, trigger

### <b>Directory contents</b>

  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32wbaxx_nucleo_conf.h BSP configuration file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Inc/main.h                   Header for main.c module
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/stm32wbaxx_it.c          Interrupt handlers
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/stm32wbaxx_hal_msp.c     HAL MSP module
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/main.c                   Main program
  - ADC/ADC_SingleConversion_TriggerTimer_DMA/Src/system_stm32wbaxx.c      STM32WBAxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA52CGUx devices.

  - This example has been tested with NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

