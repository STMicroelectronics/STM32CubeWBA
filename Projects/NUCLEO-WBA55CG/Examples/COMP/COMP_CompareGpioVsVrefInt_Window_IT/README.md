## <b>COMP_CompareGpioVsVrefInt_Window_IT example Description</b>

How to use a pair of comparator peripherals to compare a voltage level applied
on a GPIO pin to two thresholds: the internal voltage reference (VrefInt)
and a fraction of the internal voltage reference (VrefInt/2), in interrupt mode.
This example is based on the STM32WBAxx COMP HAL API.

Example configuration:

Comparator instances COMP1 and COMP2 are configured to work together
in window mode to compare 2 thresholds:

- COMP1 input plus set to a GPIO pin (cf pin below) in analog mode.
  COMP2 input plus is not used (connected internally to the 
  same input plus as COMP1).
- COMP1 input minus set to internal voltage reference VrefInt 
  (voltage level 1.2V, refer to device datasheet for min/typ/max values)
- COMP2 input minus set to 1/2 of internal voltage reference VrefInt 
  (voltage level 0.6V)
Comparator interruption is enabled with trigger edge set to
both edges rising and falling.

Example execution:

From the main program execution, comparator is enabled.
Each time the voltage level applied on GPIO pin (comparator input plus) 
is crossing one of the two thresholds related to VrefInt (comparator input minus),
the comparator with corresponding threshold generates an interruption.

LD1 is used to monitor program execution status:

- Normal operation:

  - LED turned on if comparator voltage level is inside window thresholds
    (between VrefInt and 1/2 VrefInt: between 1.2V and 0.6V)
  - LED turned off is out of window thresholds
  
- Error: LED remaining turned on

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).
Note: If no power supply available, this pin can be let floating (in this case
      comparator output state will be undetermined).

Other peripherals used:

  1 GPIO for LD1
  1 GPIO for analog input: pin PA.02 (Arduino connector CN7 pin A2, Morpho connector CN3 pin 32)

### <b>Keywords</b>

Comparator, COMP, analog, voltage, analog input, threshold, VrefInt, window

### <b>Directory contents</b>

    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32wbaxx_nucleo_conf.h     BSP configuration file
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/stm32wbaxx_it.h          Interrupt handlers header file
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Inc/main.h                  Header for main.c module
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/stm32wbaxx_it.c          Interrupt handlers
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/stm32wbaxx_hal_msp.c     HAL MSP module
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/main.c                  Main program
    - COMP/COMP_CompareGpioVsVrefInt_Window_IT/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
    
  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

