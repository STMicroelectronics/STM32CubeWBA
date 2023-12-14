## <b>COMP_CompareGpioVsVrefInt_IT_Init Example Description</b>

How to use a comparator peripheral to compare a voltage level applied on
a GPIO pin to the the internal voltage reference (VrefInt), in interrupt mode.
This example is based on the STM32WBAxx COMP LL API.
The peripheral initialization is done using LL unitary service functions
for optimization purposes (performance and size).

Example configuration:

Comparator instance COMP1 is configured to compare:

- input plus set to a GPIO pin (cf pin below) in analog mode.
- input minus set to internal voltage reference VrefInt (voltage level 1.2V,
  refer to device datasheet for min/typ/max values)
  
Comparator interruption is enabled through EXTI line 17 with trigger edge
set to both edges rising and falling.

Example execution:

From the main program execution, comparator is enabled.
Each time the voltage level applied on GPIO pin (comparator input plus) 
is crossing VrefInt voltage threshold (comparator input minus),
the comparator generates an interruption.

LD1 is used to monitor program execution status:

- Normal operation:
  - LED turned on if comparator voltage level on input plus is higher than input minus
  - LED turned off if comparator voltage level on input plus is lower than input minus
- Error: LED toggles slowly

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

**Note:** If no power supply available, this pin can be let floating (in this case
comparator output state will be undetermined).

Other peripherals used:

 - 1 GPIO for LD1
 - 1 GPIO for analog input: pin PA.02 (Arduino connector CN7 pin A2, Morpho connector CN3 pin 32)

### <b>Keywords</b>

Comparator, COMP, analog, voltage, analog input, threshold, VrefInt

### <b>Directory contents</b>

  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Inc/main.h                  Header for main.c module
  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Src/stm32wbaxx_it.c          Interrupt handlers
  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Src/main.c                  Main program
  - COMP/COMP_CompareGpioVsVrefInt_IT_Init/Src/system_stm32wbaxx.c      STM32WBAxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32WBA55CGUx devices.
    
  - This example has been tested with NUCLEO-WBA55CG board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

